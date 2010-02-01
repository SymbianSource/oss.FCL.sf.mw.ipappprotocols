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
// Name        : extensiondescrparam.cpp
// Part of     : SIP Profile
// implementation
// Version     : 1.0
// INCLUDE FILES
//



#include "extensiondescrparam.h"
 
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CExtensionDescrParam::NewLC
// -----------------------------------------------------------------------------
//
CExtensionDescrParam*
CExtensionDescrParam::NewLC(TUint32 aID, const TDesC8& aValue)
	{
	CExtensionDescrParam* self = new (ELeave) CExtensionDescrParam(aID);
    CleanupStack::PushL(self);
    self->ConstructL(aValue);
    return self;
	}
	
// -----------------------------------------------------------------------------
// CExtensionDescrParam::CExtensionDescrParam
// -----------------------------------------------------------------------------
//
CExtensionDescrParam::CExtensionDescrParam(TUint32 aID) :
	iID(aID),
	iValue(NULL)
	{
	}

// -----------------------------------------------------------------------------
// CExtensionDescrParam::ConstructL
// -----------------------------------------------------------------------------
//
void CExtensionDescrParam::ConstructL(const TDesC8& aValue)	
	{
	iValue = aValue.AllocL();
	}

// -----------------------------------------------------------------------------
// CExtensionDescrParam::~CExtensionDescrParam
// -----------------------------------------------------------------------------
//
CExtensionDescrParam::~CExtensionDescrParam()
	{
	delete iValue;
	}

// -----------------------------------------------------------------------------
// CExtensionDescrParam::InternalizeL
// -----------------------------------------------------------------------------
//
CExtensionDescrParam*
CExtensionDescrParam::InternalizeL(RReadStream& aReadStream)
	{
	TUint32 id = aReadStream.ReadUint32L();
	CExtensionDescrParam* self = new (ELeave) CExtensionDescrParam(id);	
	CleanupStack::PushL(self);

	TUint32 valueLength = aReadStream.ReadInt32L();
	self->iValue = HBufC8::NewL(valueLength);
	TPtr8 valuePtr(self->iValue->Des());
	aReadStream.ReadL(valuePtr, valueLength);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CExtensionDescrParam::ExternalizeL
// -----------------------------------------------------------------------------
//
void CExtensionDescrParam::ExternalizeL(RWriteStream& aWriteStream) const
	{
	__ASSERT_ALWAYS(iValue != NULL, User::Leave(KErrNotFound));

	aWriteStream.WriteUint32L(iID);
	aWriteStream.WriteInt32L(iValue->Length());
	aWriteStream.WriteL(*iValue);
	}

// -----------------------------------------------------------------------------
// CExtensionDescrParam::ExternalizedSizeL
// -----------------------------------------------------------------------------
//
TUint CExtensionDescrParam::ExternalizedSizeL() const
	{
	__ASSERT_ALWAYS(iValue != NULL, User::Leave(KErrNotFound));

	//CExtensionDescrParam::ExternalizeL writes descriptor's length as TInt32
	const TInt KLengthFieldSize = sizeof(TInt32);	
	return sizeof(iID) + KLengthFieldSize + iValue->Length();
	}

// -----------------------------------------------------------------------------
// CExtensionDescrParam::operator==
// -----------------------------------------------------------------------------
//
TBool CExtensionDescrParam::operator==(const CExtensionDescrParam& aParam) const
	{
	return (ID() == aParam.ID()) && (Value().Compare(aParam.Value()) == 0);
	}

// -----------------------------------------------------------------------------
// CExtensionDescrParam::ID
// -----------------------------------------------------------------------------
//
TUint32 CExtensionDescrParam::ID() const
	{
	return iID;
	}

// -----------------------------------------------------------------------------
// CExtensionDescrParam::Value
// -----------------------------------------------------------------------------
//		
const TDesC8& CExtensionDescrParam::Value() const
	{
	if (iValue)
		{		
		return *iValue;
		}
	return KNullDesC8;
	}
