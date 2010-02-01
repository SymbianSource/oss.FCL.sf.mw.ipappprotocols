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
// Name        : extensionboolparam.cpp
// Part of     : SIP Profile
// implementation
// Version     : 1.0
// INCLUDE FILES
//



#include "extensionboolparam.h"
 
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TExtensionBoolParam::TExtensionBoolParam
// -----------------------------------------------------------------------------
//
TExtensionBoolParam::TExtensionBoolParam() : iID(0), iValue(EFalse)
	{	
	}
	
// -----------------------------------------------------------------------------
// TExtensionBoolParam::TExtensionBoolParam
// -----------------------------------------------------------------------------
//
TExtensionBoolParam::TExtensionBoolParam(const TExtensionBoolParam& aParam) :
	iID(aParam.ID()),
	iValue(aParam.Value())
	{		
	}

// -----------------------------------------------------------------------------
// TExtensionBoolParam::TExtensionBoolParam
// -----------------------------------------------------------------------------
//
TExtensionBoolParam::TExtensionBoolParam(TUint32 aID, TBool aValue) :
	iID(aID),
	iValue(aValue)
	{
	}

// -----------------------------------------------------------------------------
// TExtensionBoolParam::InternalizeL
// -----------------------------------------------------------------------------
//
TExtensionBoolParam TExtensionBoolParam::InternalizeL(RReadStream& aReadStream)
	{
	TUint32 id = aReadStream.ReadUint32L();
	TBool value = aReadStream.ReadUint8L();

	return TExtensionBoolParam(id, value);	
	}

// -----------------------------------------------------------------------------
// TExtensionBoolParam::ExternalizeL
// Boolean value fits in 8-bit 
// -----------------------------------------------------------------------------
//
void TExtensionBoolParam::ExternalizeL(RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteUint32L(iID);
	aWriteStream.WriteUint8L(iValue);	
	}

// -----------------------------------------------------------------------------
// TExtensionBoolParam::ExternalizedSizeL
// -----------------------------------------------------------------------------
//
TUint TExtensionBoolParam::ExternalizedSizeL() const
	{
	return sizeof(iID) + sizeof(TUint8);
	}

// -----------------------------------------------------------------------------
// TExtensionBoolParam::operator==
// -----------------------------------------------------------------------------
//
TBool TExtensionBoolParam::operator==(const TExtensionBoolParam& aParam) const
	{
	return (ID() == aParam.ID()) && (Value() == aParam.Value());
	}
		
// -----------------------------------------------------------------------------
// TExtensionBoolParam::ID
// -----------------------------------------------------------------------------
//
TUint32 TExtensionBoolParam::ID() const
	{
	return iID;
	}

// -----------------------------------------------------------------------------
// TExtensionBoolParam::Value
// -----------------------------------------------------------------------------
//		
TBool TExtensionBoolParam::Value() const
	{
	return iValue;
	}
