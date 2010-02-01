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
// Name          : CSIPURIHeaders.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "CSIPURIHeaders.h"
#include "CSIPTokenizer.h"
#include "CSIPParam.h"
#include "sipcodecerr.h"
#include "TSIPChar.h"

// -----------------------------------------------------------------------------
// CSIPURIHeaders::DecodeL
// -----------------------------------------------------------------------------
//
CSIPURIHeaders* CSIPURIHeaders::DecodeL (const TDesC8& aValue)
	{
	__ASSERT_ALWAYS (aValue.Length() > 0, User::Leave(KErrSipCodecURIHeaders));

	CSIPURIHeaders* headers = new(ELeave)CSIPURIHeaders;
	CleanupStack::PushL (headers);
	CSIPTokenizer* tokenizer = CSIPTokenizer::NewLC(aValue,'&');
	for (TInt i=0; i < tokenizer->Tokens().Count(); i++)
		{
		CSIPParam* header = CSIPParam::DecodeL(tokenizer->Tokens()[i]);
		CleanupStack::PushL(header);
		headers->AddHeaderL(header);
		CleanupStack::Pop(header);
		}
	CleanupStack::PopAndDestroy(tokenizer);
	CleanupStack::Pop(); // headers
	return headers;
	}

// -----------------------------------------------------------------------------
// CSIPURIHeaders::NewL
// -----------------------------------------------------------------------------
//
CSIPURIHeaders* CSIPURIHeaders::NewL (const CSIPURIHeaders& aHeaders)
	{
	CSIPURIHeaders* self = CSIPURIHeaders::NewLC (aHeaders);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPURIHeaders::NewLC
// -----------------------------------------------------------------------------
//
CSIPURIHeaders* CSIPURIHeaders::NewLC (const CSIPURIHeaders& aHeaders)
	{
	CSIPURIHeaders* self = new(ELeave)CSIPURIHeaders;
	CleanupStack::PushL(self);
	self->ConstructL (aHeaders);
	return self;
	}	

// -----------------------------------------------------------------------------
// CSIPURIHeaders::CSIPURIHeaders
// -----------------------------------------------------------------------------
//
CSIPURIHeaders::CSIPURIHeaders ()
	{
	}

// -----------------------------------------------------------------------------
// CSIPURIHeaders::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPURIHeaders::ConstructL (const CSIPURIHeaders& aHeaders)
	{
	for (TInt i=0; i < aHeaders.iHeaders.Count(); i++)
		{
		CSIPParam* header = CSIPParam::NewLC(*aHeaders.iHeaders[i]);
		AddHeaderL(header);
		CleanupStack::Pop(header);
		}
	}

// -----------------------------------------------------------------------------
// CSIPURIHeaders::~CSIPURIHeaders
// -----------------------------------------------------------------------------
//
CSIPURIHeaders::~CSIPURIHeaders ()
	{
	iHeaders.ResetAndDestroy();
	}

// -----------------------------------------------------------------------------
// CSIPURIHeaders::HasHeaders
// -----------------------------------------------------------------------------
//
TBool CSIPURIHeaders::HasHeaders (const TDesC8& aName) const
	{
	for (TInt i=0; i < iHeaders.Count(); i++)
		{
		if (aName.CompareF(iHeaders[i]->Name().DesC()) == 0)
			{
			return ETrue;
			}
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPURIHeaders::HeaderValuesL
// -----------------------------------------------------------------------------
//
CPtrC8Array* CSIPURIHeaders::HeaderValuesL (const TDesC8& aName) const
	{
	CPtrC8Array* values = new(ELeave)CPtrC8Array(1);
	CleanupStack::PushL(values);
	for (TInt i=0; i < iHeaders.Count(); i++)
		{
		if (aName.CompareF(iHeaders[i]->Name().DesC()) == 0)
			{
			values->AppendL(iHeaders[i]->Value().DesC());
			}
		}
	CleanupStack::Pop(values);
	if (values->Count() == 0)
		{
		delete values;
		return NULL;
		}
	return values;
	}

// -----------------------------------------------------------------------------
// CSIPURIHeaders::AddHeaderL
// -----------------------------------------------------------------------------
//
void CSIPURIHeaders::AddHeaderL (const TDesC8& aName, const TDesC8& aValue)
	{
	CSIPParam* header = CSIPParam::NewLC(aName,aValue);
	AddHeaderL(header);
	CleanupStack::Pop(header);
	}

// -----------------------------------------------------------------------------
// CSIPURIHeaders::DeleteAll
// -----------------------------------------------------------------------------
//
void CSIPURIHeaders::DeleteAll ()
	{
	iHeaders.ResetAndDestroy();
	}

// -----------------------------------------------------------------------------
// CSIPURIHeaders::operator==
// -----------------------------------------------------------------------------
//
TBool CSIPURIHeaders::operator==(const CSIPURIHeaders& aHeaders) const
	{
	// All URI-headers must be present in both and their values must match.
	for (TInt i=0; i < iHeaders.Count(); i++)
		{
		if (!aHeaders.HasHeader(*(iHeaders[i]))) 
            {
            return EFalse;
            }
		}
	for (TInt j=0; j < aHeaders.iHeaders.Count(); j++)
		{
		if (!HasHeader(*(aHeaders.iHeaders[j]))) 
            {
            return EFalse;
            }
		}
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPURIHeaders::ToTextLC
// -----------------------------------------------------------------------------
//
HBufC8* CSIPURIHeaders::ToTextLC () const
	{
	TUint encodedLength = 0;

	RPointerArray<HBufC8> headersAsText;
	CleanupStack::PushL (TCleanupItem(ResetAndDestroy,&headersAsText));

	for (TInt i=0; i < iHeaders.Count(); i++)
		{
		HBufC8* headerAsText = iHeaders[i]->ToTextLC();
		encodedLength += headerAsText->Length();
		if (i < iHeaders.Count()-1) 
			{
			encodedLength += 1; // header separator '&'
			}
		headersAsText.AppendL(headerAsText);
		CleanupStack::Pop(headerAsText);
		}
	
	HBufC8* encodedHeaders = HBufC8::NewL (encodedLength);
	TPtr8 encodedHeadersPtr = encodedHeaders->Des();

	for (TInt j=0; j < headersAsText.Count(); j++)
		{
		encodedHeadersPtr.Append (*headersAsText[j]);
		if (j < headersAsText.Count()-1)
			{
			encodedHeadersPtr.Append('&');
			}
		}

	CleanupStack::PopAndDestroy(1); // headersAsText
	CleanupStack::PushL(encodedHeaders);
	return encodedHeaders;
	}	

// -----------------------------------------------------------------------------
// CSIPURIHeaders::AddHeaderL
// -----------------------------------------------------------------------------
//
void CSIPURIHeaders::AddHeaderL (const CSIPParam* aHeader)
	{
	CheckHeaderL (aHeader);
	User::LeaveIfError (iHeaders.Append(aHeader));
	}

// -----------------------------------------------------------------------------
// CSIPURIHeaders::CheckHeaderL
// -----------------------------------------------------------------------------
//
void CSIPURIHeaders::CheckHeaderL (const CSIPParam* aHeader) const
	{
	__ASSERT_ALWAYS (aHeader != 0, User::Leave(KErrSipCodecURIHeaders));

	if (!aHeader->HasValue())
		{
		User::Leave(KErrSipCodecURIHeaders);
		}
	if (!CheckSIPURIHeaderToken(aHeader->Name().DesC()))
		{
		User::Leave(KErrSipCodecURIHeaders);
		}
	if (!CheckSIPURIHeaderToken(aHeader->Value().DesC()))
		{
		User::Leave(KErrSipCodecURIHeaders);
		}
	}

// -----------------------------------------------------------------------------
// CSIPURIHeaders::CheckSIPURIHeaderToken
// -----------------------------------------------------------------------------
//
TBool CSIPURIHeaders::CheckSIPURIHeaderToken (const TDesC8& aHeaderToken)
	{
	if (aHeaderToken.Length() == 0) 
        {
        return EFalse;
        }

	TLex8 lex(aHeaderToken);
	TSIPChar sipChr = lex.Get();
	while (sipChr)
		{
		// unreserved / escaped / hnv-unreserved
		if (!(sipChr.IsUnreserved() || sipChr == '%' ||
			  sipChr == '[' || sipChr == ']' || sipChr == '/' || 
			  sipChr == '?' || sipChr == ':' || sipChr == '+' || 
			  sipChr == '$'))
			{			
			return EFalse;
			}
		if (sipChr == '%') // escaped: "%" HEXDIG HEXDIG
			{
			if (!lex.Get().IsHexDigit()) 
                {
                return EFalse;
                }
			if (!lex.Get().IsHexDigit()) 
                {
                return EFalse;
                }
			}
		sipChr = lex.Get();
		}
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPURIHeaders::HasHeader
// -----------------------------------------------------------------------------
//
TBool CSIPURIHeaders::HasHeader (const CSIPParam& aHeader) const
	{
	for (TInt i=0; i < iHeaders.Count(); i++)
		{
		if (aHeader == *(iHeaders[i])) 
            {
            return ETrue;
            }
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPURIHeaders::ResetAndDestroy
// -----------------------------------------------------------------------------
//
void CSIPURIHeaders::ResetAndDestroy (TAny* anArray)
	{
	(reinterpret_cast<RPointerArray<HBufC8>*> (anArray))->ResetAndDestroy();
	}
