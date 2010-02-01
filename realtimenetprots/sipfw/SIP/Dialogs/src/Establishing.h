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
* Name          : Establishing.h
* Part of       : SIPDialogs
* Version       : SIP/6.0 
*
*/




/**
 @internalComponent
*/


#ifndef TESTABLISHING_H
#define TESTABLISHING_H

#include <stringpool.h>
#include "DialogStateBase.h"
#include "sipinternalstates.h"

/**
 * Dialog item has four states, establishing is one of those. 
 * Dialog item is in establishing state
 * when request is handled in the dialogs subsystem. 
 * No response is got to the request.
 */
class TEstablishing : public TDialogStateBase
	{
public:
	
	TEstablishing (MDialogContext& aDialog,
	               TDialogStateBase& aStateEarly,
	               TDialogStateBase& aStateConfirmed,
	               TDialogStateBase& aStateTerminated);	
	
	const TDesC8& Name () const;
	
    TBool RequestsWithinDialogAllowed () const;
    
    TBool Accept (CSIPRequest& aRequest) const;

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

	// Needed for cleanup of a MTransactionHeaders:
	static void DeleteTransactionHeaders (TAny* aTransactionHeaders);
	
private: // Data
    
    RStringF iTagParamName;
	TDialogStateBase& iStateEarly;
	TDialogStateBase& iStateConfirmed;
	TDialogStateBase& iStateTerminated;
	
private: // For testing purposes

	UNIT_TEST(CEstablishingStateTest)  
	};

#endif // TESTABLISHING_H

// End of File
