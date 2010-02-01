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
* Name          : Confirmed.h
* Part of       : SIPDialogs
* Version       : SIP/6.0 
*
*/




/**
 @internalComponent
*/


#ifndef TCONFIRMED_H
#define TCONFIRMED_H

#include "DialogStateBase.h"

/**
 * Dialog item has four states, confirmed is one of those. 
 * A dialog is in confirmed state
 * when 2xx response is received for request.
 */
class TConfirmed : public TDialogStateBase
    {
public:

	TConfirmed (MDialogContext& aDialog,
	            TDialogStateBase& aStateTerminated);

	const TDesC8& Name () const;

    void SendL (TTransactionId& aTransactionId,
                CSIPRequest* aRequest,
                CTransactionItemBase& aTransactionItem,
                CURIContainer& aRemoteTarget);

	void SendL (CTransactionItemBase& aTransactionItem,
				CSIPResponse* aResponse);

	void ReceiveL (CTransactionItemBase& aTransactionItem,
                   CSIPResponse* aResponse);
	
	void ReceiveL (TTransactionId aTransactionId,
                   CSIPRequest* aRequest);

	void EndTransaction (CTransactionItemBase& aTransactionItem,
                         TInt aReason);

    void GetState (CSIPInternalStates::TState& aState);

private:
	
	TDialogStateBase& iStateTerminated;

private: // For testing purposes

	UNIT_TEST(CConfirmedStateTest)
    };

#endif // TCONFIRMED_H

// End of File
