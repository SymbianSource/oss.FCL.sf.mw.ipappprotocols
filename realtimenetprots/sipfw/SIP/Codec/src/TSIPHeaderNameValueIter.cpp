// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : TSIPHeaderNameValueIter.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "TSIPHeaderNameValueIter.h"
#include "CSIPPreParser.h"
//#include "CSIPHeaderNameValue.h"

// -----------------------------------------------------------------------------
// TSIPHeaderNameValueIter::TSIPHeaderNameValueIter
// -----------------------------------------------------------------------------
//
TSIPHeaderNameValueIter::TSIPHeaderNameValueIter (CSIPPreParser& aPreParser)
 : iPreParser(aPreParser),
   iCurrIndex(0)
	{
	}

// -----------------------------------------------------------------------------
// TSIPHeaderNameValueIter::NextL
// -----------------------------------------------------------------------------
//
CSIPHeaderNameValue* TSIPHeaderNameValueIter::NextL ()
	{
	__ASSERT_ALWAYS (iCurrIndex < iPreParser.HeaderCount(),
					User::Leave (KErrOverflow));

	CSIPHeaderNameValue* headerNameValue = 0;
	headerNameValue = iPreParser.ParseNameValueL (iCurrIndex++);
	return headerNameValue;
	}

// -----------------------------------------------------------------------------
// TSIPHeaderNameValueIter::End
// -----------------------------------------------------------------------------
//
TBool TSIPHeaderNameValueIter::End ()
	{
	if (iCurrIndex == iPreParser.HeaderCount()) 
        {
        return ETrue;
        }
	return EFalse;
	}

// -----------------------------------------------------------------------------
// TSIPHeaderNameValueIter::SetToFirst
// -----------------------------------------------------------------------------
//
void TSIPHeaderNameValueIter::SetToFirst ()
	{
	iCurrIndex = 0;
	}
