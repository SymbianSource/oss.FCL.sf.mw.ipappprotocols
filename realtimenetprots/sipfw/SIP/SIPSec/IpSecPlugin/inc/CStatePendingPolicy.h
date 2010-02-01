/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CStatePendingPolicy.h
* Part of       : SIPSec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSTATEPENDINGPOLICY_H
#define CSTATEPENDINGPOLICY_H

#include "CState.h"

/**
 * @brief CSipSecAgreeRecord state "pending policy"
 *        Record is waiting for IPSec policy activation
 * 
 */
class CPendingPolicy : public CState
	{
public:

	CPendingPolicy();
	
	CState::TSecAgreeRecordState State();
	
	void PolicyActivatedL( CSipSecAgreeRecord& aRecord );
	
	void TimerExpiredL( CSipSecAgreeRecord& aRecord,
						TTimerId aTimerId, 
						TAny* aTimerParam );
						
	void Resp4xxL( CSipSecAgreeRecord& aRecord, 
	               CSIPResponse& aResponse,
	               CSIPRequest& aRequest );
	
	void ClearSaL( CSipSecAgreeRecord& aRec );	
					      
	};
    
#endif // CSTATEPENDINGPOLICY_H

// End of File
