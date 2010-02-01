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
* Name        : csipregistering.h
* Part of     : SIP Registration
* Version     : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPREGISTERING_H
#define CSIPREGISTERING_H

//  INCLUDES
#include "csipregistrationstate.h"

// FORWARD DECLARATIONS
class MRegistrationContext;

class CSIPRegistering : public CSIPRegistrationState
	{
public:
    static CSIPRegistering* NewL(MRegistrationContext& aRegistration);
	static CSIPRegistering* NewLC(MRegistrationContext& aRegistration);

	void SetNeighbourStates(CSIPRegistrationState* aResponse423Received,
							CSIPRegistrationState* aRegistered,
							CSIPRegistrationState* aUnregistered);

	~CSIPRegistering();

	void RegisterL(TTransactionId&  aTransactionId, 
		           CSIPRequest*     aRequest,
		           CSIPRouteHeader* aOutboundProxy);

	void ReceiveL(TTransactionId aTransactionId,
		          CSIPResponse*  aResponse);

	TInt TransactionEnded(TTransactionId aTransactionId, 
		                  TInt           aReason);

private:

	CSIPRegistering(MRegistrationContext& aRegistration);

private: // Data

	// registering neighbour states
	CSIPRegistrationState* iResponse423Received;
	CSIPRegistrationState* iRegistered;
    CSIPRegistrationState* iUnregistered;
	};

#endif // CSIPREGISTERING_H

// end of file
