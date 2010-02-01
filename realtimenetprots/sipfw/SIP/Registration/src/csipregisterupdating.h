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
* Name        : csipregisterupdating.h
* Part of     : SIP Registration
* Version     : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPREGISTERUPDATING_H
#define CSIPREGISTERUPDATING_H

//  INCLUDES
#include "csipregistrationstate.h"

// FORWARD DECLARATIONS
class MRegistrationContext;

class CSIPRegisterUpdating : public CSIPRegistrationState
	{
public:
    static CSIPRegisterUpdating* NewL(MRegistrationContext& aRegistration);
	static CSIPRegisterUpdating* NewLC(MRegistrationContext& aRegistration);

	void SetNeighbourStates(CSIPRegistrationState* aResponse423Received,
							CSIPRegistrationState* aRegistered,
							CSIPRegistrationState* aUnregistered);

	~CSIPRegisterUpdating();

	void UpdateL(TTransactionId& aTransactionId,
				 CSIPRequest*    aRequest);

	void ReceiveL(TTransactionId aTransactionId,
		          CSIPResponse*  aResponse);

	TInt TransactionEnded(TTransactionId aTransactionId, 
		                  TInt           aReason);


private:

	CSIPRegisterUpdating(MRegistrationContext& aRegistration);

private: // Data

	// register updating neighbour states
	CSIPRegistrationState* iResponse423Received;
	CSIPRegistrationState* iRegistered;
    CSIPRegistrationState* iUnregistered;
	};

#endif // CSIPREGISTERUPDATING_H

// end of file
