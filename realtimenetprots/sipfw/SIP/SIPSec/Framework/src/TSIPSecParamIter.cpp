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
// Name          : TSIPSecParamIter.cpp
// Part of       : SIPSec
// Version       : SIP/4.0 
//



#include "TSIPSecParamIter.h"

// ----------------------------------------------------------------------------
// TSIPSecParamIter::TSIPSecParamIter
// ----------------------------------------------------------------------------
//
TSIPSecParamIter::TSIPSecParamIter(RPointerArray<CSIPSecParam>& aSIPSecParams) :
	iPos(0),
	iSIPSecParams(aSIPSecParams)
	{	
	}

// ----------------------------------------------------------------------------
// TSIPSecParamIter::Next
// ----------------------------------------------------------------------------
//
CSIPSecParam* TSIPSecParamIter::Next()
	{
	if (Count() > 0 && iPos < (Count() - 1))
		{
		iPos++;
		return iSIPSecParams[iPos];
		}
	return NULL;
	}

// ----------------------------------------------------------------------------
// TSIPSecParamIter::First
// ----------------------------------------------------------------------------
//
CSIPSecParam* TSIPSecParamIter::First()
	{	
	iPos = 0;
	return Current();
	}

// ----------------------------------------------------------------------------
// TSIPSecParamIter::Current
// ----------------------------------------------------------------------------
//
CSIPSecParam* TSIPSecParamIter::Current()
	{
	if (Count() > 0)
		{
		return iSIPSecParams[iPos];
		}
	return NULL;
	}

// ----------------------------------------------------------------------------
// TSIPSecParamIter::DeleteCurrent
// ----------------------------------------------------------------------------
//
void TSIPSecParamIter::DeleteCurrent()
	{
	CSIPSecParam* sipsecparam = Current();
	if (sipsecparam)
		{		
		iSIPSecParams.Remove(iPos);
		delete sipsecparam;
		}
	}

// ----------------------------------------------------------------------------
// TSIPSecParamIter::Count
// ----------------------------------------------------------------------------
//
TInt TSIPSecParamIter::Count() const
	{
	return iSIPSecParams.Count();
	}
