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
* Name          : CStatePendingKey.h
* Part of       : SIPSec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSTATEPENDINGKEY_H
#define CSTATEPENDINGKEY_H

#include "CState.h"

/**
 * @brief CSipSecAgreeRecord state "pending key"
 *        Record is waiting for keying material from SIPSec FW
 * 
 */
class CPendingKey : public CState
	{
public:

	CPendingKey();
	
	CState::TSecAgreeRecordState State();
	
	void TimerExpiredL( CSipSecAgreeRecord& aRecord, 
						TTimerId aTimerId, 
						TAny* aTimerParam );	
								  	  	
	void Resp4xxL( CSipSecAgreeRecord& aRecord, 
	               CSIPResponse& aResponse,
	               CSIPRequest& aRequest );
	
	TBool AuthKeyL( CSipSecAgreeRecord& aRecord, const TDesC8& aAuthKey );
	
	void AuthKeyFailedL( CSipSecAgreeRecord& aRec );
	
	void ClearSaL( CSipSecAgreeRecord& aRec );	
						      
	};
    
#endif // CSTATEPENDINGKEY_H

// End of File
