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
* Name        : unregistered.h
* Part of     : SIPAPI
* Interface   : 
* Version     :
*
*/




/**
 @internalComponent
*/

#ifndef CUNREGISTERED_H
#define CUNREGISTERED_H

// INCLUDES
#include "sipregistrationstate.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION

class CUnregistered : public CRegistrationState
	{
public: // Constructor and destructor

    static CUnregistered* NewL();

	~CUnregistered();

public: // From CRegistrationState
    
	CSIPClientTransaction* RegisterL(CRegBindingImplementation& aRegistration,
                                     CSIPMessageElements* aElements) const;
    	
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

    void ConnectionLost(CRegBindingImplementation& aRegistration) const;

public: // New functions

	void SetNeighbourStates(CRegistrationState& aRegistering);

private: // For internal use

	CUnregistered();

private: // Data

	// Next possible state, not owned.
    CRegistrationState* iRegistering;
	};

#endif
