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
* Name          : MSipSecPolicyObserver.h
* Part of       : SIPSec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef MSIPSECPOLICYOBSERVER_H
#define MSIPSECPOLICYOBSERVER_H

#include "CIpSecPolicyHandler.h"

/**
 * @brief Observer for callbacks of policy activating/unloading/errors
 * 
 * 
 */
class MSipSecPolicyObserver
	{

public:

    virtual void PolicyActivatedL() = 0;

	virtual void PolicyUnloaded() = 0;
	
	virtual void PolicyError( CIpSecPolicyHandler::TPolicyHandlerState aState, 
	                          TInt aError ) = 0;

	};

#endif // MSIPSECPOLICYOBSERVER_H


// End of File
