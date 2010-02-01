/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Name          : UserAgentCreateParams.h
* Part of       : TransactionUser
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/

#ifndef USERAGENTCREATEPARAMS_H
#define USERAGENTCREATEPARAMS_H

// INCLUDES
#include <e32base.h>
#include "SipStackServerDefs.h"
#include "TSIPTransportParams.h"

// FORWARD DECLARATIONS
class CTransactionMgr;
class MSipRegistrations;
class MSipRegistrationContact;
class MTimerManager;
class MSigComp;
class CDeleteMgr;
class TSIPTransportParams;

class CSIPMessageUtility;
class MTransactionOwner;
class CTransactionStore;
class CUserAgentState;
class TTimerValues;	


// CLASS DECLARATION
/**
 * CUserAgentCreateParams is a wrapper for grouping together the parameters
 * which are common to all different UserAgent types. This is to avoid having
 * so many parameters in the NewL() methods of the CUserAgent derived classes.
 */
class CUserAgentCreateParams : public CBase
	{
	friend class CTransactionUser;
	friend class CUserAgent;
	friend class CInviteUAC;
public: // Constructors and destructor

	static CUserAgentCreateParams*
        NewLC(CTransactionMgr& aTransactionMgr,
              MTimerManager& aTimers,
			  CSIPMessageUtility& aSIPMsgUtility,
			  TTimerValues& aTimerValues,
			  MSipRegistrations& aRegistrations,
              MSipRegistrationContact& aRegistrationContact,
              MSigComp& aSigComp,
              CTransactionStore& aTransactionStore,
              CDeleteMgr& aDeleteMgr,
			  TBool aOwnsOutgoingMsg,
              const TSIPTransportParams& aTransportParams);

	~CUserAgentCreateParams();

private: // Constructor, for internal use

	CUserAgentCreateParams(CTransactionMgr& aTransactionMgr,
                           MTimerManager& aTimers,
						   CSIPMessageUtility& aSIPMsgUtility,
						   TTimerValues& aTimerValues,
						   MSipRegistrations& aRegistrations,
                           MSipRegistrationContact& aRegistrationContact,
                           MSigComp& aSigComp,
                           CTransactionStore& aTransactionStore,
                           CDeleteMgr& aDeleteMgr,
						   TBool aOwnsOutgoingMsg,
                           const TSIPTransportParams& aTransportParams);

private: // Data

	TTransactionId iTransactionId;

	// For UAS, this is NULL, as it is not known at the time the UAS is created
	MTransactionOwner* iObserver;

	CTransactionMgr& iTransactionMgr;
	MTimerManager& iTimers;
	MSipRegistrations& iRegistrations;
    MSipRegistrationContact& iRegistrationContact;
    MSigComp& iSigComp;
	CTransactionStore& iTransactionStore;
	CDeleteMgr& iDeleteMgr;

	// State where the UA state machine begins execution
	CUserAgentState* iInitialUaState;

	CSIPMessageUtility& iSIPMsgUtility;

	// Timer values, read from a configuration file
	TTimerValues& iTimerValues;

	// If ETrue, UA owns the SIP message in CUserAgent::iOutgoingMsg
	TBool iOwnsOutgoingMsg;

    // Transport parameters to use when sending a SIP message to ConnectionMgr
    TSIPTransportParams iTransportParams;

private: //For testing purposes
#ifdef CPPUNIT_TEST	
	friend class CNormalCTa_Completed_Test;
	friend class CNormalCTa_Proceeding_Test;
	friend class CNormalCTa_Trying_Test;
	friend class CNormalSTa_Completed_Test;
	friend class CNormalSTa_Proceeding_Test;
	friend class CNormalSTa_Trying_Test;
	friend class CNormalUAC_ResolveAddress_Test;
	friend class CNormalUAC_WaitResponse_Test;	
	friend class CNormalUAS_GetTxOwner_Test;
	friend class CNormalUAS_Start_Test;
	friend class CNormalUAS_WaitRespFromApp_Test;
	friend class CNormalUAS_FinalRespSent_Test;
	friend class CInviteUAC_Start_Test;
	friend class CInviteUAC_ResolveAddress_Test;
	friend class CInviteUAC_WaitResponse_Test;
	friend class CInviteUAC_WaitAckFromApp_Test;
	friend class CInviteUAC_ResolveAckAddress_Test;
	friend class CInviteUAC_SendingAck_Test;
	friend class CInviteCTa_Calling_Test;
	friend class CInviteCTa_Completed_Test;
	friend class CInviteUAS_Accepted_Test;
	friend class ClientTransaction_Test;
	friend class CServerTransaction_Test;
	friend class CInviteServerTa_Test;
	friend class CUserAgentClient_Test;
	friend class CUserAgentServer_Test;
	friend class CUserAgent_Test;
	friend class CTransactionStore_Test;
	friend class CTransactionInfo_Test;
	friend class CSIPMessageUtility_Test;
	friend class CUserAgentStub;
#endif
	};

#endif // end of USERAGENTCREATEPARAMS_H

// End of File
