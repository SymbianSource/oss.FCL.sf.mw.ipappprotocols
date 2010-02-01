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
* Name          : RSIP.h
* Part of       : SIPClient
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef RSIP_H
#define RSIP_H

#include <e32std.h>
#include "MSIPITC.h"


class RSIP : public RSessionBase, public MSIPITC
	{
public: // Constructor

	RSIP();

public: // From MSIPITC

    TInt Send (TSipItcFunctions aFunction, TIpcArgs& aArgs) const;
    void Receive (TIpcArgs& aArgs, TRequestStatus& aStatus);
    TInt Receive (TIpcArgs& aArgs);
    void CancelReceive ();

public: // New functions

	TInt Connect (const TUid& aUid);
    TInt Connect ();
	void Close ();
	TVersion Version () const;

private:

	TInt SendClientUid (const TUid& aUid) const;
    void Panic (TInt aReason) const;

    TBool iConnected;

#ifdef CPPUNIT_TEST
    // For testing purposes only:
    TBool iReceiveCalled;
    TRequestStatus* iStatus;
#endif
	};

#endif // RSIP_H

// End of File
