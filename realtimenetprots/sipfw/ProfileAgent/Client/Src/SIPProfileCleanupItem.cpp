// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : sipprofilecleanupitem
// Part of     : SIP Profile Client
// implementation
// Version     : 1.0
// INCLUDE FILES
//



#include "SipProfileCleanupItem.h"
#include "sipmanagedprofile.h"
#include "sipconcreteprofile.h"
#include "SIPProfileITC.h"

// -----------------------------------------------------------------------------
// CSIPProfileCleanupItem::CSIPProfileCleanupItem
// -----------------------------------------------------------------------------
//	
CSIPProfileCleanupItem::CSIPProfileCleanupItem (CSIPProfileITC* aITC)
	{
	iITC = aITC;
	iStored = EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPProfileCleanupItem::CrashRevert
// -----------------------------------------------------------------------------
//	
void CSIPProfileCleanupItem::CrashRevertL()
	{
	if (iManagedProfile)
		{
		if (iStored)
			{
			delete iManagedProfile;
			iManagedProfile = NULL;
			}
		else
			{
			iITC->DeleteSIPProfileL(iProfileId);
			delete iManagedProfile;
			iManagedProfile = NULL;
			}
		}
	else if (iConcreteProfile)
		{
		iITC->DeleteSIPProfileL(iProfileId);
		delete iConcreteProfile;
		iConcreteProfile = NULL;
		}
	else if (iProfileId)
		{
		iITC->DeleteSIPProfileL(iProfileId);
		}
	else
		{
		// do nothing!
		}
	}
