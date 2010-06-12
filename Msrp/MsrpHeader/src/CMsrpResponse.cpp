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
#include "CMsrpResponse.h"

// EXTERNAL HEADER
#include <MsrpStrConsts.h>

// INTERNAL HEADER
#include "MsrpCommon.h"
#include "TMSRPHeaderUtil.h"
#include "CMSRPFromPathHeader.h"
#include "CMSRPToPathHeader.h"
#include "CMSRPMessageIdHeader.h"
#include "CMSRPByteRangeHeader.h"
#include "CMSRPContentTypeHeader.h"
#include "CMSRPFailureReportHeader.h"
#include "CMSRPSuccessReportHeader.h"

// -----------------------------------------------------------------------------
// CMSRPResponse::CMSRPResponse
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPResponse::CMSRPResponse( TUint aStatusCode, RStringF aReasonPhrase )
	: iStatusCode( aStatusCode )
	{
	iReasonPhrase = aReasonPhrase.Copy();
	}

// -----------------------------------------------------------------------------
// CMSRPResponse::CMSRPResponse
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPResponse::CMSRPResponse( )
	{
	}

// -----------------------------------------------------------------------------
// CMSRPResponse::~CMSRPResponse
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPResponse::~CMSRPResponse()
	{
	}

// -----------------------------------------------------------------------------
// CMSRPResponse::SetStatusCodeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMSRPResponse::SetStatusCodeL( TUint aStatusCode )
	{
	// make sure the status code is valid
	// sip status codes must be between 200 - 600
	__ASSERT_ALWAYS( aStatusCode < 600, User::Leave( KErrArgument ) ); // codescanner::magicnumbers
	__ASSERT_ALWAYS( aStatusCode >= 200, User::Leave( KErrArgument ) ); // codescanner::magicnumbers

	iStatusCode = aStatusCode;
	}

// -----------------------------------------------------------------------------
// CMSRPResponse::StatusCode
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CMSRPResponse::StatusCode() const
	{
	return iStatusCode;
	}

// -----------------------------------------------------------------------------
// CMSRPResponse::SetReasonPhraseL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMSRPResponse::SetReasonPhrase( RStringF aReasonPhrase )
	{
	iReasonPhrase = aReasonPhrase.Copy();
	}

// -----------------------------------------------------------------------------
// CMSRPResponse::ReasonPhrase
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CMSRPResponse::ReasonPhrase() const
	{
	return iReasonPhrase;
	}

// -----------------------------------------------------------------------------
// CMSRPResponse::InternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPResponse* CMSRPResponse::InternalizeL( RReadStream& aReadStream )
	{
	CMSRPResponse* self = new (ELeave ) CMSRPResponse();
	CleanupStack::PushL( self );
    self->DoInternalizeL( aReadStream );
	CleanupStack::Pop( self );
    return self;
	}

// -----------------------------------------------------------------------------
// CMSRPResponse::ExternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMSRPResponse::ExternalizeL( RWriteStream& aWriteStream )
	{
	// first add the MSRP Message identifier
	aWriteStream.WriteUint8L( 101 ); // MSRP Response identifier

	// Reason code
	TBuf8< KMaxLengthOfStatusCode > value;

	// status code
	value.AppendNum( iStatusCode, EDecimal );
	aWriteStream.WriteInt32L( value.Length() );
	aWriteStream.WriteL( value );

	// then reason phrase
	aWriteStream.WriteInt32L( iReasonPhrase.DesC().Length() );
	aWriteStream.WriteL( iReasonPhrase.DesC() );

    if( iFromPath )
        {
		aWriteStream.WriteUint8L(1); // more headers in the stream flag
        iFromPath->ExternalizeValueL( aWriteStream );
        }
    if( iToPath )
        {
		aWriteStream.WriteUint8L(2); // more headers in the stream flag
        iToPath->ExternalizeValueL( aWriteStream );
        }
    	
	aWriteStream.WriteUint8L(0); // no more headers in the stream flag
	}

// -----------------------------------------------------------------------------
// CMSRPResponse::DoInternalizeL
// -----------------------------------------------------------------------------
//
void CMSRPResponse::DoInternalizeL( RReadStream& aReadStream )
	{
	TUint8 dataType( 0 );

	RStringPool strings;
	CleanupClosePushL( strings );
	strings.OpenL( MSRPStrConsts::Table );

	// read the identifier
	if ( aReadStream.ReadUint8L() != 101 )
		{
		User::Leave( KErrArgument );
		}

	// The reason code
	TUint32 codeLength = aReadStream.ReadUint32L();
	HBufC8* codeInString = HBufC8::NewLC( codeLength );
	TPtr8 codePtr( codeInString->Des() );
	aReadStream.ReadL( codePtr, codeLength );
	iStatusCode = TMSRPHeaderUtil::ConvertToNumber( codeInString->Des() );
	CleanupStack::PopAndDestroy( codeInString );

	// then reason phrase
	TUint32 phraseLength = aReadStream.ReadUint32L();
	HBufC8* phraseInString = HBufC8::NewLC( codeLength );
	TPtr8 phrasePtr( phraseInString->Des() );
	aReadStream.ReadL( phrasePtr, phraseLength );
	iReasonPhrase = strings.OpenFStringL( phrasePtr );
	CleanupStack::PopAndDestroy( phraseInString );
	
	// and headers
	do
		{
		dataType = aReadStream.ReadUint8L();

		switch( dataType )
			{
			case 1: // from path
				{
				iFromPath = CMSRPFromPathHeader::InternalizeValueL( aReadStream );
				break;
				}
			case 2: // to path
				{
				iToPath = CMSRPToPathHeader::InternalizeValueL( aReadStream );
				break;
				}
			default:
				{
				// nothing to do
				break;
				}
			}

		} while( dataType );

	CleanupStack::PopAndDestroy( ); // strings, codescanner::cleanup
	}

// -----------------------------------------------------------------------------
// CMSRPResponse::IsResponse
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CMSRPResponse::IsResponse( const TDesC8& aBuffer )
	{
	if ( aBuffer.Length() )
		{
		if ( aBuffer[ 0 ] == 101 )
			{
			return ETrue;
			}
		}

	return EFalse;
	}

// End of File
