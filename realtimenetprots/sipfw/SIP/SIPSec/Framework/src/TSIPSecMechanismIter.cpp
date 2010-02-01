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
// Name          : TSIPSecMechanismIter.cpp
// Part of       : SIPSec
// Version       : SIP/4.0 
//



#include "TSIPSecMechanismIter.h"
#include "MSIPSecSecurityMechanism.h"

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
TSIPSecMechanismIter::TSIPSecMechanismIter(
	RPointerArray<MSIPSecSecurityMechanism>& aMechanisms) :
	iPos(0),
	iMechanisms(aMechanisms)
	{
	}

// ----------------------------------------------------------------------------
// TSIPSecMechanismIter::Next
// ----------------------------------------------------------------------------
//
MSIPSecSecurityMechanism* TSIPSecMechanismIter::Next()
	{
	if (Count() > 0 && iPos < (Count() - 1))
		{
		iPos++;
		return iMechanisms[iPos];
		}
	return NULL;
	}

// ----------------------------------------------------------------------------
// TSIPSecMechanismIter::First
// ----------------------------------------------------------------------------
//
MSIPSecSecurityMechanism* TSIPSecMechanismIter::First()
	{	
	if (Count() > 0)
		{
		iPos = 0;
		return iMechanisms[iPos];
		}
	return NULL;
	}
	
// ----------------------------------------------------------------------------
// TSIPSecMechanismIter::Count
// This function must not be const, otherwise crashes when calling
// iMechanisms.Count().
// ----------------------------------------------------------------------------
//
TInt TSIPSecMechanismIter::Count()
	{
	return iMechanisms.Count();
	}
