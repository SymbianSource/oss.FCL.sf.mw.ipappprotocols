/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSIPSecSIMCredentialsObserver.h
* Part of       : SIPSec/DigestPlugin
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef __SIPSEC_SIMCREDENTIALSOBSERVER_H__
#define __SIPSEC_SIMCREDENTIALSOBSERVER_H__

// INCLUDES
#include "CSIPSecDigestObserver.h"

/**
 * @brief Class represents SIP security observer listening cache update
 *        completion from SIM
 *
 * @class CSIPSecSIMCredentialsObserver CSIPSecSIMCredentialsObserver.h
 *		  "CSIPSecSIMCredentialsObserver.h"
 */
class CSIPSecSIMCredentialsObserver : public CSIPSecDigestObserver
	{
public: // Constructors and destructor

    /**
    * Constructor
    * @param aObserver SIPSec Observer
    */
    CSIPSecSIMCredentialsObserver(
    	MSIPSecSecurityMechanismObserver& aObserver );

    /**
    * Destructor
    */
    ~CSIPSecSIMCredentialsObserver();

public: // From CSIPSecUserCredentialsRequest

	void OperationCompleted( TStatus aReason, TBool aAuthenticateSuccessful );

protected: // From CSIPSecUserCredentialsRequest

    void DoCompleted();

private: // Data

	// Indicates whether authentication has succeeded	
	TBool iAuthenticated;
	};
	
#endif // __SIPSEC_SIMCREDENTIALSREQUEST_H__

// End of File
