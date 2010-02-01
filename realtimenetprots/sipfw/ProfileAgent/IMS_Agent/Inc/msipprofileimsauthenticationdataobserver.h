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
* Name          : msipprofileimsauthenticationdataobserver.h
* Part of       : ProfileAgent
* Version       : SIP/4.1 
*
*/




/**
 @internalComponent
*/


#ifndef MSIPPROFILEIMSAUTHENTICATIONDATAOBSERVER_H
#define MSIPPROFILEIMSAUTHENTICATIONDATAOBSERVER_H

// INCLUDES
#include <etelmm.h>
#include <mmretrieve.h>

// FORWARD DECLARATIONS
class CSIPProfileSIMRecord;
class MSIPProfileIMSAuthenticationDataObserver;

/**
 * @brief Class is observer of async call-backs from SIM
 * 
 * @class MSIPProfileIMSAuthenticationDataObserver CSIPProfileUsimProvider 
 * "CSIPProfileUsimProvider.h"
 */
class MSIPProfileIMSAuthenticationDataObserver
	{	
public:
	
    /**
    * Called when Authorization information 
    * from USIM/ISIM/SIM is been resolved
    */		
    virtual void AuthorizedL() = 0;
    
    /**
    * Called when authorization failed
    */		
	virtual void AuthorizationFailed() = 0;
	
	/**
    * Called when authorization failed
    */		
	virtual void AuthorizationSIMFailed() = 0;

	virtual void AsyncDeltaTimer() = 0;
	
	virtual void UpdateRegistrationsL() = 0;
	};

#endif // end of MSIPPROFILEIMSAUTHENTICATIONDATAOBSERVER_H

// End of File
