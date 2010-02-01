
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
*
*/



#ifndef CSIPEXTIMEOUTIMER_H
#define CSIPEXTIMEOUTIMER_H

// INCLUDES
#include    <e32base.h>

// CONSTANTS
const TInt KTimeOutInSeconds( 30 );
const TInt KTimeOut( KTimeOutInSeconds * 1000000 );

// FORWARD DECLARATIONS
class MSIPExTimeOutNotify;

// CLASS DECLARATIONS
/**
* Implements timeout timer.
*/
class CSIPExTimeOutTimer: public CTimer
	{
    public:
        
        /**
        * Static constructor
        * @param aTimeOutNotify The reference to the timeout notifier.
        * @return A new instance of CSIPExTimeOutTimer
        */
        static CSIPExTimeOutTimer* NewL( MSIPExTimeOutNotify& aTimeOutNotify );

        /**
        * Destructor.
        */
        ~CSIPExTimeOutTimer();

    protected:

        /**
        * C++ default constructor.
        * @param aTimeOutNotify The reference to the timeout notifier.
        */
        CSIPExTimeOutTimer( MSIPExTimeOutNotify& aTimeOutNotify );

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
        MSIPExTimeOutNotify& iNotifier;
	};

#endif // CSIPEXTIMEOUTIMER_H

// End of file
