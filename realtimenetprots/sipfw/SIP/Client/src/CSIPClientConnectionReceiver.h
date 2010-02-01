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
* Name          : CSIPClientConnectionReceiver.h
* Part of       : SIPClient
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPCLIENTCONNECTIONRECEIVER_H
#define CSIPCLIENTCONNECTIONRECEIVER_H

#include "CSIPITCReceiverBase.h"
#include "sipconnection.h"
#include "_sipcodecdefs.h"

class RSIPConnection;
class MSIPClientConnectionObserver;


class CSIPClientConnectionReceiver : public CSIPITCReceiverBase
	{
public:

	CSIPClientConnectionReceiver(RSIPConnection& aConnection,
                                 MSIPClientConnectionObserver& aObserver);

	~CSIPClientConnectionReceiver();

private: // From CSIPITCReceiverBase

	void IncomingRequestL(TSIPIds& aIds,
                           CSIPRequestElements* aRequest);

	void IncomingResponseL(TSIPIds& aIds,
                           CSIPResponseElements* aResponse);

    void ErrorOccuredL(TSIPIds& aIds, TInt aError);

private: // New functions

    void ErrorOccured(TSIPIds& aIds, TInt aError);
    void InviteCompleted(TSIPIds& aIds);
    void ConnectionStateChanged(CSIPConnection::TState aState);

private: // Data
	
    RSIPConnection& iConnection; // uses to receive through ITC
	MSIPClientConnectionObserver& iObserver;

private: // For testing purposes

    UNIT_TEST(CSIPClientConnectionReceiverTest)
	};

#endif // CSIPCLIENTCONNECTIONRECEIVER_H

// End of File
