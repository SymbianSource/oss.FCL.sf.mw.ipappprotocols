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

#ifndef MMSRPLISTENEROBSERVER_H_
#define MMSRPLISTENEROBSERVER_H_

// INCLUDES
#include <e32base.h>
#include <in_sock.h>


class MMSRPListenerObserver
    {
    public:
        
        virtual void ListenerStateL( TInt aNewState, RSocket* aDataSocket, TInt aStatus ) = 0;
                
    };


#endif /* MMSRPLISTENEROBSERVER_H_ */
