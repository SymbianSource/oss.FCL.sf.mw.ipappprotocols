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
* Name          : CStateDeleting.h
* Part of       : SIPSec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSTATEDELETING_H
#define CSTATEDELETING_H

#include "CState.h"

/**
 * @brief CSipSecAgreeRecord state "deleting"
 *        Deleting SA and unloading policy
 * 
 */
class CDeleting : public CState
	{
public:
	
	CDeleting();
	
	CState::TSecAgreeRecordState State();
	
	void RegisterL( CSipSecAgreeRecord& aRec,
                    CSIPRequest& aRequest,
                    TSIPTransportParams& aTransportParams,
                   	const TDesC8& aOutboundProxy );
	
	void SaDeleted( CSipSecAgreeRecord& aRecord );
	
	void PolicyUnloaded( CSipSecAgreeRecord& aRec );
	
	void ClearSaL( CSipSecAgreeRecord& aRecord );
	
private:

    void CompleteIfReady( CSipSecAgreeRecord& aRec );
				      
	};
    
#endif // CSTATEDELETING_H

// End of File
