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

#ifndef MMSRPLISTENER_H_
#define MMSRPLISTENER_H_

// INCLUDES
#include <e32base.h>

class MMSRPListener
    {
    public:
        
    enum TListenerState
       {
       ENotListening,
       EAccepting,
       ENotAccepting,
       EListenAccepted,
       ETimedOut,
       EListenPortError,
       ETerminate
       };   
        
    
        inline virtual ~MMSRPListener(){}
        /**
         * Listen() call, increment listen count if already listening
         */
        virtual void ListenL() = 0;
        
        /**
         * Cancel Listen, do not issue an actual cancel accept,
         * decrement listen count and delete listener if count  zero 
         */
        virtual void ListenCancel() = 0;
        
        
        virtual void ListenCompletedL(TBool aValid) = 0;
                                
        /**
         * cleanup or reset
         */
        //virtual void StopListening() = 0;
                                     
        
        /**
         * as part of destruction, no client interface 
         */
        //virtual void CancelAccept() = 0;
                                
    };


#endif /* MMSRPLISTENER_H_ */
