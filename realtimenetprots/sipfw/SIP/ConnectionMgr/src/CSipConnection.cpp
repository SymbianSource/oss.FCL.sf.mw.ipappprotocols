// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CSipConnection.cpp
// Part of       : ConnectionMgr
// Version       : SIP/5.0
//



#include "CSipConnection.h"
#include "CSipConnectionMgr.h"
#include "SipLogs.h"
#include "CTransportUdp.h"
#include "CTransportTcp.h"
#include "CTransportTls.h"
#include "CIcmpReceiver.h"
#include "MTransactionFinder.h"
#include "CSIPServerResolver.h"
#include "CTcpListeningPoint.h"
#include "COutgoingData.h"
#include "CTcpResSender.h"
#include "MConnectionStateObserver.h"
#include "CLocalAddrResolver.h"
#include "MSigCompController.h"
#include "SipAssert.h"
#include "CConnectionContainer.h"
#include "CNetworkManager.h"
#include "CStateModel.h"
#include "CommonConsts.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "TSIPTransportParams.h"
#include "COwnerSettingsList.h"
#include "siperr.h"
#include "NetworkInfo.h"
#include "sipviaheader.h"
#include "COutgoingRequestQueue.h"
#include "sipuri.h"
#include "uricontainer.h"
#include "siphostport.h"
#include "siprouteheader.h"
#include "sipaddress.h"
#include <uriutils.h>
using namespace CommsDat;

// Max length for an IP address string representation
// (The format is either "d.d.d.d" or "h:h:h:h:h:h:h:h"
//  depending on the address family, where "d" is an 8-bit decimal number
//  and "h" is a 16-bit hexadecimal number. )
const TUint KMaxAddressLength( 50 );

const TInt CTransportBase::iOffset = _FOFF( CTransportBase, iLink );

// -----------------------------------------------------------------------------
// CSipConnection::NewLC
// -----------------------------------------------------------------------------
//
CSipConnection* CSipConnection::NewLC(
								MTransactionFinder& aFinder,
								MServerTaFactory& aTaFactory,
								MTimerManager& aTimer,
								TUint aTOne,
								const TSIPTransportParams& aParams,
								RSocketServ& aServer,
								MSigCompController& aSigCompHandler,
								MConnectionStateObserver& aTUNetStatusObserver,
								CNetworkManager& aNetworkManager,
								MSipConnectionOwner& aOwner,
								CNetworkInfo& aNetworkInfo,
								CSIPNATTraversalController& aNATTraversal )
	{
	CSipConnection* self =
		new( ELeave ) CSipConnection( aFinder, aTaFactory, aTimer, aTOne,
									  aParams, aServer, aSigCompHandler,
									  aTUNetStatusObserver, aNetworkManager,
									  aOwner, aNetworkInfo, aNATTraversal );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;

	}

// -----------------------------------------------------------------------------
// CSipConnection::SetOptL
// -----------------------------------------------------------------------------
//	
void CSipConnection::SetOptL( 
    const TSIPTransportParams& aParams,
    TUint aOptionName,
	TUint aOptionLevel,
	const TDesC8& aOption )
    {	
    // Remember opt, it will be used when this caller is using sockets
    iSettingsList->AddSettingsL( aParams, aOptionName, aOptionLevel, aOption );
    }

// -----------------------------------------------------------------------------
// CSipConnection::SetOptL
// -----------------------------------------------------------------------------
//		         	                         
void CSipConnection::SetOptL( 
    const TSIPTransportParams& aParams,
    TUint aOptionName,
	TUint aOptionLevel,
	TInt aOption )
    {
    // Remember opt, it will be used when this caller is using sockets
    iSettingsList->AddSettingsL( aParams, aOptionName, aOptionLevel, aOption );
    }
	
// -----------------------------------------------------------------------------
// CSipConnection::SendL
// -----------------------------------------------------------------------------
//
void CSipConnection::SendL( 
    const TSIPTransportParams& aParams,
    CSIPResponse& aResponse,
	TTransactionId aId,
	TRequestStatus& aStatus )
	{
	__SIP_MESSAGE_LOG( "Connection Manager::SendL", aResponse )
	iStateModel->SendL( aParams, aResponse, aId, aStatus );
	}

// -----------------------------------------------------------------------------
// CSipConnection::SendL
// -----------------------------------------------------------------------------
//
void CSipConnection::SendL( 
    const TSIPTransportParams& aParams,
    CSIPRequest& aRequest,
    TBool aForceUDP,
    const CUri8* aOutboundProxy,
	const TInetAddr& aRemoteAddr,
	TRequestStatus& aStatus )
	{
	__SIP_MESSAGE_LOG( "Connection Manager::SendL", aRequest )
	if ( aRemoteAddr.IsMulticast() )
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete( status, KErrNotSupported );
		}
	else
		{
		iStateModel->SendL( aParams, 
		                    aRequest, 
		                    aForceUDP,
		                    aOutboundProxy, 
		                    aRemoteAddr, 
		                    aStatus );
		}
	}

// -----------------------------------------------------------------------------
// CSipConnection::CancelSend
// -----------------------------------------------------------------------------
//
TInt CSipConnection::CancelSend( TRequestStatus &aStatus )
	{
	TBool cancelled = EFalse;
	CTransportBase* listItem = 0;
	iListIter.SetToFirst();
	while ( ( listItem = iListIter++ ) != 0 )
		{
		cancelled = listItem->CancelSend( aStatus );
		if(cancelled)
			{
			return KErrNone;
			}
		}

	CTcpResponseSender* listsender( NULL );
	iTcpSenderListIter.SetToFirst();
	while ( ( listsender = iTcpSenderListIter++ ) != 0 )
		{
		if ( listsender->CancelSend( aStatus ) )
			{
			return KErrNone;
			}
		}
		
    if ( iRequestQueue )
        {
        if ( iRequestQueue->CancelSend( aStatus ) )
            {
            return KErrNone;
            }
        }
	return KErrNotFound;
	}

// -----------------------------------------------------------------------------
// CSipConnection::CancelResponseSend
// -----------------------------------------------------------------------------
//
TInt CSipConnection::CancelResponseSend( 
    TTransactionId aId, 
    TBool aCancelAlso2xxResponses )
	{
	TBool cancelled = EFalse;
	CTransportBase* listItem = 0;
	iListIter.SetToFirst();
	while ( ( listItem = iListIter++ ) != 0 )
		{
		cancelled = listItem->CancelResponseSend( aId, aCancelAlso2xxResponses );
		if ( cancelled )
			{
			return KErrNone;
			}
		}
	return KErrNotFound;
	}

// -----------------------------------------------------------------------------
// CSipConnection::TcpDisconnect
// -----------------------------------------------------------------------------
//
void CSipConnection::TcpDisconnect( 
    const TSIPTransportParams& aParams,
    const TInetAddr& aRemoteAddress )
	{
	CTransportBase* listItem = 0;
	iListIter.SetToFirst();

	while ( ( listItem = iListIter++ ) != 0 )
		{
		if ( listItem->HasRemoteHost( aParams, 
		                              aRemoteAddress, 
		                              SIPStrings::StringF(SipStrConsts::ETCP) ) )
			{
			listItem->Disconnect( aRemoteAddress );
			}
		}
	}

// -----------------------------------------------------------------------------
// CSipConnection::FreeResources
// -----------------------------------------------------------------------------
//	
void CSipConnection::FreeResources( 
    MSIPNATBindingObserver& aSIPNATBindingObserver )
    {
    if ( iRequestQueue )
        {
        iRequestQueue->FreeResources( aSIPNATBindingObserver );
        }
    
    // Remove all matching persistent transports    
    CTransportBase* listItem;
    iListIter.SetToFirst();
    while ( ( listItem = iListIter++ ) != 0 )
        {   
        if ( listItem->Remove( aSIPNATBindingObserver ) )
            {
            RemoveTransport( listItem );
            }
        }
    }

// -----------------------------------------------------------------------------
// CSipConnection::SendToTransportL
// -----------------------------------------------------------------------------
//
void CSipConnection::SendToTransportL(
    const TSIPTransportParams& aParams,
    CSIPResponse& aResponse,
	TTransactionId aId,
	TRequestStatus &aStatus )
	{
	CTransportBase* transport = 0;

	CSIPViaHeader* viaHeader = TopViaHeader( &aResponse );
	if ( viaHeader )
	{
		TBool isPortPresent = viaHeader->SentByHostPort().HasPort();
		if (isPortPresent)
		{
			TUint sentByPort = viaHeader->SentByHostPort().Port();
			if ((sentByPort > KMaxPortValue) || (sentByPort < KMinPortValue))
			{
				TRequestStatus* status = &aStatus;
		    	__SIP_LOG( "Sent-by port is topmost viaheader is invalid, dropping response" )
		    	User::RequestComplete( status, CSipConnectionMgr::EPortUnreachable );
		    	return;
			}
		}
	}
	
	TBool isTransportUdp = IsTransportUdp( &aResponse );

    transport = FindTransportL( aParams, &aResponse, aId, !isTransportUdp );

    if ( !isTransportUdp && !transport )
		{
		// When connected transport is used, there might be several connections
    	// with same remote address, in that case we had to search through all
    	// transports for matching the ta id. At this point any matching
    	// transport wasn't found, now we can take also remote address in count.
		transport = FindTransportL( aParams, &aResponse, aId, EFalse );
		
		if ( !transport )
		    {
    		// If there's saved settings regarding remote address, use them 
    		// instead of address passed from upper layers (probably only the
    		// port differs)
    		TInetAddr addrFromResponse;
            const TInetAddr* address;
            const TInetAddr* savedAddr = 
                iSettingsList->SavedRemoteAddr( aParams.TransportId() );
            if ( savedAddr )
                {
                address = savedAddr;
                }
            else
                {
    			CTransportBase* listItem;
    			if ( !iList.IsEmpty() )
    				{
    				iListIter.SetToFirst();
    				listItem = iListIter;
    				listItem->GetAddrFromResponseL( aParams, 
    				                                aResponse, 
    				                                addrFromResponse );
    				}
                address = &addrFromResponse;
                }    
    		
    		if ( !address->IsWildAddr() )
    			{
        		CreateConnectedTransportPointL( aParams, aResponse, *address );
        		transport = FindTransportL( aParams, &aResponse, aId, EFalse );
    			}
    		}
		}
	
	if ( transport )
		{
		transport->Send( aParams, aResponse, aId, aStatus );
		}
	else
	    {	
        TRequestStatus* stat = &aStatus;
        User::RequestComplete( stat, KErrNotFound );
	    }
	}

// -----------------------------------------------------------------------------
// CSipConnection::SendToTransportL
// -----------------------------------------------------------------------------
//
void CSipConnection::SendToTransportL(
    const TSIPTransportParams& aParams,
    CSIPRequest& aRequest,
    TBool aForceUDP,
    const CUri8* aOutboundProxy,
	const TInetAddr& aAddr,
	TRequestStatus &aStatus )
	{
	TUint origTransport = 
	    UpdateTransportIfNeededL( aRequest, aParams, aForceUDP, aAddr );

    // If there's saved settings regarding remote address, use them instead of
    // address passed from upper layers (probably only the port differs)
    const TInetAddr* address = 
        iSettingsList->SavedRemoteAddr( aParams.TransportId() );
    if ( !address )
        {
        address = &aAddr;
        }
        
    TInetAddr localAddr;  
    GetLocalAddrL( localAddr, address );    
    iRequestQueue->NATTraverseL( aParams, 
                                 aRequest, 
                                 aForceUDP,
                                 *address, 
                                 aOutboundProxy,
                                 origTransport, 
                                 aStatus,
                                 localAddr );
	}

// -----------------------------------------------------------------------------
// CSipConnection::ContinueSendToTransportL
// -----------------------------------------------------------------------------
//	
void CSipConnection::ContinueSendToTransportL( 
    const TSIPTransportParams& aParams,
    CSIPRequest& aRequest,
    TBool aForceUDP,
    const TInetAddr& aRemoteAddr,
    TUint aLocalPort,
    TUint aOrigTransport,
    TRequestStatus& aStatus,
    TBool aPublicAddrResolved )
    {
    __ASSERT_ALWAYS( IsConnectionActive(), User::Leave( KErrNotReady ) );

	CTransportBase* transport = 
	    FindTransportL( aParams, &aRequest, aRemoteAddr, aLocalPort );
	if ( transport != 0 )
		{
		aStatus = KRequestPending;
		transport->Send( aParams, 
		                 aRequest,
		                 aForceUDP,
		                 aRemoteAddr, 
		                 aOrigTransport, 
		                 aStatus, 
		                 aPublicAddrResolved );
		}
	else
		{
		if ( aRequest.HasHeader( SIPStrings::StringF(SipStrConsts::EViaHeader) ) )
			{
            if ( IsTransportUdp( &aRequest ) )
                {
            	AddUdpListenerL( 
            	    TSIPTransportParams( KDefaultOwnerId, iIapId ),
            	    KDefaultSipPort );
            	}
            else
            	{	
            	CreateConnectedTransportPointL( aParams, aRequest, aRemoteAddr );
            	}

            transport = FindTransportL( aParams, &aRequest, aRemoteAddr );
            if ( transport )
                {
                aStatus = KRequestPending;
                transport->Send( aParams, 
                                 aRequest,
                                 aForceUDP, 
                                 aRemoteAddr, 
                                 aOrigTransport, 
                                 aStatus,
                                 aPublicAddrResolved );
            	}
            else
            	{
            	TRequestStatus* stat = &aStatus;
            	User::RequestComplete( stat, KErrNotReady );
            	}	
			}
		}
    }

// -----------------------------------------------------------------------------
// CSipConnection::RequestFailed
// -----------------------------------------------------------------------------
//   
void CSipConnection::RequestFailed( TRequestStatus& aStatus, TInt aError )
    {
    if ( aStatus == KRequestPending )
        {
        TRequestStatus* status = &aStatus;
		User::RequestComplete( status, aError );
        }
    }

// -----------------------------------------------------------------------------
// CSipConnection::GetUdpSocketL
// -----------------------------------------------------------------------------
//    
RSocket& CSipConnection::GetUdpSocketL( 
    const TSIPTransportParams& aParams, 
    const TInetAddr& aRemoteAddr,
    TUint& aLocalPort,
    MSocketUsagePermissionObserver* aObserver,
    TBool& aPermissionToUse )
    {
    CTransportBase* transport = 
        FindTransport( aParams, 
                       SIPStrings::StringF( SipStrConsts::EUDP ), 
                       aRemoteAddr,
                       aLocalPort,
                       EFalse );
    if ( !transport )
        {
        if ( !aLocalPort )
            {
            aLocalPort = iSettingsList->GetPrivatePortL( aParams );	
            }
        transport = AddUdpListenerL( aParams, aLocalPort, ETrue );
        }

    RSocket* udpSocket = 
        transport->AcquireUdpSocket( aObserver, aRemoteAddr, aPermissionToUse );
        
    __ASSERT_ALWAYS( udpSocket, User::Leave( KErrNotFound ) );
    
    return *udpSocket;
    }

// -----------------------------------------------------------------------------
// CSipConnection::UdpSocketFree
// -----------------------------------------------------------------------------
//       
void CSipConnection::UdpSocketFree( 
    const TSIPTransportParams& aParams,
    const TInetAddr& aRemoteAddr,
    TUint aLocalPort,
    MSocketUsagePermissionObserver* aObserver,
    RSocket& aSocket )
    {
    CTransportBase* transport = 
        FindTransport( aParams, 
                       SIPStrings::StringF( SipStrConsts::EUDP ), 
                       aRemoteAddr,
                       aLocalPort,
                       EFalse );
    if ( transport )
        {
        transport->UdpSocketReleased( aObserver, aSocket );
        }
    }

// -----------------------------------------------------------------------------
// CSipConnection::UpdateTransportIfNeededL
// -----------------------------------------------------------------------------
//
TUint CSipConnection::UpdateTransportIfNeededL( 
    CSIPMessage& aMessage,
    const TSIPTransportParams& aParams,
    TBool aForceUDP,
    const TInetAddr& aAddr )
	{
    TUint origTransport = TransportProtocolL( aMessage );
		
	CSIPViaHeader* via = TopViaHeader( &aMessage );
	
	if ( via )
	    {
    	TBool tlsForced = 
	        iSettingsList->HasSavedTLSTransportInfo( aParams.TransportId() );
	    
		if ( tlsForced && origTransport != KProtocolTls )
		    {
		    via->SetTransportL( SIPStrings::StringF(SipStrConsts::ETLS) );
		    
		    // Upper layer may give incorrect port information in some 
		    // situations when TLS is in use, ensure that at least
		    // default TLS port is used.
		    iSettingsList->SaveTLSRemoteL( aParams, aAddr );
		    }
	    else if ( origTransport == KProtocolInetUdp && !aForceUDP &&
	            ( aMessage.EncodedHeaderPartLengthL() +
    		      aMessage.Content().Length() ) > KMaxUdpMessageSize )
	        {
	       	via->SetTransportL( SIPStrings::StringF(SipStrConsts::ETCP) );
	        }
	    else
	        {
	        }
	    }
	    
    return origTransport;
	}

// -----------------------------------------------------------------------------
// CSipConnection::CreateConnectedTransportPointL
// -----------------------------------------------------------------------------
//
void CSipConnection::CreateConnectedTransportPointL( 
    const TSIPTransportParams& aParams,
    CSIPMessage& aMessage, 
    const TInetAddr& aAddress )
	{
	TUint protocol = TransportProtocolL( aMessage );
	switch ( protocol )
	    {
	    case KProtocolInetTcp:
	        {
	        CreateTcpTransportPointL( aParams, aAddress );
	        break;
	        }
	    case KProtocolTls:
	        {
	        CreateTlsTransportPointL( aParams, aAddress );
	        //Check if the SIP message is request.Below code sets the
			//domain name into CTransportTls object which will be later
			//used in domain name matching during TLS Negotiation
	        HBufC8 *domainName = NULL;
	        CTransportTls *transport;
	        if(aMessage.IsRequest())
	        {
	        	//If the SIP request has Route header then the host part will be extracted
	        	//from the Route header
	        	const RStringF route = SIPStrings::StringF(SipStrConsts::ERouteHeader);	        	
	        	if(aMessage.HasHeader(route))
	        	{
	        		TSglQueIter<CSIPHeaderBase> iter = aMessage.Headers( route );
	        	    iter.SetToFirst();
	        	    CSIPHeaderBase* header = iter;
	        	    if ( header )
	        	    {
	        			CSIPRouteHeader* routeHeader =	static_cast<CSIPRouteHeader*>( header );
	        			domainName = routeHeader->SIPAddress().URI().SIPURI()->HostPort().Host().AllocL();	        			
	        		}	        
	        	}
	        	else
	        	{
	        		//If the Route header is absent from the SIP Request, request-line
	        		//gets preference and host part is extracted from the request-line					
       				CSIPRequest &request = static_cast<CSIPRequest&>(aMessage ) ;
       				domainName = request.RequestURI()->SIPURI()->HostPort().Host().AllocL();
	        	}	        	
	        }
	        else
	        {
	        	//Message is response. Host part will be extracted from the top
	        	//Via header
	            CSIPViaHeader* viaHeader = TopViaHeader( &aMessage );
	            if(viaHeader)
	            	{
	            	domainName = viaHeader->SentByHostPort().Host().AllocL();	
	            	}            
	        }
	        transport = static_cast<CTransportTls*>(FindTransport( aParams, 
	        	        								SIPStrings::StringF( SipStrConsts::ETLS ),
	        	        								aAddress));
			//If the domain name is of type "Text", set the domain name for the
			//TLS object.
			if ( ( UriUtils::HostType( *domainName ) == UriUtils::ETextHost ))
		        transport->SetDomainNameL(domainName->Des());
		    delete domainName;	
	        break;
	        }
	    default:
	        {
	        User::Leave( KErrArgument );
	        break;
	        }
	    }
	}
	
// -----------------------------------------------------------------------------
// CSipConnection::CreateTcpTransportPointL
// -----------------------------------------------------------------------------
//
void CSipConnection::CreateTcpTransportPointL( 
    const TSIPTransportParams& aParams, 
    const TInetAddr& aAddress )
	{
	// Leave if tcp connection with remote address exists already
	CTransportBase* listItem = 0;
	iListIter.SetToFirst();
	while( ( listItem = iListIter++ ) != 0 )
		{
		if( listItem->HasRemoteHost( aParams, 
		                             aAddress, 
		                             SIPStrings::StringF(SipStrConsts::ETCP) ) )
		    {
		    User::Leave( KErrSIPTransportFailure );
		    }
		}	
	
	CTransportBase* transport =	
	    CTransportTcp::NewLC( 
	        iServer, iFinder, iTaFactory,
			*this, iTimer, aParams, aAddress, iTOne,
			*iServerResolver, Connection(), iSigCompHandler, 
			iSettingsList->SavedLocalSendPort( aParams.TransportId() ),
			iNetworkInfo, *iSettingsList, iNATTraversal );        
	AddTransportL( transport );
	CleanupStack::Pop( transport );
	}
	
// -----------------------------------------------------------------------------
// CSipConnection::CreateTlsTransportPointL
// -----------------------------------------------------------------------------
//
void CSipConnection::CreateTlsTransportPointL( 
    const TSIPTransportParams& aParams, 
    const TInetAddr& aAddress )
	{
	// Leave if tcp connection with remote address exists already
	CTransportBase* listItem = 0;
	iListIter.SetToFirst();
	while( ( listItem = iListIter++ ) != 0 )
		{
		if( listItem->HasRemoteHost( aParams, 
		                             aAddress,
		                             SIPStrings::StringF(SipStrConsts::ETLS) ) )
		    {
		    User::Leave( KErrSIPTransportFailure );
		    }
		}	
	
	CTransportBase* transport =	
	    CTransportTls::NewLC( 
	        iServer, iFinder, iTaFactory,
			*this, iTimer, aParams, aAddress, iTOne,
			*iServerResolver, Connection(), iSigCompHandler, 
			iNetworkInfo, *iSettingsList, iNATTraversal );        
	AddTransportL( transport );
	CleanupStack::Pop( transport );
	}

// -----------------------------------------------------------------------------
// CSipConnection::IsTransportUdp
// -----------------------------------------------------------------------------
//
TBool CSipConnection::IsTransportUdp( CSIPMessage* aMessage )
	{
	CSIPViaHeader* via = TopViaHeader( aMessage );
	if( via )
		{
		if( via->Transport() == SIPStrings::StringF(SipStrConsts::EUDP) )
			{
			return ETrue;
			}
		}

	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSipConnection::TransportProtocolL
// -----------------------------------------------------------------------------
//	
TUint CSipConnection::TransportProtocolL( CSIPMessage& aMessage )
    {
    CSIPViaHeader* via = TopViaHeader( &aMessage );
    __ASSERT_ALWAYS( via, User::Leave( KErrNotFound ) );

	RStringF transport = via->Transport(); 
	if ( transport == SIPStrings::StringF( SipStrConsts::EUDP ) )
		{
		return KProtocolInetUdp;
		}
    else if ( transport == SIPStrings::StringF( SipStrConsts::ETCP ) )
		{
		return KProtocolInetTcp;
		}
    else if ( transport == SIPStrings::StringF( SipStrConsts::ETLS ) )
		{
		return KProtocolTls;
		}
	else
	    {
	    User::Leave( KErrNotSupported );
	    }
	return 0;
    }

// -----------------------------------------------------------------------------
// CSipConnection::ResetTransportsL
// -----------------------------------------------------------------------------
//
void CSipConnection::ResetTransportsL()
	{
	RemoveAllTransports();
	TSIPTransportParams defaultParams( KDefaultOwnerId, iIapId );
	AddTcpListenerL( defaultParams, KDefaultSipPort );
	AddUdpListenerL( defaultParams, KDefaultSipPort );
	}

// -----------------------------------------------------------------------------
// CSipConnection::IsInList
// -----------------------------------------------------------------------------
//
TBool CSipConnection::IsInList( CTransportBase* aTransport )
	{
	if( aTransport )
		{
		CTransportBase* listItem;
		iListIter.SetToFirst();
		while ( ( listItem = iListIter++ ) != 0 )
			{
			if( listItem == aTransport )
				{
				return ETrue;
				}
			}
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSipConnection::AddTransportL
// -----------------------------------------------------------------------------
//
void CSipConnection::AddTransportL( CTransportBase* aTransport )
	{
	if( aTransport )
		{
		if( !IsInList( aTransport ) )
			{
			iList.AddLast( *aTransport );
			}
		else
			{
			User::Leave( KErrAlreadyExists );
			}
		}
	else
		{
		User::Leave( KErrGeneral );
		}
	}

// -----------------------------------------------------------------------------
// CSipConnection::RemoveTransport
// -----------------------------------------------------------------------------
//
TInt CSipConnection::RemoveTransport( CTransportBase* aTransport )
	{
	if( IsInList( aTransport ) )
		{
		iList.Remove( *aTransport );
		delete aTransport;
		return KErrNone;
		}
	else
		{
		return KErrNotFound;
		}
	}

// -----------------------------------------------------------------------------
// CSipConnection::RemoveAllTransports
// -----------------------------------------------------------------------------
//
void CSipConnection::RemoveAllTransports()
	{
	CTransportBase* listItem( NULL );
	iListIter.SetToFirst();
	while ( ( listItem = iListIter++ ) != 0 )
		{
		iList.Remove( *listItem );
		delete listItem;
		}
	}

// -----------------------------------------------------------------------------
// CSipConnection::FindTransportL
// -----------------------------------------------------------------------------
//
CTransportBase* CSipConnection::FindTransportL( 
    const TSIPTransportParams& aParams,
    CSIPMessage* aMessage,
	TTransactionId aId,
	TBool aIsStrict )
	{
	iListIter.SetToFirst();

	CTransportBase* listItem = NULL;
	while ( ( listItem = iListIter++ ) != 0 )
		{
		if( listItem->HandleL( aParams, aMessage, aId, aIsStrict ) )
			{
			return listItem;
			}
		}
	return 0;
	}

// -----------------------------------------------------------------------------
// CSipConnection::FindTransportL
// -----------------------------------------------------------------------------
//
CTransportBase* CSipConnection::FindTransportL( 
    const TSIPTransportParams& aParams,
    CSIPMessage* aMessage,
	const TInetAddr& aRemoteAddr,
	TUint aLocalPort,
	TBool aIsStrict )
	{
	CTransportBase* listItem;
	iListIter.SetToFirst();

	while ( ( listItem = iListIter++ ) != 0 )
		{
		if ( listItem->HandleL( aParams, 
		                        aMessage, 
		                        aRemoteAddr, 
		                        aLocalPort,
		                        aIsStrict ) )
			{
			return listItem;
			}
		}
	return 0;
	}

// -----------------------------------------------------------------------------
// CSipConnection::FindTransportL
// -----------------------------------------------------------------------------
//
CTransportBase* CSipConnection::FindTransport( 
    const TSIPTransportParams& aParams,
    RStringF aProtocol,
	const TInetAddr& aRemoteAddr,
	TUint aLocalPort,
	TBool aIsStrict )
	{
	CTransportBase* listItem;
	iListIter.SetToFirst();
	while ( ( listItem = iListIter++ ) != 0 )
		{
		if ( listItem->HandleMessage( aParams, 
		                              aProtocol, 
		                              aRemoteAddr, 
		                              aLocalPort,
		                              aIsStrict ) )
			{
			return listItem;
			}
		}
	return 0;
	}
	
// -----------------------------------------------------------------------------
// CSipConnection::RemoveTcpSender
// -----------------------------------------------------------------------------
//
void CSipConnection::RemoveTcpSender( CTcpResponseSender* aThis )
	{
	if ( !iTcpSenderList.IsEmpty() )
		{
		iTcpSenderList.Remove( *aThis );
		delete aThis;
		}
	}

// -----------------------------------------------------------------------------
// CSipConnection::RemoveAllTcpSender
// -----------------------------------------------------------------------------
//
void CSipConnection::RemoveAllTcpSender()
	{
	CTcpResponseSender* listItem( NULL );
	iTcpSenderListIter.SetToFirst();
	while ( ( listItem = iTcpSenderListIter++ ) != 0 )
		{
		iTcpSenderList.Remove( *listItem );
		delete listItem;
		}
	}

// -----------------------------------------------------------------------------
// CSipConnection::TcpConnected
// -----------------------------------------------------------------------------
//
void CSipConnection::TcpConnected( const TInetAddr& aAddr )
	{
	CTcpResponseSender* listItem( NULL );
	iTcpSenderListIter.SetToFirst();
	while ( ( listItem = iTcpSenderListIter++ ) != 0 )
		{
		listItem->Connected( aAddr );
		}
	}

// -----------------------------------------------------------------------------
// CSipConnection::ReRouteL
// -----------------------------------------------------------------------------
//
void CSipConnection::ReRouteL( TUint aProtocol, COutgoingData* aData )
	{
	__ASSERT_ALWAYS( aData, User::Leave( KErrArgument ) );

	const TSIPTransportParams& tpParams = aData->TransportParams();
	if ( aProtocol == KProtocolInetTcp &&
		 aData->Message().IsRequest() )
		{
		CSIPViaHeader* via = TopViaHeader( &aData->Message() );
		if(via)
		{
		    via->SetTransportL( SIPStrings::StringF(SipStrConsts::ETCP) );
		}
				
		// Public address resolved value can be passed as true, since
		// at this point all necessary via and contact header updates
		// have been done (i.e. no need to update).
		ContinueSendToTransportL(
		    tpParams,
		    static_cast<CSIPRequest&>( aData->Message() ),
		    aData->ForceUDP(),
			aData->Address(), 
			0,
			aData->OrigTransport(), 
			*aData->Status(),
			ETrue );
		delete aData;
		return;
		}

	if ( aProtocol == KProtocolInetTcp )
		{
		CTcpResponseSender* tcpsender( NULL );
		while ( (tcpsender = iTcpSenderListIter++ ) != 0 )
			{
			if ( tcpsender->IsInProgress( *aData ) )
				{
				tcpsender->ConnectionFailedL();
				delete aData;
				return;
				}
			}
		tcpsender =
			new ( ELeave ) CTcpResponseSender( tpParams, 
			                                   *iServerResolver, 
			                                   *this );
		iTcpSenderList.AddLast( *tcpsender );
		tcpsender->ResolveL( *aData );
		tcpsender->SetData( aData );
		}

	if ( aProtocol == KProtocolInetUdp &&
		 ( aData->Message().EncodedHeaderPartLengthL() +
		   aData->Message().Content().Length() ) > KMaxMessageLength )
		{
		TRequestStatus* stat = aData->Status();
		User::RequestComplete( stat, KErrCouldNotConnect );
		delete aData;
		return;
		}

	if ( aProtocol == KProtocolInetUdp )
		{
		CSIPViaHeader* via = TopViaHeader( &aData->Message() );
		if ( via )
			{
			via->SetTransportL( SIPStrings::StringF(SipStrConsts::EUDP) );
			}
		CSender* send = GetSender( tpParams, aProtocol, aData->Address() );
		if ( send )
			{
			send->SendL( aData );
			}
		else
			{
			AddUdpListenerL( 
			    TSIPTransportParams( KDefaultOwnerId, iIapId ),
			    KDefaultSipPort );
			send = GetSender( tpParams, aProtocol, aData->Address() );
			if ( send )
				{
				send->SendL( aData );
				}
			else
				{
				TRequestStatus* stat = aData->Status();
				User::RequestComplete( stat, KErrCouldNotConnect );
				delete aData;
				}
			}
		}
	}

// -----------------------------------------------------------------------------
// CSipConnection::GetSender
// -----------------------------------------------------------------------------
//
CSender* CSipConnection::GetSender( 
    const TSIPTransportParams& aParams,
    TUint aProtocol, 
    const TInetAddr& aAddr )
	{
	CTransportBase* listItem;
	iListIter.SetToFirst();

	CSender* sender = NULL;
	while( ( ( listItem = iListIter++ ) != 0 ) && !sender )
		{
		sender = listItem->Sender( aParams, aProtocol, aAddr );
		}

	return sender;
	}

// -----------------------------------------------------------------------------
// CSipConnection::AddListenerL
// -----------------------------------------------------------------------------
//
void CSipConnection::AddListenerL( 
    const TSIPTransportParams& aParams,
    TUint aProtocol, 
    TUint aPort )
	{
	AddTcpListenerL( aParams, aPort );
	if( aProtocol != KProtocolInetTcp )
		{
		AddUdpListenerL( aParams, aPort );
		}
	}

// -----------------------------------------------------------------------------
// CSipConnection::AddTcpListenerL
// -----------------------------------------------------------------------------
//
void CSipConnection::AddTcpListenerL( 
    const TSIPTransportParams& aParams, 
    TUint aPort,
    TBool aIsReserved )
	{
	if( !IsListened( KProtocolInetTcp, aPort ) )
		{                       
		CTransportBase* transportTcp =
			CTcpListeningPoint::NewLC( iServer, iFinder, iTaFactory, *this,
									   iTimer, aPort, iTOne, *iServerResolver,
									   Connection(), iSigCompHandler,
									   iNetworkInfo, *iSettingsList, aParams,
									   iNATTraversal );
		AddTransportL( transportTcp );
		CleanupStack::Pop( transportTcp );
		transportTcp->WaitL();
		}
    else
        {
        __ASSERT_ALWAYS( !aIsReserved, User::Leave( KErrSIPTransportFailure ) );
        }
	}

// -----------------------------------------------------------------------------
// CSipConnection::AddUdpListenerL
// -----------------------------------------------------------------------------
//
CTransportBase* CSipConnection::AddUdpListenerL( 
    const TSIPTransportParams& aParams,
    TUint aPort,
    TBool aIsReserved,
    TBool aOnlyForReceiving )
	{
	CTransportBase* transport( NULL );
	if( !IsListened( KProtocolInetUdp, aPort ) )
		{
		transport =
			CTransportUdp::NewLC( iServer, aPort, iFinder,
			                      iTaFactory, *this, iTimer, *iServerResolver,
			                      Connection(), iSigCompHandler, iTOne,
			                      iNetworkInfo, *iSettingsList, aParams,
			                      iNATTraversal, aOnlyForReceiving );
		AddTransportL( transport );
		CleanupStack::Pop( transport );
		transport->WaitL();
		}
    else
        {
        __ASSERT_ALWAYS( !aIsReserved, User::Leave( KErrSIPTransportFailure ) );
        }
        
    return transport;
	}

// -----------------------------------------------------------------------------
// CSipConnection::AddTcpListenerUserL
// -----------------------------------------------------------------------------
//
void CSipConnection::AddTcpListenerUserL( 
    const TSIPTransportParams& aOldParams,
    const TSIPTransportParams& aParams,
    TUint aReceivePort )
    {
    CTransportBase* listItem( NULL );
	iListIter.SetToFirst();
	
	TBool added( EFalse );
   	while ( ( listItem = iListIter++ ) != 0 && !added )
		{
		if( listItem->Handle( aOldParams.TransportId() ) &&
		    listItem->ProtocolType() == KProtocolInetTcp )
			{
			added = listItem->AddUserL( aParams, aReceivePort );
			}
		}
	if ( !added )
	    {
        User::Leave( KErrNotFound );
	    }
    }
    
// -----------------------------------------------------------------------------
// CSipConnection::AddUdpListenerUserL
// -----------------------------------------------------------------------------
//
void CSipConnection::AddUdpListenerUserL( 
    const TSIPTransportParams& aOldParams,
    const TSIPTransportParams& aParams,
    TUint aReceivePort )
    {
    CTransportBase* listItem( NULL );
	iListIter.SetToFirst();
	
	TBool added( EFalse );
   	while ( ( listItem = iListIter++ ) != 0 && !added )
		{
		if( listItem->Handle( aOldParams.TransportId() ) &&
		    listItem->ProtocolType() == KProtocolInetUdp )
			{
			added = listItem->AddUserL( aParams, aReceivePort );
			}
		}
	if ( !added )
	    {
        User::Leave( KErrNotFound );
	    }
    }
    
// -----------------------------------------------------------------------------
// CSipConnection::IsListened
// -----------------------------------------------------------------------------
//
TBool CSipConnection::IsListened( TInt aProtocol, TUint aPort )
	{
	CTransportBase* listItem( NULL );
	iListIter.SetToFirst();

	while ( ( listItem = iListIter++ ) != 0 )
		{
		if( listItem->IsWaiting( aProtocol, aPort ) )
			{
			return ETrue;
			}
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSipConnection::CancelAllRequests
// -----------------------------------------------------------------------------
//
void CSipConnection::CancelAllRequests( CSIPConnection::TState aReason )
	{
	CTransportBase* listItem( NULL );
	iListIter.SetToFirst();
	while ( ( listItem = iListIter++ ) != 0 )
		{
		listItem->CancelAllRequests( aReason );
		}
	}

// -----------------------------------------------------------------------------
// CSipConnection::IcmpError
// -----------------------------------------------------------------------------
//
void CSipConnection::IcmpError( const TInetAddr& aAddress )
	{
	TRAP_IGNORE( NotifyIcmpErrorL( aAddress ) )
	}

// -----------------------------------------------------------------------------
// CSipConnection::GetByURIL
// -----------------------------------------------------------------------------
//
void CSipConnection::GetByURIL( const TSIPTransportParams& aParams,
                                const CURIContainer& aURI,
                                RPointerArray<MSIPResolvingResult>& aResult,
		                        MSIPServerResolverObserver* aObserver )
	{
	__ASSERT_ALWAYS( iServerResolver, User::Leave( KErrNotReady ) );
	
	// Have to do const cast in order to get binding observer
	MSIPNATBindingObserver* bindingObserver = 
	    const_cast<TSIPTransportParams&>( aParams ).NATBindingObserver();

	TBool persistentConnectionFound( EFalse );
	TInetAddr* remoteAddr = 0;
	TUint protocol = 0;
	
	CTransportBase* listItem;
    iListIter.SetToFirst();
	while ( ( listItem = iListIter++ ) != 0 && !persistentConnectionFound )
		{
		protocol = listItem->ProtocolType();
		remoteAddr = listItem->PersistentRemoteAddr( bindingObserver );
		
		persistentConnectionFound = 
		    ( remoteAddr &&
		    ( protocol == KProtocolInetTcp || protocol == KProtocolTls ) );
	    }
	    
	TBool skipNaptrAndSrv = aParams.SkipNaptrAndSrv();    
	    
	if ( persistentConnectionFound )
	    {
	    CURIContainer* uriContainer = CURIContainer::NewLC( aURI );
		UpdateURIL( *uriContainer, *remoteAddr, protocol );
		iServerResolver->GetByURIL( *uriContainer, skipNaptrAndSrv, 
		                            aResult, aObserver );
		CleanupStack::PopAndDestroy( uriContainer );
		return;
	    }

    iServerResolver->GetByURIL( aURI, skipNaptrAndSrv, aResult, aObserver );
	}

// -----------------------------------------------------------------------------
// CSipConnection::CancelGetByUri
// -----------------------------------------------------------------------------
//
void CSipConnection::CancelGetByUri( MSIPServerResolverObserver* aObserver )
	{
	if( iServerResolver )
	    { 
    	iServerResolver->Cancel( aObserver );
	    }
	}

// -----------------------------------------------------------------------------
// CSipConnection::GetLocalAddress
// -----------------------------------------------------------------------------
//
TInt CSipConnection::GetLocalAddress( TInetAddr& aAddr )
	{
	if ( !iLocalAddrResolver )
		{
		return KErrNotReady;
		}

	TRAPD( err, GetLocalAddrL( aAddr, NULL ) );
		
	return err;
	}

// -----------------------------------------------------------------------------
// CSipConnection::IsLocalAddr
// -----------------------------------------------------------------------------
//
TBool CSipConnection::IsLocalAddr( TInetAddr& aAddr ) const
    {
    return iLocalAddrResolver->IsLocalAddr( aAddr );
    }

// -----------------------------------------------------------------------------
// CSipConnection::GetLocalAddrL
// -----------------------------------------------------------------------------
//
void CSipConnection::GetLocalAddrL( 
    TInetAddr& aAddr, 
    const TInetAddr* aNextHop )
	{
	if( iLocalAddrResolver )
		{
	    iLocalAddrResolver->RefreshLocalAddressesL();
	    FillWithMatchingAddrFamily( aAddr, aNextHop );
		}
	}

// -----------------------------------------------------------------------------
// CSipConnection::GetLocalIPAddr
// -----------------------------------------------------------------------------
//
TBool CSipConnection::GetLocalIPAddr( TUint aFamily, TInetAddr& aAddr )
    {
    TBool found( EFalse );
    if( iLocalAddrResolver )
		{
		if ( aFamily == KAfInet )
		    {
		    if ( !iLocalAddrResolver->IPv4Addr().IsUnspecified() )
		        {
        		aAddr = iLocalAddrResolver->IPv4Addr();
        		found = ETrue;
		        }
		    }
		else
		    {
		    if ( !iLocalAddrResolver->IPv6Addr().IsUnspecified() )
		        {
        		aAddr = iLocalAddrResolver->IPv6Addr();
        		found = ETrue;
		        }
		    }

		}
	return found;
    }

// -----------------------------------------------------------------------------
// CSipConnection::FillWithMatchingAddrFamily
// -----------------------------------------------------------------------------
//
void CSipConnection::FillWithMatchingAddrFamily( 
    TInetAddr& aAddr,
    const TInetAddr* aNextHop )
    {
    __SIP_ADDR_LOG("CSipConnection::FillWithMatchingAddrFamily: local IPv6 addr",
                   iLocalAddrResolver->IPv6Addr())
    __SIP_ADDR_LOG("CSipConnection::FillWithMatchingAddrFamily: local IPv4 addr",               
                   iLocalAddrResolver->IPv4Addr())
    
    if ( !iLocalAddrResolver->IPv4Addr().IsUnspecified() )
	    {
	    aAddr = iLocalAddrResolver->IPv4Addr();
	    }
	else
	    {
	    aAddr = iLocalAddrResolver->IPv6Addr();
	    }
    
	if ( aNextHop )
	    {
        __SIP_ADDR_LOG("CSipConnection::FillWithMatchingAddrFamily: next hop addr",               
                       *aNextHop)	    
	    
                       
	    if ( aNextHop->Address() ) // Contains IPv4 address
	        {
        	__SIP_INT_LOG1("CSipConnection::FillWithMatchingAddrFamily: next hop family",
                       KAfInet)
	        if ( iLocalAddrResolver->IPv4Addr().IsUnspecified() &&
	             ( aAddr.IsV4Compat() || aAddr.IsV4Mapped() ) )
	            {
	            aAddr.ConvertToV4();
	            }
	        }
	    else
	        {
        	__SIP_INT_LOG1("CSipConnection::FillWithMatchingAddrFamily: next hop family",
                       KAfInet6)
	        if ( iLocalAddrResolver->IPv6Addr().IsUnspecified() )
	            {
	            aAddr = iLocalAddrResolver->IPv4Addr();
	            aAddr.ConvertToV4Mapped();
	            }
	        else
	            {
	            aAddr = iLocalAddrResolver->IPv6Addr();
	            }
	        }
	    }
	__SIP_ADDR_LOG("CSipConnection::FillWithMatchingAddrFamily returns",aAddr)
    }

// -----------------------------------------------------------------------------
// CSipConnection::DecreaseUserCount
// -----------------------------------------------------------------------------
//
TInt CSipConnection::DecreaseUserCount( const TSIPTransportParams& aParams )
	{
	__SIP_ASSERT_RETURN_VALUE( iUserCount > 0, KErrUnderflow );	
	
	iSettingsList->RemoveSettings( aParams );
	
	// Remove all related transports, default transports are not removed
	//
   	CTransportBase* listItem;
	iListIter.SetToFirst();

	while ( ( listItem = iListIter++ ) != 0 )
		{
		if ( listItem->Remove( aParams ) )
			{
			RemoveTransport( listItem );
			}
		}
		
	iUserCount--;	
	return iUserCount;
	}

// -----------------------------------------------------------------------------
// CSipConnection::IncreaseUserCount
// -----------------------------------------------------------------------------
//
void CSipConnection::IncreaseUserCount()
	{
	iUserCount++;
	}

// -----------------------------------------------------------------------------
// CSipConnection::Handle
// -----------------------------------------------------------------------------
//
TBool CSipConnection::Handle( TUint32 aIapId ) const
	{
	return ( aIapId == iIapId );
	}

// -----------------------------------------------------------------------------
// CSipConnection::IapId
// -----------------------------------------------------------------------------
//	
TUint32 CSipConnection::IapId() const
    { 
    return iIapId;
    }
    
// -----------------------------------------------------------------------------
// CSipConnection::ReserveTransportL
// -----------------------------------------------------------------------------
//		
void CSipConnection::ReserveTransportL( 
    const TSIPTransportParams& aOldTransportParams,
    const TSIPTransportParams& aTransportParams,
    TUint aLocalSendPort,
	TUint aLocalReceivePort,
	const TInetAddr& aRemoteAddr,
	TUint aRemoteSendPort )
    {
    // First reserve ports and save information about transport.
    // - will leave with KErrSIPTransportFailure if some of the ports are 
    // already reserved (if reservations for old transport id exist already,
    // duplicate reservations are allowed).
    
    if ( !iSettingsList->SaveTransportInfoL( aOldTransportParams,
                                             aTransportParams, 
                                             aLocalSendPort, 
                                             aLocalReceivePort,  
                                             aRemoteAddr,
                                             aRemoteSendPort ) )
        {
        // Because no previous settings matching aOldTransportParams were found
        // also listeners don't exist yet, create them
        //
        AddTcpListenerL( aTransportParams, aLocalReceivePort, ETrue );
    	
    	AddUdpListenerL( aTransportParams, aLocalReceivePort, ETrue, ETrue );
        }
   else
        {
        // For existing tcp and udp listeners, add knowledge about second user
        //
        AddTcpListenerUserL( aOldTransportParams, 
                             aTransportParams, 
                             aLocalReceivePort );
                             
        AddUdpListenerUserL( aOldTransportParams, 
                             aTransportParams, 
                             aLocalReceivePort );                  
        }
        
   AddUdpListenerL( aTransportParams, aLocalSendPort, ETrue );
	                                 
   // TCP connection for outgoing requests is not created until sending occurs
   
   }

// -----------------------------------------------------------------------------
// CSipConnection::ReserveTLSTransportL
// -----------------------------------------------------------------------------
//
void CSipConnection::ReserveTLSTransportL( 
    TSIPTransportParams& aTransportParams )
    {
    iSettingsList->SaveTLSTransportInfoL( aTransportParams );
    }
    
// -----------------------------------------------------------------------------
// CSipConnection::RemoveTransport
// -----------------------------------------------------------------------------
//				                
TBool CSipConnection::RemoveReservedTransport( 
    TUint32 aTransportId, 
    MSIPTransportRemovalObserver* aRemovalObserver,
    TInt& aNumAsyncRemovals )
    {
    // First remove corresponding settings
    if( !iSettingsList->RemoveSettings( aTransportId ) )
        {
        return EFalse;
        }
    
    // If some settings were removed there is also transports which has to 
    // be removed
   	CTransportBase* listItem;
	iListIter.SetToFirst();

	while ( ( listItem = iListIter++ ) != 0 )
		{
		if ( listItem->Handle( aTransportId ) )
			{
			// Remove call will remove everything inside the transport related
			// to the transport id and if Remove() returns ETrue, transport
			// can be removed completely (if asynchronous shutdown is
			// not required)
			TBool lastUser = listItem->Remove( aTransportId );
			
			if ( listItem->Shutdown( aTransportId, aRemovalObserver ) )
			    {
			    // Caller has to wait asynchronous completion of transport
			    // removal
			    aNumAsyncRemovals++;
			    }
			else if ( lastUser )
			    {
			    RemoveTransport( listItem );
			    }
			else
			    {
			    }
			}
		}
    
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CSipConnection::Match
// -----------------------------------------------------------------------------
//
TBool CSipConnection::Match( const TDesC8& aAddress )
	{
	TInetAddr address;
	GetLocalAddress( address );

	TBuf16< KMaxAddressLength > addressout16;
	TBuf8< KMaxAddressLength > addressout8;
	address.Output( addressout16 );
	addressout8.Copy( addressout16 );

	return ( addressout8 == aAddress );
	}

// -----------------------------------------------------------------------------
// CSipConnection::CreateTransportResourcesL
// -----------------------------------------------------------------------------
//
void CSipConnection::CreateTransportResourcesL()
	{
	__SIP_LOG( "CreateTransportResourcesL" )
	//Resources must not exist already
	__SIP_ASSERT_LEAVE( !iIcmpReceiver, KErrAlreadyExists );
	__SIP_ASSERT_LEAVE( !iServerResolver, KErrAlreadyExists );
	__SIP_ASSERT_LEAVE( !iLocalAddrResolver, KErrAlreadyExists );

	iIcmpReceiver = CIcmpReceiver::NewL( *this, iServer );
	iServerResolver = CSIPServerResolver::NewL( iServer, Connection() );
	iLocalAddrResolver = 
	    CLocalAddrResolver::NewL( iServer, Connection());
	iRequestQueue = COutgoingRequestQueue::NewL( *this, 
	                                             iNATTraversal, 
	                                             Connection(), 
	                                             iIapId );
	}

// -----------------------------------------------------------------------------
// CSipConnection::CreateDefaultTransportsL
// -----------------------------------------------------------------------------
//
void CSipConnection::CreateDefaultTransportsL()
    {
    TSIPTransportParams defaultParams( KDefaultOwnerId, iIapId );
    if ( !IsListened( KProtocolInetTcp, KDefaultSipPort ) )
        {
        AddTcpListenerL( defaultParams, KDefaultSipPort );
        }
    if ( !IsListened( KProtocolInetUdp, KDefaultSipPort ) )
        {
        AddUdpListenerL( defaultParams, KDefaultSipPort );
        }
    }

// -----------------------------------------------------------------------------
// CSipConnection::ReleaseTransportResources
// -----------------------------------------------------------------------------
//
void CSipConnection::ReleaseTransportResources( TBool aReleaseAllResources )
	{
	__SIP_LOG( "ReleaseTransportResources" )

	delete iIcmpReceiver;
	iIcmpReceiver = NULL;
	RemoveAllTransports();
	RemoveAllTcpSender();
	delete iServerResolver;
	iServerResolver = NULL;
	delete iLocalAddrResolver;
	iLocalAddrResolver = NULL;
	delete iRequestQueue;
	iRequestQueue = NULL;

    if ( aReleaseAllResources )
        {
#if ( !defined( __WINS__ ) && !defined( __WINSCW__ ) )
	    // In HW RConnection must be closed when network coverage is lost	
   	    CloseConnection();      
#endif
        }
	}

// -----------------------------------------------------------------------------
// CSipConnection::DoTransportResourcesExist
// -----------------------------------------------------------------------------
//
TBool CSipConnection::DoTransportResourcesExist() const
	{
	return ( iIcmpReceiver != 0 &&
			 iServerResolver != 0 &&
			 iLocalAddrResolver != 0 &&
			 iRequestQueue != 0 );
	}

// -----------------------------------------------------------------------------
// CSipConnection::OpenConnection
// -----------------------------------------------------------------------------
//
TInt CSipConnection::OpenConnection()
	{
	return iConnection->OpenConnection( iIapId, iServer );
	}

// -----------------------------------------------------------------------------
// CSipConnection::CloseConnection
// -----------------------------------------------------------------------------
//
void CSipConnection::CloseConnection()
	{
	if ( iConnection )
		{
		iConnection->CloseConnection();
		}
	}

// -----------------------------------------------------------------------------
// CSipConnection::ResolveLocalAddress
// -----------------------------------------------------------------------------
//
void CSipConnection::ResolveLocalAddress( TInetAddr& aAddress )
	{
	GetLocalAddress( aAddress );
	}

// -----------------------------------------------------------------------------
// CSipConnection::StartMonitoringL
// -----------------------------------------------------------------------------
//
void CSipConnection::StartMonitoringL( TInt aError )
	{
#ifdef CPPUNIT_TEST
	aError; // silence warning
#else
	// In unit testing hard coded IAPs cannot be expected 
	// to be valid in all environments. 
	// Not trying to load monitor plug-in by IAP in unit tests 
    iNetworkManager.RegisterObserverL( *iStateModel, iIapId, aError );
#endif
	}

// -----------------------------------------------------------------------------
// CSipConnection::StopMonitoring
// -----------------------------------------------------------------------------
//
void CSipConnection::StopMonitoring()
	{
	iNetworkManager.Deregister( *iStateModel );
	}
	
// -----------------------------------------------------------------------------
// CSipConnection::ContinueMonitoring
// -----------------------------------------------------------------------------
//
TBool CSipConnection::ContinueMonitoring( TInt aError )
	{
	return iNetworkManager.ContinueMonitoring( *iStateModel, aError );
	}
	
// -----------------------------------------------------------------------------
// CSipConnection::MonitorConnectionClosure
// -----------------------------------------------------------------------------
//	
TInt CSipConnection::MonitorConnectionClosure()
    {
    return iConnection->MonitorConnectionClosure( iServer );
    }	

// -----------------------------------------------------------------------------
// CSipConnection::StateModelChanged
// -----------------------------------------------------------------------------
//
void CSipConnection::StateModelChanged( CStateModel::TState aState )
	{
	CSIPConnection::TState oldState = iState;
	SetState( aState );
	if( oldState != iState )
		{
		TRAP_IGNORE( 
		    iTUNetStatusObserver.ConnectionStateChangedL( iIapId, iState ) )
		}
	}
	
// -----------------------------------------------------------------------------
// CSipConnection::Destroy
// -----------------------------------------------------------------------------
//	
void CSipConnection::Destroy()
	{
	iOwner.Destroy( iIapId );
	}

// -----------------------------------------------------------------------------
// CSipConnection::DetermineConnectionTypeL
// -----------------------------------------------------------------------------
//
TInt CSipConnection::DetermineConnectionTypeL()
	{
	TInt bearer = KErrNotFound;
  CMDBSession* cdb = CMDBSession::NewLC( KCDVersion1_1 ); 
  cdb->SetAttributeMask( ECDHidden );
  // Create an iap record
  CCDIAPRecord* iapRecord = static_cast<CCDIAPRecord*>( 
              CCDRecordBase::RecordFactoryL( KCDTIdIAPRecord ) );
  CleanupStack::PushL( iapRecord );
  iapRecord->SetRecordId( IapId() );
  iapRecord->LoadL( *cdb );
  TPtrC bearerType(iapRecord->iBearerType);
  if(bearerType.CompareF(TPtrC(KCDTypeNameLANBearer)) == 0)
	bearer =  MSIPConnectionContext::ELanBearer;
  else if(bearerType.CompareF(TPtrC(KCDTypeNameModemBearer)) == 0)
	bearer =  MSIPConnectionContext::EModemBearer;
  CleanupStack::PopAndDestroy( iapRecord );
  CleanupStack::PopAndDestroy( cdb );
  return bearer;
	}



// -----------------------------------------------------------------------------
// CSipConnection::State
// -----------------------------------------------------------------------------
//	
CSIPConnection::TState CSipConnection::State()
	{
	return iState;
	}

// -----------------------------------------------------------------------------
// CSipConnection::SystemStarted
// -----------------------------------------------------------------------------
//	
void CSipConnection::SystemStarted()
    {
    iStateModel->EnterState( MStateModel::EInit, KErrNone );
    }

// -----------------------------------------------------------------------------
// CSipConnection::CellularUsageAllowed
// -----------------------------------------------------------------------------
//	

void CSipConnection::CellularUsageAllowed()
{
	if(iWaitingForDataUsage)
	{
	iWaitingForDataUsage=EFalse;
	__SIP_LOG( "CSipConnection::CellularUsageAllowed Execution" )
    iStateModel->EnterState( MStateModel::EInit, KErrNone );
	}
}


// -----------------------------------------------------------------------------
// CSipConnection::TopViaHeader
// -----------------------------------------------------------------------------
//
CSIPViaHeader* CSipConnection::TopViaHeader( CSIPMessage* aMessage )
	{
	CSIPViaHeader* via = NULL;
	if( aMessage->HasHeader( SIPStrings::StringF(SipStrConsts::EViaHeader) ) )
		{
		TSglQueIter< CSIPHeaderBase > iter =
			aMessage->Headers( SIPStrings::StringF(SipStrConsts::EViaHeader) );
		CSIPHeaderBase* header = iter;
		via = static_cast< CSIPViaHeader* >( header );
		}
	return via;
	}

// -----------------------------------------------------------------------------
// CSipConnection::SetState
// -----------------------------------------------------------------------------
//	
void CSipConnection::SetState ( MStateModel::TState aState )
	{
	switch( aState )
		{
		case MStateModel::EInit:
			{
			iState = CSIPConnection::EInit;
			break;
			}
		case MStateModel::EInactiveMonitoring:
		case MStateModel::EInactiveConnecting:
			{
			iState = CSIPConnection::EInactive;
			break;
			}
		case MStateModel::EActiveResolving:
			{
			// Does not yet change the current SIP connection state.
			// The SIP connection state changes with the next state transition.
			break;	
			}			
		case MStateModel::EActiveResolved:
			{
			iState = CSIPConnection::EActive;
			break;
			}
		case MStateModel::ESuspended:
			{
			iState = CSIPConnection::ESuspended;
			break;
			}
		case MStateModel::EUnavailable:
		default:
			{
			iState = CSIPConnection::EUnavailable;
			break;
			}
		}	
	}
	
// -----------------------------------------------------------------------------
// CSipConnection::Connection
// -----------------------------------------------------------------------------
//
RConnection& CSipConnection::Connection()
	{
	return iConnection->Connection();
	}	

// -----------------------------------------------------------------------------
// CSipConnection::NotifyIcmpErrorL
// -----------------------------------------------------------------------------
//	
void CSipConnection::NotifyIcmpErrorL( const TInetAddr& aAddress )
    {
    CSipConnectionMgr::TICMPError error = CSipConnectionMgr::ENetUnreachable;
	iFinder.IcmpErrorL( aAddress, error );
	CTransportBase* listItem;
	iListIter.SetToFirst();
	while ( ( listItem = iListIter++ ) != 0 )
		{
		listItem->ICMPErrorL( error, aAddress );
		}
    }

// -----------------------------------------------------------------------------
// CSipConnection::IsConnectionActive
// -----------------------------------------------------------------------------
//   
TBool CSipConnection::IsConnectionActive() const
    {
    return ( iState == CSIPConnection::EActive );
    }

// -----------------------------------------------------------------------------
// CSipConnection::UpdateURIL
// -----------------------------------------------------------------------------
// 
void CSipConnection::UpdateURIL( 
    CURIContainer& aURI, 
    const TInetAddr& aAddress,
    TUint aProtocol ) const
    {
    __ASSERT_ALWAYS( aURI.IsSIPURI(), User::Leave( KErrSIPInvalidURIType ) );
	
	CSIPURI& uri = *aURI.SIPURI();
	
	RStringF transport;
    if ( aProtocol == KProtocolInetTcp )
        {
        transport = SIPStrings::StringF( SipStrConsts::ETCP );
        }
    else if ( aProtocol == KProtocolTls )
        {
        transport = SIPStrings::StringF( SipStrConsts::ETLS );
        }
    else
        {
        User::Leave( KErrArgument );
        }
	
    uri.SetParamL( SIPStrings::StringF( SipStrConsts::ETransport ), transport );

    uri.HostPort().SetPort( aAddress.Port() );
    
    HBufC8* addressText = CTransport::ConvertInetAddrToTextL( aAddress );
    CleanupStack::PushL( addressText );
    uri.HostPort().SetHostL( *addressText );
    CleanupStack::PopAndDestroy( addressText );
    }
    
// -----------------------------------------------------------------------------
// CSipConnection::CSipConnection
// -----------------------------------------------------------------------------
//
CSipConnection::CSipConnection( MTransactionFinder& aFinder,
								MServerTaFactory& aTaFactory,
								MTimerManager& aTimer,
								TUint aTOne,
								const TSIPTransportParams& aParams,
								RSocketServ& aServer,
								MSigCompController& aSigCompHandler,
								MConnectionStateObserver& aTUNetStatusObserver,
								CNetworkManager& aNetworkManager,
								MSipConnectionOwner& aOwner,
								CNetworkInfo& aNetworkInfo,
								CSIPNATTraversalController& aNATTraversal )
	: iFinder( aFinder ),
	  iTaFactory( aTaFactory ),
	  iTUNetStatusObserver( aTUNetStatusObserver ),
	  iSigCompHandler( aSigCompHandler ),
	  iServer( aServer ),
	  iList( CTransportBase::iOffset ),
	  iListIter( iList ),
	  iTcpSenderList( CTcpResponseSender::iSenderOffset ),
	  iTcpSenderListIter( iTcpSenderList ),
	  iTOne( aTOne ),
	  iNetworkManager( aNetworkManager ),
	  iTimer( aTimer ),
	  iOwner( aOwner ),
	  iIapId( aParams.IapId() ),
	  iState( CSIPConnection::EInit ),
	  iNetworkInfo( aNetworkInfo ),
	  iNATTraversal( aNATTraversal )
	{
	}

// -----------------------------------------------------------------------------
// CSipConnection::~CSipConnection
// -----------------------------------------------------------------------------
//
CSipConnection::~CSipConnection()
	{	
	delete iSettingsList;
	iNetworkManager.Deregister( *iStateModel );
	delete iStateModel;
     // Use scope operator for keeping PC-Lint happy
	CSipConnection::ReleaseTransportResources( ETrue );
	delete iConnection;	
	}

// -----------------------------------------------------------------------------
// CSipConnection::ConstructL
// -----------------------------------------------------------------------------
//
void CSipConnection::ConstructL()
	{	
	iWaitingForDataUsage = EFalse;
	iSettingsList = COwnerSettingsList::NewL();	
	iStateModel = CStateModel::NewL( *this, iTimer );
	iConnection = CConnectionContainer::NewL( *iStateModel );	
    if ( iOwner.IsSystemStarted())
		{ 
		if ((ConnectionType() == MSIPConnectionContext::EModemBearer) && (!iOwner.IsCellularDataUsageAllowed()) )
			{
			iWaitingForDataUsage = ETrue;
			__SIP_LOG( "CSipConnection::ConstructL() is blocking Connection Start Up..." )
			return;
			}
		else
			{
			__SIP_LOG( "CSipConnection::ConstructL() is Allowing Connection Start Up..." )
	        iStateModel->EnterState( MStateModel::EInit, KErrNone );
			}
       	}
	}

// -----------------------------------------------------------------------------
// CSipConnection::ConnectionError
// -----------------------------------------------------------------------------
//
TInt CSipConnection::ConnectionError()
	{
	return iStateModel->Error();
	}

// -----------------------------------------------------------------------------
// CSipConnection::ConnectionType()
// -----------------------------------------------------------------------------
//
TInt CSipConnection::ConnectionType()
	{
	TInt type = MSIPConnectionContext::EModemBearer;
	TRAP_IGNORE( type = DetermineConnectionTypeL( ) )
	return type;
	}

