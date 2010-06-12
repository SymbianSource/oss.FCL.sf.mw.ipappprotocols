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

#ifndef CMSRPSCHEDULER_H
#define CMSRPSCHEDULER_H

// INCLUDE FILES
#include <e32base.h>

// FORWARD DECLARATIONS
class CMSRPServer;

//  CLASS DECLARATION

/**
*  MSRP Server includes a scheduler of it's own to simplify
*  client request handling code. All active object leaves are
*  handled in function Error(). The behaviour of Error()
*  depends on whether there is an outstanding client request
*  or not: it will tell the client the reason for inability
*  to handle the client request. It will also panic the
*  client if the descriptor passed to the server was malformed.
*  Finally, a panic is raised if the error does not originate
*  from the server itself.
*/

class CMSRPScheduler : public CActiveScheduler
    {
    public: // Constructors

        /**
        *   Default constructor
        */
        CMSRPScheduler();

        /**
        *   Gives a pointer of the current CMSRPServer instance
        *   to the scheduler.
        *   @param aServer The current server pointer
        */
        void SetServer( CMSRPServer* aServer );

    public:     // from CActiveScheduler

        /**
        *   Overridden to handle leaves in request functions and
        *   unexpected situations.
        *   @param aError Error code, see EPOC system documentation.
        */
        void Error( TInt aError ) const;

    private:    // data

        // server instance
        CMSRPServer* iServer;
    };

#endif          // CMSRPSCHEDULER_H

// End of File
