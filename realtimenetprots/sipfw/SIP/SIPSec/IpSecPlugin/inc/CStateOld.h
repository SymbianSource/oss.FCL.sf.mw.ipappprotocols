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
* Name          : CStateOld.h
* Part of       : SIPSec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSTATEOLD_H
#define CSTATEOLD_H

#include "CState.h"

/**
 * @brief CSipSecAgreeRecord state "old"
 *        Other record has been taken in use but old SA is still needed, when
 *        first request is sent through new SA, old one will change its state
 *        to dying
 * 
 */
class COld : public CState
	{
public:

	COld();
	
	CState::TSecAgreeRecordState State();
	
	void TimerExpiredL( CSipSecAgreeRecord& aRecord,
						TTimerId aTimerId, 
						TAny* aTimerParam );
						
	void ReguestL( CSipSecAgreeRecord& aRecord, 
	               CSIPRequest& aRequest, 
	               TSIPTransportParams& aTransportParams,
	               const TDesC8& aOutboundProxy );
		      
	};		
    
#endif // CSTATEOLD_H

// End of File
