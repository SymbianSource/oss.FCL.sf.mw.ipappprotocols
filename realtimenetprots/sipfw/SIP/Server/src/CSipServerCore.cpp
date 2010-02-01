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
// Name          : CSipServerCore.cpp
// Part of       : SIPServerCore
// Version       : SIP/4.0
//



#include <ecom/ecom.h>
#include <featmgr.h>
#include "CSipServerCore.h"
#include "sipclientserver.h"
#include "CleanupResetAndDestroy.h"
#include "CSipCSServer.h"
#include "CSipCSServerITC.h"
#include "CSipCSServerCloseTimer.h"
#include "CSipActiveScheduler.h"
#include "CSipCSSession.h"
#include "TimerManager.h"
#include "CSIPRoutingRequestStore.h"
#include "SipRequestHandlerBase.h"
#include "SipRequestHandlerInitParams.h"
#include "sipsec.h"
#include "CTransactionUser.h"
#include "CSipDialogMgr.h"
#include "MSipDialogs.h"
#include "CSIPRegistrationMgr.h"
#include "CSipRefreshMgr.h"
#include "CSipConnectionMgr.h"
#include "TSIPLogger.h"
#include "SipLogs.h"
#include "sipstrings.h"
#include "SIPHeaderLookup.h"


#if (defined (__WINS__) || defined(__WINSCW__))
#include "TSIPMemAllocFailureSimulation.h"
#endif

const TUint KServerCloseWaitTime = 500; // milliseconds


// -----------------------------------------------------------------------------
// CSipServerCore::NewL
// -----------------------------------------------------------------------------
//
CSipServerCore* CSipServerCore::NewL ()
	{
    CSipServerCore* self = CSipServerCore::NewLC();
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSipServerCore::NewLC
// -----------------------------------------------------------------------------
//
CSipServerCore* CSipServerCore::NewLC ()
	{
	CSipServerCore* self = new(ELeave)CSipServerCore;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
	}

// -----------------------------------------------------------------------------
// CSipServerCore::ConstructL
// -----------------------------------------------------------------------------
//
void CSipServerCore::ConstructL ()
	{
	SIPStrings::OpenL();
	SIPHeaderLookup::OpenL();
	SIPHeaderLookup::SetConvertToSIPURI();
	FeatureManager::InitializeLibL();
	iFeatMgrInitialized = ETrue;
    iActiveScheduler = new (ELeave) CSipActiveScheduler ();
    CActiveScheduler::Install (iActiveScheduler);
	iITC = CSipCSServerITC::NewL();
    iContainerIndex = CObjectConIx::NewL ();
	iCloseTimer = CSipCSServerCloseTimer::NewL();

    iTimerMgr = CTimerManager::NewL ();
    
	iRoutingRequestStore = CSIPRoutingRequestStore::NewL(*this);
	LoadRequestHandlerL();
	
    iTU = CTransactionUser::NewL (iRoutingRequestStore,iTimerMgr,*this);
	iRefreshMgr = CSipRefreshMgr::NewL (*iTU,*iTimerMgr,SIPSec());
	iRegistrationMgr = CSIPRegistrationMgr::NewL(*iTU,*iRefreshMgr,*iTimerMgr,
	                                             SIPSec(),SigComp(),
	                                             iTU->SipConnectionMgr());
	iTU->SetRegistrations (iRegistrationMgr,iRegistrationMgr);
	iDialogMgr = CSipDialogMgr::NewL(*iTU,*iRefreshMgr,*iRegistrationMgr,
	                                 SIPSec(),SigComp());
	iTU->SetDialogs (iDialogMgr);

#if (defined (__WINS__) || defined(__WINSCW__))
	TSIPMemAllocFailureSimulation::Start();
#endif

	iServer = CSipCSServer::NewL(*this);

    __SIP_LOG("SIP Server Created")
	}

// -----------------------------------------------------------------------------
// CSipServerCore::CSipServerCore
// -----------------------------------------------------------------------------
//
CSipServerCore::CSipServerCore ()
	{
	iFeatMgrInitialized = EFalse;
	}

// -----------------------------------------------------------------------------
// CSipServerCore::~CSipServerCore
// -----------------------------------------------------------------------------
//
CSipServerCore::~CSipServerCore ()
	{
	// This must be deleted first to remove SIP server from system's
	// list of servers asap.
	delete iServer;

#if (defined (__WINS__) || defined(__WINSCW__))
	TSIPMemAllocFailureSimulation::Stop();
#endif

    delete iDialogMgr;
    delete iRegistrationMgr;
    delete iRefreshMgr;
    delete iTU;
    delete iRequestHandler;
    delete iRoutingRequestStore;
    delete iTimerMgr;
	delete iCloseTimer;
    delete iContainerIndex;
	delete iITC;
    delete iActiveScheduler;
	CActiveScheduler::Install (NULL);
	SIPHeaderLookup::Close();
	SIPStrings::Close();
	REComSession::FinalClose();
	if(iFeatMgrInitialized)
		{
		FeatureManager::UnInitializeLib();
		}
	
	__SIP_LOG("SIP server shutdown completed")

#ifdef _BullseyeCoverage
    // Store coverage measurement
    cov_write();
#endif	
	}

// -----------------------------------------------------------------------------
// CSipServerCore::IncrementSessions
// -----------------------------------------------------------------------------
//
void CSipServerCore::IncrementSessions ()
	{
	if (iCloseTimer->IsActive())
        {
        iCloseTimer->Cancel();
        }
    iSessionCount++;
	}

// -----------------------------------------------------------------------------
// CSipServerCore::DecrementSessions
// -----------------------------------------------------------------------------
//
void CSipServerCore::DecrementSessions ()
	{
    iSessionCount--;
	if (iSessionCount == 0)
		{
		iCloseTimer->StopActiveSchedulerAfter(KServerCloseWaitTime);
		}
	}
	
// -----------------------------------------------------------------------------
// CSipServerCore::CreateTransportOwnerId
// -----------------------------------------------------------------------------
//
TUint32 CSipServerCore::CreateTransportOwnerId()
	{
	if (iTransportOwnerIdCounter < KMaxTUint32)
		{
		iTransportOwnerIdCounter++;
		}
	else
		{
		iTransportOwnerIdCounter = 1;
		}	
	return iTransportOwnerIdCounter;
	}	

// -----------------------------------------------------------------------------
// CSipServerCore::Server
// -----------------------------------------------------------------------------
//
CServer2& CSipServerCore::Server ()
	{
	return *iServer;
	}

// -----------------------------------------------------------------------------
// CSipServerCore::ObjectConIx
// -----------------------------------------------------------------------------
//
CObjectConIx& CSipServerCore::ObjectConIx ()
	{
	return *iContainerIndex;
	}

// -----------------------------------------------------------------------------
// CSipServerCore::ITC
// -----------------------------------------------------------------------------
//
CSipCSServerITC& CSipServerCore::ITC ()
	{
	return *iITC;
	}

// -----------------------------------------------------------------------------
// CSipServerCore::TU
// -----------------------------------------------------------------------------
//
CTransactionUser& CSipServerCore::TU ()
	{
    return *iTU;
	}

// -----------------------------------------------------------------------------
// CSipServerCore::DialogMgr
// -----------------------------------------------------------------------------
//
CSipDialogMgr& CSipServerCore::DialogMgr ()
	{
    return *iDialogMgr;
	}

// -----------------------------------------------------------------------------
// CSipServerCore::RegistrationMgr
// -----------------------------------------------------------------------------
//
CSIPRegistrationMgr& CSipServerCore::RegistrationMgr ()
	{
    return *iRegistrationMgr;
	}

// -----------------------------------------------------------------------------
// CSipServerCore::Registrations
// -----------------------------------------------------------------------------
//
MSipRegistrations& CSipServerCore::Registrations ()
	{
    return *iRegistrationMgr;
	}

// -----------------------------------------------------------------------------
// CSipServerCore::RefreshMgr
// -----------------------------------------------------------------------------
//
MSipRefreshMgr& CSipServerCore::RefreshMgr ()
	{
    return *iRefreshMgr;
	}
	
// -----------------------------------------------------------------------------
// CSipServerCore::SIPSec
// -----------------------------------------------------------------------------
//	
CSIPSec& CSipServerCore::SIPSec()
    {
    return iTU->SIPSec();
    }
   
// -----------------------------------------------------------------------------
// CSipServerCore::RequestHandler
// -----------------------------------------------------------------------------
//	   
CSIPRequestHandlerBase& CSipServerCore::RequestHandler()
	{
	return *iRequestHandler;
	}
	
// -----------------------------------------------------------------------------
// CSipServerCore::SigComp
// -----------------------------------------------------------------------------
//	
MSigComp& CSipServerCore::SigComp()
    {
    return iTU->SipConnectionMgr();
    }
    
// -----------------------------------------------------------------------------
// CSipServerCore::ConnectionMgr
// -----------------------------------------------------------------------------
//    
CSipConnectionMgr& CSipServerCore::ConnectionMgr()
    {
    return iTU->SipConnectionMgr();
    }	
   
// -----------------------------------------------------------------------------
// CSipServerCore::FindOwner
// -----------------------------------------------------------------------------
//    
MTransactionOwner* CSipServerCore::FindOwner(TUint32 aIapId, const TUid& aUid)
	{
	return iServer->FindOwner(aIapId,aUid);
	}

// -----------------------------------------------------------------------------
// CSipServerCore::ConnectionStateChangedL
// From MConnectionStateObserver:
// -----------------------------------------------------------------------------
//
void CSipServerCore::ConnectionStateChangedL (TUint32 aIapId,
                                              CSIPConnection::TState aState)
	{
	if (iServer)
		{
		iServer->ConnectionStateChangedL(aIapId,aState);
		}
	// inform all the interested subsystems if they exist
	if (iRefreshMgr)
        {
        iRefreshMgr->ConnectionStateChangedL (aIapId,aState);
        }
	if (iRegistrationMgr)
        {
        iRegistrationMgr->ConnectionStateChangedL(aIapId,aState);
        }
	if (iDialogMgr)
        {
        iDialogMgr->ConnectionStateChangedL (aIapId,aState);
        }
	}

// -----------------------------------------------------------------------------
// CSipServerCore::LoadRequestHandlerL
// -----------------------------------------------------------------------------
//	
void CSipServerCore::LoadRequestHandlerL()
	{
	RImplInfoPtrArray infoArray;
	REComSession::ListImplementationsL(KSIPRequestHandlerIFUid,
	                                   infoArray);
	                                   
	CleanupResetAndDestroyPushL(infoArray);
	
	if (infoArray.Count() > 0 && infoArray[ 0 ])
	    {
	    CImplementationInformation& info = *(infoArray[ 0 ]);
	    TSIPRequestHandlerInitParams initParams(KServerUid3,
	                                            iRoutingRequestStore);
	    TEComResolverParams resolverParams;                                        
	    resolverParams.SetDataType(info.DataType());
	 
		iRequestHandler = 
	        reinterpret_cast< CSIPRequestHandlerBase* >( 
	            REComSession::CreateImplementationL(
	                KSIPRequestHandlerIFUid, 
                    _FOFF(CSIPRequestHandlerBase,iInstanceKey),
                    &initParams,
                    resolverParams));
	    }
	else
	    {
	    User::Leave(KErrNotFound);
	    }
	    
	CleanupStack::PopAndDestroy(1); // infoArray		
	}

// -----------------------------------------------------------------------------
// CSipServerCore::RefreshConnection
// -----------------------------------------------------------------------------
//
void CSipServerCore::RefreshConnectionL(TUint32 aIapId)
	{
    ConnectionMgr().RefreshConnectionL(aIapId);
	}

// -----------------------------------------------------------------------------
// CSipServerCore::GetConnectionErrorL
// -----------------------------------------------------------------------------
//
void CSipServerCore::GetConnectionErrorL(TUint32 aIapId,TInt& aError)
	{
	aError = ConnectionMgr().ConnectionErrorL(aIapId);
	}
