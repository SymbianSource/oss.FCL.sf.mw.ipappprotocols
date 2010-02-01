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
* Name          : CSIPCRSessionReceiver.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/
#ifndef CSIPCRSESSIONRECEIVER_H
#define CSIPCRSESSIONRECEIVER_H

//  INCLUDES
#include <e32base.h>
#include "_sipcodecdefs.h"

// FORWARD DECLARATIONS
class CSIPCRResponseQueue;
class CSIPCRITCUtility;
class CSIPResponse;


class CSIPCRSessionReceiver : public CBase
    {
    public: // Constructors and destructor

    	static CSIPCRSessionReceiver* NewL(CSIPCRITCUtility& aITC);
    	
    	static CSIPCRSessionReceiver* NewLC(CSIPCRITCUtility& aITC);
    	
    	~CSIPCRSessionReceiver();
    	
    public:	// New functions
    	
    	void ClientReadyToReceiveL(const RMessage2& aMessage);

    	void CancelClientReceiveL();

    	void WriteSipResponseToClientL(const RMessage2& aMessage);

    	void AddErrorResponseL(TUint32 aRequestId, CSIPResponse* aResponse);

    	void RequestCompletedL(TUint32 aRequestId, TUid aChannel);
    	
    	void ErrorReceivedL(TUint32 aRequestId, TInt aError);

    	void RemoveByRequestId(TUint32 aRequestId);
    	
    	void RemoveAll();
    	
    private: // Constructors

        CSIPCRSessionReceiver(CSIPCRITCUtility& aITC);
        void ConstructL();
        
    private: // Data
        
        CSIPCRResponseQueue* iResponseQueue;
        CSIPCRITCUtility& iITC;
        TBool iClientReadyToReceive;
        RMessage2 iReceiveRMessage;
        RMutex iReceiverMutex;

    private: // For testing purposes

    	UNIT_TEST(CSIPCRSessionReceiverTest)
        UNIT_TEST(CSipCRServerSessionTest)
    };

#endif // CSIPCRSESSIONRECEIVER_H

// End of File
