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
* Name          : Early.h
* Part of       : SIPDialogs
* Version       : SIP/6.0 
*
*/




/**
 @internalComponent
*/


#ifndef TEARLY_H
#define TEARLY_H

#include "DialogStateBase.h"


/**
 * Dialog item has four states, early is one of those. 
 * Dialog item is in early state when 101-199 response is received 
 * for the initial request.
 */
class TEarly : public TDialogStateBase
	{
public :

	TEarly (MDialogContext& aDialog,
	        TDialogStateBase& aStateConfirmed,
	        TDialogStateBase& aStateTerminated);

	const TDesC8& Name () const;

	void SendL (TTransactionId& aTransactionId,
				CSIPRequest* aSipRequest, 
				CTransactionItemBase& aTransactionItem,
				CURIContainer& aRemoteTarget);

	void SendL (CTransactionItemBase& aTransactionItem,
				CSIPResponse* aSipResponse);

	void ReceiveL (CTransactionItemBase& aTransactionItem,
                   CSIPResponse* aResponse);

	void ReceiveL (TTransactionId aTransactionId,
                   CSIPRequest* aRequest);

	void EndTransaction (CTransactionItemBase& aTransactionItem,
                         TInt aReason);

    void GetState (CSIPInternalStates::TState& aState);
    
private:

    TDialogStateBase& iStateConfirmed;
	TDialogStateBase& iStateTerminated;
	
private: // For testing purposes

	UNIT_TEST(CEarlyStateTest)
	};

#endif // TEARLY_H

// End of File
