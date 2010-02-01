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
* Name        : registering.h
* Part of     : SIPAPI
* Interface   : 
* Version     :
*
*/




/**
 @internalComponent
*/

#ifndef CREGISTERING_H
#define CREGISTERING_H

// INCLUDES
#include "sipregistrationstate.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

class CRegistering : public CRegistrationState
	{
public: // Constructors and destructor
    static CRegistering* NewL();
	~CRegistering();
	
public: // From CRegistrationState    	
    TBool IncomingResponseL(CRegBindingImplementation& aRegistration,
                            CSIPResponseElements* aElements,
		                    TUint32 aRequestId,
					        TUint32 aRegistrationId,
					        TUint32 aRefreshId,
                            CConnectionCallback& aCallback) const;

    TBool ErrorOccured(CRegBindingImplementation& aRegistration,
                       TInt aError,
                       TUint32 aRequestId,
                       CConnectionCallback& aCallback) const;

    CSIPRefresh::TState RefreshState() const;

    void ConnectionLost(CRegBindingImplementation& aRegistration) const;

public: // New functions
	void SetNeighbourStates(CRegistrationState& aRegistered,
							CRegistrationState& aUnregistered);

private: // For internal use
	CRegistering();

private: // Data
    //Next possible states	
	CRegistrationState* iRegistered;
    CRegistrationState* iUnregistered;
	};

#endif
