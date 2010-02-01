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
* Name        : csipunregistering.h
* Part of     : SIP Registrations
* Version     : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPUNREGISTERING_H
#define CSIPUNREGISTERING_H

// INCLUDES
#include "csipregistrationstate.h"

// FORWARD DECLARATIONS
class MRegistrationContext;

class CSIPUnregistering : public CSIPRegistrationState
	{
public:
    static CSIPUnregistering* NewL(MRegistrationContext& aRegistration);
	static CSIPUnregistering* NewLC(MRegistrationContext& aRegistration);

	void SetNeighbourStates(CSIPRegistrationState* aUnregistered);

	~CSIPUnregistering();

	void UnregisterL(TTransactionId& aTransactionId,
					 CSIPRequest*    aRequest);

	void ReceiveL(TTransactionId aTransactionId,
		          CSIPResponse*  aResponse);

	TInt TransactionEnded(TTransactionId aTransactionId, 
		                  TInt           aReason);

private:

	CSIPUnregistering(MRegistrationContext& aRegistration);

private: // Data

    CSIPRegistrationState* iUnregistered;
	};

#endif // CSIPUNREGISTERING_H

// end of file
