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
* Name          : CStatePendingSa.h
* Part of       : SIPSec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSTATEPENDINGSA_H
#define CSTATEPENDINGSA_H

#include "CState.h"

/**
 * @brief CSipSecAgreeRecord state "pending sa"
 *        Record is waiting for SA creation completing
 * 
 */
class CPendingSa : public CState
	{
public:

	CPendingSa();
	
	CState::TSecAgreeRecordState State();
	
	void SaAddedL( CSipSecAgreeRecord& aRecord );
	
	void TimerExpiredL( CSipSecAgreeRecord& aRecord,
						TTimerId aTimerId, 
						TAny* aTimerParam );
						
	void Resp4xxL( CSipSecAgreeRecord& aRecord, 
	               CSIPResponse& aResponse,
	               CSIPRequest& aRequest );
	
	void ClearSaL( CSipSecAgreeRecord& aRec );	
					      
	};
    
#endif // CSTATEPENDINGSA_H

// End of File
