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
// Name          : CSIPRetryAfterHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "sipretryafterheader.h"
#include "CSIPTokenizer.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "CSIPRetryAfterHeaderParams.h"
#include "sipcodecutils.h"
#include "_sipcodecdefs.h"

const TInt KMaxUintLength(10);
_LIT8(KTUintFormat, "%u");


// ----------------------------------------------------------------------------
// CSIPRetryAfterHeader::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPRetryAfterHeader* 
CSIPRetryAfterHeader::DecodeL(const TDesC8& aValue)
    {
    CSIPRetryAfterHeader* header = new(ELeave)CSIPRetryAfterHeader;
    CleanupStack::PushL(header);
    header->ConstructL();
    header->ParseL(aValue, EFalse);
    CleanupStack::Pop(header);
    return header;
    }

// ----------------------------------------------------------------------------
// CSIPRetryAfterHeader::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPRetryAfterHeader* CSIPRetryAfterHeader::NewL(TUint aRetryAfter)
    {
    CSIPRetryAfterHeader* self = CSIPRetryAfterHeader::NewLC(aRetryAfter);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// CSIPRetryAfterHeader::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPRetryAfterHeader* CSIPRetryAfterHeader::NewLC(TUint aRetryAfter)
    {
    CSIPRetryAfterHeader* self = new (ELeave)CSIPRetryAfterHeader(aRetryAfter);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CSIPRetryAfterHeader::CSIPRetryAfterHeader
// ----------------------------------------------------------------------------
//
CSIPRetryAfterHeader::CSIPRetryAfterHeader()
: CSIPParameterHeaderBase(';')
    {
    }

// ----------------------------------------------------------------------------
// CSIPRetryAfterHeader::CSIPRetryAfterHeader
// ----------------------------------------------------------------------------
//
CSIPRetryAfterHeader::CSIPRetryAfterHeader(TUint aRetryAfter)
: CSIPParameterHeaderBase(';'), iRetryAfter(aRetryAfter)
    {
    }

// ----------------------------------------------------------------------------
// CSIPRetryAfterHeader::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPRetryAfterHeader::ConstructL()
    {
    iParams = new(ELeave) CSIPRetryAfterHeaderParams;
    iComment = KNullDesC8().AllocL();
    }

// ----------------------------------------------------------------------------
// CSIPRetryAfterHeader::ConstructL
// ----------------------------------------------------------------------------
//
void 
CSIPRetryAfterHeader::ConstructL(const CSIPRetryAfterHeader& aRetryAfterHeader)
    {
    iRetryAfter = aRetryAfterHeader.iRetryAfter;
    iParams = CSIPRetryAfterHeaderParams::NewL(*(aRetryAfterHeader.iParams));
    TPtrC8 comment(aRetryAfterHeader.Comment());
    delete iComment; iComment = 0;
    iComment = comment.AllocL();
    }

// ----------------------------------------------------------------------------
// CSIPRetryAfterHeader::~CSIPRetryAfterHeader
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPRetryAfterHeader::~CSIPRetryAfterHeader()
    {
    delete iParams;
    delete iComment;
    }

// ----------------------------------------------------------------------------
// CSIPRetryAfterHeader::SetRetryAfter
// ----------------------------------------------------------------------------
//
EXPORT_C void CSIPRetryAfterHeader::SetRetryAfter(TUint aValue)
    {
    iRetryAfter = aValue;
    }

// ----------------------------------------------------------------------------
// CSIPRetryAfterHeader::RetryAfter
// ----------------------------------------------------------------------------
//
EXPORT_C TUint CSIPRetryAfterHeader::RetryAfter() const
    {
    return iRetryAfter;
    }

// ----------------------------------------------------------------------------
// CSIPRetryAfterHeader::Comment
// ----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSIPRetryAfterHeader::Comment() const
    {
    return *iComment;
    }

// ----------------------------------------------------------------------------
// CSIPRetryAfterHeader::DurationParameter
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPRetryAfterHeader::DurationParam() const
    {
    return iParams->IntParamValue(
                            SIPStrings::StringF(SipStrConsts::EDuration));
    }

// ----------------------------------------------------------------------------
// CSIPRetryAfterHeader::SetDurationParameterL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSIPRetryAfterHeader::SetDurationParamL(TInt aDurationParam)
    {
    iParams->SetParamL(
            SIPStrings::StringF(SipStrConsts::EDuration), aDurationParam);
    }

// ----------------------------------------------------------------------------
// CSIPRetryAfterHeader::InternalizeValueL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* 
CSIPRetryAfterHeader::InternalizeValueL(RReadStream& aReadStream)
    {
    CSIPRetryAfterHeader* header = new(ELeave) CSIPRetryAfterHeader;
    CleanupStack::PushL(header);
    header->DoInternalizeValueL(aReadStream);
    CleanupStack::Pop(header);
    return header;
    }

// ----------------------------------------------------------------------------
// CSIPRetryAfterHeader::CloneL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPRetryAfterHeader::CloneL() const
    {
    CSIPRetryAfterHeader* header = new(ELeave) CSIPRetryAfterHeader;
    CleanupStack::PushL(header);
    header->ConstructL(*this);
    CleanupStack::Pop(header);
    return header;
    }

// ----------------------------------------------------------------------------
// CSIPRetryAfterHeader::Name
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPRetryAfterHeader::Name() const
    {
    return SIPStrings::StringF(SipStrConsts::ERetryAfterHeader);
    }

// ----------------------------------------------------------------------------
// CSIPRetryAfterHeader::PreferredPlaceInMessage
// ----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace 
CSIPRetryAfterHeader::PreferredPlaceInMessage() const
    {
    return CSIPHeaderBase::EMiddleBottom;
    }

// ----------------------------------------------------------------------------
// CSIPRetryAfterHeader::BaseDecodeL
// ----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> 
CSIPRetryAfterHeader::BaseDecodeL(const TDesC8& aValue)
    {
	CSIPRetryAfterHeader* header = DecodeL(aValue);
	CleanupStack::PushL (header);
	RPointerArray<CSIPHeaderBase> headers;
	User::LeaveIfError(headers.Append(header));
	CleanupStack::Pop(header);
	return headers;
    }

// ----------------------------------------------------------------------------
// CSIPRetryAfterHeader::ExternalizeValueL
// ----------------------------------------------------------------------------
//
void CSIPRetryAfterHeader::ExternalizeValueL(RWriteStream& aWriteStream) const
    {
    aWriteStream.WriteUint32L(iRetryAfter);

    TPtrC8 comment(Comment());        
    aWriteStream.WriteUint32L(comment.Length());
    if(comment.Length() > 0)
        {
        aWriteStream.WriteL(comment);
        }
    iParams->ExternalizeL(aWriteStream);
    }

// ----------------------------------------------------------------------------
// CSIPRetryAfterHeader::ToTextMandatoryPartLC
// ----------------------------------------------------------------------------
//
HBufC8* CSIPRetryAfterHeader::ToTextMandatoryPartLC() const
    {
    HBufC8* retryAfter = HBufC8::NewLC(KMaxUintLength+1+iComment->Length());
    TPtr8 retryAfterPtr(retryAfter->Des());
    retryAfterPtr.Format(KTUintFormat, iRetryAfter);
    if (iComment->Length() > 0)
        {
        retryAfterPtr.Append(' ');
        retryAfterPtr.Append(*iComment);        
        }
    return retryAfter;
    }

// ----------------------------------------------------------------------------
// CSIPRetryAfterHeader::ParseMandatoryPartL
// ----------------------------------------------------------------------------
//
void CSIPRetryAfterHeader::ParseMandatoryPartL(const TDesC8& aMandatoryPart)
    {
    TLex8 lex(aMandatoryPart);
    TPtrC8 retryAfter(lex.NextToken());
    TLex8 retryAfterLex(retryAfter);
    TInt err = retryAfterLex.Val(iRetryAfter);

    if(err != KErrNone)
        {
        User::Leave(KErrSipCodecRetryAfterHeader); // Value not TUint
        }
    lex.SkipSpace();
    
    // Check if there is comment available
    TPtrC8 remainder(lex.Remainder());
    if(remainder.Length() == 0)
        {
        return; // No comment
        }

    HBufC8* comment = remainder.AllocLC();
    TPtr8 commentPtr(comment->Des());
    commentPtr.Trim();
    if(!SIPSyntaxCheck::Comment(commentPtr)) 
        {
        User::Leave(KErrSipCodecRetryAfterHeader); // Not valid comment
        }
    
    delete iComment; iComment = 0;
    iComment = commentPtr.AllocL();
    CleanupStack::PopAndDestroy(comment);
    }

// ----------------------------------------------------------------------------
// CSIPRetryAfterHeader::Params
// ----------------------------------------------------------------------------
//
const CSIPParamContainerBase& CSIPRetryAfterHeader::Params() const
    {
    return *iParams;
    }

// ----------------------------------------------------------------------------
// CSIPRetryAfterHeader::Params
// ----------------------------------------------------------------------------
//
CSIPParamContainerBase& CSIPRetryAfterHeader::Params()
    {
    return *iParams;
    }

// ----------------------------------------------------------------------------
// CSIPRetryAfterHeader::DoInternalizeValueL
// ----------------------------------------------------------------------------
//
void CSIPRetryAfterHeader::DoInternalizeValueL(RReadStream& aReadStream)
    {
    iRetryAfter = aReadStream.ReadUint32L();
    iComment = SIPCodecUtils::ReadDescFromStreamL(aReadStream);
    iParams = CSIPRetryAfterHeaderParams::InternalizeL(aReadStream);
    }

