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
* Name          : DialogStateBase.h
* Part of       : SIPDialogs
* Version       : SIP/6.0 
*
*/




/**
 @internalComponent
*/


#ifndef TDIALOGSTATEBASE_H
#define TDIALOGSTATEBASE_H

#include <e32base.h>
#include "SipStackServerDefs.h"
#include "sipinternalstates.h"
#include "_sipcodecdefs.h"

class TSIPTransportParams;
class CSIPRequest;
class CSIPResponse;
class CURIContainer;
class CTransactionItemBase;
class MDialogContext;
 

class TDialogStateBase
	{
public:

    virtual ~TDialogStateBase();

	virtual const TDesC8& Name () const = 0;

    virtual TBool RequestsWithinDialogAllowed () const;

    virtual TBool Accept (CSIPRequest& aRequest) const;

	virtual void SendL (TTransactionId& aTransactionId,
						CSIPRequest* aRequest,
						CTransactionItemBase& aTransactionItem,
						CURIContainer& aRemoteTarget) = 0;
 
	virtual void SendL (CTransactionItemBase& aTransactionItem,
						CSIPResponse* aResponse) = 0;
 
	virtual void ReceiveL (TTransactionId aTransactionId,
						   CSIPRequest* aRequest) = 0;

	virtual void ReceiveL (CTransactionItemBase& aTransactionItem,
						   CSIPResponse* aResponse) = 0;
 
	virtual void EndTransaction (CTransactionItemBase& aTransactionItem,
                                 TInt aReason) = 0;
                             
    virtual void GetState (CSIPInternalStates::TState& aState) = 0;

protected:

    TDialogStateBase (MDialogContext& aDialog);

    void PassToOwnerL (CTransactionItemBase& aTransactionItem,
                       CSIPResponse* aResponse);
    
    void PassToOwnerL (TTransactionId aTransactionId,
                       CSIPRequest* aRequest);
    
    TInt PassToOwner (CTransactionItemBase& aTransactionItem,
                      TInt aErr);
    
    void PassToOwnerL (CTransactionItemBase& aTransactionItem,
                       TInt aErr);

    MDialogContext& iDialog;

private: // For testing purposes

	UNIT_TEST(CDialogStateBaseTest)
	};

#endif // TDIALOGSTATEBASE_H

// End of File
