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
* Name          : TransactionItemBase.h
* Part of       : SIPDialogs
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CTRANSACTIONITEMBASE_H
#define CTRANSACTIONITEMBASE_H

#include <e32base.h>
#include <stringpool.h>
#include "SipStackServerDefs.h"
#include "_sipcodecdefs.h"

class TSIPTransportParams;
class CSIPRequest;
class CSIPResponse;
class MTransactionHeaders;
class CURIContainer;


class CTransactionItemBase : public CBase
	{
public:

    static TInt Offset ();
    virtual ~CTransactionItemBase ();

    void SetTransactionId (TTransactionId aTransactionId);
    TTransactionId Id () const;
	RStringF Method () const;

	void SetOverlappingRestricted (TBool aOverlappingRestricted=ETrue);
   	TBool OverlappingRestricted () const;

	void SetTargetRefresh (TBool aIsTargetRefresh=ETrue);
    TBool TargetRefresh () const;

    TBool Terminate () const;

    virtual TBool IsRefreshed() const = 0;

	TBool PassResponseToOwner () const;

	virtual void ResponseReceived (TUint aResponseCode) = 0; 

    virtual MTransactionHeaders* 
        SendAndGetHeadersL (const TSIPTransportParams& aTransportParams,
                            TTransactionId& aTransactionId,
                            TRegistrationId aRegistrationId,
                            CSIPRequest* aRequest,
                            CURIContainer& aRemoteTarget) = 0;


    virtual void SendL (const TSIPTransportParams& aTransportParams,
                        TTransactionId& aTransactionId,
                        TRegistrationId aRegistrationId,
                        CSIPRequest* aRequest,
                        CURIContainer& aRemoteTarget) = 0;

	virtual void SendL (const TSIPTransportParams& aTransportParams,
	                    CSIPResponse* aResponse,
                        TBool aAddOwner) = 0;

protected:

    CTransactionItemBase ();
    CTransactionItemBase (TTransactionId aTransactionId);
    CTransactionItemBase (TTransactionId aTransactionId, RStringF aMethod);
    void Copy (const CTransactionItemBase& aItem);

    TBool iTerminate;
	TBool iPassResponseToOwner;

private: // Data

   	TSglQueLink iLink;
    RStringF iMethod;
	TBool iOverlappingRestricted;
	TBool iIsTargetRefresh;
    TTransactionId  iTransactionId;

private: // For testing purposes

	UNIT_TEST(CTransactionItemBaseTest)
    UNIT_TEST(CTransactionItemStoreTest)
    UNIT_TEST(CEarlyStateTest)
    UNIT_TEST(CSipDialogItemTest)
    };

#endif // CTRANSACTIONITEMBASE_H

// End of File
