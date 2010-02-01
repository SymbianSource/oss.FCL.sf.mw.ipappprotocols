/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef __RTPTIMER_H
#define __RTPTIMER_H

// INCLUDES
#include <e32base.h>

/**
*  Class for time out callback.
*
*  @lib RtpService.dll
*/
class MTimeOutNotify
    {
    public:
        virtual void OnExpiredL( TInt aStatus ) = 0;
    };

// CLASS DECLARATION

/**
*  Timer class. 
*
*  @lib RtpService.dll
*/
class CRtpTimer : public CTimer
    {
    public:
        // construct/destruct
        static CRtpTimer* NewL( const TInt aPriority,
                                MTimeOutNotify& aNotify );
        ~CRtpTimer();

    protected: // From CTimer
        virtual void RunL();

    private:
        // construct/destruct
        CRtpTimer( const TInt aPriority, MTimeOutNotify& aNotify );
        void ConstructL();

    private:
        // pointers elsewhere
        MTimeOutNotify* iNotify;
    };

#endif  // __RTPTIMER_H

// End of File
