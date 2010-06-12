/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html."
* Initial Contributors:
* Nokia Corporation - initial contribution.
* Contributors:
*
* Description:
* MSRP Implementation
*
*/

// CLASS HEADER
#include "CMSRPTlsPtrs.h"

// INTERNAL INCLUDES
#include "CMSRPStrings.h"

// ----------------------------------------------------------------------------
// CMSRPTlsPtrs::CMSRPTlsPtrs
// ----------------------------------------------------------------------------
//
CMSRPTlsPtrs::CMSRPTlsPtrs( CMSRPStrings* aStrings )
 : iStrings(aStrings)
	{
	}
	
// ----------------------------------------------------------------------------
// CMSRPTlsPtrs::~CMSRPTlsPtrs
// ----------------------------------------------------------------------------
//
CMSRPTlsPtrs::~CMSRPTlsPtrs()
	{
	delete iStrings;
	}

// ----------------------------------------------------------------------------
// CMSRPTlsPtrs::DestroyStrings
// ----------------------------------------------------------------------------
//	
void CMSRPTlsPtrs::DestroyStrings()
	{
	delete iStrings;
	iStrings = NULL;	
	}
	
// ----------------------------------------------------------------------------
// CMSRPTlsPtrs::GetMsrpString
// ----------------------------------------------------------------------------
//	
CMSRPStrings* CMSRPTlsPtrs::GetMsrpString()
	{
	return iStrings;
	}


// End of file
