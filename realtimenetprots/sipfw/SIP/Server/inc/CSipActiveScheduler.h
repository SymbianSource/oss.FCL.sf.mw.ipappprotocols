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
* Name          : CSipActiveScheduler.h
* Part of       : SIPServerCore
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPACTIVESCHEDULER_H
#define CSIPACTIVESCHEDULER_H

#include <e32base.h>


class CSipActiveScheduler : public CActiveScheduler
    {
public:

	CSipActiveScheduler ();
	~CSipActiveScheduler ();

public: // From CActiveScheduler

    void Error (TInt aError) const;
    };

#endif // CSIPACTIVESCHEDULER_H

// End of File
