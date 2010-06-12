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

#ifndef MMMSRPCONNECTOROBSERVER_H
#define MMMSRPCONNECTOROBSERVER_H

#include <e32base.h>
#include <in_sock.h>

// CLASS DECLARATIONS
/**
* Interface for notifing connector events
*/
class MMSRPConnectorObserver 
    {
    public: 
                
        virtual void ConnectionEstablishedL( TInt aNewState, RSocket* aDataSocket, TInt aStatus ) = 0;
                
    };

#endif // MMMSRPCONNECTOROBSERVER_H

// End of file
