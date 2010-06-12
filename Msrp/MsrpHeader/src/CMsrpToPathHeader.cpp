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
#include "CMsrpToPathHeader.h"

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include "MsrpCommon.h"
#include "msrpstrings.h"
#include "msrpstrconsts.h"

// -----------------------------------------------------------------------------
// CMSRPToPathHeader::DecodeL
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPToPathHeader* CMSRPToPathHeader::DecodeL(
	const TDesC8& aUri )
	{
	CMSRPToPathHeader* self = new (ELeave) CMSRPToPathHeader();
    CleanupStack::PushL( self );

	// parsing the descriptor URL
    CUri8* uri = CUri8::NewLC( );
	// first is the scheme
	TChar charToFind( KColonCharacter );
	TInt matchLoc = aUri.Locate( charToFind );
	if ( matchLoc != KErrNotFound )
		{
		TPtrC8 scheme = aUri.Left( matchLoc );
		uri->SetComponentL( scheme, EUriScheme );
		// after that comes the ipaddress which must have // in front
		TPtrC8 host = aUri.Mid( matchLoc + 3 ); // codescanner::magicnumbers
		matchLoc = host.Locate( charToFind );
		if ( matchLoc != KErrNotFound )
			{
			uri->SetComponentL( host.Left( matchLoc ), EUriHost );
			// next is the port
			TPtrC8 port = host.Mid( matchLoc + 1 );
			TChar charToFind2( KDividedCharacter );
			matchLoc = port.Locate( charToFind2 );
			if ( matchLoc != KErrNotFound )
				{
				uri->SetComponentL( port.Left( matchLoc ), EUriPort );
				TPtrC8 path = port.Mid( matchLoc );
				uri->SetComponentL( path, EUriPath );
				}
			else
				{
				// there is not path set
				User::Leave( KErrArgument );
				}
			}
		else
			{
			User::Leave( KErrArgument );
			}
		}
	else
		{
		User::Leave( KErrArgument );
		}

    self->ConstructL( uri );
	CleanupStack::Pop( uri );
	CleanupStack::Pop( self );
    return self;
	}

// -----------------------------------------------------------------------------
// CMSRPToPathHeader::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPToPathHeader* CMSRPToPathHeader::NewLC(
	CUri8* aUri )
	{
	CMSRPToPathHeader* self = new (ELeave) CMSRPToPathHeader();
    CleanupStack::PushL( self );
    self->ConstructL( aUri );
    return self;
	}

// -----------------------------------------------------------------------------
// CMSRPToPathHeader::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPToPathHeader* CMSRPToPathHeader::NewL(
	CUri8* aUri )
	{
	CMSRPToPathHeader* self = CMSRPToPathHeader::NewLC( aUri );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CMSRPToPathHeader::NewL
// -----------------------------------------------------------------------------
//
CMSRPToPathHeader* CMSRPToPathHeader::NewL( )
	{
	CMSRPToPathHeader* self = new (ELeave) CMSRPToPathHeader();
    CleanupStack::PushL( self );
    self->ConstructL( );
	CleanupStack::Pop( self );
    return self;
	}

// -----------------------------------------------------------------------------
// CMSRPToPathHeader::CMSRPToPathHeader
// -----------------------------------------------------------------------------
//
CMSRPToPathHeader::CMSRPToPathHeader()
    {
	}

// -----------------------------------------------------------------------------
// CMSRPToPathHeader::~CMSRPToPathHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPToPathHeader::~CMSRPToPathHeader()
	{
	MSRPStrings::Close();
	}

// -----------------------------------------------------------------------------
// CMSRPFromPathHeader::ConstructL
// -----------------------------------------------------------------------------
//
void CMSRPToPathHeader::ConstructL( CUri8* aUri )
	{
	MSRPStrings::OpenL();
	iUri = aUri;
	}

// -----------------------------------------------------------------------------
// CMSRPFromPathHeader::ConstructL
// -----------------------------------------------------------------------------
//
void CMSRPToPathHeader::ConstructL( )
	{
	MSRPStrings::OpenL();
	}

// -----------------------------------------------------------------------------
// CMSRPToPathHeader::InternalizeValueL
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPToPathHeader* CMSRPToPathHeader::InternalizeValueL(
	RReadStream& aReadStream )
	{
	CMSRPToPathHeader* self = CMSRPToPathHeader::NewL( );
	CleanupStack::PushL( self );
	self->DoInternalizeValueL( aReadStream );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CMSRPToPathHeader::Name
// -----------------------------------------------------------------------------
//
RStringF CMSRPToPathHeader::Name() const
	{
	return MSRPStrings::StringF( MSRPStrConsts::EToPath );
	}

// End of File
