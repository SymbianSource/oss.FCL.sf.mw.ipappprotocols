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
// Name          : CSIPPAssociatedURIHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "sippassociateduriheader.h"
#include "CSIPTokenizer.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipcodecerr.h"
#include "_sipcodecdefs.h"


// ----------------------------------------------------------------------------
// CSIPPAssociatedURIHeader::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C RPointerArray<CSIPPAssociatedURIHeader>
    CSIPPAssociatedURIHeader::DecodeL(const TDesC8& aValue)
	{	
    RPointerArray<CSIPHeaderBase> baseheaders = BaseDecodeL(aValue);
    CSIPHeaderBase::PushLC(&baseheaders);
    RPointerArray<CSIPPAssociatedURIHeader> associatedHeaders;
    CleanupClosePushL(associatedHeaders);
    TInt count = baseheaders.Count();
    for (TInt i=0; i < count; i++)
		{
		CSIPPAssociatedURIHeader* header = 
            static_cast<CSIPPAssociatedURIHeader*>(baseheaders[i]);
		User::LeaveIfError (associatedHeaders.Append(header));
		}
    CleanupStack::Pop(2); //associatedHeaders, baseheaders
    baseheaders.Close();
    return associatedHeaders;	
	}

// ----------------------------------------------------------------------------
// CSIPPAssociatedURIHeader::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPPAssociatedURIHeader* 
    CSIPPAssociatedURIHeader::NewL(CSIPAddress* aSIPAddress)
	{
	CSIPPAssociatedURIHeader* self = 
        CSIPPAssociatedURIHeader::NewLC(aSIPAddress);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPPAssociatedURIHeader::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPPAssociatedURIHeader* 
    CSIPPAssociatedURIHeader::NewLC(CSIPAddress* aSIPAddress)
	{
	CSIPPAssociatedURIHeader* self = new(ELeave)CSIPPAssociatedURIHeader;
	CleanupStack::PushL(self);
	self->ConstructL(aSIPAddress);
	return self;
	}
	
// ----------------------------------------------------------------------------
// CSIPPAssociatedURIHeader::CSIPPAssociatedURIHeader
// ----------------------------------------------------------------------------
//
CSIPPAssociatedURIHeader::CSIPPAssociatedURIHeader()
	{
	}

// ----------------------------------------------------------------------------
// CSIPPAssociatedURIHeader::~CSIPPAssociatedURIHeader
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPPAssociatedURIHeader::~CSIPPAssociatedURIHeader()
	{
	}

// ----------------------------------------------------------------------------
// CSIPPAssociatedURIHeader::CloneL
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPPAssociatedURIHeader::CloneL() const
	{
	CSIPPAssociatedURIHeader* clone = new(ELeave)CSIPPAssociatedURIHeader;
	CleanupStack::PushL(clone);
	clone->ConstructL(*this);
	CleanupStack::Pop(clone);	
	return clone;
	}

// ----------------------------------------------------------------------------
// CSIPPAssociatedURIHeader::Name
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPPAssociatedURIHeader::Name() const
	{
	return SIPStrings::StringF(SipStrConsts::EPAssociatedURIHeader);
	}

// ----------------------------------------------------------------------------
// CSIPPAssociatedURIHeader::InternalizeValueL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPPAssociatedURIHeader::InternalizeValueL(
    RReadStream& aReadStream)
	{
	CSIPPAssociatedURIHeader* self = new(ELeave)CSIPPAssociatedURIHeader();
	CleanupStack::PushL(self);
	self->DoInternalizeValueL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPPAssociatedURIHeader::PreferredPlaceInMessage
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace
CSIPPAssociatedURIHeader::PreferredPlaceInMessage() const
	{
	return CSIPHeaderBase::EMiddleBottom;
	}

// ----------------------------------------------------------------------------
// CSIPPAssociatedURIHeader::MoreThanOneAllowed
// From CSIPHeaderBase:
// ----------------------------------------------------------------------------
//
TBool CSIPPAssociatedURIHeader::MoreThanOneAllowed() const
    {
    return ETrue;
    }
	
// -----------------------------------------------------------------------------
// CSIPPAssociatedURIHeader::BaseDecodeL
// -----------------------------------------------------------------------------
//	
RPointerArray<CSIPHeaderBase> CSIPPAssociatedURIHeader::BaseDecodeL(
    const TDesC8& aValue)
	{
	__ASSERT_ALWAYS (aValue.Length() > 0,
                     User::Leave(KErrSipCodecPAssociatedURIHeader));

	RPointerArray<CSIPHeaderBase> headers;
    CSIPHeaderBase::PushLC(&headers);
	CSIPTokenizer* tokenizer = CSIPTokenizer::NewLC(aValue, ',', ETrue);
	for (TInt i=0; i < tokenizer->Tokens().Count(); i++)
		{
		CSIPPAssociatedURIHeader* header = new(ELeave)CSIPPAssociatedURIHeader;
        CleanupStack::PushL(header);
        header->ConstructL();
		header->ParseL(tokenizer->Tokens()[i], ETrue, ETrue);
		headers.AppendL(header);
		CleanupStack::Pop(header);
		}
	CleanupStack::PopAndDestroy(tokenizer);
	CleanupStack::Pop(); // headers 
	return headers;
	} 	
