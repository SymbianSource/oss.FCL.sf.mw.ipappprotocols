// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : ClientMechanisms.cpp
// Part of       : SIPSec
// Version       : SIP/4.0
//



#include "ClientMechanisms.h"


// -----------------------------------------------------------------------------
// CSIPSecClientMechanisms::NewL
// -----------------------------------------------------------------------------
//
CSIPSecClientMechanisms* CSIPSecClientMechanisms::NewL()
	{
	return new (ELeave) CSIPSecClientMechanisms();
	}

// -----------------------------------------------------------------------------
// CSIPSecClientMechanisms::CSIPSecClientMechanisms
// -----------------------------------------------------------------------------
//
CSIPSecClientMechanisms::CSIPSecClientMechanisms()
#ifdef CPPUNIT_TEST
	//In unit tests, use array granularity 1 to make every append operation to
	//allocate memory.
    : iMechanismNames(1)
#endif
    {
	}

// -----------------------------------------------------------------------------
// CSIPSecClientMechanisms::~CSIPSecClientMechanisms
// -----------------------------------------------------------------------------
//
CSIPSecClientMechanisms::~CSIPSecClientMechanisms()
	{
	RemoveAll();
	}

// -----------------------------------------------------------------------------
// CSIPSecClientMechanisms::AddL
// -----------------------------------------------------------------------------
//
void CSIPSecClientMechanisms::AddL(const TDesC8& aMechanism)
	{
	HBufC8* mechanism = aMechanism.AllocLC();
	iMechanismNames.AppendL(mechanism);
    CleanupStack::Pop(mechanism);
	}

// -----------------------------------------------------------------------------
// CSIPSecClientMechanisms::RemoveAll
// -----------------------------------------------------------------------------
//
void CSIPSecClientMechanisms::RemoveAll()
	{
	iMechanismNames.ResetAndDestroy();
	}

// -----------------------------------------------------------------------------
// CSIPSecClientMechanisms::IsEmpty
// -----------------------------------------------------------------------------
//
TBool CSIPSecClientMechanisms::IsEmpty() const
	{
	return iMechanismNames.Count() == 0;
	}

// -----------------------------------------------------------------------------
// CSIPSecClientMechanisms::IsPresent
// -----------------------------------------------------------------------------
//
TBool CSIPSecClientMechanisms::IsPresent(const TDesC8& aMechanism) const
	{
	for (TInt i = 0; i < iMechanismNames.Count(); ++i)
		{
		if (iMechanismNames[i]->CompareF(aMechanism) == 0)
			{
			return ETrue;
			}
		}

	return EFalse;
	}
