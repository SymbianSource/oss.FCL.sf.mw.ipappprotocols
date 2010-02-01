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
// Name          : UserAgentCreateParams.cpp
// Part of       : TransactionUser
// Version       : SIP/5.0 
//



#include "MSipRegistrations.h"
#include "MSigComp.h"
#include "UserAgentCreateParams.h"

#ifdef CPPUNIT_TEST
#include "TestCleanupStack.h"
#endif


// -----------------------------------------------------------------------------
// CUserAgentCreateParams::NewLC
// -----------------------------------------------------------------------------
//
CUserAgentCreateParams*
CUserAgentCreateParams::NewLC(CTransactionMgr& aTransactionMgr,
                              MTimerManager& aTimers,
							  CSIPMessageUtility& aSIPMsgUtility,
							  TTimerValues& aTimerValues,
							  MSipRegistrations& aRegistrations,
                              MSipRegistrationContact& aRegistrationContact,
                              MSigComp& aSigComp,
                              CTransactionStore& aTransactionStore,
                              CDeleteMgr& aDeleteMgr,
							  TBool aOwnsOutgoingMsg,
                              const TSIPTransportParams& aTransportParams)
	{
	CUserAgentCreateParams* self =
        new (ELeave) CUserAgentCreateParams(aTransactionMgr,
                                            aTimers,
										    aSIPMsgUtility,
										    aTimerValues,
										    aRegistrations,
                                            aRegistrationContact,
                                            aSigComp,
                                            aTransactionStore,
                                            aDeleteMgr,
										    aOwnsOutgoingMsg,
                                            aTransportParams);
	CleanupStack::PushL(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CUserAgentCreateParams::CUserAgentCreateParams
// -----------------------------------------------------------------------------
//
CUserAgentCreateParams::CUserAgentCreateParams(
                               CTransactionMgr& aTransactionMgr,
                               MTimerManager& aTimers,
							   CSIPMessageUtility& aSIPMsgUtility,
							   TTimerValues& aTimerValues,
							   MSipRegistrations& aRegistrations,
                               MSipRegistrationContact& aRegistrationContact,
                               MSigComp& aSigComp,
                               CTransactionStore& aTransactionStore,
                               CDeleteMgr& aDeleteMgr,
							   TBool aOwnsOutgoingMsg,
                               const TSIPTransportParams& aTransportParams) :
	iTransactionId(KEmptyTransactionId),    
	iTransactionMgr(aTransactionMgr),
    iTimers(aTimers),
	iRegistrations(aRegistrations),
    iRegistrationContact(aRegistrationContact),
    iSigComp(aSigComp),
    iTransactionStore(aTransactionStore),
    iDeleteMgr(aDeleteMgr),
	iSIPMsgUtility(aSIPMsgUtility),
	iTimerValues(aTimerValues),    
	iOwnsOutgoingMsg(aOwnsOutgoingMsg),
    iTransportParams(aTransportParams)
	{
	}

// -----------------------------------------------------------------------------
// CUserAgentCreateParams::~CUserAgentCreateParams
// -----------------------------------------------------------------------------
//
CUserAgentCreateParams::~CUserAgentCreateParams()
	{
	}
