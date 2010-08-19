// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CTransport.cpp
// Part of       : ConnectionMgr
// Version       : SIP/6.0 
//



#include "MHandler.h"
#include "CTransport.h"
#include "MTransportOwner.h"
#include "CErrorHandler.h"
#include "MTransactionFinder.h"
#include "MServerTaFactory.h"
#include "sipmessageparser.h"
#include "MReceiverObserver.h"
#include "SipLogs.h"
#include "sipcontactheader.h"
#include "sipaddress.h"
#include "sipuri.h"
#include "siptoheader.h"
#include "sipfromheader.h"
#include "sipcallidheader.h"
#include "sipcseqheader.h"
#include "sipviaheader.h"
#include "sipheaderbase.h"
#include "sipmessageparser.h"
#include "siphostport.h"
#include "sipcontenttypeheader.h"
#include "sipmaxforwardsheader.h"
#include "CSIPServerResolver.h"
#include "utf.h"
#include "CommonConsts.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "uricontainer.h"
#include "NetworkInfo.h"
#include "COwnerSettingsList.h"
#include "TSIPTransportParams.h"
#include "MSigCompController.h"
#include "CSocketContainer.h"
#include "siperr.h"
#include <sipnatbindingobserver.h>
#include <sipnattraversalcontroller.h>


const TUint KBadRequest = 400;
const TUint KVersionError = 505;

// -----------------------------------------------------------------------------
// CTransport::CTransport
// -----------------------------------------------------------------------------
//
CTransport::CTransport( MTransactionFinder& aFinder, 
					    MServerTaFactory& aTaFactory,
					    MTransportOwner& aTransportOwner,
					    CSIPServerResolver& aServerResolver,
					    MSigCompController& aSigCompHandler,
					    const TSIPTransportParams& aTransportParams,
					    CNetworkInfo& aNetworkInfo,
					    COwnerSettingsList& aSettingsList,
					    CSIPNATTraversalController& aNATTraversal ) :
	iTransportParams( aTransportParams ),
	iSettingsList( aSettingsList ),
	iNATTraversal( aNATTraversal ),
	iTaFinder( &aFinder ),
	iTaFactory( &aTaFactory ),
	iSigCompHandler( &aSigCompHandler ),
	iNetworkInfo( aNetworkInfo )
	{
	iServerResolver = &aServerResolver;
	iTransportOwner = &aTransportOwner;
	}

// -----------------------------------------------------------------------------
// CTransport::~CTransport
// -----------------------------------------------------------------------------
//
CTransport::~CTransport()
	{
	iIDArray.Reset();
	iIDArray.Close();
	iBindingObservers.Reset();
	iBindingObservers.Close();
	}

// -----------------------------------------------------------------------------
// CTransport::RecvL
// -----------------------------------------------------------------------------
//
void CTransport::RecvL( HBufC8* aData,
					    const TInetAddr& aRemoteAddr,
					    TBool aCompressed )

	{
	__ASSERT_DEBUG( aData, User::Panic( _L("Nullpointer"), KErrGeneral ) );

	CleanupStack::PushL( aData );
	CSIPMessageParser* sipparser = CSIPMessageParser::NewLC();	
	HBufC8* dataCopy = aData->Des().AllocL();
	CSIPMessage* message( NULL );
	TInt parserError = sipparser->FromText( aData, &message );
	CleanupStack::PopAndDestroy( sipparser );
    CleanupStack::Pop(aData);
    CleanupStack::PushL(dataCopy);
	
	if ( !message && parserError )
	    {
	    // Show data to NATTraversalController if message couldn't be parsed
	    // and it was received via UDP (might be e.g. a STUN binding response).
	    TBool permissionToUse( EFalse );
	    RSocket* socket = AcquireUdpSocket( 0, aRemoteAddr, permissionToUse );
	    if ( socket )
	        {
	       	__SIP_LOG( "Received unrecognized data, passing to NATTraversal" )
	        
	        // NOTE: NATTraversal does not use the socket for sending
	        // in DataReceived phase.
	        TBool handled( EFalse );
	        iNATTraversal.DataReceivedL( *dataCopy, *socket, handled );
	        
	        // Release immediately
	        UdpSocketReleased( 0, *socket );
	        }
	    CleanupStack::PopAndDestroy( dataCopy );
	    }
	else
	    {
	    CleanupStack::PopAndDestroy( dataCopy );
	    CleanupStack::PushL( message );
	    RecvL( message, aRemoteAddr, parserError, aCompressed );
	    CleanupStack::Pop( message );
	    }
	}

// -----------------------------------------------------------------------------
// CTransport::RecvL
// -----------------------------------------------------------------------------
//
void CTransport::RecvL( CSIPMessage* aMessage,
					    const TInetAddr& aRemoteAddr, 
					    TInt aParserError,
					    TBool aCompressed )
	{
	HandleSigCompAllowDenyL( aRemoteAddr, aParserError, aCompressed ); 
	
	if ( aMessage )
		{
		if ( aParserError == KErrNone && AllMandatoryHeaders( *aMessage ) )
			{
            __SIP_ADDR_LOG( "Incoming message from", aRemoteAddr )
				
			if ( aMessage->IsRequest() )
				{
				if ( HandleServerRequestL( aMessage, aRemoteAddr ) == KErrNone )
					{
					RouteL( aMessage, aRemoteAddr );
					return;
					}
				}
			else
				{
				if ( HandleServerResponseL( aMessage, aRemoteAddr ) )
					{
					RouteL( aMessage, aRemoteAddr );
					return;
					}
				}
			}
		else
			{			
			if ( aMessage->IsRequest() && !IsAck( *aMessage ) )
				{
				SetReceivedIfNeededL( TopViaHeader( aMessage ), aRemoteAddr );
				SendErrorResponseL( KBadRequest,
			                        SipStrConsts::EPhraseBadRequest, 
					                *aMessage,
				                    aRemoteAddr );
				__SIP_LOG( "Invalid SIP Request" )
				}
			else
				{
				__SIP_LOG( "Invalid SIP Response or ACK" )
				}
				
			__SIP_DES8_LOG( "Invalid data: ", aMessage->Content() )
			__SIP_INT_LOG1( "Parser error", aParserError )
			}
		}
	else
		{
		__SIP_INT_LOG1( "Parser error", aParserError )
		}
	delete aMessage;
	}

// -----------------------------------------------------------------------------
// CTransport::RouteL
// -----------------------------------------------------------------------------
//
void CTransport::RouteL( CSIPMessage* aMessage, const TInetAddr& aRemoteAddr )
	{
	TInt leaveValue = KErrNone;
	
	MReceiverObserver* receiver = iTaFinder->Search( *aMessage );
	if ( receiver )
		{
		const TSIPTransportParams& tpParams = receiver->TransportParams();
		
		// Discard the message if receiving is not allowed
		if ( !iSettingsList.IsReceivingAllowed( tpParams, 
		                                        aRemoteAddr, 
		                                        Protocol(), 
		                                        SourcePort() ) )
		    {
		    delete aMessage;
		    return;
		    }
		
		if ( aMessage->IsRequest() )
			{
			TRAP( leaveValue,
				  receiver->ReceiveL( static_cast<CSIPRequest*>( aMessage ) ) );
			}
		else
			{
			HandleResponseNATTraversalL( 
			        static_cast< CSIPResponse& >( *aMessage ), 
			        aRemoteAddr,
			        const_cast<TSIPTransportParams&>(
			            tpParams ).NATBindingObserver() );
			   
			TRAP( leaveValue,
				  receiver->ReceiveL( static_cast<CSIPResponse*>( aMessage ) ) );
			}
		if ( leaveValue != KErrNone )
			{
			delete aMessage;
			receiver->LeaveOccurred( leaveValue );
			}
		}
	else
		{
		if ( aMessage->IsRequest() )
			{
			TTransactionId id;
			// OwnerId cannot be known at this stage, default id will be used
			TSIPTransportParams tpParams( KDefaultOwnerId,
										  iTransportOwner->IapId() );
			// Find out whether some existing SAs should be used also for
			// this new transaction
			TUint32 tpId = iSettingsList.FindTransportId( aRemoteAddr );
			
			tpParams.SetTransportId( tpId );
			
			receiver = iTaFactory->NewTransactionL(
				static_cast<CSIPRequest&>( *aMessage ),
				tpParams,
				id );
			if ( receiver )
				{
				if ( IsConnectedTransport() )
					{
					TResponseRoute route( id, aRemoteAddr );
					iIDArray.AppendL( route );
					}
				TRAP( leaveValue,
					  receiver->ReceiveL( static_cast<CSIPRequest*>( aMessage ) ) );
				if ( leaveValue != KErrNone )
					{
					delete aMessage;
					receiver->LeaveOccurred( leaveValue );
					}
				}
			else
				{
				delete aMessage;
				}
			}
		else
			{
			delete aMessage;
			}
		}
	}

// -----------------------------------------------------------------------------
// CTransport::Send
// -----------------------------------------------------------------------------
//
void CTransport::Send( const TSIPTransportParams& aParams,
                       CSIPResponse& aResponse,
					   const TInetAddr& aAddr,
					   TRequestStatus &aStatus )
	{
	TRAPD( leaveValue,
		   SendToNetL( aParams, aAddr, aResponse, EFalse, Protocol(), aStatus ) );
	if ( leaveValue != KErrNone )
		{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete( stat, leaveValue );
		}
	}

// -----------------------------------------------------------------------------
// CTransport::Send
// -----------------------------------------------------------------------------
//
void CTransport::Send( const TSIPTransportParams& aParams,
                       CSIPResponse& aResponse,
					   TTransactionId aId,
					   TRequestStatus &aStatus )
	{
	TInt leaveValue = KErrNone;
	TInetAddr remoteAddr;
	
	if ( Protocol() == KProtocolInetUdp )
		{
		TRAP( leaveValue, 
			  HandleClientResponseL( aParams, &aResponse, remoteAddr );
			  SendToNetL( aParams, remoteAddr, aId, aResponse, aStatus ) );
		if ( leaveValue != KErrNone )
			{
			TRequestStatus* stat = &aStatus;
			User::RequestComplete( stat, leaveValue );
			}
		}
	else
		{
		TInt pos;
		pos = iIDArray.Count();
		while ( pos > 0 )
			{
			if ( iIDArray[ pos - 1 ].TAId() == aId )
				{
				TRAP( leaveValue,
					  UpdateContactHeadersL( aParams, &aResponse, ETrue );
					  SendToNetL( aParams,
	                              iIDArray[ pos - 1 ].Address(),
								  aResponse,
								  EFalse,
								  0,
								  aStatus ) );
				if ( leaveValue != KErrNone )
					{
					TRequestStatus* stat = &aStatus;
					User::RequestComplete( stat, leaveValue );
					}
				return;
				}
			pos--;
			}
		TRAP( leaveValue,
			  HandleClientResponseL( aParams, &aResponse, remoteAddr );
			  SendToNetL( aParams, remoteAddr, aResponse, EFalse, Protocol(), aStatus ) );
		if ( leaveValue != KErrNone )
			{
			TRequestStatus* stat = &aStatus;
			User::RequestComplete( stat, leaveValue );
			}
		}
	}

// -----------------------------------------------------------------------------
// CTransport::Send
// -----------------------------------------------------------------------------
//
void CTransport::Send( const TSIPTransportParams& aParams,
                       CSIPRequest& aRequest,
                       TBool aForceUDP,
					   const TInetAddr& aAddr,
					   TUint aOrigTransport,
					   TRequestStatus &aStatus,
					   TBool aPublicAddrResolved )
	{
	TInt leaveValue = KErrNone;
	TRAP( leaveValue, 
		  HandleClientRequestL( aParams, aRequest, aPublicAddrResolved );
		  UpdateListenersL( aParams, &aRequest );
		  SendToNetL( aParams, aAddr, aRequest, aForceUDP, aOrigTransport, aStatus ) );
		 
	if ( leaveValue != KErrNone )
		{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete( stat, leaveValue );
		}
	}

// -----------------------------------------------------------------------------
// CTransport::HandleL
// -----------------------------------------------------------------------------
//
TBool CTransport::HandleL(
    const TSIPTransportParams& aParams,
    CSIPMessage* aMessage, 
    TTransactionId aId,
    TBool aIsStrict )
	{
	if ( !aMessage ) 
		{
		return EFalse;
		}
	TInt pos = iIDArray.Count();
	while ( pos > 0 )
		{
		if ( iIDArray[ pos - 1 ].TAId() == aId )	
			{
			return ETrue;
			}
		pos--;
		}

    if ( aIsStrict )
        {
        return EFalse;
        }
        
	TInetAddr remoteAddr;
	HandleClientResponseL( aParams, aMessage, remoteAddr );
	return HandleL( aParams, aMessage, remoteAddr, 0, ETrue );
	}

// -----------------------------------------------------------------------------
// CTransport::HandleL
// -----------------------------------------------------------------------------
//
TBool CTransport::HandleL(
    const TSIPTransportParams& aParams,
    CSIPMessage* aMessage, 
    const TInetAddr& aAddr,
    TUint aLocalPort,
    TBool aIsStrict )
	{
	if ( !aMessage ) 
		{
		return EFalse;
		}
	CSIPViaHeader* viaHeader = TopViaHeader( aMessage );
	if ( viaHeader )
		{
		if ( HandleMessage( aParams, 
		                    viaHeader->Transport(), 
		                    aAddr,
		                    aLocalPort,
		                    aIsStrict ) )
			{
			return ETrue;
			}
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CTransport::Handle
// -----------------------------------------------------------------------------
//
TBool CTransport::Handle( TUint32 aTransportId )
    {
    return ( IsReservedTransport() && 
             iTransportParams.TransportId() == aTransportId );
    }
    
// -----------------------------------------------------------------------------
// CTransport::Match
// -----------------------------------------------------------------------------
//	
TBool CTransport::Match( const TSIPTransportParams& aParams )
    {
	return ( aParams.TransportId() == 0 || Handle( aParams.TransportId() ) );
    }

// -----------------------------------------------------------------------------
// CTransport::HasRemoteHost
// -----------------------------------------------------------------------------
//
TBool CTransport::HasRemoteHost(
    const TSIPTransportParams& aParams,
    const TInetAddr& aRemoteHost,
    RStringF aProtocol )
	{
	return HandleMessage( aParams, aProtocol, aRemoteHost, 0, ETrue );
	}

// -----------------------------------------------------------------------------
// CTransport::Remove
// -----------------------------------------------------------------------------
//
TBool CTransport::Remove( const TSIPTransportParams& aParams )
    {
    // Default transports are not removed when owner closes its connection.
    // Also removal of persistent and reserved transport are handled separately.
    return ( !IsDefault() &&
             iTransportParams.OwnerId() == aParams.OwnerId() &&
             !IsPersistent() &&
             !IsReservedTransport() );
    }

// -----------------------------------------------------------------------------
// CTransport::Remove
// -----------------------------------------------------------------------------
//
TBool CTransport::Remove( MSIPNATBindingObserver& aSIPNATBindingObserver )
    {
    return ( !IsDefault() && RemoveBindingObserver( aSIPNATBindingObserver ) );
    }

// -----------------------------------------------------------------------------
// CTransport::Shutdown
// -----------------------------------------------------------------------------
//    
TBool CTransport::Shutdown( 
    TUint32 /*aTransportId*/,
    MSIPTransportRemovalObserver* /*aRemovalObserver*/ )
    {
    return EFalse;
    }
    
// -----------------------------------------------------------------------------
// CTransport::IsReservedTransport
// -----------------------------------------------------------------------------
//	
TBool CTransport::IsReservedTransport() const
    {
    return ( iTransportParams.TransportId() > 0 );
    }

// -----------------------------------------------------------------------------
// CTransport::AcquireUdpSocketL
// -----------------------------------------------------------------------------
//    
RSocket* CTransport::AcquireUdpSocket( 
    MSocketUsagePermissionObserver* /*aObserver*/,
    const TInetAddr& /*aRemoteAddr*/,
    TBool& /*aPermissionToUse*/ )
    {
    return 0;
    }

// -----------------------------------------------------------------------------
// CTransport::UdpSocketReleased
// -----------------------------------------------------------------------------
//                                        
void CTransport::UdpSocketReleased( 
    MSocketUsagePermissionObserver* /*aObserver*/,
    RSocket& /*aSocket*/ )
    {
    }

// -----------------------------------------------------------------------------
// CTransport::PersistentRemoteAddr
// -----------------------------------------------------------------------------
// 
TInetAddr* CTransport::PersistentRemoteAddr( 
    MSIPNATBindingObserver* /*aBindingObserver*/ )
    {
    return 0;
    }
    
// -----------------------------------------------------------------------------
// CTransport::HandleClientRequestL
// -----------------------------------------------------------------------------
//
void CTransport::HandleClientRequestL(
    const TSIPTransportParams& aParams,
    CSIPRequest& aRequest,
    TBool aPublicAddrResolved )
	{
	__ASSERT_DEBUG( aRequest.HasHeader(
		SIPStrings::StringF( SipStrConsts::EMaxForwardsHeader ) ),
		User::Panic( _L( "no MaxForwards header" ), KErrArgument ) );
	
	__ASSERT_DEBUG( AllMandatoryHeaders( aRequest ),
		User::Panic( _L( "Mandatory header is missing" ), KErrArgument ) );
		
    TUint32 transportId = aParams.TransportId();

   	CSIPViaHeader* viaHeader = TopViaHeader( &aRequest );
    if ( viaHeader )
    	{
    	// Currently both (aPublicAddrResolved==ETrue) and (receivePort>0)
    	// never happen at the same time.
    	if ( !aPublicAddrResolved )
    		{
    		TUint receivePort =
    			iSettingsList.SavedLocalReceivePort( transportId );
    		// If host is already filled (e.g. ConnectionMgr put public address
    		// to Via, sends request, gets 401, TU sends an updated request, Via
    		// has the public addr) don't put the private address to Via.
    		// Even if host is already set, set port if receivePort exists.
    		if ( viaHeader->SentByHostPort().Host().CompareF(
			     SIPStrings::StringF( SipStrConsts::ELocalHost ).DesC() ) == 0 )
        		{
		    	HBufC8* localaddress = LocalAddressL();
		    	CleanupStack::PushL( localaddress );
		    	viaHeader->SentByHostPort().SetHostL( *localaddress );
				CleanupStack::PopAndDestroy( localaddress );

				// Sent-by is now set for the first time, must set the port too
				if ( receivePort == 0 )
		            {
                    //If the Transport in the outgoing request is TLS and port value = 0
                    //provide 5061 for TLS and 5060 for UDP/TCP
                    if(SIPStrings::StringF(SipStrConsts::ETLS) == viaHeader->Transport())
                        receivePort = KDefaultSipsPort;
                    else
                        receivePort = KDefaultSipPort;
		            }
	        	}
	        if ( receivePort )
	            {
	            viaHeader->SentByHostPort().SetPort( receivePort );
	            }
    		}

        // Add empty rport param
       	RStringF rport = SIPStrings::Pool().OpenFStringL( KParamrport );
        CleanupClosePushL( rport );
        viaHeader->SetParamL( rport );
        CleanupStack::PopAndDestroy(); // rport
        }

    UpdateContactHeadersL( aParams, &aRequest, !aPublicAddrResolved );
	
	// Check if request is sent integrity protected 
	// (i.e. IPSec SA exists, ignore TLS)
	iNetworkInfo.HandleRequestL( aRequest,
	        iSettingsList.HasSavedTransportInfo( transportId ) &&
	        !iSettingsList.HasSavedTLSTransportInfo( transportId ) );		
	}

// -----------------------------------------------------------------------------
// CTransport::GetAddrFromResponseL
// -----------------------------------------------------------------------------
//
void CTransport::GetAddrFromResponseL(
    const TSIPTransportParams& aParams,
    CSIPMessage& aMessage, 
    TInetAddr& aAddr )
	{
	HandleClientResponseL( aParams, &aMessage, aAddr );
	}

// -----------------------------------------------------------------------------
// CTransport::HandleClientResponseL
// -----------------------------------------------------------------------------
//
void CTransport::HandleClientResponseL(
    const TSIPTransportParams& aParams,
    CSIPMessage* aMessage, 
    TInetAddr& aAddr )
	{	
	CSIPViaHeader* viaHeader = TopViaHeader( aMessage );
	if ( viaHeader )
		{
		TBool portSet( EFalse );
		
		if ( viaHeader->HasParam( SIPStrings::StringF( SipStrConsts::EMaddr ) ) )
			{
			ConvertToInetAddrL( 
			    viaHeader->ParamValue(
			        SIPStrings::StringF( SipStrConsts::EMaddr ) ).DesC(), aAddr );
			
			if ( !viaHeader->HasParam( 
			        SIPStrings::StringF( SipStrConsts::ETtl ) ) )
				{
				viaHeader->SetTtlParamL(1);
				}
			}
		else if ( viaHeader->HasParam(
		    SIPStrings::StringF( SipStrConsts::EReceived ) ) )
			{
			ConvertToInetAddrL( 
			    viaHeader->ParamValue(
			        SIPStrings::StringF( SipStrConsts::EReceived ) ).DesC(),
			        aAddr );
			
			portSet = HandleClientResponseRPortL( *viaHeader, aAddr );
			}
		else
			{
			ConvertToInetAddrL( viaHeader->SentByHostPort().Host(),
			                    aAddr );
			}
	    
	    // Port could be already set if received and rport values existed.
	    if ( !portSet )
	        {
    		if ( viaHeader->SentByHostPort().HasPort() )
    			{
    			aAddr.SetPort( viaHeader->SentByHostPort().Port() );
    			}
    		else
    			{
    			aAddr.SetPort( KDefaultSipPort );
    			}
	        }
		}
		
	UpdateContactHeadersL( aParams, aMessage, ETrue );
	}

// -----------------------------------------------------------------------------
// CTransport::HandleServerResponseL
// -----------------------------------------------------------------------------
//
TBool CTransport::HandleServerResponseL( CSIPMessage* aMessage,
										 const TInetAddr& aAddr )
	{
	__ASSERT_DEBUG( aMessage, User::Panic( _L("Nullpointer"), KErrGeneral ) );
	
	if ( !CorrectProtocolInVia( *aMessage ) )
		{
		return EFalse;
		}

	if ( !OnlyOneViaHeader( *aMessage ) )
		{
		return EFalse;
		}
	if ( Protocol() == KProtocolInetUdp )
		{
		if (CheckAndUpdateContentLengthL( aMessage, aAddr ) != KErrNone )
			{
			return EFalse;
			}
		}
	
	if ( IsConnectedTransport() &&
		 !aMessage->HasAnnouncedContentLength() )
		{
		return EFalse;
		}

	if ( !SupportedSIPVersion( *aMessage ) )
		{
		return EFalse;
		}

	return ETrue;
	}

// -----------------------------------------------------------------------------
// CTransport::HandleServerRequestL
// -----------------------------------------------------------------------------
//
TInt CTransport::HandleServerRequestL( CSIPMessage* aMessage,
									   const TInetAddr& aAddr )
	{
	__ASSERT_ALWAYS( aMessage, User::Leave( KErrArgument ) );
	
	SetReceivedIfNeededL( TopViaHeader( aMessage ), aAddr );

	if ( !SupportedSIPVersion( *aMessage ) || 
	     !CorrectProtocolInVia( *aMessage ) )
		{
		SendErrorResponseL( KVersionError,
			                SipStrConsts::EPhraseVersionNotSupported,
					        *aMessage,
					        aAddr );
		return KErrArgument;
		}

	if ( ( IsConnectedTransport() && 
	     !aMessage->HasAnnouncedContentLength() ) ||
	     !ContentTypeOk( *aMessage ) ||
	     !MethodIsSameInCSecAndReqLine( static_cast<CSIPRequest&>( *aMessage ) ) )
		{
		SendErrorResponseL( KBadRequest,
					        SipStrConsts::EPhraseBadRequest,
					        *aMessage,
				          	aAddr );
		return KErrArgument;
		}
		
	if ( Protocol() == KProtocolInetUdp )
		{
		if ( CheckAndUpdateContentLengthL( aMessage, aAddr ) != KErrNone )
			{
			return KErrGeneral;
			}
		}

	CSIPViaHeader* viaHeader = TopViaHeader( aMessage );
	if ( viaHeader )
	{
		TBool isPortPresent = viaHeader->SentByHostPort().HasPort();
		if (isPortPresent)
		{
			TUint sentByPort = viaHeader->SentByHostPort().Port();
			if ((sentByPort > KMaxPortValue) || (sentByPort < KMinPortValue))
			{
		    	__SIP_LOG( "Topmost Via-header has invalid port value, dropping request" )
		    	return KErrArgument;
			}
		}
	}

	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CTransport::AllMandatoryHeaders
// -----------------------------------------------------------------------------
//
TBool CTransport::AllMandatoryHeaders( CSIPMessage& aMessage )
	{
	return aMessage.HasHeader( SIPStrings::StringF( SipStrConsts::EViaHeader ) )
		&&
		aMessage.HasHeader( SIPStrings::StringF( SipStrConsts::EToHeader ) )
		&&
		aMessage.HasHeader( SIPStrings::StringF( SipStrConsts::EFromHeader ) )
		&&
		aMessage.HasHeader( SIPStrings::StringF( SipStrConsts::ECallIDHeader ) )
		&&
		aMessage.HasHeader( SIPStrings::StringF( SipStrConsts::ECSeqHeader ) );
	}

// -----------------------------------------------------------------------------
// CTransport::SupportedSIPVersion
// -----------------------------------------------------------------------------
//
TBool CTransport::SupportedSIPVersion( CSIPMessage& aMessage )
	{
	TSglQueIter<CSIPHeaderBase> iter = 
	    aMessage.Headers( SIPStrings::StringF( SipStrConsts::EViaHeader ) );
	CSIPHeaderBase* header = 0;
	iter.SetToFirst();
	
	while ( ( header = iter++ ) != 0 )
		{
		CSIPViaHeader* via = static_cast<CSIPViaHeader*>( header );
		if ( !( via->SentProtocolVersion() ==
		        SIPStrings::StringF( SipStrConsts::EDefaultVersionNumber ) ) )
			{
			return EFalse;
			}
		}

	return ( aMessage.SIPVersion() == 
	         SIPStrings::StringF( SipStrConsts::EDefaultProtocolVersion ) );
	}

// -----------------------------------------------------------------------------
// CTransport::CorrectProtocolInVia
// -----------------------------------------------------------------------------
//
TBool CTransport::CorrectProtocolInVia( CSIPMessage& aMessage )
	{
	TSglQueIter<CSIPHeaderBase> iter = 
	    aMessage.Headers( SIPStrings::StringF( SipStrConsts::EViaHeader ) );
	CSIPHeaderBase* header = 0;
	iter.SetToFirst();
	
	while ( ( header = iter++ ) != 0 )
		{
		CSIPViaHeader* via = static_cast<CSIPViaHeader*>( header );
		if ( via->SentProtocolName() !=
			 SIPStrings::StringF( SipStrConsts::ESIP ) )
			{
			return EFalse;
			}
		}
	
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CTransport::ContentTypeOk
// -----------------------------------------------------------------------------
//
TBool CTransport::ContentTypeOk( CSIPMessage& aMessage )
	{
	if ( aMessage.Content().Length() > 0 )
		{
		if ( !aMessage.HasHeader(
		        SIPStrings::StringF( SipStrConsts::EContentTypeHeader ) ) )
			{
			return EFalse;
			}
		}
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CTransport::SetReceivedIfNeededL
// aAddr has to be passed by value
// -----------------------------------------------------------------------------
//
void CTransport::SetReceivedIfNeededL( CSIPViaHeader* aViaHeader,
									   TInetAddr aAddr )
	{
	if ( aViaHeader )
		{
		if ( aAddr.IsV4Mapped() )
			{
			aAddr.ConvertToV4();
			}
		aAddr.SetScope( 0 );
		
		const TInt KMaxAddrLen = 64;
		TBuf<KMaxAddrLen> address;
		aAddr.Output( address );
		TBuf8<KMaxAddrLen> received;
		CnvUtfConverter::ConvertFromUnicodeToUtf8( received, address );
			            
	    RStringF rport = SIPStrings::Pool().OpenFStringL( KParamrport );
	    CleanupClosePushL( rport );
	
	    // If rport param exists but value has not been set, 
	    // set it to be the source port
	    TBool rportSet( EFalse );
	    if ( aViaHeader->HasParam( rport ) && !HasValue( *aViaHeader, rport ) )
	        {
	        const TInt KMaxTUintAsDesLen = 10;
	        TBuf8<KMaxTUintAsDesLen> sourcePortDes;
	        sourcePortDes.AppendNum( static_cast<TInt64>( aAddr.Port() ) );
	        
	        RStringF rportVal = SIPStrings::Pool().OpenFStringL( sourcePortDes );
	    	CleanupClosePushL( rportVal );
	        aViaHeader->SetParamL( rport, rportVal );
	        CleanupStack::PopAndDestroy(); // rportVal
	        rportSet = ETrue;
	        }
	        
	    CleanupStack::PopAndDestroy(); // rport
	    
	    // Set received param even if the address is same as in sent-by if
	    // rport value was set   
	    if ( aViaHeader->SentByHostPort().Host().CompareF( received ) != 0 ||
	         rportSet )
			{
			RStringF recv = SIPStrings::Pool().OpenFStringL( received );
			CleanupClosePushL( recv );
			aViaHeader->SetParamL(
			            SIPStrings::StringF(SipStrConsts::EReceived), recv );
			CleanupStack::PopAndDestroy(); //recv
			}
		}
	}

// -----------------------------------------------------------------------------
// CTransport::CheckAndUpdateContentLengthL
// -----------------------------------------------------------------------------
//
TInt CTransport::CheckAndUpdateContentLengthL( CSIPMessage* aMessage,
											   const TInetAddr& aAddr )
	{
	if ( aMessage->HasAnnouncedContentLength() )
		{
		if ( static_cast<TUint>( aMessage->Content().Length() ) == 
			 aMessage->AnnouncedContentLength() )
			{
			return KErrNone;
			}

		if ( static_cast<TUint>( aMessage->Content().Length() ) > 
			 aMessage->AnnouncedContentLength() )
			{
			HBufC8* content = aMessage->Content().AllocL();
			content->Des().Delete( aMessage->AnnouncedContentLength(),
								   aMessage->Content().Length() - 
								   aMessage->AnnouncedContentLength() );

			aMessage->SetContent( content );
			return KErrNone;
			}

		if ( aMessage->IsRequest() )
			{
			SendErrorResponseL( KBadRequest, 
			                    SipStrConsts::EPhraseBadRequest,
            			        *aMessage, 
            			        aAddr );
			}
		return KErrGeneral;
		}
	else
		{
		return KErrNone;
		}
	}

// -----------------------------------------------------------------------------
// CTransport::TopViaHeader
// -----------------------------------------------------------------------------
//
CSIPViaHeader* CTransport::TopViaHeader( CSIPMessage* aMessage )
	{
	if ( aMessage->HasHeader( SIPStrings::StringF(SipStrConsts::EViaHeader) ) )
		{
		TSglQueIter<CSIPHeaderBase> iter =
			aMessage->Headers( SIPStrings::StringF(SipStrConsts::EViaHeader) );
		CSIPHeaderBase* header = iter;
		return static_cast<CSIPViaHeader*>( header );
		}

	return 0;
	}

// -----------------------------------------------------------------------------
// CTransport::UpdateListenersL
// -----------------------------------------------------------------------------
//
void CTransport::UpdateListenersL(
    const TSIPTransportParams& aParams, 
    CSIPMessage* aMessage )
	{	
	if ( aMessage->IsRequest() )
		{
		if ( static_cast<CSIPRequest*>(aMessage)->Method() != 
		     SIPStrings::StringF(SipStrConsts::ERegister) )
			{
			return;
			}
		}
	if ( aMessage->HasHeader( 
	        SIPStrings::StringF(SipStrConsts::EContactHeader) ) )
		{
		TSglQueIter<CSIPHeaderBase> iter = 
		    aMessage->Headers( 
		        SIPStrings::StringF(SipStrConsts::EContactHeader) );
		CSIPHeaderBase* header = 0;
		CURIContainer* uriContainer = 0;
		while ( ( header = iter++ ) != 0 )
			{
			CSIPContactHeader* contact =
				static_cast<CSIPContactHeader*>( header );
				
		    CSIPAddress* sipAddr = contact->SIPAddress();
			if ( sipAddr )
			    {
			    uriContainer = &sipAddr->URI();
			
    			if ( uriContainer->IsSIPURI() )
    				{
    				CSIPURI* sipUri = uriContainer->SIPURI();
    				HBufC8* localaddress = LocalAddressL();
    				CleanupStack::PushL( localaddress );
    				TUint port = sipUri->HostPort().Port();
    				
    				if ( sipUri->HostPort().Host().CompareF( *localaddress ) == 0 &&
    				     sipUri->HostPort().HasPort() )
    					{
    					if ( sipUri->HasParam(
    					     SIPStrings::StringF(SipStrConsts::ETransport) ) )
    					    {
    					    RStringF transport = 
    					        sipUri->ParamValue(
    					            SIPStrings::StringF(SipStrConsts::ETransport) );
    					         
    					    if ( transport == SIPStrings::StringF(SipStrConsts::ETCP) )
    					        {
    				            iSettingsList.ReservePrivatePortL( aParams, port );
    					        iTransportOwner->AddListenerL( aParams,
    					                                       KProtocolInetTcp, 
    					                                       port );
    					        CleanupStack::PopAndDestroy( localaddress );
    					        return;
    					        }
    					    
    					    if ( transport == SIPStrings::StringF(SipStrConsts::ETLS) )
    					        {
    					        // No support for incoming TLS connections
    					        CleanupStack::PopAndDestroy( localaddress );
    					        return;
    					        }
    					    }
    					    
    				    iSettingsList.ReservePrivatePortL( aParams, port );     
    				    iTransportOwner->AddListenerL( aParams,
    				                                   KProtocolInetUdp, 
    				                                   port );	
    					}
    				CleanupStack::PopAndDestroy( localaddress );
    				}
			    }
			}
		}
	}

// -----------------------------------------------------------------------------
// CTransport::OnlyOneViaHeader
// -----------------------------------------------------------------------------
//
TBool CTransport::OnlyOneViaHeader( CSIPMessage& aMessage )
	{
	return aMessage.HeaderCount(
	                    SIPStrings::StringF(SipStrConsts::EViaHeader) ) == 1;	
	}

// -----------------------------------------------------------------------------
// CTransport::MethodIsSameInCSecAndReqLine
// -----------------------------------------------------------------------------
//
TBool CTransport::MethodIsSameInCSecAndReqLine( CSIPRequest& aMessage )
	{
	if ( aMessage.HasHeader( SIPStrings::StringF(SipStrConsts::ECSeqHeader) ) )
		{
		TSglQueIter<CSIPHeaderBase> iter =
			aMessage.Headers(SIPStrings::StringF(SipStrConsts::ECSeqHeader) );
		CSIPHeaderBase* header = iter;
		CSIPCSeqHeader* cseq = static_cast<CSIPCSeqHeader*>( header );
		if ( cseq->Method() == aMessage.Method() )
			{
			return ETrue;
			}
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CTransport::SetPersistencyL
// -----------------------------------------------------------------------------
//	
void CTransport::SetPersistencyL( 
    const TInetAddr& /*aRemoteAddr*/, 
    TBool aIsPersistent,
    MSIPNATBindingObserver* aBindingObserver )
    {
    if ( !aBindingObserver )
        {
        return;
        }
        
    if ( aIsPersistent )
        {
        AddBindingObserverL( *aBindingObserver );
        }
    else
        {
        RemoveBindingObserver( *aBindingObserver );
        }
    }

// -----------------------------------------------------------------------------
// CTransport::IsPersistent
// -----------------------------------------------------------------------------
//
TBool CTransport::IsPersistent() const
    {
    return ( iBindingObservers.Count() > 0 );
    }

// -----------------------------------------------------------------------------
// CTransport::IsDefault
// -----------------------------------------------------------------------------
//
TBool CTransport::IsDefault() const
    {
    return ( iTransportParams.OwnerId() == KDefaultOwnerId );
    }

// -----------------------------------------------------------------------------
// CTransport::RemoveResponseRoute
// -----------------------------------------------------------------------------
//
void CTransport::RemoveResponseRoute( const TInetAddr& aRemoteAddr )
	{
	TInt pos = iIDArray.Count();
	while ( pos > 0 )
		{
		if ( iIDArray[ pos - 1 ].Address().CmpAddr( aRemoteAddr ) )
			{
			iIDArray.Remove( pos - 1 );
			}
		pos--;
		}
	}
	
// -----------------------------------------------------------------------------
// CTransport::RemoveResponseRoute
// -----------------------------------------------------------------------------
//
void CTransport::RemoveResponseRoute( TTransactionId aId )
	{
	TInt pos = iIDArray.Count();
	while ( pos > 0 )
		{
		if ( iIDArray[ pos - 1 ].TAId() == aId )
			{
			iIDArray.Remove( pos - 1 );
			}
		pos--;
		}
	iIDArray.Compress();
	}

// -----------------------------------------------------------------------------
// CTransport::SendToNetL
// -----------------------------------------------------------------------------
//
void CTransport::SendToNetL( const TSIPTransportParams& /*aParams*/,
                             const TInetAddr& /*aAddress*/,
							 CSIPMessage& /*aMessage*/, 
							 TBool /*aForceUDP*/,
							 TUint /*aOrigTransport*/,
							 TRequestStatus& /*aStatus*/ )
	{	
	__ASSERT_DEBUG(EFalse,
		User::Panic(_L("CTransport::SendToNetL is called"),
		KErrGeneral));
	}

// -----------------------------------------------------------------------------
// CTransport::SendToNetL
// -----------------------------------------------------------------------------
//
void CTransport::SendToNetL( const TSIPTransportParams& /*aParams*/,
                             const TInetAddr& /*aAddress*/,
							 TTransactionId /*aId*/, 
							 CSIPMessage& /*aMessage*/,
						 	 TRequestStatus& /*aStatus*/ )
	{
	__ASSERT_DEBUG(EFalse,
		User::Panic(_L("CTransport::SendToNetL is called"),
		KErrGeneral));
	}

// -----------------------------------------------------------------------------
// CTransport::SendToNetL
// -----------------------------------------------------------------------------
//
void CTransport::SendToNetL( const TSIPTransportParams& /*aParams*/,
                             const TInetAddr& /*aAddress*/,
					   		 CSIPMessage& /*aMessage*/,
					   		 TRequestStatus& /*aStatus*/ )
	{
	}

// -----------------------------------------------------------------------------
// CTransport::IsAck
// -----------------------------------------------------------------------------
//
TBool CTransport::IsAck( CSIPMessage& aMessage )
	{
	if ( aMessage.IsRequest() )
		{
		if ( static_cast<CSIPRequest*>( &aMessage )->Method() == 
		     SIPStrings::StringF(SipStrConsts::EAck) )
			{
			return ETrue;
			}
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CTransport::HandleSigCompAllowDenyL
// -----------------------------------------------------------------------------
//	
void CTransport::HandleSigCompAllowDenyL(
    const TInetAddr& aRemoteAddr, 
    TInt aParserError,
    TBool aCompressed )
    {
    if ( !iSigCompHandler->IsSupported() || !aCompressed )
        {
        return;
        }
        
	if ( aParserError == KErrNone )
	    {
	    iSigCompHandler->AllowL( aRemoteAddr, 
	                             iTransportOwner->IapId() );
	    }
	else
	    {
	    iSigCompHandler->Deny();
	    }
    }

// -----------------------------------------------------------------------------
// CTransport::CancelResponseSend
// -----------------------------------------------------------------------------
//
TBool CTransport::CancelResponseSend( 
    TTransactionId aId, 
    TBool aCancelAlso2xxResponses )
	{
	// Remove response route information only after all responses related
	// to the transaction are canceled.
	if ( aCancelAlso2xxResponses )
	    {
	    RemoveResponseRoute( aId );
	    }
	
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CTransport::ProtocolType
// -----------------------------------------------------------------------------
//
TUint CTransport::ProtocolType()
	{
	return Protocol();
	}

// -----------------------------------------------------------------------------
// CTransport::LocalAddressL
// -----------------------------------------------------------------------------
//
HBufC8* CTransport::LocalAddressL()
	{
	TInetAddr address;
	iTransportOwner->GetLocalAddrL( address, NULL );
    return ConvertInetAddrToTextL( address );
	}
    
// -----------------------------------------------------------------------------
// CTransport::HandleMixedAddressFamilysL
// -----------------------------------------------------------------------------
// 
void CTransport::HandleMixedAddressFamilysL( 
    CSIPMessage& aMessage, 
	const TInetAddr& aRemoteAddr )
    {
    // Possibly update contact header if message is request
    if ( aMessage.IsRequest() )
        {
        if ( aMessage.HasHeader( 
                        SIPStrings::StringF(SipStrConsts::EContactHeader) ) )
    		{
            TSglQueIter<CSIPHeaderBase> iter = 
                aMessage.Headers( 
                            SIPStrings::StringF(SipStrConsts::EContactHeader) );
    		CSIPHeaderBase* header = 0;
    		while ( ( header = iter++ ) != 0 )
    			{
    			CSIPContactHeader* contact = 
    			        static_cast<CSIPContactHeader*>( header );
    			CSIPAddress* sipAddr = contact->SIPAddress();
    			
    			if ( sipAddr )
    			    {
    			    CURIContainer& uriContainer = sipAddr->URI();
    			    if ( uriContainer.IsSIPURI() )
    			        {
    			        ChangeHostAddressFamilyIfNeededL( 
    			                uriContainer.SIPURI()->HostPort(), 
    			                aRemoteAddr );
    			        }
    			    }
    			}
    		}
        }
        
    // Possibly update via header (also if message is response)
    CSIPViaHeader* viaHeader = TopViaHeader( &aMessage );
    if ( viaHeader )
        {
        ChangeHostAddressFamilyIfNeededL( viaHeader->SentByHostPort(), 
                                          aRemoteAddr );
        }
    }

// -----------------------------------------------------------------------------
// CTransport::IsConnectedTransport
// -----------------------------------------------------------------------------
//
TBool CTransport::IsConnectedTransport()
    {
    TUint protocol( Protocol() );
    return ( protocol == KProtocolInetTcp || protocol == KProtocolTls );
    }

// -----------------------------------------------------------------------------
// CTransport::NotifyFlowFailure
// -----------------------------------------------------------------------------
//   
TBool CTransport::NotifyFlowFailure()
    {
    // NOTE: Notification leads to synchronous removal of the bindingobserver
    // from the array and the whole transport is removed when all observers
    // has been removed.
    
    TInt observerCount( iBindingObservers.Count() );
    TInt lastIndex( observerCount - 1 );
    for ( TInt i = lastIndex; i >= 0; i-- )
        {
        iBindingObservers[ i ]->FlowFailure( KErrSIPTransportFailure );
        }
        
    return ( observerCount > 0 );
    }
    
// -----------------------------------------------------------------------------
// CTransport::SendErrorResponseL
// -----------------------------------------------------------------------------
//  
void CTransport::SendErrorResponseL( 
	TUint aResponseCode, 
	TInt aStrIndex, 
	CSIPMessage& aMessage, 
	const TInetAddr& aAddr )
    {
    if ( !IsAck( aMessage ) )
		{			
		ErrorHandler().SendErrorResponseL( 
		    iTransportParams,
		    aResponseCode,
	        SIPStrings::StringF( aStrIndex ),
			aMessage,
			aAddr );
		}
    }
  
// -----------------------------------------------------------------------------
// CTransport::UpdateContactHeadersL
// -----------------------------------------------------------------------------
//  
void CTransport::UpdateContactHeadersL( 
    const TSIPTransportParams& aParams, 
    CSIPMessage* aMessage,
    TBool aUpdatePorts )
    {
    if ( !aMessage )
        {
        return;
        }
        
    TSglQueIter<CSIPHeaderBase> iter = 
        aMessage->Headers( SIPStrings::StringF(SipStrConsts::EContactHeader) );
    CSIPHeaderBase* header = NULL;
    while ( ( header = iter++ ) != NULL )
    	{
    	CSIPContactHeader* contact = static_cast<CSIPContactHeader*>( header );
        CSIPAddress* sipAddr = contact->SIPAddress();
			
        if ( sipAddr )
            {
            CURIContainer& uri = sipAddr->URI();
            if ( uri.IsSIPURI() )
                {
                TUint receivePort = 
                    iSettingsList.SavedLocalReceivePort( aParams.TransportId() );
            
                if ( aUpdatePorts && receivePort > 0 )
                    {
                    uri.SIPURI()->HostPort().SetPort( receivePort );
                    }
                
                RStringF transportParam =
                    uri.SIPURI()->ParamValue(
                        SIPStrings::StringF( SipStrConsts::ETransport ) );
                
                RStringF tls = SIPStrings::StringF( SipStrConsts::ETLS ); 
                //If the Contact header has parameter transport=tls, remove it
                //as it is deprecated in RFC 3261
                if(transportParam == tls)
                    {
                    uri.SIPURI()->DeleteParam(SIPStrings::StringF( SipStrConsts::ETransport ));
                    }
                
                //If the message is CSIPRequest, make sure its scheme is as per From Header.
                //So if the FROM header is SIPS, Contact header is SIPS, if FROM header is SIP,
                //Contact header scheme is sip
                if(aMessage->IsRequest())
                    {
                    if ( Protocol() == KProtocolTls )
                        {                       
                        CURIContainer& FromUri = (((aMessage->From())->SIPAddress()).URI());
                        if(FromUri.IsSIPURI())
                            {
                            CSIPURI* FromSIPUri =FromUri.SIPURI();
                            if(FromSIPUri->IsSIPSURI())
                                {                                
                                uri.SIPURI()->SetSIPS( ETrue );
                                }
                            else
                                {
                                uri.SIPURI()->SetSIPS( EFalse );
                                }
                            }
                        }
                    else
                        {
                        uri.SIPURI()->SetSIPS( EFalse );
                        }
                    //set the port information as per sip scheme and if there is no
                    //port information set in the contact header.=
                    if(uri.SIPURI()->IsSIPSURI() && (0 == uri.SIPURI()->HostPort().Port()))
                        uri.SIPURI()->HostPort().SetPort(KDefaultSipsPort);
                    else if ((EFalse == uri.SIPURI()->IsSIPSURI()) && (0 == uri.SIPURI()->HostPort().Port()))
                        uri.SIPURI()->HostPort().SetPort(KDefaultSipPort);
					else
						{}
                    } //end if (aMessage->IsRequest)				 
			    } //end if (uri.IsSIPURI())
			} //end if(sipaddr)
		} //end while    
    }

// -----------------------------------------------------------------------------
// CTransport::ChangeHostAddressFamilyIfNeededL
// -----------------------------------------------------------------------------
//     
void CTransport::ChangeHostAddressFamilyIfNeededL( 
    CSIPHostPort& aHostPort, 
    const TInetAddr& aRemoteAddr )
    {
    if ( aHostPort.HostType() == CSIPHostPort::ESIPHostName ||
         aHostPort.HostType() == CSIPHostPort::ESIPNoHost )
        {
        return;
        }
        
    TInetAddr hostInetAddr;
    ConvertToInetAddrL( aHostPort.Host(), hostInetAddr );
    
    if ( iTransportOwner->IsLocalAddr( hostInetAddr ) )
        {
        // Fills hostInetAddr based on the address family of aRemoteAddr.
        // If aRemoteAddr is IPv6, then hostInetAddr is set to IPv6.
        // If aRemoteAddr is IPv4, then hostInetAddr is set to IPv4 if possible.
        iTransportOwner->FillWithMatchingAddrFamily( 
            hostInetAddr, &aRemoteAddr );
        }
    else
        {
        TBool isRemoteIPv6Addr( aRemoteAddr.Address() == 0 ||
                                aRemoteAddr.IsV4Compat() );
        
        if ( isRemoteIPv6Addr && aHostPort.HostType() == CSIPHostPort::ESIPIpv4 )
            {
             // Update host if remote address is IPv6 and host address is IPv4
            hostInetAddr.ConvertToV4Compat();
            }
        else if ( !isRemoteIPv6Addr && 
                ( hostInetAddr.IsV4Compat() || hostInetAddr.IsV4Mapped() ) )
            {
            // Update host if remote address is IPv4 address and host is
            // IPv6 address which can be converted to V4
            hostInetAddr.ConvertToV4();
            }
        else
            {
            // Otherwise no actions taken
            return;
            }
        }
        
    HBufC8* hostAddr = ConvertInetAddrToTextL( hostInetAddr );
    CleanupStack::PushL( hostAddr );
    aHostPort.SetHostL( *hostAddr );
    CleanupStack::PopAndDestroy( hostAddr );
    }
 
// -----------------------------------------------------------------------------
// CTransport::HandleClientResponseRPortL
// -----------------------------------------------------------------------------
// 
 TBool CTransport::HandleClientResponseRPortL( 
    CSIPViaHeader& aViaHeader, 
    TInetAddr& aAddr )
    {
    if ( aViaHeader.Transport() != SIPStrings::StringF( SipStrConsts::EUDP ) )
        {
        return EFalse;
        }
    
    TBool portSet( EFalse ); 
    RStringF rport = SIPStrings::Pool().OpenFStringL( KParamrport );
	CleanupClosePushL( rport );
	
	if ( HasValue( aViaHeader, rport ) )
	    {
	    TLex8 lexer( aViaHeader.ParamValue( rport ).DesC() );
    	TInt rportVal( 0 );
    	User::LeaveIfError( lexer.Val( rportVal ) );
    	aAddr.SetPort( rportVal );
    	portSet = ETrue;
	    }
	    
    CleanupStack::PopAndDestroy(); // rport
    
    return portSet;
    }

// -----------------------------------------------------------------------------
// CTransport::HandleResponseNATTraversalL
// -----------------------------------------------------------------------------
//    
void CTransport::HandleResponseNATTraversalL( 
    CSIPResponse& aResponse, 
    const TInetAddr& aRemoteAddr,
    MSIPNATBindingObserver* aBindingObserver )
    {
    TBool persistent( EFalse );
    CSIPCSeqHeader* cseq = aResponse.CSeq();
    if ( aResponse.Type() == CSIPResponse::E2XX && 
         cseq && 
         cseq->Method() == SIPStrings::StringF( SipStrConsts::ERegister ) )
        {
        CSocketContainer& container = GetSocketContainerL( aRemoteAddr );
    
        if ( container.HasSecureSocket() )
            {
            persistent = iNATTraversal.RefreshNATBindingL( 
                *container.SecureSocket(),
                aBindingObserver );
            }
        else
            {
            persistent = iNATTraversal.RefreshNATBindingL(  
                container.Socket(), 
                aBindingObserver );
            }
        SetPersistencyL( aRemoteAddr, persistent, aBindingObserver );
        }
    InformSendingStatus( aRemoteAddr );
    }

// -----------------------------------------------------------------------------
// CTransport::HasValue
// -----------------------------------------------------------------------------
//   
TBool CTransport::HasValue( CSIPViaHeader& aViaHeader, RStringF& aParam )
    {
    TBool hasVal( EFalse );
	if ( aViaHeader.HasParam( aParam ) )
	    {
		hasVal = aViaHeader.ParamValue( aParam ).DesC().Length() > 0;
	    }
	return hasVal;
    }

// -----------------------------------------------------------------------------
// CTransport::AddBindingObserverL
// -----------------------------------------------------------------------------
//     
void CTransport::AddBindingObserverL( MSIPNATBindingObserver& aBindingObserver )
    {
    // Don't mind if the observer already exists.
    TInt err = iBindingObservers.InsertInAddressOrder( &aBindingObserver );
    if ( err && err != KErrAlreadyExists )
        {
        User::Leave( err );
        }
    }

// -----------------------------------------------------------------------------
// CTransport::RemoveBindingObserver
// -----------------------------------------------------------------------------
//     
TBool CTransport::RemoveBindingObserver( 
    MSIPNATBindingObserver& aBindingObserver )
    {
    TInt index = iBindingObservers.FindInAddressOrder( &aBindingObserver );
    TBool found( index >= 0 && index < iBindingObservers.Count() );
    if ( found )
        {
        iBindingObservers.Remove( index );
        iBindingObservers.Compress();
        }
    
    // Allow removal of whole transport if no more observers exist.
    // Reserved transports are removed with different mechanism.
    return ( found && !IsPersistent() && !IsReservedTransport() );
    }
    
// -----------------------------------------------------------------------------
// CTransport::ConvertToInetAddrL
// -----------------------------------------------------------------------------
//     
void CTransport::ConvertToInetAddrL( 
    const TDesC8& aAddrStr, 
    TInetAddr& aAddr )
    {	                        
	HBufC* tempInputAddr = HBufC::NewLC( aAddrStr.Length() );
	TPtr ptempInputAddr( tempInputAddr->Des() );
	ptempInputAddr.Copy( aAddrStr );

	User::LeaveIfError( aAddr.Input( ptempInputAddr ) );
	
	CleanupStack::PopAndDestroy( tempInputAddr );
    }
    
// -----------------------------------------------------------------------------
// CTransport::ConvertInetAddrToTextL
// -----------------------------------------------------------------------------
//        
HBufC8* CTransport::ConvertInetAddrToTextL( const TInetAddr& aAddr )
    {
    const TInt KMaxIPAddrLen = 256;
    HBufC* addressOut = HBufC::NewLC( KMaxIPAddrLen );
	TPtr outputPtr( addressOut->Des() );
	aAddr.Output( outputPtr );
	
	HBufC8* addressResult = HBufC8::NewL( addressOut->Length() );
	TPtr8 resultPtr( addressResult->Des() );
	CnvUtfConverter::ConvertFromUnicodeToUtf8( resultPtr, *addressOut );
	CleanupStack::PopAndDestroy( addressOut );

	return addressResult;
    }

// End of File
