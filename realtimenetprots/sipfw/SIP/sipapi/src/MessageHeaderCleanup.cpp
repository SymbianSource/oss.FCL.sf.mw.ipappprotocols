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
// Name          : MessageHeaderCleanup.cpp
// Part of       : SIPAPI
// Version       : SIP/4.0 
//



#include "SipAssert.h"
#include "MessageHeaderCleanup.h"
#include "sipmessageelements.h"
#include "sipheaderbase.h"


// -----------------------------------------------------------------------------
// TMessageHeaderCleanup::TMessageHeaderCleanup
// -----------------------------------------------------------------------------
//
TMessageHeaderCleanup::TMessageHeaderCleanup(CSIPMessageElements& aElements) :
	iElements(aElements), iHeader(NULL), iHeader2(NULL), iHeader3(NULL)
	{	
	}

// -----------------------------------------------------------------------------
// TMessageHeaderCleanup::AddHeader
// -----------------------------------------------------------------------------
//
void TMessageHeaderCleanup::AddHeader(CSIPHeaderBase& aHeader)
	{
	__SIP_ASSERT_RETURN(!iHeader3, KErrOverflow);

	if (!iHeader)
		{
		iHeader = &aHeader;
		return;
		}
	if (!iHeader2)
		{
		iHeader2 = &aHeader;
		return;
		}

	iHeader3 = &aHeader;
	}

// -----------------------------------------------------------------------------
// TMessageHeaderCleanup::Cleanup
// -----------------------------------------------------------------------------
//	
void TMessageHeaderCleanup::Cleanup(TAny* aHeaderCleanup)
	{
	__SIP_ASSERT_RETURN(aHeaderCleanup, KErrArgument);
	
	TMessageHeaderCleanup* headerCleanup =
		reinterpret_cast<TMessageHeaderCleanup*>(aHeaderCleanup);

	CSIPHeaderBase* header = headerCleanup->iHeader;
	if (header)
		{
		headerCleanup->iElements.DetachUserHeader(header);
		delete header;
		}
	
	header = headerCleanup->iHeader2;
	if (header)
		{
		headerCleanup->iElements.DetachUserHeader(header);
		delete header;
		}

	header = headerCleanup->iHeader3;
	if (header)
		{
		headerCleanup->iElements.DetachUserHeader(header);
		delete header;
		}
	}
