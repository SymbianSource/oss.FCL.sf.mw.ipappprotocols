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
* Name          : Terminated.h
* Part of       : SIPDialogs
* Version       : SIP/6.0 
*
*/




/**
 @internalComponent
*/


#ifndef TTERMINATED_H
#define TTERMINATED_H

#include "DialogStateBase.h"


class TTerminated : public TDialogStateBase
	{
public:

	TTerminated (MDialogContext& aDialog);

	const TDesC8& Name () const;

    TBool RequestsWithinDialogAllowed () const;	

    TBool Accept (CSIPRequest& aRequest) const;

    void SendL (TTransactionId& aTransactionId,
                CSIPRequest* aRequest,
                CTransactionItemBase& aTransactionItem,
                CURIContainer& aRemoteTarget);
 
    void SendL (CTransactionItemBase& aTransactionItem,
                CSIPResponse* aResponse);
 
    void ReceiveL (TTransactionId aTransactionId,
                   CSIPRequest* aRequest);

	void ReceiveL (CTransactionItemBase& aTransactionItem,
                   CSIPResponse* aResponse);

	void EndTransaction (CTransactionItemBase& aTransactionItem,
                         TInt aReason);

    void GetState (CSIPInternalStates::TState& aState);
	};

#endif // TTERMINATED_H

// End of File
