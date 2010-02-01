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
// Name          : CSIPTimestampHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "siptimestampheader.h"
#include "CSIPTokenizer.h"
#include "sipcodecerr.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "SIPSyntaxCheck.h"
#include "_sipcodecdefs.h"

const TInt KMaxTRealLength(32);

// -----------------------------------------------------------------------------
// CSIPTimestampHeader::DecodeL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPTimestampHeader* CSIPTimestampHeader::DecodeL(const TDesC8& aValue)
    {
    CSIPTimestampHeader* header = 
        new(ELeave) CSIPTimestampHeader(0.0);
    CleanupStack::PushL(header);
    header->ParseL(aValue);
    CleanupStack::Pop(header);
    return header;
    }

// -----------------------------------------------------------------------------
// CSIPTimestampHeader::CSIPTimestampHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPTimestampHeader::CSIPTimestampHeader(TReal aTimestamp)
: iTimestamp(aTimestamp), iHasDelay(EFalse)
    {
    }

// -----------------------------------------------------------------------------
// CSIPTimestampHeader::~CSIPTimestampHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPTimestampHeader::~CSIPTimestampHeader ()
    {
    }

// -----------------------------------------------------------------------------
// CSIPTimestampHeader::SetTimestamp
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPTimestampHeader::SetTimestamp(TReal aTimestamp)
    {
    iTimestamp = aTimestamp;
    }

// -----------------------------------------------------------------------------
// CSIPTimestampHeader::Timestamp
// -----------------------------------------------------------------------------
//
EXPORT_C TReal CSIPTimestampHeader::Timestamp() const
    {
    return iTimestamp;
    }

// -----------------------------------------------------------------------------
// CSIPTimestampHeader::HasDelay
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPTimestampHeader::HasDelay() const
    {
    return iHasDelay;
    }

// -----------------------------------------------------------------------------
// CSIPTimestampHeader::SetDelay
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPTimestampHeader::SetDelay(TReal aDelay)
    {
    iDelay = aDelay;
    iHasDelay = ETrue;
    }

// -----------------------------------------------------------------------------
// CSIPTimestampHeader::Delay
// -----------------------------------------------------------------------------
//
EXPORT_C TReal CSIPTimestampHeader::Delay() const
    {
    return iDelay;
    }

// -----------------------------------------------------------------------------
// CSIPTimestampHeader::InternalizeValueL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* 
	CSIPTimestampHeader::InternalizeValueL(RReadStream& aReadStream)
    {
    CSIPTimestampHeader* self = new(ELeave) CSIPTimestampHeader(0.0);
    CleanupStack::PushL(self);
    self->DoInternalizeValueL(aReadStream);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPTimestampHeader::CloneL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPTimestampHeader::CloneL() const
    {
    CSIPTimestampHeader* clone = new(ELeave) CSIPTimestampHeader(iTimestamp);
    CleanupStack::PushL(clone);
    if(iHasDelay)
        {
        clone->SetDelay(iDelay);
        }
    CleanupStack::Pop(clone);
    return clone;
    }

// -----------------------------------------------------------------------------
// CSIPTimestampHeader::Name
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPTimestampHeader::Name() const
    {
    return SIPStrings::StringF(SipStrConsts::ETimestampHeader);
    }

// -----------------------------------------------------------------------------
// CSIPTimestampHeader::ToTextValueL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CSIPTimestampHeader::ToTextValueL() const
    {
    TBuf8<KMaxTRealLength> delay;
	TBuf8<KMaxTRealLength> timestamp;
	TRealFormat realFormat;
	realFormat.iPoint = '.'; // SIP uses always dot as a decimal point
	
	User::LeaveIfError(timestamp.Num(iTimestamp,realFormat));
    TUint encodedLength = timestamp.Length();
    if(iHasDelay)
        {
        // iDelay + SP
        User::LeaveIfError(delay.Num(iDelay,realFormat));
        encodedLength += delay.Length() + 1;
        }

	HBufC8* encodingResult = HBufC8::NewL (encodedLength);
	TPtr8 encodingResultPtr = encodingResult->Des();

	encodingResultPtr.Append(timestamp);
    if(iHasDelay)
        {
	    encodingResultPtr.Append(' '); // SP
        encodingResultPtr.Append(delay);
        }

	return encodingResult;
    }

// -----------------------------------------------------------------------------
// CSIPTimestampHeader::PreferredPlaceInMessage
// -----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace 
CSIPTimestampHeader::PreferredPlaceInMessage() const
    {
    return CSIPHeaderBase::EMiddleBottom;
    }

// -----------------------------------------------------------------------------
// CSIPTimestampHeader::BaseDecodeL
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> 
CSIPTimestampHeader::BaseDecodeL(const TDesC8& aValue)
    {
	CSIPTimestampHeader* header = DecodeL(aValue);
	CleanupStack::PushL(header);
	RPointerArray<CSIPHeaderBase> headers;
	User::LeaveIfError (headers.Append(header));
	CleanupStack::Pop(header);
	return headers;
    }

// -----------------------------------------------------------------------------
// CSIPTimestampHeader::ExternalizeValueL
// -----------------------------------------------------------------------------
//
void CSIPTimestampHeader::ExternalizeValueL (RWriteStream& aWriteStream) const
    {
	aWriteStream.WriteReal64L(iTimestamp);
    if(iHasDelay)
        {
        aWriteStream.WriteUint8L(1); // Has delay
        aWriteStream.WriteReal64L(iDelay);
        }
    else
        {
        aWriteStream.WriteUint8L(0); // No delay
        }
    }

// -----------------------------------------------------------------------------
// CSIPTimestampHeader::ParseL
// -----------------------------------------------------------------------------
//
void CSIPTimestampHeader::ParseL(const TDesC8& aValue)
    {
    __ASSERT_ALWAYS(aValue.Length() > 0, 
                    User::Leave(KErrSipCodecTimestampHeader));

    TLex8 lex(aValue);
    TPtrC8 timestamp(lex.NextToken());
    if(!SIPSyntaxCheck::Real(timestamp, iTimestamp))
        {
        User::Leave (KErrSipCodecTimestampHeader);
        }    
    lex.SkipSpace();
    if(lex.Peek() == 0) // Check if there is still data
        {
        return;
        }
    TPtrC8 delay(lex.NextToken());
    if(!SIPSyntaxCheck::Real(delay, iDelay))
        {
        User::Leave (KErrSipCodecTimestampHeader);
        }
    iHasDelay = ETrue;
    }

// -----------------------------------------------------------------------------
// CSIPTimestampHeader::DoInternalizeValueL
// -----------------------------------------------------------------------------
//
void CSIPTimestampHeader::DoInternalizeValueL(RReadStream& aReadStream)
    {
    iTimestamp = aReadStream.ReadReal64L();
    if(aReadStream.ReadUint8L()) // HasDelay
        {
        iDelay = aReadStream.ReadReal64L();
        iHasDelay = ETrue;
        }
    }
