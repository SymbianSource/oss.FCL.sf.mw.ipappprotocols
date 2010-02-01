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
* Name        : csipunregistered.h
* Part of     : SIP Registrations
* Version     : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPUNREGISTERED_H
#define CSIPUNREGISTERED_H

//  INCLUDES
#include "csipregistrationstate.h"

// FORWARD DECLARATIONS
class MRegistrationContext;

class CSIPUnregistered : public CSIPRegistrationState
	{
public:
    static CSIPUnregistered* NewL(MRegistrationContext& aRegistration);
	static CSIPUnregistered* NewLC(MRegistrationContext& aRegistration);

	~CSIPUnregistered();

	TInt TransactionEnded(TTransactionId aTransactionId, 
		                  TInt           aReason);

private:

	CSIPUnregistered(MRegistrationContext& aRegistration);
	};

#endif // CSIPUNREGISTERED_H

// end of file
