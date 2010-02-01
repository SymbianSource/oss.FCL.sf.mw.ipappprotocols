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
* Name          : CSIPCRServerCloseTimer.h
* Part of       : SIP Client Resolver
* Version       : 1.0 
*
*/




/**
 @internalComponent
*/
#ifndef CSIPCRSERVERCLOSETIMER_H
#define CSIPCRSERVERCLOSETIMER_H

//  INCLUDES
#include <e32base.h>

class CSIPCRServerCloseTimer : public CActive
    {
public:

	static CSIPCRServerCloseTimer* NewL ();

    static CSIPCRServerCloseTimer* NewLC ();
    
    ~CSIPCRServerCloseTimer ();
    
	void StopActiveSchedulerAfter (TUint aMilliSeconds);
	void RunL ();
	void DoCancel ();

private:

    CSIPCRServerCloseTimer ();
    void ConstructL ();
    
    //Data    
	RTimer iTimer;
	
    };

#endif // CSIPCRSERVERCLOSETIMER_H

//  End of File  
