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
// Name          : CSipCSSession.cpp
// Part of       : SIPServerCore
// Version       : SIP/6.0
//



#include "CSipServerCore.h"
#include "CSipCSServer.h"
#include "CSipCSSession.h"
#include "CSipCSSessionReceiver.h"
#include "CSipCSServerITC.h"
#include "CSipCSSubSession.h"
#include "CTransactionUser.h"
#include "CSipDialogMgr.h"
#include "CSIPRegistrationMgr.h"
#include "sipheaderbase.h"
#include "MSigComp.h"
#include "SipRequestHandlerBase.h"
#include "SipAssert.h"
#include "sipsec.h"
#include "MSipRefreshMgr.h"


// -----------------------------------------------------------------------------
// CSipCSSession::NewL
// -----------------------------------------------------------------------------
//
CSipCSSession* CSipCSSession::NewL (CSipServerCore& aServerCore)
	{
    CSipCSSession* self = CSipCSSession::NewLC(aServerCore);
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSipCSSession::NewLC
// -----------------------------------------------------------------------------
//
CSipCSSession* CSipCSSession::NewLC (CSipServerCore& aServerCore)
	{
    CSipCSSession* self = new(ELeave)CSipCSSession(aServerCore);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
	}

// -----------------------------------------------------------------------------
// CSipCSSession::CSipCSSession
// -----------------------------------------------------------------------------
//
CSipCSSession::CSipCSSession (CSipServerCore& aServerCore)
    : iServerCore (aServerCore),
      iByPassSIPSec(EFalse)
	{
    iServerCore.IncrementSessions();
	}

// -----------------------------------------------------------------------------
// CSipCSSession::ConstructL
// -----------------------------------------------------------------------------
//
void CSipCSSession::ConstructL ()
	{
    iReceiver = CSipCSSessionReceiver::NewL(iServerCore.ITC(),*this);
	// second-phase construct base class
	iSubSessions = iServerCore.ObjectConIx().CreateL();
    iSubSessionIndex = CObjectIx::NewL();
	}

// -----------------------------------------------------------------------------
// CSipCSSession::~CSipCSSession
// -----------------------------------------------------------------------------
//
CSipCSSession::~CSipCSSession()
	{
	if (iReceiver)
		{
		iServerCore.TU().ClearTransactionOwner(iReceiver);
		}
    iServerCore.RequestHandler().Deregister(iClientUid);
    delete iSubSessionIndex;
    iServerCore.ObjectConIx().Remove(iSubSessions);
    iSubSessions = 0;
    delete iReceiver;
    iServerCore.DecrementSessions();
	}

// -----------------------------------------------------------------------------
// CSipCSSession::ITC
// -----------------------------------------------------------------------------
//
CSipCSServerITC& CSipCSSession::ITC ()
    {
    return iServerCore.ITC();
    }

// -----------------------------------------------------------------------------
// CSipCSSession::ServerCore
// -----------------------------------------------------------------------------
//
CSipServerCore& CSipCSSession::ServerCore ()
    {
    return iServerCore;
    }
    
// -----------------------------------------------------------------------------
// CSipCSSession::FindOwner
// From MSipCSSession
// -----------------------------------------------------------------------------
//    
MTransactionOwner& CSipCSSession::FindOwner(TUint32 aIapId)
	{
	MTransactionOwner* transactionOwner = iReceiver;
    CSipCSSubSession* subSession = 0;
    for (TInt i=0; i < iSubSessions->Count(); i++)
        {
        subSession = static_cast<CSipCSSubSession*>((*iSubSessions)[i]);
        if (subSession->IapId() == aIapId)
            {
            transactionOwner = subSession->TransactionOwner();
            }
        }
    return *transactionOwner;	
	}

// -----------------------------------------------------------------------------
// CSipCSSession::SIPSecUser
// From MSipCSSession
// -----------------------------------------------------------------------------
//
const MSIPSecUser* CSipCSSession::SIPSecUser(
    const TRegistrationId& aRegistrationId)
    {
    return ServerCore().RegistrationMgr().SIPSecUser(aRegistrationId);
    }

// -----------------------------------------------------------------------------
// CSipCSSession::ByPassSIPSec()
// From MSipCSSession
// -----------------------------------------------------------------------------
//	
TBool CSipCSSession::ByPassSIPSec() const
    {
    return iByPassSIPSec;
    }

// -----------------------------------------------------------------------------
// CSipCSSession::ServiceL
// From CSession
// -----------------------------------------------------------------------------
//
void CSipCSSession::ServiceL (const RMessage2& aMessage)
	{
    DoServiceL(aMessage.Function(),aMessage);
	}

// -----------------------------------------------------------------------------
// CSipCSSession::Uid
// -----------------------------------------------------------------------------
//
TUid CSipCSSession::Uid ()
	{
    return iClientUid;
	}

// -----------------------------------------------------------------------------
// CSipCSSession::ConnectionStateChangedL
// From MConnectionStateObserver:
// -----------------------------------------------------------------------------
//
void CSipCSSession::ConnectionStateChangedL (TUint32 aIapId,
                                             CSIPConnection::TState aState)
	{
    CSipCSSubSession* subSession = 0;
    for (TInt i=0; i < iSubSessions->Count(); i++)
        {
        subSession = static_cast<CSipCSSubSession*>((*iSubSessions)[i]);
        if (subSession->IapId() == aIapId)
            {
            subSession->SetStateL(aState);
            }
        }
	}

// -----------------------------------------------------------------------------
// CSipCSSession::DoServiceL
// -----------------------------------------------------------------------------
//
void CSipCSSession::DoServiceL (TInt aFunction, const RMessage2& aMessage)
    {
    switch (aFunction)
		{
#ifndef PLAT_SEC_TEST
        case ESipItcSetAppUid:
            SetClientUidL (aMessage); break;

        case ESipItcSupportedSecurityMechanisms:
            SupportedSecurityMechanismsL (aMessage); break;
           
        case ESipItcNegotiatedSecurityMechanism:
            NegotiatedSecurityMechanismL (aMessage); break;           
           
		case ESipItcSetHttpDigestObserver:
		    SetHttpDigestObserverL(aMessage); break;
		    
		case ESipItcSetCredentials:
		    SetCredentialsL(aMessage); break;
		    
		case ESipItcRemoveCredentials:
		    RemoveCredentialsL(aMessage); break;
		
		case ESipItcIgnoreChallenge:
		    IgnoreChallengeL(aMessage); break;

        case ESipItcSetSecurityHandling:
            SetSecurityHandlingL(aMessage); break;

        case ESipItcIsSigCompSupported:
            SigCompSupportedL (); break;

        case ESipItcGetTransactionState:
            GetTransactionStateL (aMessage); break;

        case ESipItcGetDialogState:
            GetDialogStateL (aMessage); break;

        case ESipItcGetCompartmentCount:
            GetCompartmentCountL (aMessage); break;

        case ESipItcOpenSubSession:
            {
            TUint32 iapId = ITC().ReadSipIdsL(aMessage).iIapId;
            OpenSubSessionL (iapId,aMessage);
            }
			break;

        case ESipItcCloseSubSession:
            {
            TInt handle = 0;
            ITC().ReadL(aMessage,handle,ESipItcArgSubSessionHandle);
			if(iSubSessionIndex->AtL(handle))
	            CloseSubSession (handle);
			else
				{}
            } 
            break;

        case ESipItcClientReadyToReceive: // Asynchronous. Do not complete yet.
            ClientReadyToReceiveL (aMessage); return;

        case ESipItcClientReceiveSipMessage:
            ReceiveSipMessageL (aMessage); break;

        case ESipItcClientCancelReceive:
            CancelClientReceiveL (); break;

        default:
            {
            TInt handle = 0;
            ITC().ReadL(aMessage,handle,ESipItcArgSubSessionHandle);
            CallSubSessionL (handle, aMessage);
            }
            return;
#endif
		}
    ITC().Complete (aMessage, KErrNone);
    }

// -----------------------------------------------------------------------------
// CSipCSSession::SetClientUidL
// -----------------------------------------------------------------------------
//
void CSipCSSession::SetClientUidL (const RMessage2& aMessage)
	{
    iClientUid.iUid = ITC().ReadAppUidL (aMessage).iUid;
    ServerCore().RequestHandler().RegisterL(iClientUid);
	}

// -----------------------------------------------------------------------------
// CSipCSSession::SupportedSecurityMechanismsL
// -----------------------------------------------------------------------------
//
void CSipCSSession::SupportedSecurityMechanismsL (const RMessage2& aMessage)
    {
    CDesC8Array* mechanisms = 
        ServerCore().SIPSec().SupportedSecurityMechanismsL();
    __SIP_ASSERT_LEAVE (mechanisms!=0,KErrArgument);
	CleanupStack::PushL (mechanisms);
    ITC().WriteL(aMessage,*mechanisms);
    CleanupStack::PopAndDestroy(mechanisms);
    }
    
// -----------------------------------------------------------------------------
// CSipCSSession::NegotiatedSecurityMechanismL
// -----------------------------------------------------------------------------
//    
void CSipCSSession::NegotiatedSecurityMechanismL (const RMessage2& aMessage)
	{	
	HBufC8* nextHop = ITC().ReadLC(aMessage,ESipItcArgNextHop);	
    HBufC8* mechanism = 
    	ServerCore().SIPSec().NegotiatedSecurityMechanismL(*nextHop);
    CleanupStack::PopAndDestroy(nextHop);    	
    if (mechanism)
    	{
		CleanupStack::PushL(mechanism);
    	ITC().WriteL(aMessage,*mechanism,ESipItcArgAuthenticationMechanism);
    	CleanupStack::PopAndDestroy(mechanism);
    	}
    else
    	{
    	ITC().WriteL(aMessage,KNullDesC8,ESipItcArgAuthenticationMechanism);	
    	}
	}
    
// -----------------------------------------------------------------------------
// CSipCSSession::SetHttpDigestObserverL
// -----------------------------------------------------------------------------
//    
void CSipCSSession::SetHttpDigestObserverL (const RMessage2& aMessage)
    {
    TInt observerType(0);
    ITC().ReadL(aMessage,observerType,ESipItcArgHttpDigestObserverType);
    iReceiver->SetHttpDigestObserver(
        static_cast<TSipHttpDigestObserverType>(observerType));
    }
    
// -----------------------------------------------------------------------------
// CSipCSSession::SetCredentialsL
// -----------------------------------------------------------------------------
//      
void CSipCSSession::SetCredentialsL (const RMessage2& aMessage)
    {
    HBufC8* realm = ITC().ReadLC(aMessage,ESipItcArgRealm);
            
    CDesC8Array* credentials = ITC().ReadCredentialsLC(aMessage);
    TInt credentialsCount = credentials->MdcaCount();
    __ASSERT_ALWAYS(credentialsCount > 1, User::Leave(KErrArgument));
    TPtrC8 outboundproxy(KNullDesC8);
    if (credentialsCount == EOutboundProxyIndex+1)
        {
        outboundproxy.Set(credentials->MdcaPoint(EOutboundProxyIndex));
        }
    
    TSIPIds ids = ITC().ReadSipIdsL(aMessage);
    const MSIPSecUser* sipSecUser = 
        FindSIPSecUser(ids.iRefreshId,ids.iRequestId);    

    if (sipSecUser)
        {
        ServerCore().SIPSec().SetCredentialsL(
            *sipSecUser,
            *realm,
            outboundproxy,
            credentials->MdcaPoint(EUserIndex),
            credentials->MdcaPoint(EPasswdIndex));
        }
    else
        {
        ServerCore().SIPSec().SetCredentialsL(
            KEmptyTransactionId,
            *realm,
            outboundproxy,
            credentials->MdcaPoint(EUserIndex),
            credentials->MdcaPoint(EPasswdIndex));
        }
    
    CleanupStack::PopAndDestroy(credentials);
    CleanupStack::PopAndDestroy(realm);
    }
    
// -----------------------------------------------------------------------------
// CSipCSSession::RemoveCredentialsL
// -----------------------------------------------------------------------------
//     
void CSipCSSession::RemoveCredentialsL (const RMessage2& aMessage)
    {
    HBufC8* realm = ITC().ReadLC(aMessage,ESipItcArgRealm);    
    User::LeaveIfError(ServerCore().SIPSec().RemoveCredentials(*realm));
    CleanupStack::PopAndDestroy(realm);
    }

// -----------------------------------------------------------------------------
// CSipCSSession::IgnoreChallengeL
// -----------------------------------------------------------------------------
// 	
void CSipCSSession::IgnoreChallengeL (const RMessage2& aMessage)
    {
    HBufC8* realm = ITC().ReadLC(aMessage,ESipItcArgRealm);
    TSIPIds ids = ITC().ReadSipIdsL(aMessage);
    TTransactionId taId = ids.iRequestId;
    const MSIPSecUser* sipSecUser = FindSIPSecUser(ids.iRefreshId,taId);
    User::LeaveIfError(
        ServerCore().SIPSec().IgnoreChallenge(taId,*realm,sipSecUser));
    CleanupStack::PopAndDestroy(realm);
    }

// -----------------------------------------------------------------------------
// CSipCSSession::SetSecurityHandlingL
// -----------------------------------------------------------------------------
//
void CSipCSSession::SetSecurityHandlingL(const RMessage2& aMessage)
    {
    TInt securityHandlingEnabled(0);
    ITC().ReadL(aMessage,securityHandlingEnabled,ESipItcArgSecurityHandling);
    iByPassSIPSec = !securityHandlingEnabled;
    }

// -----------------------------------------------------------------------------
// CSipCSSession::SigCompSupportedL
// -----------------------------------------------------------------------------
//
void CSipCSSession::SigCompSupportedL ()
    {
    if (!ServerCore().SigComp().IsSupported())
        {
		User::Leave(KErrNotSupported);
        }
    }

// -----------------------------------------------------------------------------
// CSipCSSession::GetTransactionStateL
// -----------------------------------------------------------------------------
//
void CSipCSSession::GetTransactionStateL (const RMessage2& aMessage)
    {
    TSIPIds ids = ITC().ReadSipIdsL(aMessage);
    CSIPInternalStates::TState state;
    ServerCore().TU().GetStateL(ids.iRequestId,state);
    ITC().WriteL(aMessage,state,ESipItcArgInternalState);
    }

// -----------------------------------------------------------------------------
// CSipCSSession::GetDialogStateL
// -----------------------------------------------------------------------------
//
void CSipCSSession::GetDialogStateL (const RMessage2& aMessage)
    {
    TSIPIds ids = ITC().ReadSipIdsL(aMessage);
    CSIPInternalStates::TState state;
    ServerCore().DialogMgr().GetStateL(ids.iDialogId,state);
    ITC().WriteL(aMessage,state,ESipItcArgInternalState);
    }

// -----------------------------------------------------------------------------
// CSipCSSession::GetCompartmentCountL
// -----------------------------------------------------------------------------
//
void CSipCSSession::GetCompartmentCountL (const RMessage2& aMessage)
    {
    TInt count = ServerCore().SigComp().CompartmentCount();
    ITC().WriteL(aMessage,count,ESipItcArgInternalState);
    }

// -----------------------------------------------------------------------------
// CSipCSSession::OpenSubSessionL
// -----------------------------------------------------------------------------
//
void CSipCSSession::OpenSubSessionL (TUint32 aIapId, const RMessage2& aMessage)
    {
    TInt err;
    if (Exists(aIapId))
        {
        User::Leave(KErrAlreadyExists);
        }
    CSipCSSubSession* subSession = 
        CSipCSSubSession::NewLC(*this,*iReceiver,aIapId);
    iReceiver->MovePendingErrorsL(aIapId,subSession->Receiver());
	iSubSessions->AddL(subSession);
    TInt handle = iSubSessionIndex->AddL(subSession);
    CleanupStack::Pop(); // subSession
    TRAP (err, subSession->OpenIapL(aMessage));
    if (err)
        {
        iSubSessionIndex->Remove(handle);
        User::Leave(err);
        }
    TRAP (err, ITC().WriteL(aMessage,handle,ESipItcArgSubSessionHandle));
    if (err)
        {
        iSubSessionIndex->Remove(handle);
        User::Leave(err);
        }
    }

// -----------------------------------------------------------------------------
// CSipCSSession::CloseSubSession
// -----------------------------------------------------------------------------
//
void CSipCSSession::CloseSubSession (TInt aSubSessionHandle)
    {
	iSubSessionIndex->Remove(aSubSessionHandle);
    }

// -----------------------------------------------------------------------------
// CSipCSSession::ClientReadyToReceiveL
// -----------------------------------------------------------------------------
//
void CSipCSSession::ClientReadyToReceiveL (const RMessage2& aMessage)
	{
    iReceiver->ClientReadyToReceiveL(aMessage);
	}

// -----------------------------------------------------------------------------
// CSipCSSession::ReceiveSipMessageL
// -----------------------------------------------------------------------------
//
void CSipCSSession::ReceiveSipMessageL (const RMessage2& aMessage)
	{
    iReceiver->WriteSipMessageToClientL(aMessage);
	}

// -----------------------------------------------------------------------------
// CSipCSSession::CancelClientReceiveL
// -----------------------------------------------------------------------------
//
void CSipCSSession::CancelClientReceiveL ()
	{
    iReceiver->CancelClientReceiveL();
	}

// -----------------------------------------------------------------------------
// CSipCSSession::CallSubSessionL
// -----------------------------------------------------------------------------
//
void CSipCSSession::CallSubSessionL (TInt aSubSessionHandle,
                                     const RMessage2& aMessage)
    {
    CSipCSSubSession* subSession =
        static_cast<CSipCSSubSession*>(
            iSubSessionIndex->AtL(aSubSessionHandle));
    TSIPIds ids = ITC().ReadSipIdsL(aMessage);
    subSession->ServiceL(ids,aMessage.Function(),aMessage);
    }

// -----------------------------------------------------------------------------
// CSipCSSession::Exists
// -----------------------------------------------------------------------------
//
TBool CSipCSSession::Exists (TUint32 aIapId)
    {
    for (TInt i=0; i < iSubSessions->Count(); i++)
        {
        CSipCSSubSession* subSession =
            static_cast<CSipCSSubSession*>((*iSubSessions)[i]);
        if (subSession->IapId() == aIapId)
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSipCSSession::FindSIPSecUser
// -----------------------------------------------------------------------------
//
const MSIPSecUser* CSipCSSession::FindSIPSecUser(
    TRefreshId aRefreshId, 
    TTransactionId aTransactionId)
    {
    if (aRefreshId != KEmptyRefreshId)
        {
        return ServerCore().RefreshMgr().SIPSecUser(aRefreshId);
        }
    if (aTransactionId == KEmptyTransactionId)
    	{
    	return NULL;
    	}

    const MSIPSecUser* user = 
        ServerCore().RegistrationMgr().SIPSecUserForTransaction(aTransactionId);
    if (!user)
        {
        user = 
            ServerCore().DialogMgr().SIPSecUserForTransaction(aTransactionId);
        }
    if (!user)
        {
        // If the transaction ID is not empty, 
        // the user of SIP API implements MSIPHttpDigestChallengeObserver2
        user = iReceiver;
        }
    return user;
    }
