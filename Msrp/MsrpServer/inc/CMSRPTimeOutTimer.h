/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html."
* Initial Contributors:
* Nokia Corporation - initial contribution.
* Contributors:
*
* Description:
* MSRP Implementation
*
*/

#ifndef CMSRPTIMEOUTIMER_H
#define CMSRPTIMEOUTIMER_H

// EXTERNAL INCLUDES
#include    <e32base.h>

// CONSTANTS


// FORWARD DECLARATIONS
class MMSRPTimeOutObserver;

// CLASS DECLARATIONS
/**
* Implements timeout timer.
*/
class CMSRPTimeOutTimer: public CTimer
	{
    public:
        
        /**
        * Static constructor
        * @param aTimeOutNotify The reference to the timeout notifier.
        * @return A new instance of CMSRPTimeOutTimer
        */
        static CMSRPTimeOutTimer* NewL( MMSRPTimeOutObserver& aTimeOutNotify );

        /**
        * Destructor.
        */
        ~CMSRPTimeOutTimer();

    protected:

        /**
        * C++ default constructor.
        * @param aTimeOutNotify The reference to the timeout notifier.
        */
        CMSRPTimeOutTimer( MMSRPTimeOutObserver& aTimeOutNotify );

        /**
        * Symbian 2nd phase constructor that might leave.
        */
        void ConstructL( );

    private: // From base classes
        /**
        * From CActive
        */
        virtual void RunL();

    private: // Member variables

        // The reference to the notifier
        MMSRPTimeOutObserver& iNotifier;
	};

#endif // CSIPEXTIMEOUTIMER_H

// End of file
