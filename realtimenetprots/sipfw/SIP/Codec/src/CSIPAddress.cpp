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
// Name          : CSIPAddress.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "sipaddress.h"
#include "sipuri.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "uricontainer.h"
#include "_sipcodecdefs.h"


_LIT8 (KLaQuot, "<");
_LIT8 (KRaQuot, ">");

// ----------------------------------------------------------------------------
// CSIPAddress::DecodeL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAddress* CSIPAddress::DecodeL (const TDesC8& aValue)
	{
	__ASSERT_ALWAYS (aValue.Length() > 0, 
		User::Leave (KErrSipCodecSIPAddress));

	TLex8 lex(aValue);
	lex.SkipSpace();
	if (lex.Peek() == 0) 
        {
        User::Leave (KErrSipCodecSIPAddress);
        }
	TPtrC8 value(lex.Remainder());

	CSIPAddress* sipAddress = new(ELeave)CSIPAddress;
	CleanupStack::PushL(sipAddress);
	sipAddress->ConstructL();	
	TInt doubleQuotePos = value.Locate ('"');
	if (doubleQuotePos == 0) // has display-name as quoted string and URI
		{
		const TInt KMinQuotedStrLength = 2;
		TInt quotedStrLen = sipAddress->QuotedStringLength(value);
		if (quotedStrLen < KMinQuotedStrLength || 
		    quotedStrLen == value.Length()) 
			{
			User::Leave (KErrSipCodecSIPAddress);
			}
		sipAddress->SetDisplayNameL (value.Left(quotedStrLen));
		sipAddress->ParseURIInAngleBracketsL (value.Mid(quotedStrLen));
		}
	else if (doubleQuotePos < 0)
		{
		TInt openingAngleBracketPos = value.Locate('<');
		if (openingAngleBracketPos < 0) // has only URI
			{
			sipAddress->ParseURIL (value);
			}
		else if (openingAngleBracketPos == 0) // has only URI in angle brackets
			{
			sipAddress->ParseURIInAngleBracketsL (value);
			}
		else // has both display-name and URI 
			{
			TPtrC8 displayName (value.Left(openingAngleBracketPos));
			sipAddress->SetDisplayNameL (displayName);
			value.Set(value.Mid(openingAngleBracketPos));
			sipAddress->ParseURIInAngleBracketsL (value);
			}
		}
	else
		{
		User::Leave (KErrSipCodecSIPAddress);
		}
	CleanupStack::Pop(sipAddress);
	return sipAddress;
	}

// ----------------------------------------------------------------------------
// CSIPAddress::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAddress* CSIPAddress::NewL (CUri8* aUri8)
	{
	CSIPAddress* self = CSIPAddress::NewLC (aUri8);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPAddress::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAddress* CSIPAddress::NewLC (CUri8* aUri8)
	{
	CSIPAddress* self = new(ELeave)CSIPAddress;
	CleanupStack::PushL(self);
	self->ConstructL(aUri8);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPAddress::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAddress* 
CSIPAddress::NewL (const TDesC8& aDisplayName, CUri8* aUri8)
	{
	CSIPAddress* self = CSIPAddress::NewLC (aDisplayName, aUri8);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPAddress::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAddress* 
CSIPAddress::NewLC (const TDesC8& aDisplayName, CUri8* aUri8)
	{
	CSIPAddress* self = new(ELeave)CSIPAddress;
	CleanupStack::PushL(self);
	self->ConstructL(aDisplayName, aUri8);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPAddress::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAddress* CSIPAddress::NewL (const CSIPAddress& aSIPAddress)
	{
	CSIPAddress* self = CSIPAddress::NewLC (aSIPAddress);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPAddress::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAddress* CSIPAddress::NewLC (const CSIPAddress& aSIPAddress)
	{
	CSIPAddress* self = new(ELeave)CSIPAddress;
	CleanupStack::PushL(self);
	self->ConstructL(aSIPAddress);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPAddress::NewLC
// ----------------------------------------------------------------------------
//
CSIPAddress* CSIPAddress::NewLC(CURIContainer* aURI)
    {
	CSIPAddress* self = new(ELeave)CSIPAddress;
	CleanupStack::PushL(self);
	self->ConstructL(aURI);
	return self;    
    }

// ----------------------------------------------------------------------------
// CSIPAddress::CSIPAddress
// ----------------------------------------------------------------------------
//
CSIPAddress::CSIPAddress()
	{
	}

// ----------------------------------------------------------------------------
// CSIPAddress::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPAddress::ConstructL ()
	{
	iDisplayName = KNullDesC8().AllocL();
	}

// ----------------------------------------------------------------------------
// CSIPAddress::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPAddress::ConstructL (CUri8* aUri)
	{
    iDisplayName = KNullDesC8().AllocL();
	SetUri8L (aUri);
	}

// ----------------------------------------------------------------------------
// CSIPAddress::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPAddress::ConstructL (const TDesC8& aDisplayName, CUri8* aUri)
	{
	SetDisplayNameL (aDisplayName);
	SetUri8L (aUri);
	}

// ----------------------------------------------------------------------------
// CSIPAddress::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPAddress::ConstructL (const CSIPAddress& aSIPAddress)
	{
	__ASSERT_ALWAYS (aSIPAddress.iURI != 0, User::Leave(KErrArgument));

	if (aSIPAddress.HasDisplayName())
		{
		SetDisplayNameL(aSIPAddress.DisplayName());
		}
    else
        {
        iDisplayName = KNullDesC8().AllocL();
        }
    if(aSIPAddress.URI().IsSIPURI())
        {
        HBufC8* value = aSIPAddress.iURI->SIPURI()->ToTextL();
        CleanupStack::PushL(value);
        TPtrC8 valuePtr(value->Des());
        iURI = CURIContainer::DecodeL(valuePtr);
        CleanupStack::PopAndDestroy(value);
        }
    else
        {
        iURI = CURIContainer::DecodeL(aSIPAddress.iURI->Uri8()->Uri().UriDes());
        }	
	}
	
// ----------------------------------------------------------------------------
// CSIPAddress::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPAddress::ConstructL (CURIContainer* aURI)
	{
    __ASSERT_ALWAYS (aURI != 0, User::Leave(KErrArgument));	
    
    iDisplayName = KNullDesC8().AllocL();
	iURI = aURI;
	}	

// ----------------------------------------------------------------------------
// CSIPAddress::~CSIPAddress
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAddress::~CSIPAddress()
	{
	delete iDisplayName;
	delete iURI;
	}

// ----------------------------------------------------------------------------
// CSIPAddress::operator==
// ----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPAddress::operator==(const CSIPAddress& aSIPAddress) const
	{
	if (aSIPAddress.HasDisplayName() != HasDisplayName()) 
		{
		return EFalse;
		}

	if (aSIPAddress.HasDisplayName())
		{
		// In quoted strings the case sensitivity matters.
		if (aSIPAddress.DisplayName().Locate('"') == 0)
			{
			if (aSIPAddress.DisplayName().Compare(*iDisplayName) != 0)
				{
				return EFalse;
				}
			}
		else
			{
			if (aSIPAddress.DisplayName().CompareF(*iDisplayName) != 0)
				{
				return EFalse;
				}
			}
		}

	return (*(aSIPAddress.iURI) == *iURI);
	}

// ----------------------------------------------------------------------------
// CSIPAddress::DisplayName
// ----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSIPAddress::DisplayName() const
	{
	return *iDisplayName;
	}

// ----------------------------------------------------------------------------
// CSIPAddress::SetDisplayNameL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSIPAddress::SetDisplayNameL(const TDesC8& aDisplayName)
	{
	HBufC8* tmp = aDisplayName.AllocLC();
	tmp->Des().Trim();
	__ASSERT_ALWAYS (CheckDisplayName(*tmp),
	                 User::Leave(KErrSipCodecDisplayName));
	CleanupStack::Pop(tmp);
	delete iDisplayName;
	iDisplayName = tmp;
	}

// ----------------------------------------------------------------------------
// CSIPAddress::URI
// ----------------------------------------------------------------------------
//
EXPORT_C const CURIContainer& CSIPAddress::URI() const
	{
	return *iURI;
	}

// ----------------------------------------------------------------------------
// CSIPAddress::URI
// ----------------------------------------------------------------------------
//
EXPORT_C CURIContainer& CSIPAddress::URI()
	{
	return *iURI;
	}

// ----------------------------------------------------------------------------
// CSIPAddress::Uri8
// ----------------------------------------------------------------------------
//
EXPORT_C const CUri8& CSIPAddress::Uri8() const
    {
    return *(iURI->Uri8());
    }

// ----------------------------------------------------------------------------
// CSIPAddress::SetURIL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSIPAddress::SetUri8L (CUri8* aUri8)
	{
	__ASSERT_ALWAYS(aUri8 != 0, User::Leave(KErrArgument));

    CURIContainer* tmp = CURIContainer::DecodeL(aUri8->Uri().UriDes());
    delete aUri8;
    delete iURI;
    iURI = 0;
    iURI = tmp;
	}

// ----------------------------------------------------------------------------
// CSIPAddress::ToTextLC
// ----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CSIPAddress::ToTextLC(TBool aUseAngleBrackets) const
	{
	TUint encodedLength = 0;
	if (HasDisplayName()) 
        {
        encodedLength += iDisplayName->Length() + 1; // SP
        }
	HBufC8* encodedURI = iURI->ToTextL();
	CleanupStack::PushL (encodedURI);
	encodedLength += encodedURI->Length();

	TBool uriContainsSeparators = ContainsSeparators(*encodedURI);

	if (HasDisplayName() || uriContainsSeparators || aUseAngleBrackets)
		{
		encodedLength += KLaQuot().Length();
		encodedLength += KRaQuot().Length();
		}

	HBufC8* encodedSIPAddress = HBufC8::NewL (encodedLength);
	TPtr8 encodedPtr = encodedSIPAddress->Des();

	if (HasDisplayName())
		{
		encodedPtr.Append(*iDisplayName);
		encodedPtr.Append(' '); // SP
		}

	if (HasDisplayName() || uriContainsSeparators || aUseAngleBrackets)
		{
		encodedPtr.Append(KLaQuot);
		}

	encodedPtr.Append(*encodedURI);
	
	if (HasDisplayName() || uriContainsSeparators || aUseAngleBrackets)
		{
		encodedPtr.Append(KRaQuot);
		}

	CleanupStack::PopAndDestroy(encodedURI);
	CleanupStack::PushL (encodedSIPAddress);
	return encodedSIPAddress;
	}

// ----------------------------------------------------------------------------
// CSIPAddress::InternalizeL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPAddress* CSIPAddress::InternalizeL(RReadStream& aReadStream)
	{
	CSIPAddress* self = new(ELeave)CSIPAddress;
	CleanupStack::PushL(self);
	self->DoInternalizeL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPAddress::ExternalizeL
// ----------------------------------------------------------------------------
//
EXPORT_C void CSIPAddress::ExternalizeL (RWriteStream& aWriteStream)
	{
    iURI->ExternalizeL(aWriteStream);

	if (HasDisplayName())
		{
		aWriteStream.WriteUint8L(1);
		aWriteStream.WriteUint32L (iDisplayName->Length());
		aWriteStream.WriteL (*iDisplayName, iDisplayName->Length());
		}
	else
		{
		aWriteStream.WriteUint8L(0);
		}
	}

// ----------------------------------------------------------------------------
// CSIPAddress::DoInternalizeL
// ----------------------------------------------------------------------------
//
void CSIPAddress::DoInternalizeL (RReadStream& aReadStream)
	{
	iURI = CURIContainer::InternalizeL(aReadStream);

	if (aReadStream.ReadUint8L() == 1) // has display name
		{
		TUint32 displayNameLength = aReadStream.ReadUint32L();
		iDisplayName = HBufC8::NewL (displayNameLength);
		TPtr8 displayName(iDisplayName->Des());
		aReadStream.ReadL (displayName,displayNameLength);
		}
	else
		{
		iDisplayName = KNullDesC8().AllocL();	
		}
	}

// ----------------------------------------------------------------------------
// CSIPAddress::QuotedStringLength
// ----------------------------------------------------------------------------
//
TInt CSIPAddress::QuotedStringLength (const TDesC8& aValue)
	{
	if (aValue.Length() == 0) 
        {
        return KErrNotFound;
        }
	TLex8 lex(aValue);
	if (lex.Get() != '"')
        {
        return KErrNotFound;
        }
	TUint chrCount = 1;
	TChar currChr = lex.Get();
	TBool endOfQuotedStringFound = EFalse;
	TBool previousWasBackSlash = EFalse;
	while (currChr != 0 && !endOfQuotedStringFound)
		{
		if (currChr == '\\')
			{
			// Backslash can be used to quote another backslash
			previousWasBackSlash = !previousWasBackSlash;
			}
		else
			{
			if (currChr == '"' && !previousWasBackSlash)
				{
				endOfQuotedStringFound = ETrue;
				}
			previousWasBackSlash = EFalse;
			}
		chrCount++;		
		currChr = lex.Get();
		}
	if (!endOfQuotedStringFound) 
        {
        return KErrNotFound;
        }
	return chrCount;
	}

// ----------------------------------------------------------------------------
// CSIPAddress::CheckDisplayName
// ----------------------------------------------------------------------------
//
TBool CSIPAddress::CheckDisplayName (const TDesC8& aValue)
	{
	if (aValue.Length() == 0) return EFalse;
	TLex8 lex(aValue);
	lex.SkipSpace();
	if (lex.Peek() == '"') // quoted-string
		{
		return SIPSyntaxCheck::QuotedString(aValue);
		}
	else // *(token LWS)
		{
		TPtrC8 token(lex.NextToken());
		while (token.Length() > 0)
			{
			if (!SIPSyntaxCheck::Token(token)) 
                {
                return EFalse;
                }
			token.Set(lex.NextToken());
			}
		}
	return ETrue;
	}

// ----------------------------------------------------------------------------
// CSIPAddress::ParseURIInAngleBracketsL
// ----------------------------------------------------------------------------
//
void CSIPAddress::ParseURIInAngleBracketsL (const TDesC8& aValue)
	{
	__ASSERT_ALWAYS (aValue.Length() > 0, 
		User::Leave (KErrSipCodecSIPAddress));

    HBufC8* trimmedValue = aValue.AllocLC();
    TPtr8 trimmedValuePtr(trimmedValue->Des());
    trimmedValuePtr.Trim();

	TInt openingAngleBracketPos = trimmedValuePtr.Locate('<');
	if (openingAngleBracketPos != 0) 
		{
		User::Leave (KErrSipCodecSIPAddress);
		}
	
	TInt closingAngleBracketPos = trimmedValuePtr.LocateReverse('>');
	if (closingAngleBracketPos < 0 ||
		closingAngleBracketPos != trimmedValuePtr.Length()-1)
		{
		User::Leave (KErrSipCodecSIPAddress);
		}

    const TInt KURIStartPos = 1;
    const TInt KURIEndPos = trimmedValuePtr.Length()-2;
	ParseURIL (trimmedValuePtr.Mid(KURIStartPos,KURIEndPos));
	
	CleanupStack::PopAndDestroy(trimmedValue);	
	}

// ----------------------------------------------------------------------------
// CSIPAddress::ParseURIL
// ----------------------------------------------------------------------------
//
void CSIPAddress::ParseURIL (const TDesC8& aValue)
	{
	__ASSERT_ALWAYS (aValue.Length() > 0, 
		             User::Leave (KErrSipCodecURI));

	CURIContainer* uri = CURIContainer::DecodeL (aValue);
	delete iURI;
	iURI = uri;
	}

// ----------------------------------------------------------------------------
// CSIPAddress::ContainsSeparators
// ----------------------------------------------------------------------------
//
TBool CSIPAddress::ContainsSeparators(const TDesC8& aValue) const
	{
	TLex8 lex(aValue);
	TChar chr = lex.Get();
	while (chr)
		{
		if (chr == ',' || chr == '?' || chr == ';') 
            {
            return ETrue;
            }
		chr = lex.Get();
		}	
	return EFalse;
	}
	
// ----------------------------------------------------------------------------
// CSIPAddress::HasDisplayName
// ----------------------------------------------------------------------------
//	
TBool CSIPAddress::HasDisplayName() const
	{
	return (DisplayName().Length() > 0);
	}
