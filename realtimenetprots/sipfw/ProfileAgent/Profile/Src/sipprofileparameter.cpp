// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : sipprofileparameter
// Part of     : SIP Profile
// implementation
// Version     : 1.0
// INCLUDE FILES
//



#include "sipprofileparameter.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPProfileParameter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPProfileParameter* CSIPProfileParameter::NewL(
    const TDesC8& aKey,
    const TDesC8& aValue)
	{
	CSIPProfileParameter* self = CSIPProfileParameter::NewLC(aKey, aValue);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPProfileParameter::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPProfileParameter* CSIPProfileParameter::NewLC(
    const TDesC8& aKey,
    const TDesC8& aValue)
	{
	CSIPProfileParameter* self = new (ELeave) CSIPProfileParameter();
	CleanupStack::PushL(self);
	self->ConstructL(aKey, aValue);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPProfileParameter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPProfileParameter* CSIPProfileParameter::NewL(const TDesC8& aKey)
	{
	CSIPProfileParameter* self = CSIPProfileParameter::NewLC(aKey);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPProfileParameter::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPProfileParameter* CSIPProfileParameter::NewLC(const TDesC8& aKey)
	{
	CSIPProfileParameter* self = new (ELeave) CSIPProfileParameter();
	CleanupStack::PushL(self);
	self->ConstructL(aKey);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPProfileParameter::~CSIPProfileParameter
// -----------------------------------------------------------------------------
//
CSIPProfileParameter::~CSIPProfileParameter()
	{
	delete iParameterKey;
	iParameterKey = 0;
	delete iParameterValue;
	iParameterValue = 0;
	delete iEmpty;
	iEmpty = 0;
	}

// -----------------------------------------------------------------------------
// CSIPProfileParameter::Key
// -----------------------------------------------------------------------------
//
const TDesC8& CSIPProfileParameter::Key() const
	{
	return *iParameterKey;
	}

// -----------------------------------------------------------------------------
// CSIPProfileParameter::ValueL
// -----------------------------------------------------------------------------
//
const TDesC8& CSIPProfileParameter::ValueL() const
	{
	__ASSERT_ALWAYS (iParameterValue != 0, User::Leave (KErrNotFound));
	return *iParameterValue;
	}

// -----------------------------------------------------------------------------
// CSIPProfileParameter::Value
// -----------------------------------------------------------------------------
//
const TDesC8& CSIPProfileParameter::Value() const
	{
	if (iParameterValue)
		{
		return *iParameterValue;
		}
	else
		{
		return *iEmpty;
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfileParameter::InternalizeL
// -----------------------------------------------------------------------------
//
CSIPProfileParameter* CSIPProfileParameter::InternalizeL(
    RReadStream& aReadStream)
	{
	CSIPProfileParameter* self = new (ELeave) CSIPProfileParameter();
	CleanupStack::PushL(self);
	self->DoInternalizeL (aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPProfileParameter::ExternalizeL
// -----------------------------------------------------------------------------
//
void CSIPProfileParameter::ExternalizeL(RWriteStream& aWriteStream) const
	{
	__ASSERT_ALWAYS (iParameterKey != 0, User::Leave (KErrArgument));
	__ASSERT_ALWAYS (iParameterValue != 0, User::Leave (KErrArgument));

	aWriteStream.WriteInt32L(iParameterKey->Length());
	aWriteStream.WriteL(*iParameterKey);
	aWriteStream.WriteInt32L(iParameterValue->Length());
	aWriteStream.WriteL(*iParameterValue);
	}

// -----------------------------------------------------------------------------
// CSIPProfileParameter::ExternalizedSizeL
// -----------------------------------------------------------------------------
//
TUint CSIPProfileParameter::ExternalizedSizeL() const
	{
	__ASSERT_ALWAYS (iParameterKey != 0, User::Leave (KErrArgument));
	__ASSERT_ALWAYS (iParameterValue != 0, User::Leave (KErrArgument));

	TUint size = 8;
	size = size + iParameterKey->Length();
	size = size + iParameterValue->Length();

	return size;
	}

// -----------------------------------------------------------------------------
// CSIPProfileParameter::operator==
// -----------------------------------------------------------------------------
//
TBool CSIPProfileParameter::operator==(
    const CSIPProfileParameter& aParameter) const 
	{
	if (iParameterKey->Compare(aParameter.Key()) != 0)
		{
		return EFalse;
		}

	if (iParameterValue != 0)
		{
		if (iParameterValue->Compare(aParameter.Value()) != 0)
			{
			return EFalse;
			}
		}
	else
		{
		if (aParameter.Value().Length() > 0)
			{
			return EFalse;
			}
		}
	return ETrue;
    }

// -----------------------------------------------------------------------------
// CSIPProfileParameter::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPProfileParameter::ConstructL(const TDesC8& aKey, const TDesC8& aValue)
	{
	iEmpty = HBufC8::NewL(0);
	iParameterKey = aKey.AllocL();
	iParameterKey->Des().Trim();
	if (iParameterKey->Length() == 0) User::Leave(KErrArgument);

	iParameterValue = aValue.AllocL();
	iParameterValue->Des().Trim();
	}

// -----------------------------------------------------------------------------
// CSIPProfileParameter::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPProfileParameter::ConstructL(const TDesC8& aKey)
	{
	iEmpty = HBufC8::NewL(0);
	iParameterKey = aKey.AllocL();
	iParameterKey->Des().Trim();
	if (iParameterKey->Length() == 0) User::Leave(KErrArgument);
	}

// -----------------------------------------------------------------------------
// CSIPProfileParameter::CSIPProfileParameter
// -----------------------------------------------------------------------------
//
CSIPProfileParameter::CSIPProfileParameter()
	{
	}

// -----------------------------------------------------------------------------
// CSIPProfileParameter::DoInternalizeL
// -----------------------------------------------------------------------------
//
void CSIPProfileParameter::DoInternalizeL(RReadStream& aReadStream)
	{
	TInt32 keyLength = aReadStream.ReadInt32L();
	__ASSERT_ALWAYS (keyLength >= 0, User::Leave (KErrCorrupt));
	__ASSERT_ALWAYS (keyLength < KMaxTInt/2, User::Leave (KErrCorrupt));
	iParameterKey = HBufC8::NewL (keyLength);
	TPtr8 key(iParameterKey->Des());
	aReadStream.ReadL (key, keyLength);

	TInt32 valueLength = aReadStream.ReadInt32L();
	__ASSERT_ALWAYS (valueLength >= 0, User::Leave (KErrCorrupt));
	__ASSERT_ALWAYS (valueLength < KMaxTInt/2, User::Leave (KErrCorrupt));
	iParameterValue = HBufC8::NewL (valueLength);
	TPtr8 value(iParameterValue->Des());
	aReadStream.ReadL (value, valueLength);
	}
