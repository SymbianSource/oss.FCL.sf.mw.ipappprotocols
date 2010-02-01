/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSIPITCReceiverBase.h
* Part of       : SIPClient
* Version       : SIP/5.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPITCRECEIVERBASE_H
#define CSIPITCRECEIVERBASE_H

#include <e32base.h>
#include <s32mem.h>
#include "sipclientserver.h"
#include "_sipcodecdefs.h"

class MSIPITC;
class CSIPRequestElements;
class CSIPResponseElements;
class MSIPHttpDigestChallengeObserver;
class MSIPHttpDigestChallengeObserver2;


class CSIPITCReceiverBase : public CActive
	{
public:

	virtual ~CSIPITCReceiverBase();

    void SetHttpDigestObserver(MSIPHttpDigestChallengeObserver& aObserver);
    
    void SetHttpDigestObserver(MSIPHttpDigestChallengeObserver2& aObserver);

    void RemoveHttpDigestObserver();

protected:

    CSIPITCReceiverBase (MSIPITC& aITC);

	virtual void IncomingRequestL (TSIPIds& aIds,
                                   CSIPRequestElements* aRequest) = 0;

	virtual void IncomingResponseL (TSIPIds& aIds,
                                    CSIPResponseElements* aResponse) = 0;

    virtual void ErrorOccuredL (TSIPIds& aIds, TInt aError) = 0;

private: // From CActive

	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();

private: // New functions

    void ReceiveNext ();
    
    void ChallengeReceivedL (TSIPIds& aIds);

    void IncomingMessageL (TSIPIds& aIds);

	void IncomingRequestL (RReadStream& aReadStream,
                           HBufC8* aContent,
                           TSIPIds& aIds);

    void IncomingResponseL (RReadStream& aReadStream,
                            HBufC8* aContent,
                            TSIPIds& aIds);

    void ReadHeadersAndContentLC(HBufC8*& aHeaders, HBufC8*& aContent);

    static void DetachContent (TAny* aElements);

protected: // Data

    MSIPHttpDigestChallengeObserver2* iHttpDigestObserver2; // Not owned

private: // Data

    MSIPITC& iITC;
    TIpcArgs iITCMsgArgs;  
    TPckgBuf<TSIPIds> iIdsPckg;
    TPckgBuf<TSIPMessageBufSizes> iSizesPckg;
    MSIPHttpDigestChallengeObserver* iHttpDigestObserver; // Not owned

private: // For testing purposes

    UNIT_TEST(CSIPITCReceiverBaseTest)
    UNIT_TEST(CSIPClientReceiverTest)
    UNIT_TEST(CSIPClientConnectionReceiverTest)
	};

#endif // CSIPITCRECEIVERBASE_H

// End of File
