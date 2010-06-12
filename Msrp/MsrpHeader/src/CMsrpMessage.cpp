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
#include "CMsrpMessage.h"

// EXTERNAL INCLUDES
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
#include "TMSRPHeaderUtil.h"
// -----------------------------------------------------------------------------
// CMSRPMessage::CMSRPMessage
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPMessage::CMSRPMessage()
	{
	}

// -----------------------------------------------------------------------------
// CMSRPMessage::~CMSRPMessage
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPMessage::~CMSRPMessage()
	{
	delete iContentBuffer;
	}

// -----------------------------------------------------------------------------
// CMSRPMessage::SetContentL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMSRPMessage::SetContent( HBufC8* aContent )
	{
    if( iContentBuffer )
        {
        delete iContentBuffer;
        iContentBuffer = NULL;
        }
	iContentBuffer = aContent;
	}

// -----------------------------------------------------------------------------
// CMSRPMessage::Content
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CMSRPMessage::Content( )
	{
    if( iContentBuffer )
        {
        return *iContentBuffer;
        }
	return KNullDesC8();
	}

// -----------------------------------------------------------------------------
// CMSRPMessage::IsContent
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CMSRPMessage::IsContent( ) const
	{
	if ( iContentBuffer )
		{
		return ETrue;
		}

	return EFalse;
	}

// -----------------------------------------------------------------------------
// CMSRPMessage::SetFileName
// -----------------------------------------------------------------------------
//
EXPORT_C void CMSRPMessage::SetFileName( const TFileName& aFileName )
    {
    iFileName = aFileName;
    }

// -----------------------------------------------------------------------------
// CMSRPMessage::GetFileName
// -----------------------------------------------------------------------------
//
EXPORT_C TFileName& CMSRPMessage::GetFileName( )
    {
    return iFileName;
    }

// -----------------------------------------------------------------------------
// CMSRPMessage::SetFileSize
// -----------------------------------------------------------------------------
//
EXPORT_C void CMSRPMessage::SetFileSize( const TInt aFileSize )
    {
    iFileSize = aFileSize;
    }

// -----------------------------------------------------------------------------
// CMSRPMessage::GetFileSize
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CMSRPMessage::GetFileSize( )
    {
    return iFileSize;
    }
// -----------------------------------------------------------------------------
// CMSRPMessage::InternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C CMSRPMessage* CMSRPMessage::InternalizeL( RReadStream& aReadStream )
	{
	CMSRPMessage* self = new (ELeave) CMSRPMessage();
	CleanupStack::PushL( self );
    self->DoInternalizeL( aReadStream );
	CleanupStack::Pop( self );
    return self;
	}

// -----------------------------------------------------------------------------
// CMSRPMessage::DoInternalizeL
// -----------------------------------------------------------------------------
//
void CMSRPMessage::DoInternalizeL( RReadStream& aReadStream )
    {
    TUint8 dataType( 0 );

    // read the identifier
    if ( aReadStream.ReadUint8L() != 100 )
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
            case 10: // content buffer
                {
                TUint32 contentLength = aReadStream.ReadUint32L();
                iContentBuffer = HBufC8::NewL( contentLength );
                TPtr8 bufferPtr = iContentBuffer->Des();
                aReadStream.ReadL( bufferPtr, contentLength );
                break;
                }
            case 11: //filename
                {
                 TUint32 nameLength = aReadStream.ReadUint32L();
                 aReadStream.ReadL( iFileName, nameLength );
                 break;
                }
            case 12: //filesize
                 {
                  TUint32 val = aReadStream.ReadUint32L();
                  if( val > 0 )
                    {
                    HBufC8* tempString = HBufC8::NewLC( val );
                    TPtr8 tempValue( tempString->Des() );
                    aReadStream.ReadL( tempValue, val );
                    iFileSize = TMSRPHeaderUtil::ConvertToNumber( tempString->Des() );
                    CleanupStack::PopAndDestroy( tempString );
                    }
                   break;              
                  }         
            case 13: //granularity
                  {
                 TUint32 val = aReadStream.ReadUint32L();
                 if( val > 0 )
                    {
                     HBufC8* tempString = HBufC8::NewLC( val );
                     TPtr8 tempValue( tempString->Des() );
                     aReadStream.ReadL( tempValue, val );
                     iNotifyProgress = TMSRPHeaderUtil::ConvertToNumber( tempString->Des() );
                     CleanupStack::PopAndDestroy( tempString );
                     }
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
// CMSRPMessage::ExternalizeL
// -----------------------------------------------------------------------------
//

EXPORT_C void CMSRPMessage::ExternalizeL( RWriteStream& aWriteStream )
        {
        // first add the MSRP Message identifier
        aWriteStream.WriteUint8L( 100 ); // MSRP Message identifier

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
        if ( IsContent() )
            {
            aWriteStream.WriteUint8L( 10 ); 
            aWriteStream.WriteInt32L( iContentBuffer->Length()  );
            aWriteStream.WriteL( *iContentBuffer, iContentBuffer->Length() );
            }
        if ( IsFile() )
            {
            aWriteStream.WriteUint8L( 11 ); // 2 = file ID
            aWriteStream.WriteInt32L( iFileName.Length() );
            aWriteStream.WriteL( iFileName );
            }

        if( iFileSize > 0 )
            {
            aWriteStream.WriteUint8L( 12 );
            TBuf8< 50 > val;
            val.AppendNum( iFileSize );
            aWriteStream.WriteInt32L( val.Length() );
            aWriteStream.WriteL( val );
             }
        if( iNotifyProgress > 0 )
            {
            aWriteStream.WriteUint8L( 13 );
            TBuf8< 50 > val;
            val.AppendNum( iNotifyProgress );
            aWriteStream.WriteInt32L( val.Length() );
            aWriteStream.WriteL( val );
             }
        aWriteStream.WriteUint8L(0); // no more headers in the stream flag
        }

// -----------------------------------------------------------------------------
// CMSRPMessage::IsMessage
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CMSRPMessage::IsMessage( const TDesC8& aBuffer )
	{
	if ( aBuffer.Length() )
		{
		if ( aBuffer[ 0 ] == 100 )
			{
			return ETrue;
			}
		}

	return EFalse;
	}

EXPORT_C TBool CMSRPMessage::IsFile()
    {
   if ( iFileName.Length() )
        {
        return ETrue;
        }
    return EFalse;
      
}


EXPORT_C void CMSRPMessage::SetNotifyProgress(TBool aNotify)
      {
      iNotifyProgress = aNotify;
      }
    
EXPORT_C TBool CMSRPMessage::GetNotifyProgress()
      {
      return iNotifyProgress;
      } 
// End of File
