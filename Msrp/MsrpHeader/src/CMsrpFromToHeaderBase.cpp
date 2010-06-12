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
#include "CMsrpFromToHeaderBase.h"

// -----------------------------------------------------------------------------
// CMSRPFromToHeaderBase::CMSRPFromToHeaderBase
// -----------------------------------------------------------------------------
//
CMSRPFromToHeaderBase::CMSRPFromToHeaderBase()
	{
	}

// -----------------------------------------------------------------------------
// CMSRPFromToHeaderBase::~CMSRPFromToHeaderBase
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPFromToHeaderBase::~CMSRPFromToHeaderBase()
	{
	delete iUri;
	iUri = NULL;
	}

// -----------------------------------------------------------------------------
// CMSRPFromToHeaderBase::operator==
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CMSRPFromToHeaderBase::operator==(
	const CMSRPFromToHeaderBase& aHeader ) const
	{
	if ( iUri->Uri().UriDes() == aHeader.iUri->Uri().UriDes() )
		{
		return ETrue;
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CMSRPFromToHeaderBase::DoInternalizeValueL
// -----------------------------------------------------------------------------
//
void CMSRPFromToHeaderBase::DoInternalizeValueL( RReadStream& aReadStream )
    {
	delete iUri;
	iUri = NULL;
	// URI comes in one string which will be parsed
	TUint32 valueLength = aReadStream.ReadUint32L();
	HBufC8* tempBuffer = HBufC8::NewLC( valueLength );
	TPtr8 tempValue( tempBuffer->Des() );
	aReadStream.ReadL( tempValue, valueLength );
    TUriParser8 parser;
    User::LeaveIfError( parser.Parse( tempValue ) );
    iUri = CUri8::NewL( parser );
	CleanupStack::PopAndDestroy( tempBuffer );
	}

// -----------------------------------------------------------------------------
// CMSRPFromToHeaderBase::ExternalizeValueL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C void CMSRPFromToHeaderBase::ExternalizeValueL( RWriteStream& aWriteStream ) const
	{
	aWriteStream.WriteInt32L( iUri->Uri().UriDes().Length() );
	aWriteStream.WriteL( iUri->Uri().UriDes() );
	}

// -----------------------------------------------------------------------------
// CMSRPFromToHeaderBase::ToTextValueL
// From CSIPHeaderBase
// -----------------------------------------------------------------------------
//
HBufC8* CMSRPFromToHeaderBase::ToTextValueL() const
	{
    if( iUri )
        {
        return iUri->Uri().UriDes().AllocL();
        }
	return NULL;
	}
