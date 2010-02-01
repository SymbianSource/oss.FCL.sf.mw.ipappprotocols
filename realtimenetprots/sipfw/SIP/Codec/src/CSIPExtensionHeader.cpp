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
// Name          : CSIPExtensionHeader.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "sipextensionheader.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipcodecutils.h"
#include "_sipcodecdefs.h"


// -----------------------------------------------------------------------------
// CSIPExtensionHeader::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPExtensionHeader* CSIPExtensionHeader::NewL(
    const TDesC8& aName,
    const TDesC8& aValue)
	{
	CSIPExtensionHeader* self = CSIPExtensionHeader::NewLC(aName, aValue);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPExtensionHeader::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPExtensionHeader* CSIPExtensionHeader::NewLC(
    const TDesC8& aName,
    const TDesC8& aValue)
	{
	CSIPExtensionHeader* self = new(ELeave)CSIPExtensionHeader;
	CleanupStack::PushL (self);
	self->ConstructL(aName, aValue);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPExtensionHeader::CSIPExtensionHeader
// -----------------------------------------------------------------------------
//
CSIPExtensionHeader::CSIPExtensionHeader()
	{
	}

// -----------------------------------------------------------------------------
// CSIPExtensionHeader::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPExtensionHeader::ConstructL(const TDesC8& aName, const TDesC8& aValue)
	{
	SetNameL(aName);
	SetValueL(aValue);
	}

// -----------------------------------------------------------------------------
// CSIPExtensionHeader::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPExtensionHeader::ConstructL(
    const CSIPExtensionHeader& aExtensionHeader)
	{
	iName = aExtensionHeader.Name().Copy();
	iValue = aExtensionHeader.iValue->AllocL ();
	}

// -----------------------------------------------------------------------------
// CSIPExtensionHeader::~CSIPExtensionHeader
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPExtensionHeader::~CSIPExtensionHeader()
	{
	delete iValue;
	iName.Close();
	}

// -----------------------------------------------------------------------------
// CSIPExtensionHeader::CloneL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPExtensionHeader::CloneL() const
	{
	CSIPExtensionHeader* clone = new(ELeave)CSIPExtensionHeader;
	CleanupStack::PushL(clone);
	clone->ConstructL(*this);
	CleanupStack::Pop(clone);
	return clone;	
	}

// -----------------------------------------------------------------------------
// CSIPExtensionHeader::SetValueL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPExtensionHeader::SetValueL(const TDesC8& aValue)
	{
	HBufC8* tmp = aValue.AllocLC();
	tmp->Des().Trim();
	if (!CheckValue(*tmp)) 
		{
		User::Leave(KErrSipCodecHeaderValue);
		}
	CleanupStack::Pop(tmp);
	delete iValue;
	iValue = tmp;
	}

// -----------------------------------------------------------------------------
// CSIPExtensionHeader::Value
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSIPExtensionHeader::Value() const
	{
	return *iValue;
	}

// -----------------------------------------------------------------------------
// CSIPExtensionHeader::Name
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPExtensionHeader::Name() const
	{
	return iName;
	}

// -----------------------------------------------------------------------------
// CSIPExtensionHeader::HasCompactName
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
TBool CSIPExtensionHeader::HasCompactName() const
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPExtensionHeader::CompactName
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
RStringF CSIPExtensionHeader::CompactName() const
	{
	return iName;
	}

// -----------------------------------------------------------------------------
// CSIPExtensionHeader::EncodeMultipleToOneLine
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
TBool CSIPExtensionHeader::EncodeMultipleToOneLine() const
	{
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPExtensionHeader::MoreThanOneAllowed
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
TBool CSIPExtensionHeader::MoreThanOneAllowed() const
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPExtensionHeader::IsExtensionHeader
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
TBool CSIPExtensionHeader::IsExtensionHeader() const
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPExtensionHeader::ExternalizeSupported
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPExtensionHeader::ExternalizeSupported() const
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPExtensionHeader::InternalizeValueL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPExtensionHeader* CSIPExtensionHeader::InternalizeValueL(
    RReadStream& aReadStream)
	{
	CSIPExtensionHeader* self = new(ELeave)CSIPExtensionHeader;
	CleanupStack::PushL(self);
	self->DoInternalizeValueL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPExtensionHeader::ExternalizeValueL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
void CSIPExtensionHeader::ExternalizeValueL(RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteUint32L (iValue->Length());
	aWriteStream.WriteL (*iValue);
	}

// -----------------------------------------------------------------------------
// CSIPExtensionHeader::DoInternalizeValueL
// -----------------------------------------------------------------------------
//
void CSIPExtensionHeader::DoInternalizeValueL(RReadStream& aReadStream)
	{
    iValue = SIPCodecUtils::ReadDescFromStreamL(aReadStream);
	}

// -----------------------------------------------------------------------------
// CSIPExtensionHeader::ToTextValueL
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CSIPExtensionHeader::ToTextValueL() const
	{
	return iValue->AllocL();
	}

// -----------------------------------------------------------------------------
// CSIPExtensionHeader::PreferredPlaceInMessage
// From CSIPHeaderBase:
// -----------------------------------------------------------------------------
//
CSIPHeaderBase::TPreferredPlace 
CSIPExtensionHeader::PreferredPlaceInMessage() const
	{
	return CSIPHeaderBase::EMiddleBottom;
	}

// -----------------------------------------------------------------------------
// CSIPExtensionHeader::CheckValue
// -----------------------------------------------------------------------------
//
TBool CSIPExtensionHeader::CheckValue(const TDesC8& aValue)
	{
	return SIPSyntaxCheck::ExtensionHeaderValue(aValue);
	}
	
// -----------------------------------------------------------------------------
// CSIPExtensionHeader::SetNameL
// -----------------------------------------------------------------------------
//
void CSIPExtensionHeader::SetNameL(const TDesC8& aName)
	{
    RStringF tmp = 
        SIPCodecUtils::CheckAndCreateTokenL(aName, KErrSipCodecHeaderName);
	iName.Close();
    iName = tmp;
	}	
