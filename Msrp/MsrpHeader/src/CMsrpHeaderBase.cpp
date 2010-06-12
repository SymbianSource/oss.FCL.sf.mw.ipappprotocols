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

//CLASS HEADER
#include "CMsrpHeaderBase.h"

// INTERNAL INCLUDES
#include "TMSRPHeaderUtil.h"

// -----------------------------------------------------------------------------
// CMSRPHeaderBase::CMSRPHeaderBase
// -----------------------------------------------------------------------------
//
CMSRPHeaderBase::CMSRPHeaderBase()
	{
	}

// -----------------------------------------------------------------------------
// CMSRPHeaderBase::~CMSRPHeaderBase
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPHeaderBase::~CMSRPHeaderBase()
	{
	}

// -----------------------------------------------------------------------------
// CMSRPHeaderBase::ToTextL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CMSRPHeaderBase::ToTextL () const
	{
	TPtrC8 fullName( Name().DesC() );
	TUint headerLength = fullName.Length();
	headerLength += KColonAndSpace().Length();
	HBufC8* encodedHeaderValue = ToTextValueL();
	headerLength += encodedHeaderValue->Length();

	CleanupStack::PushL( encodedHeaderValue );

	HBufC8* encodedHeader = HBufC8::NewL( headerLength );
	CleanupStack::PushL( encodedHeader );

	TPtr8 encodedHeaderPtr = encodedHeader->Des();
	TMSRPHeaderUtil::AppendStringL( fullName, encodedHeaderPtr );
	TMSRPHeaderUtil::AppendStringL( KColonAndSpace, encodedHeaderPtr );
	TMSRPHeaderUtil::AppendStringL( *encodedHeaderValue, encodedHeaderPtr );

	CleanupStack::Pop( encodedHeader ); // encodedHeader
	CleanupStack::PopAndDestroy( encodedHeaderValue );

	return encodedHeader;
	}

// -----------------------------------------------------------------------------
// CMSRPHeaderBase::ToTextLC
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CMSRPHeaderBase::ToTextLC () const
	{
    HBufC8* encodedHeader = ToTextL();
    CleanupStack::PushL (encodedHeader);
    return encodedHeader;
	}

// -----------------------------------------------------------------------------
// CMSRPHeaderBase::ToTextValueLC
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CMSRPHeaderBase::ToTextValueLC () const
    {
    HBufC8* encodedHeaderValue = ToTextValueL();
    CleanupStack::PushL (encodedHeaderValue);
    return encodedHeaderValue;
    }

// End of File
