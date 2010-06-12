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

#ifndef MMMSRPTIMEOUTOBSERVER_H
#define MMMSRPTIMEOUTOBSERVER_H

// CLASS DECLARATIONS
/**
* Interface for notifing timer's timeout.
*/
class MMSRPTimeOutObserver 
    {
    public: 
        virtual void TimerExpiredL() = 0;
    };

#endif // MMMSRPTimeOutObserver_H

// End of file
