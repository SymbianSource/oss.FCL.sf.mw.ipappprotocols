/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CStateInUse.h
* Part of       : SIPSec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSTATEINUSE_H
#define CSTATEINUSE_H

#include "CState.h"

/**
 * @brief CSipSecAgreeRecord state "in use"
 *        Record is in autheticated state - 200 OK has been received through
 *        SA. Record can be used for all requests (not just for registration
 *        as temporal record).
 * 
 */
class CInUse : public CState
	{
public:

	CInUse();
	
	CState::TSecAgreeRecordState State();
	
	void TimerExpiredL( CSipSecAgreeRecord& aRecord, 
						TTimerId aTimerId, 
						TAny* aTimerParam );
			  	  	
	void RegisterL( CSipSecAgreeRecord& aRecord, 
	                CSIPRequest& aRequest, 
	                TSIPTransportParams& aTransportParams,
	                const TDesC8& aOutboundProxy );
	
	void ReguestL( CSipSecAgreeRecord& aRecord, 
	               CSIPRequest& aRequest, 
	               TSIPTransportParams& aTransportParams,
	               const TDesC8& aOutboundProxy );
				   
	void Resp2xxL( CSipSecAgreeRecord& aRecord, CSIPResponse& aResponse );
	
	void ProcessSecVerifyL( 
	            CSipSecAgreeRecord& aRec,
	            RPointerArray<CSIPSecurityVerifyHeader>& aSecurityVerify );
				      
	};
    
#endif // CSTATEINUSE_H

// End of File
