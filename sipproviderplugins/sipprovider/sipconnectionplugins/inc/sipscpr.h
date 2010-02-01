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
// SIP SubConnection Provider class.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_SIPSCPR_H
#define SYMBIAN_SIPSCPR_H

#include <comms-infras/ss_subconnprov.h>
#include <comms-infras/corescpr.h>
#include <comms-infras/corescprstates.h>
#include <comms-infras/corescpractivities.h>
#include <cs_subconevents.h>
#include <cs_subconparams.h>
#include <sip_subconevents.h>
#include <sip_subconparams.h>
#include <comms-infras/ss_nodemessages.h>
#include <comms-infras/ss_mmnode.h>
#include <comms-infras/ss_parameterfamilybundle.h>

#include <comms-infras/ss_log.h>
#include "sipscprfactory.h"
#include "SipStateMachine.h"	
#include "transitionengine.h"
#include "sipprovisioninfo.h" 
#include "sipcprmessages.h"


#ifdef __CFLOG_ACTIVE
	#define KSipSCprTag KESockSubConnectionTag
	_LIT8(KSipSCprSubTag, "sipscpr");
#endif


namespace SipSCprStates
	{

	class TNoTagOrAlreadyStarted;
    class TNoTagOrAlreadyStopped;
   	class TStoreProvision;
    class TStartSession;
    class TAwaitingSessionEstablished;    
    class TStopSession;
    class TAwaitingSessionTerminated;
    class TSetParameters;
    //class TSendProviderDown;
	}


namespace SipSCpr
{
enum TSCprStage
  	{
   	EFresh = 0,
   	EStarting,
   	EActive,
   	EStopping,
   	EStopped
   	};
}
	
class TSipSCprMessages
	{
public:
	enum 
	{ERealmId = 0x200041F6};
private:
	enum
	{	
	ESipSCprSessionEstablished = Messages::TEBase::TNull::EId+8000,
	ESipSCprSessionTerminated,
	ESipSCprSessionError,
	EConnectionUp
	};
public:
	typedef Messages::TMessageSigNumber<ESipSCprSessionEstablished, TSipSCprMessages::ERealmId> TSipSessionEstablished;
	typedef Messages::TMessageSigNumber<ESipSCprSessionTerminated, TSipSCprMessages::ERealmId> TSipSessionTerminated;
	typedef Messages::TMessageSigNumber<ESipSCprSessionError, TSipSCprMessages::ERealmId> TSipSessionError;
	typedef Messages::TMessageSigNumber<EConnectionUp, TSipSCprMessages::ERealmId> TConnectionUp;	
	};


class CSipSubConnectionProvider : public CCoreSubConnectionProvider,
									public MSIPStateMachineClient
/** SIP subconnection provider

@internalTechnology
@released Since 9.4 */
    {
    friend class CSipSubConnectionProviderFactory;
	friend class SipSCprStates::TNoTagOrAlreadyStarted;
    friend class SipSCprStates::TNoTagOrAlreadyStopped;
   	friend class SipSCprStates::TStoreProvision;
    friend class SipSCprStates::TStartSession;
    friend class SipSCprStates::TAwaitingSessionEstablished;    
    friend class SipSCprStates::TStopSession;
    friend class SipSCprStates::TAwaitingSessionTerminated;
    friend class SipSCprStates::TSetParameters;
    //friend class SipSCprStates::TSendProviderDown;
    
	
public:
    typedef CSipSubConnectionProviderFactory FactoryType;   
    
    static CSipSubConnectionProvider* NewL(ESock::CSubConnectionProviderFactoryBase& aFactory, TSubConnOpen::TSubConnType aSubConnType);
    
    /**
    Destructor of the class
    */
	virtual ~CSipSubConnectionProvider();
	
	/**
	Returns the address of the object of this class. This address will be used to create 
	SIP high-level state machine
	*/
	MSIPStateMachineClient* NodeAddr();
	
	//-=========================================================
	// MSIPStateMachineClient methods
	//-=========================================================	
	virtual void CallEstablished();
	virtual void CallTerminated(TInt aError, TInt aSipCode);
	virtual void IncomingCall(TIncomingCallParameters& aCallParams);
	virtual void CredentialsRequired(const TDesC8 & aRealm);    
	virtual void ReceiveNotification(TDesC8 & aNotification);
	
	TSubConnOpen::TSubConnType subconType;

protected:
    CSipSubConnectionProvider(ESock::CSubConnectionProviderFactoryBase& aFactory);
    /**
    Second-level of the construction of the object of the class
    
    @aSubConnType  subconnection type (Could be CreateNew, Default and WaitIncoming)
    */
    void ConstructL(TSubConnOpen::TSubConnType aSubConnType);
    
    virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);
    
    /**
    intializes the iParameterBundle with the incoming conneciton paramenter values
    
    @param aCallParams, incoming connection parameter values
    */
	void InitialiseParametersL(TIncomingCallParameters& aCallParams);
	
	// Sets the parameters(invite or subscribe) into SIP state machine
	void DoParametersAboutToBeSetL(ESock::RCFParameterFamilyBundle_const& aParameterBundle);
	
	/**
	Creates the SIP High-level state machine 
	*/
	void CreateStateMachineL();
	
		
protected:
	TBool					iAwaitingSubConnection;
	SipSCpr::TSCprStage		iStage;
	CSipStateMachine*		iSipSm;
	CSIPTransitionEngine*	iTransitionEngine;
	//TPtrC8					iFromField;
	TDesC8*					iFromField;
	TInt					iStopCode;
	TInt 					iActivityAwaitingResponse;
    };

#endif //SYMBIAN_SIPSCPR_H

