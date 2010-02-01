// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CSIPHeaderBase.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "sipheaderbase.h"
#include "SIPHeaderLookup.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "_sipcodecdefs.h"

_LIT8(KColonAndSpace, ": ");
_LIT8(KExpiresHeaderNameDes, "Expires");
_LIT8(KRequireHeaderNameDes, "Require");

// -----------------------------------------------------------------------------
// CSIPHeaderBase::CSIPHeaderBase
// -----------------------------------------------------------------------------
//
CSIPHeaderBase::CSIPHeaderBase()
	{
	}

// -----------------------------------------------------------------------------
// CSIPHeaderBase::~CSIPHeaderBase
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase::~CSIPHeaderBase()
	{
	}

// -----------------------------------------------------------------------------
// CSIPHeaderBase::HasCompactName
// -----------------------------------------------------------------------------
//
TBool CSIPHeaderBase::HasCompactName() const
	{
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPHeaderBase::CompactName
// -----------------------------------------------------------------------------
//
RStringF CSIPHeaderBase::CompactName() const
	{
	return SIPStrings::StringF(SipStrConsts::EEmpty);
	}

// -----------------------------------------------------------------------------
// CSIPHeaderBase::IsExtensionHeader
// -----------------------------------------------------------------------------
//
TBool CSIPHeaderBase::IsExtensionHeader() const
	{
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPHeaderBase::EncodeMultipleToOneLine
// -----------------------------------------------------------------------------
//
TBool CSIPHeaderBase::EncodeMultipleToOneLine() const
	{
	return ETrue;		
	}

// -----------------------------------------------------------------------------
// CSIPHeaderBase::MoreThanOneAllowed
// -----------------------------------------------------------------------------
//
TBool CSIPHeaderBase::MoreThanOneAllowed() const
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSIPHeaderBase::ToTextL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CSIPHeaderBase::ToTextL() const
	{
	TPtrC8 fullname(Name().DesC());
    // Because SIP Codec's string table has entry "expires" before "Expires",
    // and entry "require" before "Require",
    // literal "Expires" and "Require" must be used here. 
    // Changing the order of string table entries would have caused a SC break.	
	if (Name() == SIPStrings::StringF(SipStrConsts::EExpiresHeader))
	    {
        fullname.Set(KExpiresHeaderNameDes);
	    }
	else if (Name() == SIPStrings::StringF(SipStrConsts::ERequireHeader))
	    {
	    fullname.Set(KRequireHeaderNameDes);
	    }
	else
	    {
	    }
	TUint headerLength = fullname.Length();
	headerLength += KColonAndSpace().Length();
	HBufC8* encodedHeaderValue = ToTextValueL();
	headerLength += encodedHeaderValue->Length();

	CleanupStack::PushL (encodedHeaderValue);

	HBufC8* encodedHeader = HBufC8::NewL(headerLength);
	TPtr8 encodedHeaderPtr = encodedHeader->Des(); 
	encodedHeaderPtr.Append(fullname);
	encodedHeaderPtr.Append(KColonAndSpace);
	encodedHeaderPtr.Append(*encodedHeaderValue);
	
	CleanupStack::PopAndDestroy(encodedHeaderValue);

	return encodedHeader;
	}

// -----------------------------------------------------------------------------
// CSIPHeaderBase::ToTextLC
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CSIPHeaderBase::ToTextLC() const
	{
    HBufC8* encodedHeader = ToTextL();
    CleanupStack::PushL (encodedHeader);
    return encodedHeader;
	}

// -----------------------------------------------------------------------------
// CSIPHeaderBase::ToTextValueLC
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CSIPHeaderBase::ToTextValueLC() const
    {
    HBufC8* encodedHeaderValue = ToTextValueL();
    CleanupStack::PushL (encodedHeaderValue);
    return encodedHeaderValue;
    }

// -----------------------------------------------------------------------------
// CSIPHeaderBase::ExternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPHeaderBase::ExternalizeL(
    RWriteStream& aWriteStream,
    TBool aAddName) const
	{
	if (ExternalizeSupported())
		{
        if (aAddName)
            {
		    ExternalizeNameL (aWriteStream);
            }
		ExternalizeValueL (aWriteStream);
		}
	}

// -----------------------------------------------------------------------------
// CSIPHeaderBase::ExternalizeSupported
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPHeaderBase::ExternalizeSupported() const
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPHeaderBase::PushLC
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPHeaderBase::PushLC(RPointerArray<CSIPHeaderBase>* aArray)
    {
	CleanupStack::PushL(TCleanupItem(ResetAndDestroy,aArray));
    }

// -----------------------------------------------------------------------------
// CSIPHeaderBase::ExternalizeNameL
// -----------------------------------------------------------------------------
//
void CSIPHeaderBase::ExternalizeNameL (RWriteStream& aWriteStream) const
	{
	TPtrC8 fullname(Name().DesC());
	aWriteStream.WriteUint32L (fullname.Length());
	aWriteStream.WriteL (fullname);
	}

// -----------------------------------------------------------------------------
// CSIPHeaderBase::ExternalizeValueL
// -----------------------------------------------------------------------------
//
void 
CSIPHeaderBase::ExternalizeValueL(RWriteStream& /*aWriteStream*/) const
	{
	// default implementation empty
	}

// -----------------------------------------------------------------------------
// CSIPHeaderBase::ResetAndDestroy
// -----------------------------------------------------------------------------
//
void CSIPHeaderBase::ResetAndDestroy(TAny* anArray)
	{
    RPointerArray<CSIPHeaderBase>* array =
        reinterpret_cast<RPointerArray<CSIPHeaderBase>*>(anArray);
    if (array)
        {
        array->ResetAndDestroy();
        }
	}
