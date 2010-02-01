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
* Name        : sipregistrationstate.h
* Part of     : SIPAPI
* Interface   : 
* Version     :
*
*/




/**
 @internalComponent
*/

#ifndef CSIPREGISTRATIONSTATE_H
#define CSIPREGISTRATIONSTATE_H

// INCLUDES
#include <e32base.h>
#include "siprefresh.h"

// FORWARD DECLARATIONS
class CSIPRefresh;
class CSIPMessageElements;
class CSIPResponseElements;
class CSIPClientTransaction;
class CRegBindingImplementation;
class CConnectionCallback;

// CLASS DECLARATION

/*
 * Base class for all registration states.
 */
class CRegistrationState : public CBase
	{
public:
	virtual ~CRegistrationState();

    virtual TBool IsContextActive() const;

	virtual CSIPClientTransaction* 
        RegisterL(CRegBindingImplementation& aRegistration,
                  CSIPMessageElements* aElements) const;
    
	virtual CSIPClientTransaction* 
        DeregisterL(CRegBindingImplementation& aRegistration,
                    CSIPMessageElements* aElements) const;    

    virtual CSIPClientTransaction*
        UpdateL(CRegBindingImplementation& aRegistration,
                CSIPMessageElements* aElements,
                TUint* aExpirationValue) const;

    virtual TBool IncomingResponseL(CRegBindingImplementation& aRegistration,
                                    CSIPResponseElements* aElements,
		                            TUint32 aRequestId,
					                TUint32 aRegistrationId,
					                TUint32 aRefreshId,
                                    CConnectionCallback& aCallback) const;

    virtual TBool ErrorOccured(CRegBindingImplementation& aRegistration,
                               TInt aError,
                               TUint32 aSIPRequestId,
                               CConnectionCallback& aCallback) const;

    virtual CSIPRefresh::TState RefreshState() const;

    virtual void
    	ConnectionLost(CRegBindingImplementation& aRegistration) const = 0;
	};

#endif
