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
// Name          : CSIPRAckHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "siprackheader.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipcodecutils.h"
#include "_sipcodecdefs.h"

const TInt KMaxTRealLength(32);
const TInt KSpace = 32;
_LIT8(KTUintFormat, "%u");


// ----------------------------------------------------------------------------
// CSIPRAckHeader::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPRAckHeader* CSIPRAckHeader::DecodeL(const TDesC8& aValue)
	{
    CSIPRAckHeader* header = new (ELeave) CSIPRAckHeader;
    CleanupStack::PushL(header);
    header->ParseL(aValue);
    CleanupStack::Pop(header);
    return header;
	}

// ----------------------------------------------------------------------------
// CSIPRAckHeader::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPRAckHeader* CSIPRAckHeader::NewL(TUint aSeq,
                                             TUint aCSeqNum,
                                             RStringF aMethod)
    {
    CSIPRAckHeader* self = CSIPRAckHeader::NewLC(aSeq, aCSeqNum, aMethod);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// CSIPRAckHeader::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPRAckHeader* CSIPRAckHeader::NewLC(TUint aSeq,
                                              TUint aCSeqNum,
                                              RStringF aMethod)
    {
    CSIPRAckHeader* self = new (ELeave) CSIPRAckHeader(aSeq, aCSeqNum);
    CleanupStack::PushL(self);
    self->ConstructL(aMethod);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPRAckHeader::CSIPRAckHeader
// -----------------------------------------------------------------------------
//
CSIPRAckHeader::CSIPRAckHeader()
	{
	}

// ----------------------------------------------------------------------------
// CSIPRAckHeader::CSIPRAckHeader
// ----------------------------------------------------------------------------
//
CSIPRAckHeader::CSIPRAckHeader(TUint aSeq, TUint aCSeqNum)
    {
    SetSeq(aSeq);
    SetCSeqNum(aCSeqNum);
    }

// ----------------------------------------------------------------------------
// CSIPRAckHeader::~CSIPRAckHeader
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPRAckHeader::~CSIPRAckHeader()
    {
    iMethod.Close();
    }

// ----------------------------------------------------------------------------
// CSIPRAckHeader::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPRAckHeader::ConstructL(RStringF aMethod)
    {
    SetMethodL(aMethod);
    }

// ----------------------------------------------------------------------------
// CSIPRAckHeader::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPRAckHeader::ConstructL(const CSIPRAckHeader& aRAckHeader)
    {
    SetSeq(aRAckHeader.Seq());
    SetCSeqNum(aRAckHeader.CSeqNum());
    SetMethodL(aRAckHeader.Method());
    }

// ----------------------------------------------------------------------------
// CSIPRAckHeader::Seq
// ----------------------------------------------------------------------------
//
EXPORT_C TUint CSIPRAckHeader::Seq() const
    {
    return iSeq;
    }

// ----------------------------------------------------------------------------
// CSIPRAckHeader::SetSeq
// ----------------------------------------------------------------------------
//
EXPORT_C void CSIPRAckHeader::SetSeq(TUint aSeq)
    {
    iSeq = aSeq;
    }

// ----------------------------------------------------------------------------
// CSIPRAckHeader::CSeqNum
// ----------------------------------------------------------------------------
//
EXPORT_C TUint CSIPRAckHeader::CSeqNum() const
    {
    return iCSeqNum;
    }

// ----------------------------------------------------------------------------
// CSIPRAckHeader::SetCSeqNum
// ----------------------------------------------------------------------------
//
EXPORT_C void CSIPRAckHeader::SetCSeqNum(TUint aCSeqNum)
    {
    iCSeqNum = aCSeqNum;
    }

// ----------------------------------------------------------------------------
// CSIPRAckHeader::Method
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPRAckHeader::Method() const
    {
    return iMethod;
    }

// ----------------------------------------------------------------------------
// CSIPRAckHeader::SetMethodL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSIPRAckHeader::SetMethodL(RStringF aMethod)
    {
    SetMethodL(aMethod.DesC());
    }

// ----------------------------------------------------------------------------
// CSIPRAckHeader::SetMethodL
// ----------------------------------------------------------------------------
//
void CSIPRAckHeader::SetMethodL(const TDesC8& aMethod)
    {
    RStringF tmpString = 
        SIPCodecUtils::CheckAndCreateTokenL(aMethod, KErrSipCodecSIPMethod);
	iMethod.Close();
	iMethod = tmpString;
    }

// ----------------------------------------------------------------------------
// CSIPRAckHeader::InternalizeValueL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* 
CSIPRAckHeader::InternalizeValueL(RReadStream& aReadStream)
    {
    CSIPRAckHeader* self = new(ELeave) CSIPRAckHeader;
    CleanupStack::PushL(self);
    self->DoInternalizeValueL(aReadStream);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// CSIPRAckHeader::CloneL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPRAckHeader::CloneL() const
    {
    CSIPRAckHeader* clone = new(ELeave) CSIPRAckHeader;
    CleanupStack::PushL(clone);
    clone->ConstructL(*this);
    CleanupStack::Pop(clone);
    return clone;
    }

// ----------------------------------------------------------------------------
// CSIPRAckHeader::Name
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPRAckHeader::Name() const
    {
    return SIPStrings::StringF(SipStrConsts::ERAckHeader);
    }

// ----------------------------------------------------------------------------
// CSIPRAckHeader::ToTextValueL
// ----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CSIPRAckHeader::ToTextValueL() const
    {
	TBuf8<KMaxTRealLength> seqAsText;
	seqAsText.Format(KTUintFormat, iSeq);

    TBuf8<KMaxTRealLength> cSeqAsText;
    cSeqAsText.Format(KTUintFormat, iCSeqNum);

    // seq+SP+cseq+SP
	TUint encodedLength = seqAsText.Length() + 1 + cSeqAsText.Length() + 1;
	TPtrC8 method(iMethod.DesC());
	encodedLength += method.Length(); 

	HBufC8* encodingResult = HBufC8::NewL (encodedLength);
	TPtr8 encodingResultPtr = encodingResult->Des();

	encodingResultPtr.Append(seqAsText);
	encodingResultPtr.Append(KSpace);
    encodingResultPtr.Append(cSeqAsText);
    encodingResultPtr.Append(KSpace);
	encodingResultPtr.Append(method);

	return encodingResult;
    }

// ----------------------------------------------------------------------------
// CSIPRAckHeader::PreferredPlaceInMessage
// ----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace CSIPRAckHeader::PreferredPlaceInMessage() const
    {
    return CSIPHeaderBase::EMiddle;
    }

// ----------------------------------------------------------------------------
// CSIPRAckHeader::BaseDecodeL
// ----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> 
CSIPRAckHeader::BaseDecodeL(const TDesC8& aValue)
    {
	CSIPRAckHeader* header = DecodeL(aValue);
	CleanupStack::PushL(header);
	RPointerArray<CSIPHeaderBase> headers;
	User::LeaveIfError (headers.Append(header));
	CleanupStack::Pop(header);
	return headers;
    }

// ----------------------------------------------------------------------------
// CSIPRAckHeader::ExternalizeValueL
// ----------------------------------------------------------------------------
//
void CSIPRAckHeader::ExternalizeValueL (RWriteStream& aWriteStream) const
    {
	aWriteStream.WriteUint32L(this->Seq());
    aWriteStream.WriteUint32L(this->CSeqNum());
	TPtrC8 method(iMethod.DesC());
	aWriteStream.WriteUint32L(method.Length());
	if (method.Length() > 0)
		{
		aWriteStream.WriteL(method);
		}
    }

// ----------------------------------------------------------------------------
// CSIPRAckHeader::DoInternalizeValueL
// ----------------------------------------------------------------------------
//
void CSIPRAckHeader::DoInternalizeValueL(RReadStream& aReadStream)
    {
	iSeq = aReadStream.ReadUint32L();
    iCSeqNum = aReadStream.ReadUint32L();

    HBufC8* method = SIPCodecUtils::ReadDescFromStreamL(aReadStream);
    CleanupStack::PushL(method);
    iMethod = SIPStrings::Pool().OpenFStringL(*method);
    CleanupStack::PopAndDestroy(method);
    }

// ----------------------------------------------------------------------------
// CSIPRAckHeader::ParseL
// ----------------------------------------------------------------------------
//
void CSIPRAckHeader::ParseL(const TDesC8& aValue)
    {
    __ASSERT_ALWAYS(aValue.Length() > 0,
        User::Leave(KErrSipCodecRAckHeader));

	TLex8 lex(aValue);
	if (!SIPSyntaxCheck::UInt(lex.NextToken(),iSeq))
		{
		User::Leave (KErrSipCodecRAckHeader);
		}
	lex.SkipSpace();
    if (!SIPSyntaxCheck::UInt(lex.NextToken(), iCSeqNum))
        {
        User::Leave (KErrSipCodecRAckHeader);
        }
    lex.SkipSpace();
	TPtrC8 method(lex.Remainder());
	SetMethodL(method);
    }
