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
// Name        : extensiondescrarrayparam.cpp
// Part of     : SIP Profile
// implementation
// Version     : 1.0
//



//  INCLUDE FILES
#include "extensiondescrarrayparam.h"
#include "sipconcreteprofile.h"
 
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CExtensionDescrArrayParam::NewLC
// -----------------------------------------------------------------------------
//
CExtensionDescrArrayParam*
CExtensionDescrArrayParam::NewLC(TUint32 aID, const MDesC8Array& aValue)
	{
	CExtensionDescrArrayParam* self =
		new (ELeave) CExtensionDescrArrayParam(aID);
    CleanupStack::PushL(self);
    self->ConstructL(aValue);	
    return self;
	}
	
// -----------------------------------------------------------------------------
// CExtensionDescrArrayParam::CExtensionDescrArrayParam
// -----------------------------------------------------------------------------
//
CExtensionDescrArrayParam::CExtensionDescrArrayParam(TUint32 aID) :
	iID(aID),
	iValue(NULL)
	{
	}

// -----------------------------------------------------------------------------
// CExtensionDescrArrayParam::ConstructL
// -----------------------------------------------------------------------------
//
void CExtensionDescrArrayParam::ConstructL(const MDesC8Array& aValue)
	{
	iValue = CSIPConcreteProfile::CopyDesArrayL(aValue);    
	}

// -----------------------------------------------------------------------------
// CExtensionDescrArrayParam::~CExtensionDescrArrayParam
// -----------------------------------------------------------------------------
//
CExtensionDescrArrayParam::~CExtensionDescrArrayParam()
	{
	if (iValue)
        {
        iValue->Reset();
        }
	delete iValue;
	}

// -----------------------------------------------------------------------------
// CExtensionDescrArrayParam::InternalizeL
// -----------------------------------------------------------------------------
//
CExtensionDescrArrayParam*
CExtensionDescrArrayParam::InternalizeL(RReadStream& aReadStream)
	{
	TUint32 id = aReadStream.ReadUint32L();
	CExtensionDescrArrayParam* self =
		new (ELeave) CExtensionDescrArrayParam(id);
	CleanupStack::PushL(self);	
	self->iValue = CSIPConcreteProfile::InternalizeDesArrayL(aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CExtensionDescrArrayParam::ExternalizeL
// -----------------------------------------------------------------------------
//
void CExtensionDescrArrayParam::ExternalizeL(RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteUint32L(iID);
	CSIPConcreteProfile::ExternalizeDesArrayL(*iValue, aWriteStream);
	}

// -----------------------------------------------------------------------------
// CExtensionDescrArrayParam::ExternalizedSizeL
// CSIPConcreteProfile::ExternalizeDesArrayL writes the amount of elements in
// the array as UInt32 and every descriptor's length as TInt32.
// -----------------------------------------------------------------------------
//
TUint CExtensionDescrArrayParam::ExternalizedSizeL() const
	{
	const TInt KElementCountSize = sizeof(TUint32);
	const TInt KLengthFieldSize = sizeof(TInt32);

	TUint size = sizeof(iID) + KElementCountSize;
	
	for (TInt i = 0; i < iValue->Count(); ++i)
		{
		const TPtrC8& value = (*iValue)[i];
		size = size + KLengthFieldSize + value.Length();
		}

	return size;
	}

// -----------------------------------------------------------------------------
// CExtensionDescrArrayParam::operator==
// Check that every descriptor element in aParam, is also present in this
// object. The elements don't need to be in the same order in the array.
// -----------------------------------------------------------------------------
//
TBool CExtensionDescrArrayParam::operator==(
	const CExtensionDescrArrayParam& aParam) const
	{
	if ( !(ID() == aParam.ID() &&
		   Value().MdcaCount() == aParam.Value().MdcaCount() ) )
		{
		return EFalse;
		}

	for (TInt i = 0; i < aParam.Value().MdcaCount(); ++i)
		{
		TInt pos(0);

		if (iValue->Find(aParam.Value().MdcaPoint(i), pos) != 0)
			{
			//Element present only in the other object
			return EFalse;
			}
		}

	return ETrue;
	}

// -----------------------------------------------------------------------------
// CExtensionDescrArrayParam::ID
// -----------------------------------------------------------------------------
//
TUint32 CExtensionDescrArrayParam::ID() const
	{
	return iID;
	}

// -----------------------------------------------------------------------------
// CExtensionDescrArrayParam::Value
// -----------------------------------------------------------------------------
//		
const MDesC8Array& CExtensionDescrArrayParam::Value() const
	{
	return *iValue;	
	}
