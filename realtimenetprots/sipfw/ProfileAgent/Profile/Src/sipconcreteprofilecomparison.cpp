// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : sipconcreteprofilecomparison.cpp
// Part of     : SIP / SIP Profile Agent / SIP Concrete Profile
// Implementation
// Version     : %version: 2.1.1 %
//



//  INCLUDE FILES
#include "sipconcreteprofilecomparison.h"
 
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// SIPConcreteProfileComparison::ValueModified
// -----------------------------------------------------------------------------
//
EXPORT_C TBool SIPConcreteProfileComparison::ValueModified(
    const CSIPConcreteProfile::TComparison aValue,
    CSIPConcreteProfile& aOldProfile,
    const CSIPConcreteProfile& aNewProfile)
	{
	if (!(aOldProfile == aNewProfile))
		{
		if (aOldProfile.LastCompareResult() == aValue)
			{
			return ETrue;
			}
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// SIPConcreteProfileComparison::ValuesModified
// -----------------------------------------------------------------------------
//
EXPORT_C TBool SIPConcreteProfileComparison::ValuesModified(
    const TInt32 aValue,
    CSIPConcreteProfile& aOldProfile,
    const CSIPConcreteProfile& aNewProfile)
	{
	if (!(aOldProfile == aNewProfile))
		{
		if (aOldProfile.LastCompareResult() == aValue)
			{
			return ETrue;
			}
		}
	return EFalse;
	}
