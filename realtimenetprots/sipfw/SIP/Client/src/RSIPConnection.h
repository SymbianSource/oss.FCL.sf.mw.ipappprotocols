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
* Name          : RSIPConnection.h
* Part of       : SIPClient
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef RSIPCONNECTION_H
#define RSIPCONNECTION_H

#include <e32std.h>
#include "sipconnection.h"
#include "MSIPITC.h"

class RSIP;


class RSIPConnection : public RSubSessionBase, public MSIPITC
	{
public:

	RSIPConnection(TUint32 aIapId);

	TInt Open (RSIP& aSip);
	void Close ();

    void SetState (CSIPConnection::TState aState);
    CSIPConnection::TState State () const;

public: // From MSIPITC

    TInt Send (TSipItcFunctions aFunction, TIpcArgs& aArgs) const;
    void Receive (TIpcArgs& aArgs, TRequestStatus& aStatus);
    TInt Receive (TIpcArgs& aArgs);
    void CancelReceive ();

private: // Data

    TBool iOpen;
    TUint32 iIapId;
    CSIPConnection::TState iState;

#ifdef CPPUNIT_TEST
    // For testing purposes only:
    TBool iReceiveCalled;
    TRequestStatus* iStatus;
#endif
	};

#endif // RSIPCONNECTION_H

// End of File
