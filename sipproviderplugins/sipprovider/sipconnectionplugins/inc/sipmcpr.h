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
// SIP Meta Connection Provider class 
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_SIPMCPR_H
#define SYMBIAN_SIPMCPR_H

#include <comms-infras/coremcpr.h>
#include <comms-infras/ss_mcprnodemessages.h>
#include <es_connpref.h>

#include <comms-infras/ss_log.h>
#include <comms-infras/coremcpractivities.h>
//#include <coreavailabilityactivity.h>
#include <comms-infras/coremcprstates.h>
#include <comms-infras/ss_corepractivities.h>
//#include <comms-infras/ss_commsprov.h>

#include "sipprovisioninfo.h"
#include "TransitionEngineMgr.h"


namespace SipMCprStates
{
class TSendProvision;
}

//
//CNetworkMetaConnectionProvider
NONSHARABLE_CLASS(CSipMetaConnectionProvider) : public CCoreMetaConnectionProvider
    {
    friend class SipMCprStates::TSendProvision;
public:
	
	/**
	Constructor of the class
	*/
	static CSipMetaConnectionProvider* NewL(ESock::CMetaConnectionProviderFactoryBase& aFactory,
	                                            const ESock::TProviderInfo& aProviderInfo);
	/**
	Destructor of the class
	*/                                         
	virtual ~CSipMetaConnectionProvider();
	
	/**
	setd the applicaiton UID. Usually it will be the UID of the test application
	
	@param aUid   Uid of the applicaion
	*/
	void SetAppUid(TUid aUid)	
		{
		iAppUid = aUid;
		}
	
	/**
	returns the application UID	
    */
	TUid AppUid()
		{
		return iAppUid;
		}
	
	/**
	Sets the profile ID
	
	@param aProfileId, an ID of the SIP profile. This will be used by the SIP CPR and SIP SCPR 		
	*/	
	void SetProfileId(TUint32 aProfileId)
		{
		iProfileId = aProfileId;
		}
	/**
	Returns the profile ID
	*/	
	TUint32 ProfileId()
		{
		return iProfileId;
		}
		
	/**
	Sets the SIP Transition engine manager
	
	@param aMgr, a Transition engine manager which is used to find or create the transition engine
	*/	
	void SetTransitionEngineManager(CTransitionEngineMgr* aMgr)
		{
		iTransitionEngineMgr = aMgr;
		}
	
protected:
    CSipMetaConnectionProvider(ESock::CMetaConnectionProviderFactoryBase& aFactory,
                                   const ESock::TProviderInfo& aProviderInfo,
                                   const MeshMachine::TNodeActivityMap& aActivityMap);
    /**
    This funtion receives the comms messages and passes it for further processing
    */
    virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);
    
    /**
    This function constructs and appends the SIP MCPR provision info that is
    being sent in ECFProvisionConfig message to the SIP CPR
    */
    void SetConfigL();
    
    void ConstructL(const ESock::TProviderInfo& aProviderInfo);
	
	
	TUint32	iProfileId;
	TUid	iAppUid;
	CTransitionEngineMgr	*iTransitionEngineMgr; // we dont own it, TierMgr does
    
    };
    
//-=========================================================
//
// States
//
//-=========================================================
namespace SipMCprStates
{
typedef MeshMachine::TNodeContext<CSipMetaConnectionProvider, MCprStates::TContext> TContext;

DECLARE_SMELEMENT_HEADER( TSendProvision, MeshMachine::TStateTransition<TContext>, NetStateMachine::MStateTransition, TContext )
	virtual void DoL();
DECLARE_SMELEMENT_FOOTER( TSendProvision )

/**
This will be used by the MCPR binder activity. SendProvision is overridev to send the 
SIP MCPR provisioninfo in the provisionConfig message to SIP CPR
*/
DECLARE_AGGREGATED_TRANSITION3(
   TProcessCprCreation,
   CoreNetStates::TAddDataClient,
   SipMCprStates::TSendProvision,
   PRActivities::CCommsBinderActivity::TStorePendingBinder
   )
}

namespace SipMcpr
{
DECLARE_EXPORT_ACTIVITY_MAP(SipMcprActivities)
}
#endif //SYMBIAN_SIPMCPR_H
