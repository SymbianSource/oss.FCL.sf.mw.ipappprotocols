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
#include "CMSRPReport.h"

// EXTERNAL INCLUDES
//#include <e32base.h>
#include <MsrpStrConsts.h>

// INTERNAL INCLUDES
#include "MsrpCommon.h"
#include "CMSRPFromPathHeader.h"
#include "CMSRPToPathHeader.h"
#include "CMSRPMessageIdHeader.h"
#include "CMSRPByteRangeHeader.h"
#include "CMSRPContentTypeHeader.h"
#include "CMSRPFailureReportHeader.h"
#include "CMSRPSuccessReportHeader.h"
#include "CMSRPStatusHeader.h"

// -----------------------------------------------------------------------------
// CMSRPReport::CMSRPReport
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPReport::CMSRPReport()
	{
	}

// -----------------------------------------------------------------------------
// CMSRPReport::~CMSRPReport
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPReport::~CMSRPReport()
	{
	}

// -----------------------------------------------------------------------------
// CMSRPReport::InternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPReport* CMSRPReport::InternalizeL( RReadStream& aReadStream )
	{
	CMSRPReport* self = new (ELeave) CMSRPReport();
	CleanupStack::PushL( self );
    self->DoInternalizeL( aReadStream );
	CleanupStack::Pop( self );
    return self;
	}

// -----------------------------------------------------------------------------
// CMSRPReport::DoInternalizeL
// -----------------------------------------------------------------------------
//
void CMSRPReport::DoInternalizeL( RReadStream& aReadStream )
	{
	TUint8 dataType( 0 );

	// read the identifier
	if ( aReadStream.ReadUint8L() != 102 )
		{
		User::Leave( KErrArgument );
		}

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
			case 3: // message id
				{
				iMessageId = CMSRPMessageIdHeader::InternalizeValueL( aReadStream );
				break;
				}
			case 4: // byte range
				{
				iByteRange = CMSRPByteRangeHeader::InternalizeValueL( aReadStream );
				break;
				}
			case 5: // content type
				{
				iContentType = CMSRPContentTypeHeader::InternalizeValueL( aReadStream );
				break;
				}
			case 6: // failure report
				{
				iFailureReport = CMSRPFailureReportHeader::InternalizeValueL( aReadStream );
				break;
				}
			case 7: // success report
				{
				iSuccessReport = CMSRPSuccessReportHeader::InternalizeValueL( aReadStream );
				break;
				}
			case 8: // status
				{
				iStatusHeader = CMSRPStatusHeader::InternalizeValueL( aReadStream );
				break;
				}

			default:
				{
				// nothing to do
				break;
				}
			}

		} while( dataType );
	}

// -----------------------------------------------------------------------------
// CMSRPReport::ExternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMSRPReport::ExternalizeL( RWriteStream& aWriteStream )
	{
	// first add the MSRP Message identifier
	aWriteStream.WriteUint8L( 102 ); // MSRP Report identifier

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
    if ( iMessageId )
        {
		aWriteStream.WriteUint8L(3); // more headers in the stream flag
        iMessageId->ExternalizeValueL( aWriteStream );
        }
    if( iByteRange )
        {
        aWriteStream.WriteUint8L(4); // more headers in the stream flag
        iByteRange->ExternalizeValueL( aWriteStream );
        }
    if( iContentType )
        {
        aWriteStream.WriteUint8L(5); // more headers in the stream flag
        iContentType->ExternalizeValueL( aWriteStream );
        }
    if( iFailureReport )
        {
        aWriteStream.WriteUint8L(6); // more headers in the stream flag
        iFailureReport->ExternalizeValueL( aWriteStream );
        }
    if( iSuccessReport )
        {
        aWriteStream.WriteUint8L(7); // more headers in the stream flag
        iSuccessReport->ExternalizeValueL( aWriteStream );
        }
	if ( iStatusHeader )
		{
		aWriteStream.WriteUint8L( 8 );
		iStatusHeader->ExternalizeValueL( aWriteStream );
		}

	aWriteStream.WriteUint8L(0); // no more headers in the stream flag
	}

// -----------------------------------------------------------------------------
// CMSRPReport::IsReport
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CMSRPReport::IsReport( const TDesC8& aBuffer )
	{
	if ( aBuffer.Length() )
		{
		if ( aBuffer[ 0 ] == 102 )
			{
			return ETrue;
			}
		}

	return EFalse;
	}

// End of File
