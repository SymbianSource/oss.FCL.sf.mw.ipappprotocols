/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CState.h
* Part of       : SIPSec
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef CSTATE_H
#define CSTATE_H

#include <e32base.h>

#include "Lwtimer.h"

class CSIPSecurityVerifyHeader;
class CSIPRequest;
class CSIPResponse;
class CSipSecAgreeRecord;
class TSIPTransportParams;

/**
 * @brief base state of CSipSecAgreeRecord states
 */
class CState : public CBase
	{
public:

    // States of IpSec SecAgree record
    enum TSecAgreeRecordState
    	{
        ETentative = 0,
        EPendingKey,
        EPendingSa,
        EPendingPolicy,
        ETemporal,
        EInUse,
        EOld,
    	EDying,
    	EDeleting,
    	EClearing
        };
	
	virtual ~CState();
		
	virtual CState::TSecAgreeRecordState State() = 0;
	
	virtual void SaAddedL( CSipSecAgreeRecord& aRecord );
	
	virtual void SaDeleted( CSipSecAgreeRecord& aRecord );
	
	virtual void PolicyActivatedL( CSipSecAgreeRecord& aRec );
	
	virtual void PolicyUnloaded( CSipSecAgreeRecord& aRec );
	
	virtual void TimerExpiredL( CSipSecAgreeRecord& aRecord, 
								TTimerId aTimerId, 
								TAny* aTimerParam );	
										  	  	
	virtual void Resp4xxL( CSipSecAgreeRecord& aRecord, 
	                       CSIPResponse& aResponse,
	                       CSIPRequest& aRequest );
							  
    virtual TBool AuthKeyL( CSipSecAgreeRecord& aRecord,
						    const TDesC8& aAuthKey );
						    
    virtual void AuthKeyFailedL( CSipSecAgreeRecord& aRec );
							  
	virtual void RegisterL( CSipSecAgreeRecord& aRecord, 
						    CSIPRequest& aRequest,
						    TSIPTransportParams& aTransportParams,
						    const TDesC8& aOutboundProxy );

	virtual void ReguestL( CSipSecAgreeRecord& aRecord, 
						   CSIPRequest& aRequest,
						   TSIPTransportParams& aTransportParams,
						   const TDesC8& aOutboundProxy );
	
	virtual void Resp2xxL( CSipSecAgreeRecord& aRecord,
					       CSIPResponse& aResponse );
					       
	virtual void ProcessSecVerifyL( 
	                CSipSecAgreeRecord& aRec,
	                RPointerArray<CSIPSecurityVerifyHeader>& aSecurityVerify );
					   
	virtual void ClearSaL( CSipSecAgreeRecord& aRecord );
			
protected:
		
	virtual void ReInitializeL( CSipSecAgreeRecord& aRecord,
								CSIPResponse& aResponse,
								TBool aFullReInitialize );
								
	virtual void DeleteSasL( CSipSecAgreeRecord& aRec );
	
	virtual void DoClearSaL( CSipSecAgreeRecord& aRec );	
	
	virtual void DeleteSasAndPolicyL( CSipSecAgreeRecord& aRec );

	virtual void UpdateExpirationTimeL( CSipSecAgreeRecord& aRec,
                                    	CSIPResponse& aResponse );
	};

#endif // CSTATE_H

// End of File
