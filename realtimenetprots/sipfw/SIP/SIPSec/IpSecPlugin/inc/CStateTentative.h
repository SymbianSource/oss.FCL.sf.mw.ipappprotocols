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
* Name          : CStateTentative.h
* Part of       : SIPSec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSTATETENTATIVE_H__
#define CSTATETENTATIVE_H__

#include "CState.h"

/**
 * @brief CSipSecAgreeRecord state "tentative"
 *        Record is waiting for challenge
 * 
 */
class CTentative : public CState
	{
public:

	CTentative();
	
	CState::TSecAgreeRecordState State();
			  	  	
	void Resp4xxL( CSipSecAgreeRecord& aRecord, 
	               CSIPResponse& aResponse, 
	               CSIPRequest& aRequest );				      
    };
    
#endif // CSTATETENTATIVE_H__

// End of File
