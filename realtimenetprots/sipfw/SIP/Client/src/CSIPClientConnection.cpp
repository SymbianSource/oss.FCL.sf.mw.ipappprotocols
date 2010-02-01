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
// Name          : CSIPClientConnection.cpp
// Part of       : SIPClient
// Version       : SIP/6.0 
//




#include "sipclientserver.h"
#include "sipclientconnection.h"
#include "sipclient.h"
#include "RSIPConnection.h"
#include "CSIPITCSender.h"
#include "CSIPClientConnectionReceiver.h"
#include "sipmessageelements.h"
#include "siprequestelements.h"
#include "sipresponseelements.h"
#include "siptoheader.h"
#include "sipfromheader.h"
#include "sipcontactheader.h"
#include "sipexpiresheader.h"
#include "siptoheader.h"
#include "siprouteheader.h"
#include "sipcallidheader.h"
#include "sipaddress.h"
#include "uricontainer.h"
#include "sipuri.h"
#include "sipstrings.h"
#include "sipstrconsts.h"


// -----------------------------------------------------------------------------
// CSIPClientConnection::NewL
// -----------------------------------------------------------------------------
//
CSIPClientConnection* 
CSIPClientConnection::NewL (CSIPClient& aClient,
                            MSIPClientConnectionObserver& aObserver,
                            TUint32 aIapId)
	{
    CSIPClientConnection* self = 
        CSIPClientConnection::NewLC (aClient,aObserver,aIapId);
    CleanupStack::Pop (self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPClientConnection::NewLC
// -----------------------------------------------------------------------------
//
CSIPClientConnection* 
CSIPClientConnection::NewLC (CSIPClient& aClient,
                             MSIPClientConnectionObserver& aObserver,
                             TUint32 aIapId)
	{
	CSIPClientConnection* self = 
        new(ELeave)CSIPClientConnection(aClient,aObserver,aIapId);
    CleanupStack::PushL (self);
    self->ConstructL ();
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPClientConnection::CSIPClientConnection
// -----------------------------------------------------------------------------
//
CSIPClientConnection::CSIPClientConnection (
                                        CSIPClient& aClient,
                                        MSIPClientConnectionObserver& aObserver,
                                        TUint32 aIapId)
 : iClient (aClient),
   iObserver (aObserver),
   iIapId (aIapId),
   iAdded (EFalse)
	{
	iTag = SIPStrings::StringF(SipStrConsts::ETag);
    iContactHeaderName = SIPStrings::StringF(SipStrConsts::EContactHeader);
    iExpiresHeaderName = SIPStrings::StringF(SipStrConsts::EExpiresHeader);
	}

// -----------------------------------------------------------------------------
// CSIPClientConnection::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPClientConnection::ConstructL ()
	{
	iConnection = new(ELeave)RSIPConnection(iIapId);
    TInt err = iConnection->Open(iClient.SIP());
    if (err != KErrNone)
        {
        User::Leave(err);
        }
    iSender = CSIPITCSender::NewL(*iConnection);
    iReceiver = new(ELeave)CSIPClientConnectionReceiver(*iConnection,iObserver);
    iClient.AddL(this);
    iAdded = ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPClientConnection::~CSIPClientConnection
// -----------------------------------------------------------------------------
//
CSIPClientConnection::~CSIPClientConnection()
	{
    if (iAdded)
        {
        iClient.Remove(this);
        }
    delete iSender;
    delete iReceiver;
	if (iConnection)
        {
        iConnection->Close();
        }
    delete iConnection;
	}

// -----------------------------------------------------------------------------
// CSIPClientConnection::State
// -----------------------------------------------------------------------------
//
CSIPConnection::TState CSIPClientConnection::State () const
    {
    return iConnection->State();
    }

// -----------------------------------------------------------------------------
// CSIPClientConnection::IapId
// -----------------------------------------------------------------------------
//
TUint32 CSIPClientConnection::IapId () const
    {
    return iIapId;
    }

// -----------------------------------------------------------------------------
// CSIPClientConnection::GetLocalAddrL
// -----------------------------------------------------------------------------
//    
void CSIPClientConnection::GetLocalAddrL(TInetAddr& aAddr) const
    {
    TPckgBuf<TInetAddr> addrBuf;
    TIpcArgs args(TIpcArgs::ENothing);
    args.Set(ESipItcArgLocalAddr, &addrBuf);
    // SIP ids always required by the SIP server
    TSIPIds dummyIds;
    TPckgBuf<TSIPIds> dummySipIdsPckg(dummyIds);
    args.Set(ESipItcArgIds, &dummySipIdsPckg);
	TInt err = iConnection->Send(ESipItcGetLocalAddr,args);
	if (err != KErrNone)
	    {
	    User::Leave(err);
	    }
	aAddr = addrBuf();
    }
    
// -----------------------------------------------------------------------------
// CSIPClientConnection::SetOptL
// -----------------------------------------------------------------------------
//    
void CSIPClientConnection::SetOptL (TUint aOptionName,
                                    TUint aOptionLevel,
                                    const TDesC8& aOption)
    {
    TPckgBuf<TSIPSockOpt> opt(TSIPSockOpt(aOptionName,aOptionLevel));
    TIpcArgs args(TIpcArgs::ENothing);
    args.Set(ESipItcArgSockOpt, &opt);
    args.Set(ESipItcArgSockOptDes, &aOption);
    // SIP ids always required by the SIP server
    TSIPIds dummyIds;
    TPckgBuf<TSIPIds> dummySipIdsPckg(dummyIds);
    args.Set(ESipItcArgIds, &dummySipIdsPckg);
	TInt err = iConnection->Send(ESipItcSetSIPSockOptWithDes,args);
	if (err != KErrNone)
	    {
	    User::Leave(err);
	    }
    }
	   
// -----------------------------------------------------------------------------
// CSIPClientConnection::SetOptL
// -----------------------------------------------------------------------------
// 	         	                         
void CSIPClientConnection::SetOptL (TUint aOptionName,
                                    TUint aOptionLevel,
                                    TInt aOption)
    {
    TPckgBuf<TSIPSockOpt> opt(TSIPSockOpt(aOptionName,aOptionLevel,aOption));
    TIpcArgs args(TIpcArgs::ENothing);
    args.Set(ESipItcArgSockOpt, &opt);    
    // SIP ids always required by the SIP server
    TSIPIds dummyIds;
    TPckgBuf<TSIPIds> dummySipIdsPckg(dummyIds);
    args.Set(ESipItcArgIds, &dummySipIdsPckg);
	TInt err = iConnection->Send(ESipItcSetSIPSockOpt,args);
	if (err != KErrNone)
	    {
	    User::Leave(err);
	    }  
    }
    
// -----------------------------------------------------------------------------
// CSIPClientConnection::SetOutboundProxyL
// -----------------------------------------------------------------------------
//    
void 
CSIPClientConnection::SetOutboundProxyL (TUint32 aRegistrationId,
                                         const CSIPRouteHeader& aOutboundProxy)
    {
    __ASSERT_ALWAYS (aRegistrationId > 0, User::Leave(KErrArgument));
    
	TSIPIds ids;
	ids.iRegistrationId = aRegistrationId;
	iSender->SetOutboundProxyL(ids,aOutboundProxy);    
    }

// -----------------------------------------------------------------------------
// CSIPClientConnection::RemoveOutboundProxy
// -----------------------------------------------------------------------------
//
TInt CSIPClientConnection::RemoveOutboundProxy (TUint32 aRegistrationId)
	{
    if (!aRegistrationId)
        {
        return KErrArgument;
        }
	TSIPIds ids;
	ids.iRegistrationId = aRegistrationId;
	return iSender->Send(ids,ESipItcRemoveOutboundProxy);
	}

// -----------------------------------------------------------------------------
// CSIPClientConnection::OutboundProxyL
// -----------------------------------------------------------------------------
//
CSIPRouteHeader* CSIPClientConnection::OutboundProxyL (TUint32 aRegistrationId)
	{
    __ASSERT_ALWAYS (aRegistrationId > 0, User::Leave(KErrArgument));

	TSIPIds ids;
	ids.iRegistrationId = aRegistrationId;
	return static_cast<CSIPRouteHeader*>(
	    iSender->ReadSIPHeaderL(ids,ESipItcOutboundProxy,
	        SIPStrings::StringF(SipStrConsts::ERouteHeader)));
	}

// -----------------------------------------------------------------------------
// CSIPClientConnection::AorL
// -----------------------------------------------------------------------------
//
CSIPFromHeader* CSIPClientConnection::AorL (TUint32 aRegistrationId)
    {
    __ASSERT_ALWAYS (aRegistrationId > 0, User::Leave(KErrArgument));

	TSIPIds ids;
	ids.iRegistrationId = aRegistrationId;
	return static_cast<CSIPFromHeader*>(
	    iSender->ReadSIPHeaderL(ids,ESipItcGetFromHeader,
	        SIPStrings::StringF(SipStrConsts::EFromHeader)));
    }

// -----------------------------------------------------------------------------
// CSIPClientConnection::SendRegisterL
// -----------------------------------------------------------------------------
//
void CSIPClientConnection::SendRegisterL (TUint32& aRequestId,
										  const CSIPToHeader& aTo,
										  const CSIPFromHeader& aFrom,
										  const CSIPMessageElements& aElements,
										  const CUri8* aRemoteUri,
										  const CSIPRouteHeader* aProxy,
										  TBool aRefresh,
										  TBool aSendWithExpires,
										  TBool aCacheOutboundProxyIP)
    {
    __ASSERT_ALWAYS (!aTo.HasParam(iTag), User::Leave(KErrArgument));
    __ASSERT_ALWAYS (!aFrom.HasParam(iTag), User::Leave(KErrArgument));

    CheckContactExpiresL(aElements);
    if (aElements.UserHeaderCount(iExpiresHeaderName) > 0)
        {
        CheckExpiresHeaderL(aElements);
        }
    TSipItcFunctions func;
	TSIPIds ids;
	ids.iRefresh = aRefresh;
	ids.iSendWithExpires = aSendWithExpires;
	ids.iCacheOutboundProxyIP = aCacheOutboundProxyIP;
    if (aProxy)
    	{
        func = ESipItcRegisterAndSetOutboundProxy;
        }
    else
        {
		func = ESipItcRegister;
		}
	iSender->SendL(ids,func,SIPStrings::StringF(SipStrConsts::EEmpty),
	               aRemoteUri,&aFrom,&aTo,&aElements,aProxy);
	aRequestId = ids.iRequestId;
	}

// -----------------------------------------------------------------------------
// CSIPClientConnection::UpdateRegistrationL
// -----------------------------------------------------------------------------
//
void 
CSIPClientConnection::UpdateRegistrationL (TUint32 aRegistrationId,
                                           TUint32& aNewRequestId,
                                           const CSIPMessageElements* aElements)
	{
    __ASSERT_ALWAYS (aRegistrationId > 0, User::Leave(KErrArgument));
    if (aElements && aElements->UserHeaderCount(iContactHeaderName) > 0)
        {
        CheckContactExpiresL(*aElements);
        }
    if (aElements && aElements->UserHeaderCount(iExpiresHeaderName) > 0)
        {
        CheckExpiresHeaderL(*aElements);
        }

	TSIPIds ids;
	ids.iRegistrationId = aRegistrationId;
	iSender->SendL(ids,ESipItcUpdateRegistration,aElements);
	aNewRequestId = ids.iRequestId;
	}

// -----------------------------------------------------------------------------
// CSIPClientConnection::UnregisterL
// -----------------------------------------------------------------------------
//
void CSIPClientConnection::UnregisterL (TUint32 aRegistrationId,
				                        TUint32& aNewRequestId,
                                        const CSIPMessageElements* aElements)
	{
    __ASSERT_ALWAYS (aRegistrationId > 0, User::Leave(KErrArgument));
	if (aElements != 0)
		{
		__ASSERT_ALWAYS (aElements->UserHeaderCount(iContactHeaderName) == 0,
		                 User::Leave(KErrArgument));
        __ASSERT_ALWAYS (aElements->UserHeaderCount(iExpiresHeaderName) == 0,
		                 User::Leave(KErrArgument)); 
		}
	TSIPIds ids;
	ids.iRegistrationId = aRegistrationId;
	iSender->SendL(ids,ESipItcUnRegister,aElements);
	aNewRequestId = ids.iRequestId;
	}

// -----------------------------------------------------------------------------
// CSIPClientConnection::FetchRegistrationsL
// -----------------------------------------------------------------------------
//
void 
CSIPClientConnection::FetchRegistrationsL (const CSIPRequestElements& aElements,
                                           TUint32& aRequestId)
	{
    __ASSERT_ALWAYS (aElements.Method().DesC().Length()==0,
                     User::Leave(KErrArgument));
    __ASSERT_ALWAYS (aElements.FromHeader() != 0, User::Leave(KErrArgument));
	__ASSERT_ALWAYS (
        aElements.MessageElements().UserHeaderCount(iContactHeaderName) == 0,
        User::Leave(KErrArgument));
    __ASSERT_ALWAYS (
        aElements.MessageElements().UserHeaderCount(iExpiresHeaderName) == 0,
		User::Leave(KErrArgument));

	TSIPIds ids;
	iSender->SendL(ids,ESipItcFetchRegistrations,aElements);
	aRequestId = ids.iRequestId;
	}

// -----------------------------------------------------------------------------
// CSIPClientConnection::SendRequestL
// -----------------------------------------------------------------------------
//
void CSIPClientConnection::SendRequestL (const CSIPRequestElements& aElements,
                                         TUint32& aRequestId,
                                         TUint32 aRegistrationId,
                                         TBool aRefresh)
	{
    __ASSERT_ALWAYS (aElements.Method().DesC().Length() > 0,
                     User::Leave(KErrArgument));
    if (aRefresh)
        {
        CheckExpiresHeaderL(aElements.MessageElements());
        }
	TSIPIds ids;
	ids.iRegistrationId = aRegistrationId;
	if (!aRegistrationId)
        {
        __ASSERT_ALWAYS(aElements.FromHeader()!=0, User::Leave(KErrArgument));
        }
    ids.iRefresh = aRefresh;    
	iSender->SendL(ids,ESipItcSendRequest,aElements);
	aRequestId = ids.iRequestId;
	}

// -----------------------------------------------------------------------------
// CSIPClientConnection::SendRequestAndUpdateRefreshL
// -----------------------------------------------------------------------------
//
void CSIPClientConnection::SendRequestAndUpdateRefreshL (
                                        TUint32 aRefreshId,
                                        TUint32& aNewRequestId,
						                const CSIPMessageElements* aElements)
	{
    __ASSERT_ALWAYS (aRefreshId > 0, User::Leave(KErrArgument));

    TSIPIds ids;
	ids.iRefreshId = aRefreshId;
	iSender->SendL(ids,ESipItcSendRequestAndUpdateRefresh,aElements);
	aNewRequestId = ids.iRequestId;
	}


// -----------------------------------------------------------------------------
// CSIPClientConnection::SendRequestAndTerminateRefreshL
// -----------------------------------------------------------------------------
//
void CSIPClientConnection::SendRequestAndTerminateRefreshL (
                            TUint32 aRefreshId,
                            TUint32& aNewRequestId,
						    const CSIPMessageElements* aElements)
	{
    __ASSERT_ALWAYS (aRefreshId > 0, User::Leave(KErrArgument));

    if (aElements)
        {
        RPointerArray<CSIPHeaderBase> expireHeaders = 
            aElements->UserHeadersL(iExpiresHeaderName);
        CleanupClosePushL(expireHeaders);
        if (expireHeaders.Count() == 1)
            {
            CSIPExpiresHeader* expires = 
                static_cast<CSIPExpiresHeader*>(expireHeaders[0]);
            __ASSERT_ALWAYS (expires->Value()==0, User::Leave(KErrArgument));
            }
        CleanupStack::PopAndDestroy(1); // expireHeaders
        }
	
    TSIPIds ids;
	ids.iRefreshId = aRefreshId;
	iSender->SendL(ids,ESipItcSendRequestAndTerminateRefresh,aElements);
	aNewRequestId = ids.iRequestId;
	}

// -----------------------------------------------------------------------------
// CSIPClientConnection::SendResponseL
// -----------------------------------------------------------------------------
//
void CSIPClientConnection::SendResponseL (const CSIPResponseElements& aElements,
										  TUint32 aRequestId)
	{
    __ASSERT_ALWAYS (aRequestId > 0, User::Leave(KErrArgument));

	TSIPIds ids;
	ids.iRequestId = aRequestId;
	iSender->SendL(ids,ESipItcSendResponse,aElements);
	}

// -----------------------------------------------------------------------------
// CSIPClientConnection::SendAckL
// -----------------------------------------------------------------------------
//
void CSIPClientConnection::SendAckL (TUint32 aDialogId,
									 TUint32 aRequestId,
									 const CSIPMessageElements* aElements)
	{
    __ASSERT_ALWAYS (aRequestId > 0, User::Leave(KErrArgument));
    __ASSERT_ALWAYS (aDialogId > 0, User::Leave(KErrArgument));

	TSIPIds ids;
	ids.iDialogId = aDialogId;
	ids.iRequestId = aRequestId;
	iSender->SendL(ids,ESipItcSendRequestInDialog,
	               SIPStrings::StringF(SipStrConsts::EAck),aElements);
	}

// -----------------------------------------------------------------------------
// CSIPClientConnection::SendCancelL
// -----------------------------------------------------------------------------
//
void CSIPClientConnection::SendCancelL (TUint32 aInviteRequestId,
										TUint32& aCancelRequestId)
	{
    __ASSERT_ALWAYS (aInviteRequestId > 0, User::Leave(KErrArgument));

	TSIPIds ids;
    ids.iRequestId = aInviteRequestId;
	iSender->SendL(ids,ESipItcSendCancel);
	aCancelRequestId = ids.iRequestId;
	}

// -----------------------------------------------------------------------------
// CSIPClientConnection::SendResponseAndCreateDialogL
// -----------------------------------------------------------------------------
//
void CSIPClientConnection::SendResponseAndCreateDialogL (
                                    const CSIPResponseElements& aElements,
								    TUint32 aRequestId,
								    TUint32& aDialogId,
								    TUint32 aRegistrationId)
	{
    __ASSERT_ALWAYS (aRequestId > 0, User::Leave(KErrArgument));
    
    CheckContactL(aElements.MessageElements());

	TSIPIds ids;
	ids.iRequestId = aRequestId;
	ids.iRegistrationId = aRegistrationId;
	iSender->SendL(ids,ESipItcSendResponseAndCreateDialog,aElements);
	aDialogId= ids.iDialogId;
	}

// -----------------------------------------------------------------------------
// CSIPClientConnection::SendResponseWithinDialogL
// -----------------------------------------------------------------------------
//
void CSIPClientConnection::SendResponseWithinDialogL (
                                 const CSIPResponseElements& aElements,
								 TUint32 aRequestId,
								 TUint32 aDialogId,
								 TBool aTargetRefresh)
	{
    __ASSERT_ALWAYS (aRequestId > 0, User::Leave(KErrArgument));
    __ASSERT_ALWAYS (aDialogId > 0, User::Leave(KErrArgument));

	TSIPIds ids;
	ids.iRequestId = aRequestId;
	ids.iDialogId = aDialogId;
	ids.iTargetRefresh = aTargetRefresh;
    if (aTargetRefresh)
        {
        CheckContactL(aElements.MessageElements());
        }
    iSender->SendL(ids,ESipItcSendResponseInDialog,aElements);
	}

// -----------------------------------------------------------------------------
// CSIPClientConnection::SendRequestAndCreateDialogL
// -----------------------------------------------------------------------------
//
void CSIPClientConnection::SendRequestAndCreateDialogL (
                                    TUint32& aRequestId,
                                    TUint32& aRefreshId,
                                    TUint32& aDialogId,
                                    RStringF aMethod,
                                    const CUri8& aRemoteUri,
                                    const CSIPFromHeader* aFrom,
                                    const CSIPMessageElements* aElements,
                                    const CSIPToHeader* aTo,
                                    TUint32 aRegistrationId,
                                    TBool aRefresh)
	{
    __ASSERT_ALWAYS (aMethod.DesC().Length() > 0,User::Leave(KErrArgument));
    CheckDialogCreationRequestL (aTo,aFrom,aElements,aRegistrationId,aRefresh);
	TSIPIds ids;
	ids.iDialogId = aDialogId;
    ids.iRegistrationId = aRegistrationId;
    TSipItcFunctions func = ESipItcSendRequestAndCreateDialog;
    ids.iRefresh = aRefresh;
    iSender->SendL(ids,func,aMethod,&aRemoteUri,aFrom,aTo,aElements);
	aRequestId = ids.iRequestId;
	aRefreshId = ids.iRefreshId;
	aDialogId= ids.iDialogId;
	}

// -----------------------------------------------------------------------------
// CSIPClientConnection::SendRequestWithinDialogL
// -----------------------------------------------------------------------------
//
void CSIPClientConnection::SendRequestWithinDialogL (
                                TUint32 aDialogId,
							    TUint32& aNewRequestId,
							    TUint32& aRefreshId,
                                RStringF aMethod,
							    const CSIPMessageElements* aElements,
								TBool aTargetRefresh,
								TBool aRefresh)
	{
    __ASSERT_ALWAYS (aDialogId > 0, User::Leave(KErrArgument));
    __ASSERT_ALWAYS (aMethod.DesC().Length() > 0, User::Leave(KErrArgument));
    __ASSERT_ALWAYS (aMethod != SIPStrings::StringF(SipStrConsts::ERegister),
                     User::Leave(KErrArgument));
    __ASSERT_ALWAYS (aMethod != SIPStrings::StringF(SipStrConsts::ECancel),
                     User::Leave(KErrArgument));
    __ASSERT_ALWAYS (aMethod != SIPStrings::StringF(SipStrConsts::EAck),
                     User::Leave(KErrArgument));    
    if (aTargetRefresh)
        {
        CheckContactL(aElements);
        }
	TSIPIds ids;
    ids.iDialogId = aDialogId;
    ids.iTargetRefresh = aTargetRefresh;    
    ids.iRefresh = aRefresh;
	if (aRefresh)
		{
        __ASSERT_ALWAYS(aMethod != SIPStrings::StringF(SipStrConsts::EMessage),
                        User::Leave(KErrArgument));
		CheckExpiresHeaderL(aElements);
		}
    iSender->SendL(ids,ESipItcSendRequestInDialog,aMethod,aElements);
	aNewRequestId = ids.iRequestId;
	aRefreshId = ids.iRefreshId;
	}

// -----------------------------------------------------------------------------
// CSIPClientConnection::SendRequestWithinDialogL
// -----------------------------------------------------------------------------
//
void CSIPClientConnection::SendRequestWithinDialogL (
                                    TUint32 aDialogId,
                                    TUint32 aRefreshId,
							        TUint32& aNewRequestId,
							        const CSIPMessageElements* aElements)
	{
    __ASSERT_ALWAYS (aDialogId > 0, User::Leave(KErrArgument));
    __ASSERT_ALWAYS (aRefreshId > 0, User::Leave(KErrArgument));
    CheckContactL(aElements);
    CheckExpiresHeaderL(aElements,ETrue);

	TSIPIds ids;
    ids.iDialogId = aDialogId;
    ids.iRefreshId = aRefreshId;
    iSender->SendL(ids,ESipItcSendRequestInDialogAndUpdateRefresh,aElements);
	aNewRequestId = ids.iRequestId;
	}
	
// -----------------------------------------------------------------------------
// CSIPClientConnection::CallIDL
// -----------------------------------------------------------------------------
//	
CSIPCallIDHeader* CSIPClientConnection::CallIDL (TUint32 aDialogId)
    {
    __ASSERT_ALWAYS (aDialogId > 0, User::Leave(KErrArgument));

	TSIPIds ids;
	ids.iDialogId = aDialogId;
	return iSender->ReadCallIDHeaderL(ids);   
    }
    
// -----------------------------------------------------------------------------
// CSIPClientConnection::LocalTagL
// -----------------------------------------------------------------------------
//	
RStringF CSIPClientConnection::LocalTagL (TUint32 aDialogId)
    {
    __ASSERT_ALWAYS (aDialogId > 0, User::Leave(KErrArgument));

	TSIPIds ids;
	ids.iDialogId = aDialogId;
	return iSender->ReadLocalTagL(ids);   
    }    

// -----------------------------------------------------------------------------
// CSIPClientConnection::ContactL
// -----------------------------------------------------------------------------
//	
CSIPContactHeader* CSIPClientConnection::ContactL (TUint32 aRegistrationId)
    {
    __ASSERT_ALWAYS (aRegistrationId > 0, User::Leave(KErrArgument));

	TSIPIds ids;
	ids.iRegistrationId = aRegistrationId;
	return static_cast<CSIPContactHeader*>(
	    iSender->ReadSIPHeaderL(ids,ESipItcGetRegisteredContact,
	        SIPStrings::StringF(SipStrConsts::EContactHeader)));
    }
	
// -----------------------------------------------------------------------------
// CSIPClientConnection::TerminateDialog
// -----------------------------------------------------------------------------
//
TInt CSIPClientConnection::TerminateDialog (TUint32 aDialogId)
	{
    if (!aDialogId) 
        {
        return KErrArgument;
        }
	TSIPIds ids;
	ids.iDialogId = aDialogId;
	return iSender->Send(ids,ESipItcTerminateDialog);
	}

// -----------------------------------------------------------------------------
// CSIPClientConnection::TerminateRefresh
// -----------------------------------------------------------------------------
//
TInt CSIPClientConnection::TerminateRefresh (TUint32 aRefreshId)
	{
    if (!aRefreshId)
        {
        return KErrArgument;
        }
	TSIPIds ids;
	ids.iRefreshId = aRefreshId;
	return iSender->Send(ids,ESipItcTerminateRefresh);
	}
	
// -----------------------------------------------------------------------------
// CSIPClientConnection::SetRefreshIntervalL
// -----------------------------------------------------------------------------
//	
void CSIPClientConnection::SetRefreshIntervalL (TUint32 aRefreshId,
                                                TUint aInterval)
    {
    __ASSERT_ALWAYS (aRefreshId > 0, User::Leave(KErrArgument));
    __ASSERT_ALWAYS (aInterval > 0, User::Leave(KErrArgument));
    __ASSERT_ALWAYS (aInterval <= KMaxTInt, User::Leave(KErrArgument));        
	TSIPIds ids;
	ids.iRefreshId = aRefreshId;
    iSender->SetRefreshIntervalL(ids,aInterval);    
    }

// -----------------------------------------------------------------------------
// CSIPClientConnection::RefreshIntervalL
// -----------------------------------------------------------------------------
//
TUint CSIPClientConnection::RefreshIntervalL (TUint32 aRefreshId)
    {
    __ASSERT_ALWAYS (aRefreshId > 0, User::Leave(KErrArgument));   
	TSIPIds ids;
	ids.iRefreshId = aRefreshId;
	TInt interval = 0;
	iSender->GetRefreshIntervalL(ids, interval);
	__ASSERT_ALWAYS (interval >= 0, User::Leave(KErrUnderflow));
    return interval;
    }

// -----------------------------------------------------------------------------
// CSIPClientConnection::TerminateDialogRefresh
// -----------------------------------------------------------------------------
//
TInt CSIPClientConnection::TerminateDialogRefresh (TUint32 aDialogId,
                                                   TUint32 aRefreshId)
    {
    if (!aDialogId || !aRefreshId)
        {
        return KErrArgument;
        }
	TSIPIds ids;
    ids.iDialogId = aDialogId;
	ids.iRefreshId = aRefreshId;
	return iSender->Send(ids,ESipItcTerminateDialogRefresh);
    }

// -----------------------------------------------------------------------------
// CSIPClientConnection::TerminateRegistration
// -----------------------------------------------------------------------------
//
TInt CSIPClientConnection::TerminateRegistration (TUint32 aId)
    {
    if (!aId)
        {
        return KErrArgument;
        }
	TSIPIds ids;
	ids.iRegistrationId = aId;
	return iSender->Send(ids,ESipItcTerminateRegistration);
    }

// -----------------------------------------------------------------------------
// CSIPClientConnection::ResetDialogState
// -----------------------------------------------------------------------------
//
TInt CSIPClientConnection::ResetDialogState (TUint32 aDialogId)
    {
    if (!aDialogId) 
        {
        return KErrArgument;
        }
	TSIPIds ids;
	ids.iDialogId = aDialogId;
	return iSender->Send(ids,ESipItcResetDialogState);
    }

// -----------------------------------------------------------------------------
// CSIPClientConnection::DetachClient
// -----------------------------------------------------------------------------
//
void CSIPClientConnection::DetachClient()
	{
	iAdded = EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPClientConnection::CheckDialogCreationRequestL
// -----------------------------------------------------------------------------
//
void CSIPClientConnection::CheckDialogCreationRequestL (
    const CSIPToHeader* aTo,
    const CSIPFromHeader* aFrom,
    const CSIPMessageElements* aElements,
    TUint32 aRegistrationId,
    TBool aRefresh)
    {
    if (aTo)
        {
        __ASSERT_ALWAYS (!aTo->HasParam(iTag), User::Leave(KErrArgument));       
        }
    CheckContactL(aElements);
	if (!aRegistrationId)
        {
        __ASSERT_ALWAYS(aFrom!=0,User::Leave(KErrArgument));
        }
    if (aRefresh)
        {
        CheckExpiresHeaderL(aElements);
        }
    }

// -----------------------------------------------------------------------------
// CSIPClientConnection::CheckContactExpiresL
// -----------------------------------------------------------------------------
//
void 
CSIPClientConnection::CheckContactExpiresL (
                                    const CSIPMessageElements& aElements) const
    {
    RPointerArray<CSIPHeaderBase> contacts = 
        aElements.UserHeadersL(iContactHeaderName);
    CleanupClosePushL(contacts);

	if (contacts.Count() != 1)
        {
        User::Leave(KErrArgument);
        }

    CSIPContactHeader* contact = static_cast<CSIPContactHeader*>(contacts[0]);
	if (contact->Star())
        {
        User::Leave(KErrArgument);
        }

	if (contact->ExpiresParameter() == 0)
        {
        User::Leave(KErrArgument);
		}

    CleanupStack::PopAndDestroy(1); // contacts
    }

// -----------------------------------------------------------------------------
// CSIPClientConnection::CheckContactL
// -----------------------------------------------------------------------------
//
void 
CSIPClientConnection::CheckContactL (const CSIPMessageElements* aElements) const
	{
    if (aElements != 0)
        {
        CheckContactL(*aElements);
        }
	}

// -----------------------------------------------------------------------------
// CSIPClientConnection::CheckContactL
// -----------------------------------------------------------------------------
//
void 
CSIPClientConnection::CheckContactL (const CSIPMessageElements& aElements) const
	{
    RPointerArray<CSIPHeaderBase> contacts = 
        aElements.UserHeadersL(iContactHeaderName);
    CleanupClosePushL(contacts);
    TInt contactCount = contacts.Count();
	if (contactCount > 1)
        {
        User::Leave(KErrArgument);
        }
	if (contactCount==1 && 
        static_cast<CSIPContactHeader*>(contacts[0])->Star())
        {
        User::Leave(KErrArgument);
        }
    CleanupStack::PopAndDestroy(1); // contacts
	}

// -----------------------------------------------------------------------------
// CSIPClientConnection::CheckExpiresHeaderL
// -----------------------------------------------------------------------------
//
void
CSIPClientConnection::CheckExpiresHeaderL (const CSIPMessageElements* aElements,
                                           TBool aZeroAllowed) const
    {
	if (aElements == 0) 
        {
        User::Leave(KErrArgument);
        }
    CheckExpiresHeaderL (*aElements,aZeroAllowed);
    }

// -----------------------------------------------------------------------------
// CSIPClientConnection::CheckExpiresHeaderL
// -----------------------------------------------------------------------------
//
void
CSIPClientConnection::CheckExpiresHeaderL (const CSIPMessageElements& aElements,
                                           TBool aZeroAllowed) const
    {
    RPointerArray<CSIPHeaderBase> expireHeaders = 
        aElements.UserHeadersL(iExpiresHeaderName);
    CleanupClosePushL(expireHeaders);
    if (expireHeaders.Count() != 1)
        {
        User::Leave(KErrArgument);
        }
    if (!aZeroAllowed &&
        static_cast<const CSIPExpiresHeader*>(expireHeaders[0])->Value() == 0)
        {
        User::Leave(KErrArgument);
        }
    CleanupStack::PopAndDestroy(1); // expireHeaders
    }

// -----------------------------------------------------------------------------
// CSIPClientConnection::RefreshConnection
// -----------------------------------------------------------------------------
//
void CSIPClientConnection::RefreshConnection() const
	{
	TSIPIds ids;
	ids.iIapId = iIapId;
	iSender->Send(ids,ESipItcRefreshConnection);
	}

// -----------------------------------------------------------------------------
// CSIPClientConnection::ConnectionErrorL
// -----------------------------------------------------------------------------
//
TInt CSIPClientConnection::ConnectionErrorL() const
	{
	TSIPIds ids;
	ids.iIapId = iIapId;
	iSender->SendL(ids,ESipItcGetConnectionError);
	return ids.iConnectionError;
	}
