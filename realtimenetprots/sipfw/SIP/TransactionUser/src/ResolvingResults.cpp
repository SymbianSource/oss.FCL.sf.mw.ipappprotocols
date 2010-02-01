// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name          : ResolvingResults.cpp
// Part of       : TransactionUser
// Version       : SIP/5.0
//



#include <stringpool.h>
#include "MSIPResolvingResult.h"

#include "ResolvingResults.h"

#ifdef CPPUNIT_TEST
#include "TestCleanupStack.h"
#endif


// -----------------------------------------------------------------------------
// CResolvingResults::NewL
// -----------------------------------------------------------------------------
//
CResolvingResults* CResolvingResults::NewL()
	{
	CResolvingResults* self = new (ELeave) CResolvingResults();	
	return self;
	}

// -----------------------------------------------------------------------------
// CResolvingResults::CResolvingResults
// -----------------------------------------------------------------------------
//
CResolvingResults::CResolvingResults()
#ifdef CPPUNIT_TEST
    : iAddresses(1)
#endif
	{
	}

// -----------------------------------------------------------------------------
// CResolvingResults::~CResolvingResults
// -----------------------------------------------------------------------------
//
CResolvingResults::~CResolvingResults()
	{
	iAddresses.ResetAndDestroy();
	}

// -----------------------------------------------------------------------------
// CResolvingResults::Addresses
// -----------------------------------------------------------------------------
//
RPointerArray<MSIPResolvingResult>& CResolvingResults::Addresses()
	{
	return iAddresses;
	}
		
// -----------------------------------------------------------------------------
// CResolvingResults::GetNext
// -----------------------------------------------------------------------------
//
TBool CResolvingResults::GetNext(TInetAddr& aAddress,
								 RStringF& aTransport,
								 TBool& aForceUDP)
	{
	TInt pos = 0;
	while (pos < iAddresses.Count() && iAddresses[pos]->Failed())
		{
		pos++;
		}

	if (pos < iAddresses.Count())
		{
		aAddress = iAddresses[pos]->Address();
		aTransport.Close();
		aTransport = iAddresses[pos]->Protocol().Copy();
		aForceUDP = iAddresses[pos]->ForceUDP();
		iAddresses[pos]->SetFailed(ETrue);
		return ETrue;
		}

	return EFalse;
	}

// -----------------------------------------------------------------------------
// CResolvingResults::ContinueResolvingCurrentUri
// -----------------------------------------------------------------------------
//
TBool CResolvingResults::ContinueResolvingCurrentUri() const
	{
	return iAddresses.Count() > 0;
	}

// -----------------------------------------------------------------------------
// CResolvingResults::ClearAll
// -----------------------------------------------------------------------------
//
void CResolvingResults::ClearAll()
	{
	iAddresses.ResetAndDestroy();
	}
