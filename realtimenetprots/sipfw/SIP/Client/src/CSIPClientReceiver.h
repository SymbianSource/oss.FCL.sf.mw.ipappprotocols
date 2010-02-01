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
* Name          : CSIPClientReceiver.h
* Part of       : SIPClient
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPCLIENTRECEIVER_H
#define CSIPCLIENTRECEIVER_H

#include "CSIPITCReceiverBase.h"
#include "_sipcodecdefs.h"

class RSIP;
class MSIPClientObserver;


class CSIPClientReceiver : public CSIPITCReceiverBase
	{
public:

	CSIPClientReceiver (RSIP& aSip, MSIPClientObserver& aObserver);
	~CSIPClientReceiver ();

private: // From CSIPITCReceiverBase

	void IncomingRequestL (TSIPIds& aIds,
                           CSIPRequestElements* aRequest);

	void IncomingResponseL (TSIPIds& aIds,
                            CSIPResponseElements* aResponse);

    void ErrorOccuredL (TSIPIds& aIds, TInt aError);

private: // Data
	
	MSIPClientObserver& iObserver;

private: // For testing purposes

    UNIT_TEST(CSIPClientReceiverTest)
	};

#endif // CSIPCLIENTRECEIVER_H

// End of File
