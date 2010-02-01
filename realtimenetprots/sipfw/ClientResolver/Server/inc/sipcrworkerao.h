/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : sipcrworkerao.h
* Part of       : SIP / SIP Client Resolver
* Version       : %version: 2.1.1 %
*
*/




/**
 @internalComponent
*/

#ifndef CSIPCRWORKERAO_H
#define CSIPCRWORKERAO_H

// INCLUDES
#include "TSipClient.h"
#include "CSipCRServerSession.h"
#include <e32base.h>

// FORWARD DECLARATIONS
class MSipCrWorkerAoObserver;


class CSipCrWorkerAo : public CActive
    {
    public: // Constructors and desctructor

	    static CSipCrWorkerAo* NewL(
	        CSIPCRServerSession& aSession);
	        
        static CSipCrWorkerAo* NewLC(
	        CSIPCRServerSession& aSession);

	    static CSipCrWorkerAo* NewL(
	        TUint32 aRequestId,
            CBufBase* aSerializedSipRequest,
            HBufC8* aSipRequestBody,
            const TSipClient& aSipClient,
            const TUid& aResolverUid,
	        CSIPCRServerSession& aSession);
	        
        static CSipCrWorkerAo* NewLC(
	        TUint32 aRequestId,
            CBufBase* aSerializedSipRequest,
            HBufC8* aSipRequestBody,
            const TSipClient& aSipClient,
            const TUid& aResolverUid,
	        CSIPCRServerSession& aSession);
        
        ~CSipCrWorkerAo();
    
    public: // New functions
    
        void StartThreadL(RThread& aThread);
        CSIPCRServerSession& Session();
        CBufBase& SerializedSipRequest();
        HBufC8* GetSipRequestBody(); // Ownership transferred
        TSipClient& SipClient();
        const TUid& ResolverUid() const;           
        TUint32 RequestId() const;
        TInt CompletionCode() const;
    
    protected: // From CActive
    
	    void RunL();
	    void DoCancel();

    private: // Constructors

        CSipCrWorkerAo(CSIPCRServerSession& aSession);
        
        CSipCrWorkerAo(
            TUint32 aRequestId,
            const TSipClient& aSipClient,
            const TUid& aResolverUid,
            CSIPCRServerSession& aSession);
        
        void ConstructL();
        
        void ConstructL(
            CBufBase* aSerializedSipRequest,
            HBufC8* aSipRequestBody);
    
    private: // Data
    
        CSIPCRServerSession& iSession;
        TUint32 iRequestId;
        CBufBase* iSerializedSipRequest;
        HBufC8* iSipRequestBody;
        TSipClient iSipClient;
        TUid iResolverUid;
        RThread iThread;
    };

#endif // CSIPCRWORKERAO_H

//  End of File  
