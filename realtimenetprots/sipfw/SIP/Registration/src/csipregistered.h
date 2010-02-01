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
* Name        : csipregistered.h
* Part of     : SIP Registration
* Version     : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPREGISTERED_H
#define CSIPREGISTERED_H

//  INCLUDES
#include "csipregistrationstate.h"

// FORWARD DECLARATIONS
class MRegistrationContext;

class CSIPRegistered : public CSIPRegistrationState
	{
public:

    static CSIPRegistered* NewL(MRegistrationContext& aRegistration);
	static CSIPRegistered* NewLC(MRegistrationContext& aRegistration);

	void SetNeighbourStates(CSIPRegistrationState* aUpdating,
							CSIPRegistrationState* aUnregistering,
							CSIPRegistrationState* aUnregistered,
							CSIPRegistrationState* aResponse423Received);

	~CSIPRegistered();	

	void UpdateL(TTransactionId& aTransactionId,
				 CSIPRequest*    aRequest);

	void UnregisterL(TTransactionId& aTransactionId,
					 CSIPRequest*    aRequest);

	void ReceiveL(TTransactionId aTransactionId,
		          CSIPResponse*  aResponse);

	TInt TransactionEnded(TTransactionId aTransactionId, 
		                  TInt           aReason);

private:

	CSIPRegistered(MRegistrationContext& aRegistration);

private: // Data

	// registered neighbouring states
	CSIPRegistrationState* iUpdating;
    CSIPRegistrationState* iUnregistering;
    CSIPRegistrationState* iUnregistered;
	// response423Received state can be entered, when subsequest
	// refresh response with code 423 received
	CSIPRegistrationState* iResponse423Received; 
	};

#endif // CSIPREGISTERED_H

// end of file
