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
* Name          : CSIPSecDigestObserver.h
* Part of       : SIPSec
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef __SIPSEC_DIGESTOBSERVER_H__
#define __SIPSEC_DIGESTOBSERVER_H__

#include <e32base.h>
#include "MSIPSecSecurityMechanismObserver.h"


/**
 * @brief Class represents SIP security observer listening cache update
 * 		  completion
 * 
 * @class CSIPSecDigestObserver CSIPSecDigestObserver.h
 *		  "CSIPSecDigestObserver.h"
 *
 * This class provides functions to
 * - indicate (OperationStarted) that user credentials were not ready but they
 *	 have been requested
 * - indicate (OperationCompleted) that user credentials have arrived
 *
 */
class CSIPSecDigestObserver : public CBase
	{
public: // Enums

    enum TStatus
        {
        EWaiting,
        EComplete,
        EFailure,
        ECancelled
        };

public: // Constructor and destructor

    /**
    * Constructor
    * @param aObserver SIPSec Observer
    */	
    CSIPSecDigestObserver( MSIPSecSecurityMechanismObserver& aObserver );

    /**
    * Destructor
    */	
    ~CSIPSecDigestObserver();

public: // New functions

    /**
    * Request has been started
    */	
    void OperationStarted();

    /**
    * Called when credentials has been arrived
    * @param aReason Reason why operaion completes
    * @param aAuthenticated ETrue if authentication has completed successfully
    *		 				EFalse otherwise
    */
    virtual void OperationCompleted( TStatus aReason, TBool aAuthenticated );

    /**
    * Checks if this object is associated with the specified SIPSec observer.
    * @param aObserver the SIPSec observer
    * @return ETrue if associated with the SIPSec observer, EFalse otherwise
    */
    TBool HasObserver(
    	const MSIPSecSecurityMechanismObserver& aObserver ) const;

    /**
    * Checks if the observer has completed.
    * @return ETrue if completed, EFalse otherwise
    */
    TBool IsCompleted() const;

protected:

    /**
    * Called when credentials has been arrived
    */	
    virtual void DoCompleted();

protected: // Data

    // Count of pending operations
    TInt iPendingOperations;

	// ETrue if at least one operation ended succesfully
	TBool iAtLeastOneSuccessfulOperation;

    // SIPSec Observer, not owned    	
    MSIPSecSecurityMechanismObserver* iObserver;

    TStatus iStatus;


	// For testing purposes
#ifdef CPPUNIT_TEST
	friend class CSIPSecObserverTest;
	friend class CSIPSecDigestTest;
#endif
	};
	
#endif // __SIPSEC_DIGESTOBSERVER_H__

// End of File
