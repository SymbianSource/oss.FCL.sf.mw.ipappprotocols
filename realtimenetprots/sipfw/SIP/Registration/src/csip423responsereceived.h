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
* Name        : csip423responsereceived.h
* Part of     : SIP Registration
* Version     : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIP423RESPONSERECEIVED_H
#define CSIP423RESPONSERECEIVED_H

//  INCLUDES
#include "csipregistrationstate.h"

// FORWARD DECLARATIONS
class MRegistrationContext;

class CSIP423ResponseReceived : public CSIPRegistrationState
	{
public:

    static CSIP423ResponseReceived* NewL(MRegistrationContext& aRegistration);
	static CSIP423ResponseReceived* NewLC(MRegistrationContext& aRegistration);

	void SetNeighbourStates(CSIPRegistrationState* aRegistered,
				            CSIPRegistrationState* aUnregistered);

	~CSIP423ResponseReceived();

	void RegisterL(TTransactionId&  aTransactionId, 
		           CSIPRequest*     aRequest,
		           CSIPRouteHeader* aOutboundProxy);

	void ReceiveL(TTransactionId aTransactionId,
		          CSIPResponse*  aResponse);

	TInt TransactionEnded(TTransactionId aTransactionId, 
		                  TInt           aReason);

	TBool Retrying() const; 
	
	void SetUpdate(TBool aUpdate);

private:

	void PassResponseL(CSIPResponse*   aResponse,
		               TTransactionId  aTransactionId);

	void PassErrorL(TInt            aError, 
		            TTransactionId  aTransactionId);

private:

	CSIP423ResponseReceived(MRegistrationContext& aRegistration);

private: // Data

	// 423 response received neighbouring states
    CSIPRegistrationState* iRegistered;
    CSIPRegistrationState* iUnregistered;

	TBool iUpdate;
	};

#endif // CSIP423RESPONSERECEIVED_H

// end of file
