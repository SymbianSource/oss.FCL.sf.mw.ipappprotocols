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
// Name          : TSIPSecResponsehandlerIter.cpp
// Part of       : SIPSec
// Version       : SIP/4.0 
//



#include "TSIPSecResponsehandlerIter.h"
#include "sipsecresponsehandler.h"

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
TSIPSecResponsehandlerIter::TSIPSecResponsehandlerIter(
RPointerArray<CSIPSecResponseHandler>& aResponseHandlers) :
	iPos(0),
	iResponseHandlers(aResponseHandlers)	
	{
	}

// ----------------------------------------------------------------------------
// TSIPSecResponsehandlerIter::Next
// ----------------------------------------------------------------------------
//
CSIPSecResponseHandler* TSIPSecResponsehandlerIter::Next()
	{
	if (Count() > 0 && iPos < (Count() - 1))
		{
		iPos++;
		return iResponseHandlers[iPos];
		}
	return NULL;
	}

// ----------------------------------------------------------------------------
// TSIPSecResponsehandlerIter::First
// ----------------------------------------------------------------------------
//
CSIPSecResponseHandler* TSIPSecResponsehandlerIter::First()
	{
	if (Count() > 0)	
		{
		iPos = 0;
		return iResponseHandlers[iPos];
		}
	return NULL;
	}

// ----------------------------------------------------------------------------
// TSIPSecResponsehandlerIter::Count
// ----------------------------------------------------------------------------
//
TInt TSIPSecResponsehandlerIter::Count() const
	{
	return iResponseHandlers.Count();
	}
