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
* Name          : CStateDying.h
* Part of       : SIPSec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSTATEDYING_H
#define CSTATEDYING_H

#include "CState.h"

/**
 * @brief CSipSecAgreeRecord state "dying"
 *        New SA has been taken in use and old SA will be deleted after
 *        certain time.
 * 
 */
class CDying : public CState
	{
public:

	CDying();
	
	CState::TSecAgreeRecordState State();
	
	void TimerExpiredL( CSipSecAgreeRecord& aRecord,
						TTimerId aTimerId, 
						TAny* aTimerParam );
				      
	};
    
#endif // CSTATEDYING_H

// End of File
