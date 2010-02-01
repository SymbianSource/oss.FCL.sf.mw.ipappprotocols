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
// Name          : CSIPCSeqHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "sipcseqheader.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipcodecutils.h"
#include "_sipcodecdefs.h"

const TInt KSpace = 32;

// -----------------------------------------------------------------------------
// CSIPCSeqHeader::DecodeL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPCSeqHeader* CSIPCSeqHeader::DecodeL(const TDesC8& aValue)
	{
	CSIPCSeqHeader* self = new(ELeave)CSIPCSeqHeader;
	CleanupStack::PushL(self);	
	self->ParseL(aValue);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPCSeqHeader::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPCSeqHeader* CSIPCSeqHeader::NewL(TUint aSeq, RStringF aMethod)
	{
	CSIPCSeqHeader* self = CSIPCSeqHeader::NewLC(aSeq,aMethod);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPCSeqHeader::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPCSeqHeader* CSIPCSeqHeader::NewLC(TUint aSeq, RStringF aMethod)
	{
	CSIPCSeqHeader* self = new(ELeave)CSIPCSeqHeader;
	CleanupStack::PushL(self);
	self->ConstructL(aSeq,aMethod);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPCSeqHeader::CSIPCSeqHeader
// -----------------------------------------------------------------------------
//
CSIPCSeqHeader::CSIPCSeqHeader()
	{
	}

// -----------------------------------------------------------------------------
// CSIPCSeqHeader::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPCSeqHeader::ConstructL(TUint aSeq, RStringF aMethod)
	{
	SetMethodL(aMethod);
	iSeq = aSeq;
	}

// -----------------------------------------------------------------------------
// CSIPCSeqHeader::~CSIPCSeqHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPCSeqHeader::~CSIPCSeqHeader()
	{
	iMethod.Close();
	}

// -----------------------------------------------------------------------------
// CSIPCSeqHeader::CloneL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPCSeqHeader::CloneL() const
	{
	CSIPCSeqHeader* clone = new(ELeave)CSIPCSeqHeader;
	clone->iMethod = this->Method().Copy();
	clone->iSeq = this->iSeq;
	return clone;	
	}

// -----------------------------------------------------------------------------
// CSIPCSeqHeader::Seq
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CSIPCSeqHeader::Seq() const
	{
	return iSeq;
	}

// -----------------------------------------------------------------------------
// CSIPCSeqHeader::SetSeq
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPCSeqHeader::SetSeq(TUint aSeq)
	{
	iSeq = aSeq;
	}

// -----------------------------------------------------------------------------
// CSIPCSeqHeader::Method
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPCSeqHeader::Method() const
	{
	return iMethod;
	}

// -----------------------------------------------------------------------------
// CSIPCSeqHeader::SetMethodL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPCSeqHeader::SetMethodL(RStringF aMethod)
	{
	SetMethodL(aMethod.DesC());
	}

// -----------------------------------------------------------------------------
// CSIPCSeqHeader::FullName
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPCSeqHeader::Name() const
	{
	return SIPStrings::StringF(SipStrConsts::ECSeqHeader);
	}

// -----------------------------------------------------------------------------
// CSIPCSeqHeader::ToTextValueL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CSIPCSeqHeader::ToTextValueL() const
	{
	const TInt KMaxSeqNumLength = 80;
	TBuf8<KMaxSeqNumLength> seqAsText;
	seqAsText.Format(_L8("%u"), iSeq);

	TUint encodedLength = seqAsText.Length() + 1; // SP
	TPtrC8 method(iMethod.DesC());
	encodedLength += method.Length(); 

	HBufC8* encodingResult = HBufC8::NewL (encodedLength);
	TPtr8 encodingResultPtr = encodingResult->Des();

	encodingResultPtr.Append(seqAsText);
	encodingResultPtr.Append(KSpace);
	encodingResultPtr.Append(method);

	return encodingResult;
	}

// -----------------------------------------------------------------------------
// CSIPCSeqHeader::PreferredPlaceInMessage
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace CSIPCSeqHeader::PreferredPlaceInMessage() const
	{
	return CSIPHeaderBase::EMiddle;
	}

// -----------------------------------------------------------------------------
// CSIPCSeqHeader::BaseDecodeL
// -----------------------------------------------------------------------------
//
RPointerArray<CSIPHeaderBase> CSIPCSeqHeader::BaseDecodeL(const TDesC8& aValue)
	{
	CSIPCSeqHeader* header = DecodeL(aValue);
	CleanupStack::PushL(header);
	RPointerArray<CSIPHeaderBase> headers;
	User::LeaveIfError (headers.Append(header));
	CleanupStack::Pop(header);
	return headers;
	}	

// -----------------------------------------------------------------------------
// CSIPCSeqHeader::ParseL
// -----------------------------------------------------------------------------
//
void CSIPCSeqHeader::ParseL (const TDesC8& aValue)
	{
	__ASSERT_ALWAYS(aValue.Length() > 0,
	                User::Leave (KErrSipCodecCSeqHeader));
	
	TLex8 lex(aValue);
	if (!SIPSyntaxCheck::UInt(lex.NextToken(),iSeq))
		{
		User::Leave (KErrSipCodecCSeqHeader);
		}
	lex.SkipSpace();
	TPtrC8 method(lex.Remainder());
	SetMethodL(method);
	}

// -----------------------------------------------------------------------------
// CSIPCSeqHeader::ExternalizeValueL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
void CSIPCSeqHeader::ExternalizeValueL (RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteUint32L(this->Seq());
	TPtrC8 method(iMethod.DesC());
	aWriteStream.WriteUint32L(method.Length());
	if (method.Length() > 0)
		{
		aWriteStream.WriteL(method);
		}
	}

// -----------------------------------------------------------------------------
// CSIPCSeqHeader::InternalizeValueL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* 
CSIPCSeqHeader::InternalizeValueL (RReadStream& aReadStream)
	{
	CSIPCSeqHeader* self = new(ELeave)CSIPCSeqHeader;
	CleanupStack::PushL(self);
	self->DoInternalizeValueL (aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPCSeqHeader::DoInternalizeValueL
// -----------------------------------------------------------------------------
//
void CSIPCSeqHeader::DoInternalizeValueL (RReadStream& aReadStream)
	{
    iSeq = aReadStream.ReadUint32L();

    HBufC8* method = SIPCodecUtils::ReadDescFromStreamL(aReadStream);
    CleanupStack::PushL(method);
    iMethod = SIPStrings::Pool().OpenFStringL(*method);
    CleanupStack::PopAndDestroy(method);
	}

// -----------------------------------------------------------------------------
// CSIPCSeqHeader::SetMethodL
// -----------------------------------------------------------------------------
//	
void CSIPCSeqHeader::SetMethodL(const TDesC8& aMethod)
	{	
    RStringF tmp = 
        SIPCodecUtils::CheckAndCreateTokenL(aMethod, KErrSipCodecSIPMethod);
	iMethod.Close();
    iMethod = tmp;
	}
