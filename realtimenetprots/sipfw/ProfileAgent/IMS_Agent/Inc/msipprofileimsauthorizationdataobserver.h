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
* Name          : msipprofileimsauthorizationdataobserver.h
* Part of       : ProfileAgent
* Version       : SIP/4.1 
*
*/




/**
 @internalComponent
*/

#ifndef MSIPPROFILEIMSAUTHORIZATIONDATAOBSERVER_H
#define MSIPPROFILEIMSAUTHORIZATIONDATAOBSERVER_H

// INCLUDES
#include <etelmm.h>
#include <mmretrieve.h>

// FORWARD DECLARATIONS
class CSIPProfileSIMRecord;
class MSIPProfileIMSAuthenticationDataObserver;

class MSIPProfileIMSAuthorizationDataObserver
	{	
public:
	
    /**
    * Called when  USIM Authorization info is been resolved
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

	
	/**
    * Called when IMSI from SIM is been resolved
    */
	virtual void AuthorizedSubscriberIdL() = 0;
	
	/**
    * Called when MNC and MCC from SIM are been resolved
    */
	virtual void AuthorizedHomeNetworkIdL() = 0;

	};

#endif // end of MSIPPROFILEIMSAUTHORIZATIONDATAOBSERVER_H

// End of File
