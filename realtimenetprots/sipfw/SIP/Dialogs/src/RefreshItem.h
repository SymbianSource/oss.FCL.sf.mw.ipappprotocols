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
* Name          : RefreshItem.h
* Part of       : SIPDialogs
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CREFRESHITEM_H
#define CREFRESHITEM_H

#include "TransactionItemBase.h"
class MSipRefreshMgr;
class MRefreshOwner;
class MSIPSecUser;


class CRefreshItem : public CTransactionItemBase
	{
public:

    static CRefreshItem* NewL (MSipRefreshMgr& aRefreshMgr,
                               MRefreshOwner* aRefreshOwner,
                               MSIPSecUser& aSIPSecUser,
                               RStringF aMethod);

    static CRefreshItem* NewLC (MSipRefreshMgr& aRefreshMgr,
                                MRefreshOwner* aRefreshOwner,
                                MSIPSecUser& aSIPSecUser,
                                RStringF aMethod);
    
    ~CRefreshItem ();


public: // From CTransactionItemBase

    TBool IsRefreshed() const;

	void ResponseReceived (TUint aResponseCode); 

    MTransactionHeaders* SendAndGetHeadersL (
        const TSIPTransportParams& aTransportParams,
        TTransactionId& aTransactionId,
        TRegistrationId aRegistrationId,
        CSIPRequest* aRequest,
        CURIContainer& aRemoteTarget);

    void SendL (const TSIPTransportParams& aTransportParams,
                TTransactionId& aTransactionId,
                TRegistrationId aRegistrationId,
                CSIPRequest* aRequest,
                CURIContainer& aRemoteTarget);
    
    void SendL (const TSIPTransportParams& aTransportParams,
                CSIPResponse* aResponse,
                TBool aAddOwner);


public: // New functions

	TRefreshId RefreshId();
    void SetTerminated();


private: // Second phase constructor

    CRefreshItem (MSipRefreshMgr& aRefreshMgr,
                  MRefreshOwner* aRefreshOwner,
                  MSIPSecUser& aSIPSecUser,
                  RStringF aMethod);


private: // Data

    MSipRefreshMgr& iRefreshMgr;
    MRefreshOwner* iRefreshOwner;
    MSIPSecUser& iSIPSecUser;
	TRefreshId iRefreshId;
    TBool iFirstRequestSent;
    
private: // For testing purposes

	UNIT_TEST(CRefreshItemTest)
	UNIT_TEST(CSipDialogItemTest)
	UNIT_TEST(CTransactionItemStoreTest)
	UNIT_TEST(CSipDialogMgrTest)	
	};

#endif // CREFRESHITEM_H

// End of File
