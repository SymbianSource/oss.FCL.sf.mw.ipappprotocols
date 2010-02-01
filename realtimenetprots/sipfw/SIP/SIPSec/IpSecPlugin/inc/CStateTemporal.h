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
* Name          : CStateTemporal.h
* Part of       : SIPSec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSTATETEMPORAL_H
#define CSTATETEMPORAL_H

#include "CState.h"

/**
 * @brief CSipSecAgreeRecord state "temporal"
 *        Record uses its SA for registration and waits for 200 OK response
 *        to the register.
 * 
 */
class CTemporal : public CState
	{
public:

	CTemporal();
	
	CState::TSecAgreeRecordState State();
	
	void TimerExpiredL( CSipSecAgreeRecord& aRecord,
						TTimerId aTimerId, 
						TAny* aTimerParam );
						
	void Resp4xxL( CSipSecAgreeRecord& aRecord, 
	               CSIPResponse& aResponse,
	               CSIPRequest& aRequest );
	
	void RegisterL( CSipSecAgreeRecord& aRecord, 
	                CSIPRequest& aRequest, 
	                TSIPTransportParams& aTransportParams,
	                const TDesC8& aOutboundProxy );
	
	void Resp2xxL( CSipSecAgreeRecord& aRecord, CSIPResponse& aResponse );
				      
	};
    
#endif // CSTATETEMPORAL_H

// End of File
