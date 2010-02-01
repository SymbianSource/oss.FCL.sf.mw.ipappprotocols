// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name        : CTransportTls.cpp
// Part of     : ConnectionMgr
// implementation
// Version     : SIP/5.0
//



#include <securesocket.h>

#include "CTransportTls.h"
#include "CSipConnection.h"
#include "CErrorHandler.h"
#include "MTransactionFinder.h"
#include "MTransportOwner.h"
#include "MServerTaFactory.h"
#include "CSender.h"
#include "CSenderTcp.h"
#include "CReceiverTcp.h"
#include "CommonConsts.h"
#include "siperr.h"
#include "NetworkInfo.h"
#include "COwnerSettingsList.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "TTlsTransTcpConnecting.h"
#include "TTlsTransTlsConnecting.h"
#include "TTlsTransTlsConnected.h"
#include "CSecureSocketContainer.h"
#include <sipnattraversalcontroller.h>
#include "SipLogs.h"
#include "uricontainer.h"
#include "sipuri.h"
#include "siphostport.h"
#include <asn1dec.h>
#include <ssl_internal.h>
#include <x509cert.h>
#include <x500dn.h>
#include <x520ava.h>
#include <x509certext.h>
#include <x509gn.h>
#include <securitydefsconst.h>

const TInt KMaxConnectAttempts = 2;
_LIT(KSipDomainOID, "1.3.6.1.5.5.7.3.20");
_LIT(KAnyExtendedKeyUsage, "2.5.29.37.0");

// -----------------------------------------------------------------------------
// CTransportTls::NewL
// -----------------------------------------------------------------------------
//
CTransportTls* CTransportTls::NewL(
    RSocketServ& aServer,
	MTransactionFinder& aFinder, 
	MServerTaFactory& aTaFactory,
	MTransportOwner& aTransportOwner,
	MTimerManager& aTimer,
	const TSIPTransportParams& aTransportParams,
	const TInetAddr& aRemoteAddr,
	TUint aTOne,
	CSIPServerResolver& aServerResolver,
	RConnection& aConnection,
	MSigCompController& aSigCompHandler,
	CNetworkInfo& aNetworkInfo,
	COwnerSettingsList& aSettingsList,
	CSIPNATTraversalController& aNATTraversal )
	{
	CTransportTls* self = NewLC( aServer, 
	                             aFinder, 
	                             aTaFactory, 
								 aTransportOwner, 
								 aTimer,
								 aTransportParams,
								 aRemoteAddr, 
								 aTOne,
								 aServerResolver, 
								 aConnection, 
								 aSigCompHandler, 
								 aNetworkInfo, 
								 aSettingsList,
								 aNATTraversal );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CTransportTls::NewLC
// -----------------------------------------------------------------------------
//
CTransportTls* CTransportTls::NewLC(
    RSocketServ& aServer,
	MTransactionFinder& aFinder, 
	MServerTaFactory& aTaFactory,
	MTransportOwner& aTransportOwner,
	MTimerManager& aTimer,
	const TSIPTransportParams& aTransportParams,
	const TInetAddr& aRemoteAddr,
	TUint aTOne,
	CSIPServerResolver& aServerResolver,
	RConnection& aConnection,
	MSigCompController& aSigCompHandler,
	CNetworkInfo& aNetworkInfo,
	COwnerSettingsList& aSettingsList,
	CSIPNATTraversalController& aNATTraversal )
	{
	CTransportTls* self = new ( ELeave ) CTransportTls( aServer,
	                                                    aFinder, 
	                                                    aTaFactory, 
                					                    aTransportOwner, 
                					                    aTimer,
                					                    aTransportParams,
                					                    aRemoteAddr,
                					                    aTOne,
                					                    aServerResolver,
                					                    aConnection, 
                					                    aSigCompHandler,
                				                        aNetworkInfo,
                				                        aSettingsList,
                				                        aNATTraversal );
					   
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CTransportTls::CTransportTls
// -----------------------------------------------------------------------------
//
CTransportTls::CTransportTls(
    RSocketServ& aServer,
	MTransactionFinder& aFinder, 
	MServerTaFactory& aTaFactory,
	MTransportOwner& aTransportOwner,
	MTimerManager& aTimer,
	const TSIPTransportParams& aTransportParams,
	const TInetAddr& aRemoteAddr,
	TUint aTOne,
	CSIPServerResolver& aServerResolver,
	RConnection& aConnection,
	MSigCompController& aSigCompHandler,
	CNetworkInfo& aNetworkInfo,
	COwnerSettingsList& aSettingsList,
	CSIPNATTraversalController& aNATTraversal ) : 
	CTransport( aFinder, 
	            aTaFactory, 
	            aTransportOwner,
			    aServerResolver, 
			    aSigCompHandler,
			    aTransportParams, 
			    aNetworkInfo, 
			    aSettingsList,
			    aNATTraversal ),
	iSocketServ( aServer ),
	iTimer( aTimer ),
	iTimerValue( 64 * aTOne ),
	iConnection( aConnection ),
	iRemoteAddr( aRemoteAddr ),
	iStates( TTlsTransStateBase::EMaxStates ),
    iCurrentState( TTlsTransStateBase::ETcpConnecting ),
    iIsSending( EFalse )
	{
	}

// -----------------------------------------------------------------------------
// CTransportTls::ConstructL
// -----------------------------------------------------------------------------
//
void CTransportTls::ConstructL()
	{
	iErrorHandler =	CTransportErrorHandler::NewL( *this );
	
	User::LeaveIfError( iSocket.Open( iSocketServ, 
	                                  KAfInet, 
	                                  KSockStream,
									  KProtocolInetTcp, 
									  iConnection ) );
	
	// Secure socket will be added later to the container
	iSocketContainer = CSecureSocketContainer::NewL( iSocket );
	
	iReceiver = CReceiverTcp::NewL( *this );
	
	iSender = CSenderTcp::NewL( *this, iSettingsList );
	iDomainName = NULL;
	
	InitializeStatesL();
	}


// -----------------------------------------------------------------------------
// CTransportTls::~CTransportTls
// -----------------------------------------------------------------------------
//
CTransportTls::~CTransportTls()
	{
	iTimer.Stop( iMyTimerId );
	delete iErrorHandler;
	delete iReceiver;
	delete iSender;
	delete iSocketContainer;
	delete iDomainName;
	if ( iSecureSocket )
	    {
	    iNATTraversal.SocketIdle( EFalse, *iSecureSocket );
	    delete iSecureSocket;
	    }
	iSocket.Close();
	}
	
// -----------------------------------------------------------------------------
// CTransportTls::HandleMessage
// -----------------------------------------------------------------------------
//
TBool CTransportTls::HandleMessage(
    const TSIPTransportParams& aParams,
    RStringF aProtocol,
	const TInetAddr& aRemoteAddr,
	TUint /*aLocalPort*/,
	TBool /*aIsStrict*/ )
	{  
	return ( aProtocol == SIPStrings::StringF( SipStrConsts::ETLS ) && 
	         iRemoteAddr.CmpAddr( aRemoteAddr ) &&
	         Match( aParams ) );
	}

// -----------------------------------------------------------------------------
// CTransportTls::SourcePort
// -----------------------------------------------------------------------------
//
TUint CTransportTls::SourcePort()
	{
	return iSocket.LocalPort();
	}

// -----------------------------------------------------------------------------
// CTransportTls::SendToNetL
// -----------------------------------------------------------------------------
//
void CTransportTls::SendToNetL(
    const TSIPTransportParams& aParams,
    const TInetAddr& aAddress, 
    CSIPMessage& aMessage, 
    TBool /*aForceUDP*/,
    TUint aOrigTransport,
    TRequestStatus &aStatus )
	{
	HandleMixedAddressFamilysL( aMessage, aAddress );
	
	CurrentState().SendToNetL( aParams,
	                           aAddress,
	                           aMessage,
							   aOrigTransport,
							   aStatus );
	}

// -----------------------------------------------------------------------------
// CTransportTls::CancelSend
// -----------------------------------------------------------------------------
//
TBool CTransportTls::CancelSend( TRequestStatus& aStatus )
	{
	return iSender->FindAndCancel( aStatus );
	}

// -----------------------------------------------------------------------------
// CTransportTls::ErrorHandler
// -----------------------------------------------------------------------------
//	
CTransportErrorHandler& CTransportTls::ErrorHandler() 
    {
    return *iErrorHandler;
    }

// -----------------------------------------------------------------------------
// CTransportTls::WaitL
// -----------------------------------------------------------------------------
//
void CTransportTls::WaitL()
	{
	}

// -----------------------------------------------------------------------------
// CTransportTls::IsWaiting
// -----------------------------------------------------------------------------
//
TBool CTransportTls::IsWaiting( TUint /*aProtocol*/, TUint /*aPort*/ )
	{
	return EFalse;	
	}
	
// -----------------------------------------------------------------------------
// CTransportTls::CancelAllRequests
// -----------------------------------------------------------------------------
//
void CTransportTls::CancelAllRequests( CSIPConnection::TState aReason )
	{
	GetSender().CancelAllRequests( aReason );
	}
	
// -----------------------------------------------------------------------------
// CTransportTls::Protocol
// -----------------------------------------------------------------------------
//	
TUint CTransportTls::Protocol()
    {
    return KProtocolTls;
    }
    
// -----------------------------------------------------------------------------
// CTransportTls::GetSocketContainerL
// -----------------------------------------------------------------------------
//	
CSocketContainer& CTransportTls::GetSocketContainerL( 
    const TInetAddr& /*aRemoteAddr*/ )
    {
    return *iSocketContainer;
    }
    
// -----------------------------------------------------------------------------
// CTransportTls::InformSendingStatus
// -----------------------------------------------------------------------------
//    
void CTransportTls::InformSendingStatus( const TInetAddr& /*aRemoteAddr*/ )
    {
    Sending( iIsSending );
    }

// -----------------------------------------------------------------------------
// CTransportTls::Sender
// -----------------------------------------------------------------------------
//
CSender* CTransportTls::Sender(
    const TSIPTransportParams& aParams,
    TUint aProtocol, 
    const TInetAddr& aRemoteAddr )
	{
	if ( Protocol() == aProtocol && 
	     aRemoteAddr.CmpAddr( iRemoteAddr ) && 
	     Match( aParams ) )
		{
		return iSender;
		}
	return 0;
	}

// -----------------------------------------------------------------------------
// CTransportTls::Disconnect
// -----------------------------------------------------------------------------
//
void CTransportTls::Disconnect( const TInetAddr& /*aRemoteHost*/ )
	{
	// If this is reserved or persistent transport, 
	// only all send operations are cleared
	if ( IsReservedTransport() || IsPersistent() )
	    {
	    iSender->CancelAllRequests( KErrSIPTransportFailure );
	    }
	else
	    {
	    Remove();
	    }
	}
    
// -----------------------------------------------------------------------------
// CTransportTls::PersistentRemoteAddr
// -----------------------------------------------------------------------------
// 
TInetAddr* CTransportTls::PersistentRemoteAddr( 
    MSIPNATBindingObserver* aBindingObserver )
    {
    TInt index = iBindingObservers.FindInAddressOrder( aBindingObserver );
    if ( index >= 0 && index < iBindingObservers.Count() )
        {
        return &iRemoteAddr;
        }
    return 0;
    }

// -----------------------------------------------------------------------------
// CTransportTls::SocketContainer
// -----------------------------------------------------------------------------
//
CSocketContainer& CTransportTls::SocketContainer()
    {
    return *iSocketContainer;
    }
// -----------------------------------------------------------------------------
// CTransportTls::SocketContainer
// -----------------------------------------------------------------------------
//
CSocketContainer* CTransportTls::SocketContainer( 
    TUint /*aIPAddrFamily*/ )
    {
    return iSocketContainer;
    }   
    
// -----------------------------------------------------------------------------
// CTransportTls::ReceivedDataL
// -----------------------------------------------------------------------------
//
void CTransportTls::ReceivedDataL( 
    HBufC8* /*aData*/,
    const TInetAddr& /*aRemoteAddr*/,
    TBool /*aCompressed*/ ) 
    {
    }
        
// -----------------------------------------------------------------------------
// CTransportTls::ReceivedDataL
// -----------------------------------------------------------------------------
//
void CTransportTls::ReceivedDataL( 
    CSIPMessage* aMessage,
    const TInetAddr& aRemoteAddr,
    TInt aError,
    TBool aCompressed )
	{
	RecvL( aMessage, aRemoteAddr, aError, aCompressed );
	}

// -----------------------------------------------------------------------------
// CTransportTls::SigCompHandler
// -----------------------------------------------------------------------------
//
MSigCompController* CTransportTls::SigCompHandler()
	{
	return SigCompressionHandler();
	}
	
// -----------------------------------------------------------------------------
// CTransportTls::ResetSocketL
// -----------------------------------------------------------------------------
//
void CTransportTls::ResetSocketL()
	{
	}

// -----------------------------------------------------------------------------
// CTransportTls::ContinueL
// -----------------------------------------------------------------------------
//
void CTransportTls::ContinueL()
	{
	}

// -----------------------------------------------------------------------------
// CTransportTls::StopL
// -----------------------------------------------------------------------------
//
TBool CTransportTls::StopL()
	{
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CTransportTls::Remove
// -----------------------------------------------------------------------------
//
TInt CTransportTls::Remove()
	{
	return iTransportOwner->RemoveTransport( this );
	}
	
// -----------------------------------------------------------------------------
// CTransportTls::ConnectionOpenL
// -----------------------------------------------------------------------------
//
TBool CTransportTls::ConnectionOpenL()
	{
	return CurrentState().ConnectionOpenL();
	}
		
// -----------------------------------------------------------------------------
// CTransportTls::DisconnectedL
// -----------------------------------------------------------------------------
//
TBool CTransportTls::DisconnectedL()
	{
	TBool retVal( ETrue );
	
	// Flow failure notification may lead to immediate self deletion
	if ( !NotifyFlowFailure() )
	    {
	    retVal = CurrentState().DisconnectedL();
	    }
	    
	return retVal;
	}

// -----------------------------------------------------------------------------
// CTransportTls::ReRouteL
// -----------------------------------------------------------------------------
//
void CTransportTls::ReRouteL( TUint /*aProtocol*/, COutgoingData* aData )
	{
	// TLS transport doesn't allow rerouting
	if ( aData )
	    {
	    TRequestStatus* status = aData->Status();
    	if ( status != 0 && *status == KRequestPending )
    		{
    		User::RequestComplete( status, KErrSIPTransportFailure );
    		}
        delete aData;
	    }
	}
	
// -----------------------------------------------------------------------------
// CTransportTls::IapId
// -----------------------------------------------------------------------------
//
TUint32 CTransportTls::IapId()
	{
	return iTransportOwner->IapId();
	}

// -----------------------------------------------------------------------------
// CTransportTls::Sending
// -----------------------------------------------------------------------------
//
void CTransportTls::Sending( TBool aIsSending )
    {
    iIsSending = aIsSending;
    
    if ( iSecureSocket )
        {
        iNATTraversal.SocketIdle( !iIsSending, *iSecureSocket );
        }
    }

// -----------------------------------------------------------------------------
// CTransportTls::ChangeStateL
// -----------------------------------------------------------------------------
//
void CTransportTls::ChangeStateL( TTlsTransStateBase::TState aState )
    {
    iCurrentState = aState;
    
    CurrentState().EnterL();
    }

// -----------------------------------------------------------------------------
// CTransportTls::ConnectTcpL
// -----------------------------------------------------------------------------
//   	
void CTransportTls::ConnectTcpL()
    {
    iReceiver->Connect( iRemoteAddr );
    iConnectAttemptCount++;
    }
 
// -----------------------------------------------------------------------------
// CTransportTls::ConnectTlsL
// -----------------------------------------------------------------------------
//   	
void CTransportTls::ConnectTlsL()
    {
    if ( !iSecureSocket )
        {
        _LIT( KTLS1,"TLS1.0" );
    	iSecureSocket = CSecureSocket::NewL( iSocket, KTLS1 );
        }
	
	CheckCipherSuiteAvailabilityL();
	
	// Don't show any pop-up queries if matching certificate is not found
	// while establishing the secure connection.
	User::LeaveIfError( 
	    iSecureSocket->SetDialogMode( EDialogModeUnattended ) );
	

	iSocketContainer->SetSecureSocket( iSecureSocket );
	
	iReceiver->Connect( iRemoteAddr );
    }
 
// -----------------------------------------------------------------------------
// CTransportTls::Retry
// -----------------------------------------------------------------------------
//     
TBool CTransportTls::Retry() const
    {
    return ( iConnectAttemptCount < KMaxConnectAttempts );
    }

// -----------------------------------------------------------------------------
// CTransportTls::ResetL
// -----------------------------------------------------------------------------
//   
void CTransportTls::ResetL()
    {
    iSocketContainer->SetSecureSocket( 0 );
    iSecureSocket->FlushSessionCache();
    }
    
// -----------------------------------------------------------------------------
// CTransportTls::Destroy
// -----------------------------------------------------------------------------
//   
void CTransportTls::Destroy()
    {
    Remove();
    }

// -----------------------------------------------------------------------------
// CTransportTls::GetSender
// -----------------------------------------------------------------------------
//   
CSender& CTransportTls::GetSender()
    {
    return *iSender;
    }

// -----------------------------------------------------------------------------
// CTransportTls::StartTimerL
// -----------------------------------------------------------------------------
//     
void CTransportTls::StartTimerL( TInt aTimeout )
    {
    iTimer.Stop( iMyTimerId );
	iMyTimerId = iTimer.StartL( this, aTimeout );
    }
    
// -----------------------------------------------------------------------------
// CTransportTls::StopTimer
// -----------------------------------------------------------------------------
//
void CTransportTls::StopTimer()
    {
    iTimer.Stop( iMyTimerId );
    }
       
// -----------------------------------------------------------------------------
// CTransportTls::TimerExpiredL
// -----------------------------------------------------------------------------
//
void CTransportTls::TimerExpiredL(
    TTimerId /*aTimerId*/, 
    TAny* /*aTimerParam*/ )
	{
	if ( IsReservedTransport() || IsPersistent() )
	    {
	    iSender->CancelAllRequests( KErrSIPTransportFailure );
	    }
	else
	    {
    	Remove();
	    }
	}
	
// -----------------------------------------------------------------------------
// CTransportTls::InitializeStatesL
// -----------------------------------------------------------------------------
//
void CTransportTls::InitializeStatesL()
    {
    iStates.AppendL( TTlsTransTcpConnecting( *this ),
					 sizeof( TTlsTransTcpConnecting ) );
					 
	iStates.AppendL( TTlsTransTlsConnecting( *this ),
					 sizeof( TTlsTransTlsConnecting ) );
					 
	iStates.AppendL( TTlsTransTlsConnected( *this, iTimerValue ),
					 sizeof( TTlsTransTlsConnected ) );
					 
    ChangeStateL( TTlsTransStateBase::ETcpConnecting );
    }

// -----------------------------------------------------------------------------
// CTransportTls::CurrentState
// -----------------------------------------------------------------------------
//   
TTlsTransStateBase& CTransportTls::CurrentState()
    {
    return iStates.At( iCurrentState );
    }
    
// -----------------------------------------------------------------------------
// CTransportTls::CheckCipherSuiteAvailabilityL
// -----------------------------------------------------------------------------
//
void CTransportTls::CheckCipherSuiteAvailabilityL()
    {
    __ASSERT_DEBUG( iSecureSocket,
        User::Panic(
            _L("CTransportTls::CheckCipherSuiteAvailabilityL iSecureSocket=0"), 
            KErrNotReady ) );
            
    const TInt KCipherSuitesDesLen = 64;
    TBuf8< KCipherSuitesDesLen > ciphers;
    User::LeaveIfError( iSecureSocket->AvailableCipherSuites( ciphers ) );  
    
    // Each cipher should consist from two bytes
    TInt len( ciphers.Length() );
    const TInt KCipherSuiteInBytes = 2;
    __ASSERT_ALWAYS( !( len % KCipherSuiteInBytes ), 
                     User::Leave( KErrGeneral ) );
    
    // At least this cipher suite should be supported 
    // (value defined in RFC 3268)
    const TUint TLS_RSA_WITH_AES_128_CBC_SHA_Value = 0x2F;
    
    TBool found( EFalse );
    for ( TInt i = 0; i < len && !found; i += KCipherSuiteInBytes )
        {
        TUint cipherVal = BigEndian::Get16( ciphers.Mid( i ).Ptr() );
        found = ( cipherVal == TLS_RSA_WITH_AES_128_CBC_SHA_Value );
        }
        
    __ASSERT_ALWAYS( found, User::Leave( KErrNotSupported ) );    
    }
      
// -----------------------------------------------------------------------------
// CTransportTls::SetDomainName
// -----------------------------------------------------------------------------
void CTransportTls::SetDomainNameL(const TDesC8& aDomainName)
	{
		iDomainName = aDomainName.AllocL();
		__SIP_DES16_LOG("Domain name for TLS Certificate matching in the SIP message is: ", *iDomainName);
	}

// -----------------------------------------------------------------------------
// CTransportTls::HasSecureSocket
// -----------------------------------------------------------------------------
TBool CTransportTls::HasSecureSocket()
    {
         if(iSecureSocket)
                return ETrue;
         else
              return EFalse;        
    }

// -----------------------------------------------------------------------------------
// CTransportTls::MatchCertificate
// Below function implements the logic as per draft draft-ietf-sip-domain-certs-04
// Section 7.1 Finding SIP Identities in a Certificate, 7.2 Comparing SIP Identities
// and 7.3 Client Behavior
// -----------------------------------------------------------------------------------
TBool CTransportTls::MatchCertificateL()
    {
    //If the domain name is NULL, return ETrue. Domain name matching in the certificate
    //is carried out only when Domain name is of type 'Text'
    if(NULL == iDomainName)
        return ETrue;
    
    //Get the server Certificate
    const CX509Certificate* cert = iSecureSocket->ServerCert();
    
    //Implementations MUST check for restrictions on certificate usage declared by any
    //extendedKeyUsage extentions in the certificate.  The SIP Extended Key
    //Usage (EKU) document [draft-ietf-sip-eku-05:Section 4] defines an extendedKeyUsage 
    //for SIP.
    TBool result = ETrue;
    const CX509CertExtension* ext = cert->Extension(KExtendedKeyUsage);
    if (ext)
        {
        result = EFalse;
        CX509ExtendedKeyUsageExt* extendedKeyUsage = CX509ExtendedKeyUsageExt::NewLC(ext->Data());
        const CArrayPtrFlat<HBufC>& usages = extendedKeyUsage->KeyUsages();
        
        //Check for extended key usages
        for (TInt k = 0; k < usages.Count(); k++)
            {
            if (usages[k]->Compare(KServerAuthOID) == 0 || usages[k]->Compare(KClientAuthOID) == 0 ||
                usages[k]->Compare(KSipDomainOID)== 0 || usages[k]->Compare(KAnyExtendedKeyUsage) == 0)
                {
                result = ETrue;
                break;
                }
            else
                result = EFalse;
            }
        CleanupStack::PopAndDestroy();
        }
    
    //If the result is EFalse, it means certificate is not acceptable for use as a
    //SIP certificate, otherwise Certificate can be used as SIP Certificate so proceed
    //with next steps for domain matching
    if (!result)
        return EFalse;
    
    //Examine each value in the SubjectAltName field. The SubjectAltName field
    //can be absent or can contain one or more values. Each value in the SubjectAltName
    //has a type, the only types acceptable for SIP domain are URI and DNS.
    //The values MUST be compared as DNS names, which means that the
    //comparison is case insensitive as specified by RFC 4343    
    const CX509CertExtension* iExt = cert->Extension(KSubjectAltName);
    TInt domainlength = iDomainName->Length();
    TBool isMatch = EFalse;
    if( iExt )
       {
       CX509AltNameExt* iSubjectAltName = CX509AltNameExt::NewLC(iExt->Data());    
       const CArrayPtrFlat<CX509GeneralName>& gNs = iSubjectAltName->AltName();
       const TInt count = gNs.Count(); 

       //Go in the loop and get the general name to find the type.
       //Only value allowed is URI type and DNS type. DNS type only allowed
       //if any URI type does not have scheme sip.
       TASN1DecIA5String encStr;
       TInt pos = 0;
       HBufC* suri = NULL;
       HBufC8* buf = NULL;
       CURIContainer *uricontainer = NULL;
       for( TInt i = 0; i < count; i++ )
           {
           //Reset the position
           pos = 0;
           const CX509GeneralName* gN = gNs.At(i);
           if(EX509URI == gN->Tag())
               {
               // If the scheme of the URI value is "sip", and the URI value
               // that contains a userpart (there is an '@'), the value MUST NOT
               // be accpeted as a SIP domain identity (a value with a userpart
               // identifies an individual user, not a domain).
               // If the scheme of the URI value is "sip", and there is no              
               // userinfo component in the URI (there is no '@'), then the              
               // hostpart MUST be accepted as a SIP domain identity.
               suri = encStr.DecodeDERL(gN->Data(), pos);
               CleanupStack::PushL(suri);
               buf = HBufC8::NewL(suri->Length());
               CleanupStack::PushL(buf);
               buf->Des().Copy(*suri);                            
               uricontainer  = CURIContainer::DecodeL(*buf);               
               CleanupStack::PopAndDestroy(buf);
               CleanupStack::PopAndDestroy(suri);             
               
               //Is the URI scheme sip ?
               if(uricontainer->IsSIPURI() && !uricontainer->SIPURI()->IsSIPSURI())
                  {
                  const TDesC8& user = uricontainer->SIPURI()->User();
                  TInt length = user.Length();                  
                  if(0 == length)
                      {
                      __SIP_DES8_LOG("Data is: ", uricontainer->SIPURI()->HostPort().Host())                      
                      if((uricontainer->SIPURI()->HostPort().Host().CompareF(*iDomainName) == 0) ||
                         (uricontainer->SIPURI()->HostPort().Host().Right(domainlength).CompareF(*iDomainName) == 0))                      
                          {
                          isMatch = ETrue;
                          break;
                        }
                      }
                  }
                  delete uricontainer;
                  uricontainer = NULL;
               }//end if
           } //end for
       
       //Clear the memory created in case isMatch has turned True
       if(uricontainer)
           {
           delete uricontainer;
           uricontainer = NULL;
           CleanupStack::PopAndDestroy(iSubjectAltName);
           iSubjectAltName = NULL;
           return isMatch;
           }
       
       //There are no parameters with type value URI and scheme SIP
       //Check for domain names in type DNS
       for(TInt i = 0; i < count; i++)
           {
           // Match with domain name system identifier as a SIP domain identity
           // if and only if no other identity is found that matches the "sip"
           // URI type described
           const CX509GeneralName* gN = gNs.At(i);
           pos = 0;
           if(EX509DNSName == gN->Tag())
               {
               suri = encStr.DecodeDERL(gN->Data(), pos);
               CleanupStack::PushL(suri);
               buf = HBufC8::NewL(suri->Length());               
               buf->Des().Copy(*suri);
               CleanupStack::PopAndDestroy(suri);
               __SIP_DES8_LOG("Domain Name in type DNS is: ", *buf)               
               if((buf->CompareF(*iDomainName) == 0) ||
                   (buf->Right(domainlength).CompareF(*iDomainName) == 0))
                   {
                   isMatch = ETrue;
                   break;
                   }
               delete buf;
               buf = NULL;
             }
           }//end for
       
       //Clear the memory created in case isMatch has turned True
       if(buf)
           {
           delete buf;
           buf = NULL;
           CleanupStack::PopAndDestroy(iSubjectAltName);           
           iSubjectAltName = NULL;
           return isMatch;
           }
       
       //Destroy the local copy of SubjectAltName in case isMatch is still False
       CleanupStack::PopAndDestroy(iSubjectAltName);           
       iSubjectAltName = NULL;
       } //end if (iExt)    
    
    //If the SubjectAltName is not found then matching happens against
    //CN name in the subject field
    const CX500DistinguishedName& subjectName = cert->SubjectName();
    const TInt count = subjectName.Count();
    for( TInt i=0; i<count; ++i )
        {
        const CX520AttributeTypeAndValue& attribute = subjectName.Element(i);
    
        HBufC* valuePtr = NULL;
        const TDesC& type = attribute.Type();        
        if( type.Compare(KX520CommonName) == 0 )
            {
            valuePtr = attribute.ValueL();            
            HBufC8* buf = HBufC8::NewL(valuePtr->Length());
            buf->Des().Copy(*valuePtr);
            if((buf->CompareF(*iDomainName) == 0) ||
                 (buf->Right(domainlength).CompareF(*iDomainName) == 0))
                {
                isMatch = ETrue;
                }
            delete valuePtr;
            delete buf;
            break;
            }
        else
            isMatch = EFalse;
        }//end for
		return isMatch;
    }//end function

// End of File

