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
// Name        : extensionintparam.cpp
// Part of     : SIP Profile
// implementation
// Version     : 1.0
// INCLUDE FILES
//



#include "extensionintparam.h"
 
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TExtensionIntParam::TExtensionIntParam
// -----------------------------------------------------------------------------
//
TExtensionIntParam::TExtensionIntParam() : iID(0), iValue(0)
	{	
	}
	
// -----------------------------------------------------------------------------
// TExtensionIntParam::TExtensionIntParam
// -----------------------------------------------------------------------------
//
TExtensionIntParam::TExtensionIntParam(const TExtensionIntParam& aParam) :
	iID(aParam.ID()),
	iValue(aParam.Value())
	{		
	}

// -----------------------------------------------------------------------------
// TExtensionIntParam::TExtensionIntParam
// -----------------------------------------------------------------------------
//
TExtensionIntParam::TExtensionIntParam(TUint32 aID, TUint32 aValue) :
	iID(aID),
	iValue(aValue)
	{
	}

// -----------------------------------------------------------------------------
// TExtensionIntParam::InternalizeL
// -----------------------------------------------------------------------------
//
TExtensionIntParam TExtensionIntParam::InternalizeL(RReadStream& aReadStream)
	{
	TUint32 id = aReadStream.ReadUint32L();
	TUint32 value = aReadStream.ReadUint32L();

	return TExtensionIntParam(id, value);
	}

// -----------------------------------------------------------------------------
// TExtensionIntParam::ExternalizeL
// -----------------------------------------------------------------------------
//
void TExtensionIntParam::ExternalizeL(RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteUint32L(iID);
	aWriteStream.WriteUint32L(iValue);	
	}

// -----------------------------------------------------------------------------
// TExtensionIntParam::ExternalizedSizeL
// -----------------------------------------------------------------------------
//
TUint TExtensionIntParam::ExternalizedSizeL() const
	{
	return sizeof(iID) + sizeof(iValue);
	}

// -----------------------------------------------------------------------------
// TExtensionIntParam::operator==
// -----------------------------------------------------------------------------
//
TBool TExtensionIntParam::operator==(const TExtensionIntParam& aParam) const
	{
	return (ID() == aParam.ID()) && (Value() == aParam.Value());
	}
		
// -----------------------------------------------------------------------------
// TExtensionIntParam::ID
// -----------------------------------------------------------------------------
//
TUint32 TExtensionIntParam::ID() const
	{
	return iID;
	}

// -----------------------------------------------------------------------------
// TExtensionIntParam::Value
// -----------------------------------------------------------------------------
//		
TUint32 TExtensionIntParam::Value() const
	{
	return iValue;
	}
