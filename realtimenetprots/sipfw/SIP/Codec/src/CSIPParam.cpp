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
// Name          : CSIPParam.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//



#include "CSIPParam.h"
#include "SIPSyntaxCheck.h"
#include "sipstrings.h"
#include "sipcodecerr.h"


// -----------------------------------------------------------------------------
// CSIPParam::DecodeL
// -----------------------------------------------------------------------------
//
CSIPParam* CSIPParam::DecodeL(const TDesC8& aValue)
	{
	TPtrC8 pname;
	TPtrC8 pvalue;
	TBool hasValue = ParseL(aValue,pname,pvalue);
	if (hasValue)
		{
		return CSIPParam::NewL(pname,pvalue);
		}
	return CSIPParam::NewL(pname);
	}
	
// -----------------------------------------------------------------------------
// CSIPParam::ParseL
// -----------------------------------------------------------------------------
//	
TBool CSIPParam::ParseL(const TDesC8& aInput, TPtrC8& aName, TPtrC8& aValue)
    {
	__ASSERT_ALWAYS (aInput.Length() > 0, User::Leave (KErrSipCodecAnyParam));
	TInt equalsPos = aInput.Locate('=');
	if (equalsPos == 0 || equalsPos == aInput.Length()-1) 
		{
		User::Leave (KErrSipCodecAnyParam);
		}
	TBool hasValue = EFalse;	
	if (equalsPos > 0 && !SIPSyntaxCheck::StartsAndEndsWithQuotes(aInput))
		{
		aName.Set(aInput.Left(equalsPos));
		aValue.Set(aInput.Mid(equalsPos+1));
		hasValue = ETrue;
		}
	else
        {
	    aName.Set(aInput);    
        }
	return hasValue;    
    }

// -----------------------------------------------------------------------------
// CSIPParam::NewL
// -----------------------------------------------------------------------------
//
CSIPParam* CSIPParam::NewL(RStringF aName)
	{
	CSIPParam* self = CSIPParam::NewLC(aName);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPParam::NewLC
// -----------------------------------------------------------------------------
//
CSIPParam* CSIPParam::NewLC(RStringF aName)
	{
	CSIPParam* self = new(ELeave)CSIPParam;
	CleanupStack::PushL(self);
	self->ConstructL(aName);
	return self;
	}
	
// -----------------------------------------------------------------------------
// CSIPParam::NewL
// -----------------------------------------------------------------------------
//
CSIPParam* CSIPParam::NewL(const TDesC8& aName)
	{
	CSIPParam* self = CSIPParam::NewLC(aName);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPParam::NewLC
// -----------------------------------------------------------------------------
//
CSIPParam* CSIPParam::NewLC(const TDesC8& aName)
	{
	CSIPParam* self = new(ELeave)CSIPParam;
	CleanupStack::PushL(self);
	self->ConstructL(aName);
	return self;
	}	

// -----------------------------------------------------------------------------
// CSIPParam::NewL
// -----------------------------------------------------------------------------
//
CSIPParam* CSIPParam::NewL(RStringF aName, RStringF aValue)
	{
	CSIPParam* self = CSIPParam::NewLC(aName,aValue);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPParam::NewLC
// -----------------------------------------------------------------------------
//
CSIPParam* CSIPParam::NewLC(RStringF aName, RStringF aValue)
	{
	CSIPParam* self = new(ELeave)CSIPParam;
	CleanupStack::PushL(self);
	self->ConstructL(aName,aValue);
	return self;
	}
	
// -----------------------------------------------------------------------------
// CSIPParam::NewL
// -----------------------------------------------------------------------------
//
CSIPParam* CSIPParam::NewL(RStringF aName, const TDesC8& aValue)
	{
	CSIPParam* self = CSIPParam::NewLC(aName,aValue);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPParam::NewLC
// -----------------------------------------------------------------------------
//
CSIPParam* CSIPParam::NewLC(RStringF aName, const TDesC8& aValue)
	{
	CSIPParam* self = new(ELeave)CSIPParam;
	CleanupStack::PushL(self);
	self->ConstructL(aName,aValue);
	return self;
	}	
	
// -----------------------------------------------------------------------------
// CSIPParam::NewL
// -----------------------------------------------------------------------------
//
CSIPParam* CSIPParam::NewL(const TDesC8& aName, const TDesC8& aValue)
	{
	CSIPParam* self = CSIPParam::NewLC(aName,aValue);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPParam::NewLC
// -----------------------------------------------------------------------------
//
CSIPParam* CSIPParam::NewLC(const TDesC8& aName, const TDesC8& aValue)
	{
	CSIPParam* self = new(ELeave)CSIPParam;
	CleanupStack::PushL(self);
	self->ConstructL(aName,aValue);
	return self;
	}	

// -----------------------------------------------------------------------------
// CSIPParam::NewL
// -----------------------------------------------------------------------------
//
CSIPParam* CSIPParam::NewL(const CSIPParam& aSIPParam)
	{
	CSIPParam* self = CSIPParam::NewLC(aSIPParam);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPParam::NewLC
// -----------------------------------------------------------------------------
//
CSIPParam* CSIPParam::NewLC(const CSIPParam& aSIPParam)
	{
	CSIPParam* self = new(ELeave)CSIPParam(aSIPParam);
	CleanupStack::PushL(self);
	self->ConstructL(aSIPParam);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPParam::CSIPParam
// -----------------------------------------------------------------------------
//
CSIPParam::CSIPParam()
	{
	}
	
// -----------------------------------------------------------------------------
// CSIPParam::CSIPParam
// -----------------------------------------------------------------------------
//
CSIPParam::CSIPParam(const CSIPParam& aSIPParam) : CBase()
	{
	iName = aSIPParam.Name().Copy();
	iValue = aSIPParam.Value().Copy();
	iHasValue = aSIPParam.HasValue();
	iAddQuotesWhenEncoding = aSIPParam.iAddQuotesWhenEncoding;
	}

// -----------------------------------------------------------------------------
// CSIPParam::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPParam::ConstructL(RStringF aName)
	{
	iDesValue = KNullDesC8().AllocL();
	SetL(iName,aName.DesC());
	}
	
// -----------------------------------------------------------------------------
// CSIPParam::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPParam::ConstructL(const TDesC8& aName)
	{
	iDesValue = KNullDesC8().AllocL();
	SetL(iName,aName);
	}	

// -----------------------------------------------------------------------------
// CSIPParam::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPParam::ConstructL(RStringF aName, RStringF aValue)
	{
	iDesValue = KNullDesC8().AllocL();
	SetL(iName,aName.DesC());
	SetL(iValue,aValue.DesC(),EFalse);
	iHasValue = ETrue;
	}
	
// -----------------------------------------------------------------------------
// CSIPParam::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPParam::ConstructL(RStringF aName, const TDesC8& aValue)
	{
	iDesValue = KNullDesC8().AllocL();
	SetL(iName,aName.DesC());
	SetL(iValue,aValue,EFalse);
	iHasValue = ETrue;
	}
	
// -----------------------------------------------------------------------------
// CSIPParam::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPParam::ConstructL(const TDesC8& aName, const TDesC8& aValue)
	{
	iDesValue = KNullDesC8().AllocL();
	SetL(iName,aName);
	SetL(iValue,aValue,EFalse);
	iHasValue = ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPParam::ConstructL
// -----------------------------------------------------------------------------
//	
void CSIPParam::ConstructL(const CSIPParam& aSIPParam)
    {
    iDesValue = aSIPParam.iDesValue->AllocL();
    }

// -----------------------------------------------------------------------------
// CSIPParam::~CSIPParam
// -----------------------------------------------------------------------------
//
CSIPParam::~CSIPParam()
	{
	iName.Close();
	iValue.Close();
	delete iDesValue;
	}

// -----------------------------------------------------------------------------
// CSIPParam::Name
// -----------------------------------------------------------------------------
//
RStringF CSIPParam::Name() const
	{
	return iName;
	}

// -----------------------------------------------------------------------------
// CSIPParam::HasValue
// -----------------------------------------------------------------------------
//
TBool CSIPParam::HasValue() const
	{
	return iHasValue;
	}

// -----------------------------------------------------------------------------
// CSIPParam::Value
// -----------------------------------------------------------------------------
//
RStringF CSIPParam::Value() const
	{
	return iValue;
	}

// -----------------------------------------------------------------------------
// CSIPParam::DesValue
// -----------------------------------------------------------------------------
//
const TDesC8& CSIPParam::DesValue() const
    {
    return *iDesValue;
    }

// -----------------------------------------------------------------------------
// CSIPParam::SetValueL
// -----------------------------------------------------------------------------
//	
void CSIPParam::SetValueL(const TDesC8& aValue, TBool aAddQuotesWhenEncoding)
    {
	HBufC8* tmp = aValue.AllocLC();
	if (!aAddQuotesWhenEncoding)
	    {
	    tmp->Des().Trim();
	    }
	RStringF tmpStr = SIPStrings::Pool().OpenFStringL(*tmp);
	CleanupStack::Pop(tmp);
    iValue.Close();
    iValue = tmpStr;
    iHasValue = ETrue;
    iAddQuotesWhenEncoding = aAddQuotesWhenEncoding;
    if (aAddQuotesWhenEncoding)
        {
        delete iDesValue;
        iDesValue = tmp;
        tmp = NULL;
        }
 	delete tmp;
    }

// -----------------------------------------------------------------------------
// CSIPParam::operator==
// -----------------------------------------------------------------------------
//
TBool CSIPParam::operator==(const CSIPParam& aSIPParam) const
	{
	if (aSIPParam.HasValue() != HasValue()) 
		{
		return EFalse;
		}
	if (iName != aSIPParam.Name()) 
		{
		return EFalse;
		}
	if (HasValue() && iValue != aSIPParam.Value()) 
		{
		return EFalse;
		}
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPParam::ToTextLC
// -----------------------------------------------------------------------------
//
HBufC8* CSIPParam::ToTextLC () const
	{
	TPtrC8 name = iName.DesC();
	TPtrC8 value = iValue.DesC();
	TUint length = name.Length();
	if (HasValue())
		{
		length += 1; // '='
		if (iAddQuotesWhenEncoding)
		    {
		    const TInt KQuotesLength = 2;
		    length += KQuotesLength;
		    length += iDesValue->Length();
		    }
		else
		    {
		    length += value.Length();
		    }
		}
	HBufC8* result = HBufC8::NewLC(length);
	TPtr8 resultPtr(result->Des());
	resultPtr.Append(name);
	if (HasValue())
		{
		resultPtr.Append('=');
		if (iAddQuotesWhenEncoding)
		    {
		    resultPtr.Append('"');
		    resultPtr.Append(*iDesValue);
		    resultPtr.Append('"');
		    }
		else
		    {
		    resultPtr.Append(value);
		    }		
		}
	return result;
	}

// -----------------------------------------------------------------------------
// CSIPParam::InternalizeL
// -----------------------------------------------------------------------------
//
CSIPParam* CSIPParam::InternalizeL (RReadStream& aReadStream)
	{
	CSIPParam* self = new(ELeave)CSIPParam;
	CleanupStack::PushL(self);
	self->DoInternalizeL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPParam::ExternalizeL
// -----------------------------------------------------------------------------
//
void CSIPParam::ExternalizeL(RWriteStream& aWriteStream) const
	{
	TPtrC8 name(iName.DesC());
	aWriteStream.WriteUint32L(name.Length());
	aWriteStream.WriteL(name);
	TPtrC8 value(iValue.DesC());
	aWriteStream.WriteUint32L(value.Length());
	if (value.Length() > 0)
		{
		aWriteStream.WriteL(value);
		}
    aWriteStream.WriteUint8L(iHasValue);
    aWriteStream.WriteUint8L(iAddQuotesWhenEncoding);
    TInt desValueLength = iDesValue->Length();
    aWriteStream.WriteUint32L(desValueLength);
    if (desValueLength > 0)
        {
        aWriteStream.WriteL(*iDesValue);
        }
	}

// -----------------------------------------------------------------------------
// CSIPParam::DoInternalizeL
// -----------------------------------------------------------------------------
//
void CSIPParam::DoInternalizeL(RReadStream& aReadStream)
	{
	TUint32 nameLength = aReadStream.ReadUint32L();
	HBufC8* name = HBufC8::NewLC(nameLength);
	TPtr8 namePtr(name->Des());
	aReadStream.ReadL(namePtr, nameLength);
	iName = SIPStrings::Pool().OpenFStringL(namePtr);
	CleanupStack::PopAndDestroy(name);
	
	TUint32 valueLength = aReadStream.ReadUint32L();
	HBufC8* value = HBufC8::NewLC(valueLength);
	if (valueLength > 0)
		{
		TPtr8 valuePtr(value->Des());
		aReadStream.ReadL(valuePtr, valueLength);
		iValue = SIPStrings::Pool().OpenFStringL(valuePtr);
		}
	CleanupStack::PopAndDestroy(value);
	iHasValue = (aReadStream.ReadUint8L() != 0);
	iAddQuotesWhenEncoding = (aReadStream.ReadUint8L() != 0);
	TUint32 desValueLength = aReadStream.ReadUint32L();
	iDesValue = HBufC8::NewL(desValueLength);
	if (desValueLength > 0)
	    {
		TPtr8 desValuePtr(iDesValue->Des());
		aReadStream.ReadL(desValuePtr, desValueLength);
	    }
	}
	
// -----------------------------------------------------------------------------
// CSIPParam::SetL
// -----------------------------------------------------------------------------
//	
void CSIPParam::SetL(RStringF& aString, const TDesC8& aDes, TBool aCheckLength)
	{
	HBufC8* tmp = aDes.AllocLC();
	tmp->Des().Trim();
	if (aCheckLength && tmp->Length() == 0)
	    {
	    User::Leave(KErrSipCodecAnyParam);
	    }
	aString = SIPStrings::Pool().OpenFStringL(*tmp);
	CleanupStack::PopAndDestroy(tmp);	
	}
