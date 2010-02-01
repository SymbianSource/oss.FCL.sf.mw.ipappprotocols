/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSipCSServerCloseTimer.h
* Part of       : SIPServerCore
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPCSSERVERCLOSETIMER_H
#define CSIPCSSERVERCLOSETIMER_H

#include <e32base.h>


class CSipCSServerCloseTimer : public CActive
    {
public:

    static CSipCSServerCloseTimer* NewL ();
    static CSipCSServerCloseTimer* NewLC ();
    ~CSipCSServerCloseTimer ();

	void StopActiveSchedulerAfter (TUint aMilliSeconds);
	void RunL ();
	void DoCancel ();

private:

	CSipCSServerCloseTimer ();
	void ConstructL ();

	RTimer iTimer;
    };

#endif // CSIPCSSERVERCLOSETIMER_H

// End of File
