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
// SIP Connection Provider class 
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SYMBIAN_SIPCPR_H
#define SYMBIAN_SIPCPR_H

#include <comms-infras/ss_log.h>
#include <comms-infras/corecpr.h>
#include "sipcprfactory.h"
#include "sipcprmessages.h"
#include "sipprovisioninfo.h"
#include <comms-infras/interfacetable.h>
#include <comms-infras/ss_coreprstates.h>
#include <comms-infras/ss_corepractivities.h>
#include <comms-infras/ss_mmnode.h>
#include <elements/mm_mutexpolicies.h>
#include "TransitionEngineMgr.h"


#ifdef __CFLOG_ACTIVE
	#define KSipCprTag  KESockConnectionTag
	_LIT8(KSipCprSubTag, "sipcpr");
#endif

namespace SipCprStates
    {
    class TStoreProvision;
    class TRegisterCpr;    
    class TAwaitingRegistrationComplete;
    class TDeRegisterCpr;
    class TAwaitingDeRegistrationComplete;
    class TSendProvision;    
    class TNoTagOrDeRegister;
    class TNoTagOrRegistered;    
    class TIntializeWaitForIncmgParams;       
    class TWaitForIncomingOrUseExisting;
    class TAwaitingIncomingConnection;
    class TSetPostedToScpr;       
    }

class CSipConnectionProvider : public CCoreConnectionProvider, MSIPRegistrationClient
/** Sip Connection Provider

@internalTechnology
@released Since 9.5 */
    {
friend class CSipConnectionProviderFactory;
friend class SipCprStates::TStoreProvision;
friend class SipCprStates::TSendProvision;
friend class SipCprStates::TRegisterCpr;
friend class SipCprStates::TSetPostedToScpr;
friend class SipCprStates::TAwaitingRegistrationComplete;
friend class SipCprStates::TDeRegisterCpr;
friend class SipCprStates::TAwaitingDeRegistrationComplete;
friend class SipCprStates::TNoTagOrDeRegister;
friend class SipCprStates::TNoTagOrRegistered;
friend class SipCprStates::TIntializeWaitForIncmgParams;
friend class SipCprStates::TWaitForIncomingOrUseExisting;
friend class SipCprStates::TAwaitingIncomingConnection;

public:
    typedef CSipConnectionProviderFactory FactoryType;
    
    /**
    Constructs the SIP connection provider
    
    @param aFactory SIP conneciton provider factory    
    */
    
	static CSipConnectionProvider* NewL(ESock::CConnectionProviderFactoryBase& aFactory);
	
	/**
    Destructor of the class
    */
    
	virtual ~CSipConnectionProvider();
	
public:
	//-=========================================================
	// MSIPRegistrationClient methods
	//-=========================================================
	virtual void RegistrationComplete(TInt aError);
	virtual void DeRegistrationComplete(TInt aError);
	virtual void ErrorOccurred(TInt aError);
	virtual TUint32 GetProfileId();	
	virtual TUid GetAppUid();
	virtual CSIPTransitionEngine* GetTE();	
						
	TBool					iIsIncoming;
		
protected:	
    CSipConnectionProvider(ESock::CConnectionProviderFactoryBase& aFactory);
    virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);
	void ConstructL();

	void SetConfigL();
		    
private:
	TUint32					iProfileId;
	TUid					iAppUid;	

	CTransitionEngineMgr*	iTransitionEngineMgr;
	CSIPTransitionEngine*	iTransitionEngine;
    SipCpr::TCprStage		iStage;	
    SipCpr::TConnType		iConnStatus;
 	TDesC8*					iFromField; // this is required by SCPR and will be sent with the Provision message
 	TInt 					iActivityAwaitingResponse;
 	TSipCprProvisionInfo*	iProvisionInfo;
 	
 	Messages::RNodeInterface iSelf;    
 	TInt					iStashedStoppedCode;	
 	TSubConnOpen::TSubConnType iSubconType			;
    };
    
#endif //SYMBIAN_SIPCPR_H
