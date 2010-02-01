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
// Name          : CSipCSSubSession.cpp
// Part of       : SIPServerCore
// Version       : SIP/6.0 
//



#include "SipCSServer.pan"
#include "CSipCSSubSession.h"
#include "CSipCSSession.h"
#include "CSipServerCore.h"
#include "CSipCSServerITC.h"
#include "CSipCSSubSessionReceiver.h"
#include "CTransactionUser.h"
#include "CSipDialogMgr.h"
#include "CSIPRegistrationMgr.h"
#include "MSipRegistrations.h"
#include "MSipRefreshMgr.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "siprouteheader.h"
#include "sipexpiresheader.h"
#include "siptoheader.h"
#include "sipfromheader.h"
#include "sipcontactheader.h"
#include "sipcallidheader.h"
#include "sipaddress.h"
#include "uricontainer.h"
#include "sipuri.h"
#include "TSIPRemoteTargetAndProxy.h"
#include "SipAssert.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipcodecutils.h"
#include "sipsec.h"
#include "CSipConnectionMgr.h"


// -----------------------------------------------------------------------------
// CSipCSSubSession::NewLC
// -----------------------------------------------------------------------------
//
CSipCSSubSession* CSipCSSubSession::NewLC (CSipCSSession& aSession,
                                           MSIPSecUser& aSIPSecUser,
                                           TUint32 aIapId)
	{
    CSipCSSubSession* self =
        new(ELeave)CSipCSSubSession(aSession,aSIPSecUser,aIapId);
    CleanupClosePushL(*self); // CObject: Close will call the destructor.
    self->ConstructL();
    return self;
	}

// -----------------------------------------------------------------------------
// CSipCSSubSession::CSipCSSubSession
// -----------------------------------------------------------------------------
//
CSipCSSubSession::CSipCSSubSession (CSipCSSession& aSession,
                                    MSIPSecUser& aSIPSecUser,
									TUint32 aIapId)
: iSession (aSession),
  iSIPSecUser (aSIPSecUser),
  iTransportParams (aSession.ServerCore().CreateTransportOwnerId(), aIapId),
  iState (CSIPConnection::EInit),
  iIapOpened (EFalse)
	{
	}

// -----------------------------------------------------------------------------
// CSipCSSubSession::ConstructL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::ConstructL ()
	{
    iReceiver = CSipCSSubSessionReceiver::NewL(ITC(),iSIPSecUser,*this);
	}

// -----------------------------------------------------------------------------
// CSipCSSubSession::~CSipCSSubSession
// -----------------------------------------------------------------------------
//
CSipCSSubSession::~CSipCSSubSession()
	{
	if (iReceiver)
		{
        Server().DialogMgr().TerminateDialogsFor(iReceiver);
		Server().RegistrationMgr().ClearRegistrationOwner(iReceiver);
		Server().RefreshMgr().RemoveRefreshesFor(iReceiver);
		Server().TU().ClearTransactionOwner(iReceiver);
		}
	Server().SIPSec().ClearCache(&iSIPSecUser);
    delete iReceiver;
    if (iIapOpened && iState != CSIPConnection::EUnavailable)
        {
        // When a connection goes unavailable, 
        // ConnectionMgr destroys it immediately. 
        // After this the connection must not be closed from here, 
        // since this might mix up the reference count in ConnectionMgr.
        Server().ConnectionMgr().Close(iTransportParams);
        }
	} 

// -----------------------------------------------------------------------------
// CSipCSSubSession::TransportParams
// From MSIPCSReceiverOwner
// -----------------------------------------------------------------------------
//
const TSIPTransportParams& CSipCSSubSession::TransportParams() const
    {
    return iTransportParams;
    }

// -----------------------------------------------------------------------------
// CSipCSSubSession::TransportParams
// From MSIPCSReceiverOwner
// -----------------------------------------------------------------------------
//
TSIPTransportParams CSipCSSubSession::TransportParams(
    const TRegistrationId& aRegistrationId)
    {
    TSIPTransportParams transportParams(iTransportParams);
    if (aRegistrationId != KEmptyRegistrationId)
        {
        Server().RegistrationMgr().FillTransportParams(aRegistrationId,
                                                       transportParams);    
                                                           
        // Filling params overrides ownerid, set correct value back
        transportParams.SetOwnerId( iTransportParams.OwnerId() );
        }
    return transportParams;
    }

// -----------------------------------------------------------------------------
// CSipCSSubSession::SIPSecUser
// From MSIPCSReceiverOwner
// -----------------------------------------------------------------------------
//
const MSIPSecUser* CSipCSSubSession::SIPSecUser(
    const TRegistrationId& aRegistrationId)
    {
    return Server().RegistrationMgr().SIPSecUser(aRegistrationId);
    }

// -----------------------------------------------------------------------------
// CSipCSSubSession::TransactionOwner
// -----------------------------------------------------------------------------
//
MTransactionOwner* CSipCSSubSession::TransactionOwner ()
	{
    return iReceiver;
	}

// -----------------------------------------------------------------------------
// CSipCSSubSession::Receiver
// -----------------------------------------------------------------------------
//	
CSipCSReceiverBase& CSipCSSubSession::Receiver()
    {
    return *iReceiver;
    }

// -----------------------------------------------------------------------------
// CSipCSSubSession::IapId
// -----------------------------------------------------------------------------
//
TUint32 CSipCSSubSession::IapId () const
    {
    return iTransportParams.IapId();
    }

// -----------------------------------------------------------------------------
// CSipCSSubSession::OpenIapL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::OpenIapL (const RMessage2& aMessage)
    {
    iState = Server().ConnectionMgr().OpenL(iTransportParams);
    iIapOpened = ETrue;
    ITC().WriteL(aMessage,iState);
    }

// -----------------------------------------------------------------------------
// CSipCSSubSession::SetStateL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::SetStateL (CSIPConnection::TState aState)
	{
    if (iState != aState)
        {
        iState = aState;
	    iReceiver->ConnectionStateChangedL(aState);
        }
	}

// -----------------------------------------------------------------------------
// CSipCSSubSession::ServiceL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::ServiceL (TSIPIds& aIds, 
                                 TInt aFunction, 
                                 const RMessage2& aMessage)
    {
    switch (aFunction)
		{
		case ESipItcGetLocalAddr:
		    GetLocalAddrL(aMessage); break;
		
		case ESipItcSetSIPSockOpt:
		    SetSIPSockOptL(aMessage); break;
		    
		case ESipItcSetSIPSockOptWithDes:
		    SetSIPSockOptL(aMessage,ETrue); break;
		
		case ESipItcSetOutboundProxy:
            SetOutboundProxyL(aIds,aMessage); break;		
		
		case ESipItcOutboundProxy:
            OutboundProxyL(aIds,aMessage); break;

		case ESipItcRemoveOutboundProxy: 
			RemoveOutboundProxyL(aIds); break;

        case ESipItcRegister:
            RegisterL(aIds,aMessage); break;

		case ESipItcRegisterAndSetOutboundProxy:
			RegisterAndSetOutboundProxyL(aIds,aMessage); break;

        case ESipItcUpdateRegistration:
            UpdateRegistrationL(aIds,aMessage); break;

        case ESipItcUnRegister:
            UnRegisterL(aIds,aMessage); break;

        case ESipItcFetchRegistrations:
            FetchRegistrationsL(aMessage); break;

        case ESipItcSendRequest:
            SendRequestL(aIds,aMessage); break;

		case ESipItcSendRequestAndUpdateRefresh:
			SendRequestAndUpdateRefreshL (aIds,aMessage); break;

		case ESipItcSendRequestAndTerminateRefresh:
			SendRequestAndTerminateRefreshL (aIds,aMessage); break;

		case ESipItcSendResponse:
            SendResponseL(aIds,aMessage); break;

		case ESipItcSendCancel:
            SendCancelL(aIds,aMessage); break;

        case ESipItcSendResponseAndCreateDialog:
            SendResponseAndCreateDialogL(aIds,aMessage); break;

        case ESipItcSendResponseInDialog:
            SendResponseInDialogL(aIds,aMessage); break;

        case ESipItcSendRequestAndCreateDialog:
            SendRequestAndCreateDialogL(aIds,aMessage); break;

        case ESipItcSendRequestInDialog:
            SendRequestInDialogL(aIds,aMessage); break;

        case ESipItcSendRequestInDialogAndUpdateRefresh:
            SendRequestInDialogAndUpdateRefreshL(aIds,aMessage); break;

		case ESipItcTerminateDialog:
            TerminateDialogL(aIds.iDialogId); break;

		case ESipItcTerminateRefresh:
            TerminateRefreshL(aIds.iRefreshId); break;

		case ESipItcTerminateDialogRefresh:
            TerminateDialogRefreshL(aIds.iDialogId,aIds.iRefreshId); break;

        case ESipItcTerminateRegistration:
            TerminateRegistrationL(aIds.iRegistrationId); break;

        case ESipItcConnectionReadyToReceive: // Async. Do not complete yet.
            ClientReadyToReceiveL(aMessage); return;  

        case ESipItcConnectionReceiveSipMessage: 
            ReceiveSipMessageL(aMessage); break;

        case ESipItcConnectionCancelReceive: 
            CancelClientReceiveL(); break;

        case ESipItcGetFromHeader: 
            GetFromHeaderL(aIds.iRegistrationId,aMessage); break;
            
        case ESipItcSetRefreshInterval: 
            SetRefreshIntervalL(aIds.iRefreshId,aMessage); break;
            
        case ESipItcGetRefreshInterval: 
            GetRefreshIntervalL(aIds.iRefreshId,aMessage); break;
            
		case ESipItcGetCallIDHeader:
		    GetCallIDHeaderL(aIds.iDialogId,aMessage); break;
		    
		case ESipItcGetLocalTag:
		    GetLocalTagL(aIds.iDialogId,aMessage); break;

		case ESipItcGetRegisteredContact:
		    GetRegisteredContactL(aIds.iRegistrationId,aMessage); break;

        case ESipItcResetDialogState:
            ResetDialogStateL(aIds.iDialogId); break;
            
        case ESipItcRefreshConnection:
        	RefreshConnectionL(aIds.iIapId); break;
        	
        case ESipItcGetConnectionError:
        	GetConnectionErrorL(aIds,aMessage); break;
        	
        default: ITC().PanicClient (aMessage, EBadRequest); break;
        }
    ITC().Complete (aMessage, KErrNone);
    }
  
// -----------------------------------------------------------------------------
// CSipCSSubSession::GetLocalAddrL
// -----------------------------------------------------------------------------
//  
void CSipCSSubSession::GetLocalAddrL(const RMessage2& aMessage)
    {
    TInt err = Server().ConnectionMgr().GetLocalAddress(
        iTransportParams.IapId(),iLocalAddr);
    User::LeaveIfError(err);
    ITC().WriteL(aMessage,iLocalAddr);
    }

// -----------------------------------------------------------------------------
// CSipCSSubSession::SetSIPSockOptL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::SetSIPSockOptL(const RMessage2& aMessage, TBool aDesOpt)
    {
    TSIPSockOpt opt(ITC().ReadSIPSockOptL(aMessage));
    if (aDesOpt)
        {
        HBufC8* desOptBuf = ITC().ReadLC(aMessage,ESipItcArgSockOptDes);
        Server().ConnectionMgr().SetOptL(
            iTransportParams, opt.iOptionName, opt.iOptionLevel, *desOptBuf);
        CleanupStack::PopAndDestroy(desOptBuf);
        }
    else
        {
        Server().ConnectionMgr().SetOptL(
            iTransportParams, opt.iOptionName, opt.iOptionLevel, opt.iOption);        
        }
    }

// -----------------------------------------------------------------------------
// CSipCSSubSession::SetOutboundProxyL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::SetOutboundProxyL (TSIPIds& aIds,
                                          const RMessage2& aMessage)
	{
	CSIPRouteHeader* proxy = ITC().ReadProxyLC(aMessage);
	Server().RegistrationMgr().SetOutboundProxyL(aIds.iRegistrationId,proxy,
	                                             iReceiver);
	CleanupStack::Pop(proxy);
	}

// -----------------------------------------------------------------------------
// CSipCSSubSession::OutboundProxyL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::OutboundProxyL (TSIPIds& aIds, const RMessage2& aMessage)
	{
    if (Server().Registrations().HasOutboundProxy(aIds.iRegistrationId))
        {
	    const CSIPRouteHeader* proxy = 
		    Server().Registrations().OutboundProxy(aIds.iRegistrationId);
		if (proxy)
		    {
		    ITC().WriteL(aMessage,*proxy);
		    }
        else
            {
            User::Leave(KErrNotFound);
            }
        }
	}

// -----------------------------------------------------------------------------
// CSipCSSubSession::RemoveOutboundProxyL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::RemoveOutboundProxyL (TSIPIds& aIds)
	{
	TInt err = Server().RegistrationMgr().RemoveOutboundProxy(
	    aIds.iRegistrationId, iReceiver);
	if (err != KErrNone)
        {
        User::Leave(err);
        }
	}

// -----------------------------------------------------------------------------
// CSipCSSubSession::RegisterL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::RegisterL (TSIPIds& aIds, const RMessage2& aMessage)
	{
	CSIPRequest* request = 
	    CSIPRequest::NewLC(SIPStrings::StringF(SipStrConsts::ERegister));
    CURIContainer* remoteTarget = FillRequestLC(aMessage,*request);
    CheckSigCompInNextHopL(KEmptyRegistrationId,*remoteTarget,*request);
    
    Server().RegistrationMgr().RegisterL(aIds.iRequestId,aIds.iRegistrationId,
                                         request,*remoteTarget,iReceiver,
                                         iSIPSecUser,0,aIds.iRefresh,
                                         aIds.iSendWithExpires,
                                         aIds.iCacheOutboundProxyIP);
    CleanupStack::PopAndDestroy(remoteTarget);
    CleanupStack::Pop(request);
    ITC().WriteL(aMessage,aIds);
	}

// -----------------------------------------------------------------------------
// CSipCSSubSession::RegisterAndSetOutboundProxyL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::RegisterAndSetOutboundProxyL (TSIPIds& aIds,
                                                     const RMessage2& aMessage)
	{
	CSIPRequest* request = 
	    CSIPRequest::NewLC(SIPStrings::StringF(SipStrConsts::ERegister));
    TSIPRemoteTargetAndProxy remoteTargetAndProxy;
    FillRequestL(aMessage,*request,remoteTargetAndProxy,EFalse,ETrue);
    CleanupStack::PushL (remoteTargetAndProxy.iProxy);
    CleanupStack::PushL (remoteTargetAndProxy.iRemoteTarget);
    CURIContainer* remoteTarget = remoteTargetAndProxy.iRemoteTarget;
    CheckSigCompInNextHopL(KEmptyRegistrationId,*remoteTarget,*request);

    Server().RegistrationMgr().RegisterL(aIds.iRequestId,aIds.iRegistrationId,
                                         request,*remoteTarget,
                                         iReceiver,iSIPSecUser,
                                         remoteTargetAndProxy.iProxy, 
                                         aIds.iRefresh,
                                         aIds.iSendWithExpires,
                                         aIds.iCacheOutboundProxyIP);
    CleanupStack::PopAndDestroy(remoteTargetAndProxy.iRemoteTarget);
    CleanupStack::Pop(remoteTargetAndProxy.iProxy);
    CleanupStack::Pop(request);    
    ITC().WriteL (aMessage,aIds);
	}

// -----------------------------------------------------------------------------
// CSipCSSubSession::UpdateRegistrationL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::UpdateRegistrationL (TSIPIds& aIds,
                                            const RMessage2& aMessage)
	{
	CSIPRequest* request = 
	    CSIPRequest::NewLC(SIPStrings::StringF(SipStrConsts::ERegister));
    ITC().ReadRequestL (aMessage,*request);

    Server().RegistrationMgr().UpdateRegistrationL(aIds.iRegistrationId,
	                                               aIds.iRequestId, request,
	                                               iReceiver);
    CleanupStack::Pop(request);
    ITC().WriteL(aMessage,aIds);
	}

// -----------------------------------------------------------------------------
// CSipCSSubSession::UnRegisterL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::UnRegisterL (TSIPIds& aIds, const RMessage2& aMessage)
	{
	CSIPRequest* request = 
	    CSIPRequest::NewLC(SIPStrings::StringF(SipStrConsts::ERegister));
    ITC().ReadRequestL (aMessage,*request);

    Server().RegistrationMgr().UnRegisterL(aIds.iRegistrationId, 
                                           aIds.iRequestId, request,
                                           iReceiver);
    CleanupStack::Pop(request);
    ITC().WriteL(aMessage,aIds);
	}

// -----------------------------------------------------------------------------
// CSipCSSubSession::FetchRegistrationsL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::FetchRegistrationsL (const RMessage2& aMessage)
	{
	TSIPIds ids;
	CSIPRequest* request = 
	    CSIPRequest::NewLC(SIPStrings::StringF(SipStrConsts::ERegister));
    CURIContainer* remoteTarget = FillRequestLC(aMessage,*request);
    CheckSigCompInNextHopL(KEmptyRegistrationId,*remoteTarget,*request);

    Server().RegistrationMgr().FetchRegistrationsL(iTransportParams, 
                                                   ids.iRequestId, request,
                                                   *remoteTarget, iReceiver);
    CleanupStack::PopAndDestroy(remoteTarget);
    CleanupStack::Pop(request);
    ITC().WriteL(aMessage,ids);
	}

// -----------------------------------------------------------------------------
// CSipCSSubSession::SendRequestL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::SendRequestL (TSIPIds& aIds,
                                     const RMessage2& aMessage)
	{
    CSIPRequest* request = CSIPRequest::NewLC();
    CURIContainer* remoteTarget = FillRequestLC(aMessage,*request,ETrue);
    CheckSigCompInNextHopL(aIds.iRegistrationId,*remoteTarget,*request);
    AddFromL (aIds.iRegistrationId, *request);
    AddRouteL (aIds.iRegistrationId, *request);
	if (aIds.iRefresh)
		{
		Server().RefreshMgr().RefreshL (aIds.iRequestId, aIds.iRefreshId,
                                        aIds.iRegistrationId, request,
                                        iReceiver, iSIPSecUser,
                                        *remoteTarget, ETrue, EFalse);
		}
	else
		{
		Server().TU().SendL (aIds.iRequestId, aIds.iRegistrationId, request, 
		                     iReceiver, *remoteTarget, 
		                     TransportParams(aIds.iRegistrationId), ETrue);
		}
    CleanupStack::PopAndDestroy(remoteTarget);
    CleanupStack::Pop(request);
    ITC().WriteL(aMessage,aIds);
	}

// -----------------------------------------------------------------------------
// CSipCSSubSession::SendRequestAndUpdateRefreshL
// -----------------------------------------------------------------------------
//
void 
CSipCSSubSession::SendRequestAndUpdateRefreshL (TSIPIds& aIds,
                                                const RMessage2& aMessage)
	{
    CSIPRequest* request = CSIPRequest::NewLC();
	ITC().ReadRequestL(aMessage,*request);
	Server().RefreshMgr().UpdateRefreshL(aIds.iRequestId, aIds.iRefreshId, 
	                                     request, iReceiver, ETrue);
    CleanupStack::Pop(request);
    ITC().WriteL(aMessage,aIds);
	}

// -----------------------------------------------------------------------------
// CSipCSSubSession::SendRequestAndTerminateRefreshL
// -----------------------------------------------------------------------------
//
void 
CSipCSSubSession::SendRequestAndTerminateRefreshL (TSIPIds& aIds,
                                                   const RMessage2& aMessage)
	{
    CSIPRequest* request = CSIPRequest::NewLC();
	ITC().ReadRequestL (aMessage,*request);
	Server().RefreshMgr().TerminateRefreshL(aIds.iRequestId, aIds.iRefreshId, 
	                                        request, iReceiver, ETrue);
    CleanupStack::Pop(request);
    ITC().WriteL(aMessage,aIds);
	}

// -----------------------------------------------------------------------------
// CSipCSSubSession::SendResponseL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::SendResponseL (TSIPIds& aIds, const RMessage2& aMessage)
	{
    CSIPResponse* response = ITC().ReadResponseLC(aMessage);
    CheckSigCompInContactsL(*response);
    Server().TU().SendL(aIds.iRequestId,response,iTransportParams);
    CleanupStack::Pop(response);
	}

// -----------------------------------------------------------------------------
// CSipCSSubSession::SendCancelL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::SendCancelL (TSIPIds& aIds, const RMessage2& aMessage)
	{
	TTransactionId cancelId;
	Server().TU().SendCancelL(aIds.iRequestId,cancelId,iReceiver);
	aIds.iRequestId = cancelId;
	ITC().WriteL(aMessage,aIds);
	}

// -----------------------------------------------------------------------------
// CSipCSSubSession::SendResponseAndCreateDialogL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::SendResponseAndCreateDialogL (TSIPIds& aIds, 
                                                     const RMessage2& aMessage)
    {
    CSIPResponse* response = ITC().ReadResponseLC(aMessage);
    // Client must not add a sigcomp-parameter to Contact of a response
    CheckSigCompInContactsL(*response);
    AddContactL(aIds.iRegistrationId, *response);
    Server().DialogMgr().SendAndCreateDialogL(aIds.iRequestId, aIds.iRegistrationId, aIds.iDialogId, 
                                              response, iReceiver, iSIPSecUser);
    CleanupStack::Pop(response);
    ITC().WriteL(aMessage,aIds);
    }
    
// -----------------------------------------------------------------------------
// CSipCSSubSession::SendResponseInDialogL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::SendResponseInDialogL (TSIPIds& aIds, 
                                              const RMessage2& aMessage)
    {
    CSIPResponse* response = ITC().ReadResponseLC(aMessage);
    // Client must not add a sigcomp-parameter to Contact of a response
    CheckSigCompInContactsL(*response);  
    AddContactL(aIds.iRegistrationId, *response);
    Server().DialogMgr().SendWithinDialogL(aIds.iRequestId, aIds.iDialogId,
                                           response, iReceiver,
                                           aIds.iTargetRefresh);
    CleanupStack::Pop(response);
    }
    
// -----------------------------------------------------------------------------
// CSipCSSubSession::SendRequestAndCreateDialogL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::SendRequestAndCreateDialogL (TSIPIds& aIds, 
                                                    const RMessage2& aMessage)
    {
    CSIPRequest* request = CSIPRequest::NewLC();
    CURIContainer* remoteTarget = FillRequestLC(aMessage,*request,ETrue);
    AddFromL(aIds.iRegistrationId, *request);
    AddContactL(aIds.iRegistrationId, *request);
    AddRouteL(aIds.iRegistrationId, *request);

	Server().DialogMgr().SendAndCreateDialogL(aIds.iRequestId, aIds.iRefreshId,
                                              aIds.iRegistrationId, 
                                              aIds.iDialogId, request,
                                              *remoteTarget, iReceiver,
                                              iSIPSecUser, aIds.iRefresh);
    CleanupStack::PopAndDestroy(remoteTarget);
    CleanupStack::Pop(request);
    ITC().WriteL(aMessage,aIds);
    }

// -----------------------------------------------------------------------------
// CSipCSSubSession::SendRequestInDialogL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::SendRequestInDialogL (TSIPIds& aIds, 
                                             const RMessage2& aMessage)
    {
    CSIPRequest* request = CSIPRequest::NewLC();
    ITC().ReadRequestL(aMessage,*request,ETrue);
    TBool allowOverlapping = OverlappingInDialogAllowed(request->Method());
    
	Server().DialogMgr().SendWithinDialogL(aIds.iRequestId, aIds.iRefreshId,
	                                       aIds.iDialogId, request, 
	                                       iReceiver, aIds.iRefresh,
                                           allowOverlapping,
                                           aIds.iTargetRefresh);	
    CleanupStack::Pop(request);
    ITC().WriteL(aMessage,aIds);
    }

// -----------------------------------------------------------------------------
// CSipCSSubSession::SendRequestInDialogAndUpdateRefreshL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::SendRequestInDialogAndUpdateRefreshL (
    TSIPIds& aIds,
    const RMessage2& aMessage)
    {
    CSIPRequest* request = CSIPRequest::NewLC();
    ITC().ReadRequestL(aMessage,*request);

	Server().DialogMgr().SendAndUpdateRefreshL(aIds.iRequestId, aIds.iRefreshId,
                                               aIds.iDialogId, request,
                                               iReceiver);

    CleanupStack::Pop(request);
    ITC().WriteL(aMessage,aIds);
    }   

// -----------------------------------------------------------------------------
// CSipCSSubSession::TerminateDialogL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::TerminateDialogL (const TDialogId& aId)
	{
	TInt err = Server().DialogMgr().TerminateDialog(aId,iReceiver);
    if (err != KErrNone)
        {
        User::Leave (err);
        }
	}

// -----------------------------------------------------------------------------
// CSipCSSubSession::TerminateRefreshL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::TerminateRefreshL (const TRefreshId& aId)
	{
	TInt err = Server().RefreshMgr().TerminateRefresh(aId,iReceiver);
	if (err != KErrNone)
        {
        User::Leave(err);
        }
	}

// -----------------------------------------------------------------------------
// CSipCSSubSession::TerminateDialogRefreshL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::TerminateDialogRefreshL (const TDialogId& aDialogId,
                                                const TRefreshId& aRefreshId)
    {
	TInt err = Server().DialogMgr().TerminateDialogRefresh(
	    aDialogId,aRefreshId,iReceiver);
	if (err != KErrNone)
        {
        User::Leave(err);
        }
    }

// -----------------------------------------------------------------------------
// CSipCSSubSession::TerminateRegistrationL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::TerminateRegistrationL (const TRegistrationId& aId)
    {
	TInt err = Server().RegistrationMgr().TerminateRegistration(aId,iReceiver);
	if (err != KErrNone)
        {
        User::Leave(err);
        }
    }
    
// -----------------------------------------------------------------------------
// CSipCSSubSession::ClientReadyToReceiveL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::ClientReadyToReceiveL (const RMessage2& aMessage)
	{
    iReceiver->ClientReadyToReceiveL(aMessage);
	}

// -----------------------------------------------------------------------------
// CSipCSSubSession::ReceiveSipMessageL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::ReceiveSipMessageL (const RMessage2& aMessage)
	{
    iReceiver->WriteSipMessageToClientL(aMessage);
	}

// -----------------------------------------------------------------------------
// CSipCSSubSession::CancelClientReceiveL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::CancelClientReceiveL ()
	{
    iReceiver->CancelClientReceiveL();
	}

// -----------------------------------------------------------------------------
// CSipCSSubSession::GetFromHeaderL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::GetFromHeaderL (const TRegistrationId& aId,
                                       const RMessage2& aMessage)
    {
    CSIPFromHeader* from = Server().RegistrationMgr().CreateFromLC(aId);
    Server().ITC().WriteL(aMessage,*from);
    CleanupStack::PopAndDestroy(from);
    }
    
// -----------------------------------------------------------------------------
// CSipCSSubSession::SetRefreshIntervalL
// -----------------------------------------------------------------------------
//    
void CSipCSSubSession::SetRefreshIntervalL (const TRefreshId& aRefreshId,
                                            const RMessage2& aMessage)
    {
    TInt interval = 0;
    Server().ITC().ReadL(aMessage,interval,ESipItcArgRefreshInterval);
    Server().RefreshMgr().SetIntervalL(aRefreshId,interval);
    }

// -----------------------------------------------------------------------------
// CSipCSSubSession::GetRefreshIntervalL
// -----------------------------------------------------------------------------
//  
void CSipCSSubSession::GetRefreshIntervalL (const TRefreshId& aRefreshId,
                                            const RMessage2& aMessage)
    {
    TInt interval = Server().RefreshMgr().IntervalL(aRefreshId);
    Server().ITC().WriteL(aMessage,interval,ESipItcArgRefreshInterval);
    }    

// -----------------------------------------------------------------------------
// CSipCSSubSession::GetCallIDHeaderL
// -----------------------------------------------------------------------------
// 
void CSipCSSubSession::GetCallIDHeaderL (const TDialogId& aId, 
                                         const RMessage2& aMessage)
    {
    const CSIPCallIDHeader& callID = 
        Server().DialogMgr().CallIDL(aId,iReceiver);
    Server().ITC().WriteL(aMessage,callID);
    }
    
// -----------------------------------------------------------------------------
// CSipCSSubSession::GetLocalTagL
// -----------------------------------------------------------------------------
// 
void CSipCSSubSession::GetLocalTagL (const TDialogId& aId, 
                                     const RMessage2& aMessage)
    {
    const TDesC8& localTag = Server().DialogMgr().LocalTagL(aId,iReceiver);
    Server().ITC().WriteL(aMessage,localTag,ESipItcArgLocalTag);
    }

// -----------------------------------------------------------------------------
// CSipCSSubSession::GetRegisteredContactL
// -----------------------------------------------------------------------------
//   
void CSipCSSubSession::GetRegisteredContactL (const TRegistrationId& aId,
                                              const RMessage2& aMessage)
    {
    const CSIPContactHeader* contact = Server().RegistrationMgr().Contact(aId);
    if (!contact)
        {
        User::Leave(KErrNotFound);
        }
    Server().ITC().WriteL(aMessage,*contact);
    }

// -----------------------------------------------------------------------------
// CSipCSSubSession::ResetDialogStateL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::ResetDialogStateL(const TDialogId& aId)
    {
    Server().DialogMgr().ResetStateL(aId,iReceiver);
    }

// -----------------------------------------------------------------------------
// CSipCSSubSession::ITC
// -----------------------------------------------------------------------------
//
CSipCSServerITC& CSipCSSubSession::ITC ()
    {
    return iSession.ITC();
    }

// -----------------------------------------------------------------------------
// CSipCSSubSession::Server
// -----------------------------------------------------------------------------
//
CSipServerCore& CSipCSSubSession::Server ()
    {
    return iSession.ServerCore();
    }

// -----------------------------------------------------------------------------
// CSipCSSubSession::FillRequestLC
// -----------------------------------------------------------------------------
//
CURIContainer* CSipCSSubSession::FillRequestLC (const RMessage2& aMessage,
                                          CSIPRequest& aRequest,
                                          TBool aMethodExpected)
    {
    TSIPRemoteTargetAndProxy remoteTargetAndProxy;
    FillRequestL(aMessage,aRequest,remoteTargetAndProxy,aMethodExpected);
    __SIP_ASSERT_LEAVE(!remoteTargetAndProxy.iProxy, KErrGeneral);
    CleanupStack::PushL (remoteTargetAndProxy.iRemoteTarget);
    return remoteTargetAndProxy.iRemoteTarget;
    }
    
// -----------------------------------------------------------------------------
// CSipCSSubSession::FillRequestL
// -----------------------------------------------------------------------------
//
void 
CSipCSSubSession::FillRequestL (const RMessage2& aMessage,
                                CSIPRequest& aRequest,
                                TSIPRemoteTargetAndProxy& aRemoteTargetAndProxy,
                                TBool aMethodExpected,
                                TBool aProxyExpected)
    {
    ITC().ReadRequestL (aMessage,aRequest,aRemoteTargetAndProxy,
                        aMethodExpected,aProxyExpected);
    FillToAndRemoteTargetL (aRequest, aRemoteTargetAndProxy);
    }
    
// -----------------------------------------------------------------------------
// CSipCSSubSession::OverlappingInDialogAllowed
// -----------------------------------------------------------------------------
//    
TBool CSipCSSubSession::OverlappingInDialogAllowed(RStringF aMethod)
	{
	return (aMethod != SIPStrings::StringF(SipStrConsts::EInvite));
	}

// -----------------------------------------------------------------------------
// CSipCSSubSession::AddFromL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::AddFromL (const TRegistrationId& aRegistrationId,
                                 CSIPRequest& aRequest)
    {
    if (!aRequest.HasHeader(SIPStrings::StringF(SipStrConsts::EFromHeader)))
        {
        CSIPFromHeader* from = 
            Server().RegistrationMgr().CreateFromLC(aRegistrationId);
        aRequest.AddHeaderL(from);
        CleanupStack::Pop(from);
        }
    }

// -----------------------------------------------------------------------------
// CSipCSSubSession::AddContactL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::AddContactL (const TRegistrationId& aRegistrationId,
                                    CSIPMessage& aMessage)
    {
    if (!aMessage.HasHeader(SIPStrings::StringF(SipStrConsts::EContactHeader)))
        {
        const CSIPContactHeader* contact = 
            Server().RegistrationMgr().Contact(aRegistrationId);
        if (contact)
            {
            CSIPContactHeader* clone = 
                static_cast<CSIPContactHeader*>(contact->CloneL());
            CleanupStack::PushL(clone);
            clone->DeleteParam(SIPStrings::StringF(SipStrConsts::EExpires));
            aMessage.AddHeaderL(clone);
            CleanupStack::Pop(clone);
            }
        }
    }

// -----------------------------------------------------------------------------
// CSipCSSubSession::AddRouteL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::AddRouteL (const TRegistrationId& aRegistrationId,
                                  CSIPRequest& aRequest)
    {
    RPointerArray<CSIPRouteHeader> routeSet;
    CleanupClosePushL(routeSet);
    // Ask Registrations to add the outbound proxy to the service route
    // if the application has NOT added a preloaded route
    TBool addProxy = 
        !(aRequest.HasHeader(SIPStrings::StringF(SipStrConsts::ERouteHeader)));
    Server().RegistrationMgr().GetRouteL(aRegistrationId,routeSet,addProxy);
    // Append the route returned by Registrations
    for (TInt i=0; i < routeSet.Count(); i++)
        {
        aRequest.AddHeaderL(*(routeSet[i]));
        }
    CleanupStack::PopAndDestroy(); // routeSet
    }
    
// -----------------------------------------------------------------------------
// CSipCSSubSession::CheckSigCompInNextHopL
// -----------------------------------------------------------------------------
//    
void CSipCSSubSession::CheckSigCompInNextHopL (TRegistrationId aRegistrationId,
                                               CURIContainer& aRemoteTarget,
                                               CSIPRequest& aRequest)
    {
	CSIPHeaderBase* header = 
	    aRequest.Header(SIPStrings::StringF(SipStrConsts::ERouteHeader),0);    
    if (header)
        {
		CSIPRouteHeader* route = static_cast<CSIPRouteHeader*>(header);
		if (HasSigCompParameter(route->SIPAddress().URI()))
            {
            // First preloaded route exists, is the next hop and 
            // has comp=sigcomp parameter
            User::Leave(KErrArgument);
            }
        }
    else
        {
        if ((aRegistrationId == KEmptyRegistrationId ||
             !(Server().Registrations().HasOutboundProxy(aRegistrationId))) &&
            HasSigCompParameter(aRemoteTarget))
            {
            // No outbound proxy. Remote target is the next hop and 
            // has comp=sigcomp parameter
            User::Leave(KErrArgument);
            }
        }   
    }  

// -----------------------------------------------------------------------------
// CSipCSSubSession::FillToAndRemoteTargetL
// -----------------------------------------------------------------------------
//    
void CSipCSSubSession::FillToAndRemoteTargetL (
    CSIPRequest& aRequest,
    TSIPRemoteTargetAndProxy& aRemoteTargetAndProxy)
    {
    if (!aRemoteTargetAndProxy.iRemoteTarget)
        {
        if (!aRequest.To())
            {   
            User::Leave(KErrArgument);
            }
        aRemoteTargetAndProxy.iRemoteTarget = 
            CURIContainer::NewL(aRequest.To()->SIPAddress().URI());
        }
    else
        {
        if (!aRequest.To())
            {
	        CURIContainer* uri = 
	            CURIContainer::NewLC(*(aRemoteTargetAndProxy.iRemoteTarget));
	        CSIPAddress* sipAddress = SIPCodecUtils::CreateSIPAddressLC(uri);
	        CleanupStack::Pop(sipAddress);
	        CleanupStack::Pop(uri);
        	CleanupStack::PushL(sipAddress);
	        CSIPToHeader* to = CSIPToHeader::NewL(sipAddress);
	        CleanupStack::Pop(sipAddress);
	        CleanupStack::PushL(to);
	        aRequest.AddHeaderL(to);
	        CleanupStack::Pop(to);
            }
        }    
    }

// -----------------------------------------------------------------------------
// CSipCSSubSession::CheckSigCompInContactsL
// -----------------------------------------------------------------------------
//
void CSipCSSubSession::CheckSigCompInContactsL (CSIPMessage& aMessage)
    {    
	TSglQueIter<CSIPHeaderBase> iter = 
        aMessage.Headers(SIPStrings::StringF(SipStrConsts::EContactHeader));
	
    while (iter)
    	{
	    CSIPHeaderBase* header = iter++;
		CSIPContactHeader* contact = static_cast<CSIPContactHeader*>(header);
		CSIPAddress* sipAddress = contact->SIPAddress();
		if (sipAddress && HasSigCompParameter(sipAddress->URI()))
		    {
		    User::Leave(KErrArgument);
		    }
    	}
    }

// -----------------------------------------------------------------------------
// CSipCSSubSession::HasSigCompParameter
// -----------------------------------------------------------------------------
//
TBool CSipCSSubSession::HasSigCompParameter (CURIContainer& aURI) const
    {
    
    TBool hasSigCompParameter = EFalse;
    if (aURI.IsSIPURI())
        {
        CSIPURI* sipuri = aURI.SIPURI();
        RStringF comp = SIPStrings::StringF(SipStrConsts::EComp);
        RStringF sigcomp = SIPStrings::StringF(SipStrConsts::ESigComp);        
        hasSigCompParameter = (sipuri->ParamValue(comp) == sigcomp);
        }
    return hasSigCompParameter;
    }

// -----------------------------------------------------------------------------
// CSipCSSubSession::RefreshConnection
// -----------------------------------------------------------------------------
//
 void CSipCSSubSession::RefreshConnectionL(TUint32 aIapId)
	{
 	Server().RefreshConnectionL(aIapId);
	}

 // -----------------------------------------------------------------------------
 // CSipCSSubSession::GetConnectionErrorL
 // -----------------------------------------------------------------------------
 //
void CSipCSSubSession::GetConnectionErrorL(TSIPIds& aIds, const RMessage2& aMessage)
 	{
  	Server().GetConnectionErrorL(aIds.iIapId,aIds.iConnectionError);
  	ITC().WriteL(aMessage,aIds);
 	}
