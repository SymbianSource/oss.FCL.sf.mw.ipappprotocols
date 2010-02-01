// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : sipprofileplugins.cpp
// Part of     : SIP / SIP Profile Agent / SIP Concrete Profile
// Implementation
// Version     : %version: 2.1.1 %
//



//  INCLUDE FILES
#include "sipprofileplugins.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPProfilePlugins::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPProfilePlugins* CSIPProfilePlugins::NewL()
	{
	CSIPProfilePlugins* self = CSIPProfilePlugins::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPProfilePlugins::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPProfilePlugins* CSIPProfilePlugins::NewLC()
	{
	CSIPProfilePlugins* self = new (ELeave) CSIPProfilePlugins();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPProfilePlugins::CSIPProfilePlugins
// -----------------------------------------------------------------------------
//
CSIPProfilePlugins::CSIPProfilePlugins()
	{
	}

// -----------------------------------------------------------------------------
// CSIPProfilePlugins::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPProfilePlugins::ConstructL()
	{
	}
		
// -----------------------------------------------------------------------------
// CSIPProfilePlugins::~CSIPProfilePlugins
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPProfilePlugins::~CSIPProfilePlugins()
	{
	iPlugins.Close();
	}


// -----------------------------------------------------------------------------
// CSIPProfilePlugins::Plugins
// -----------------------------------------------------------------------------
//
EXPORT_C const RArray<TSIPProfileTypeInfo>& CSIPProfilePlugins::Plugins() const
	{
	return iPlugins;
	}

// -----------------------------------------------------------------------------
// CSIPProfilePlugins::SetPluginsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPProfilePlugins::SetPluginsL(
    const RArray<TSIPProfileTypeInfo>& aPlugins)
	{
	iPlugins.Reset();
	for (TInt i = 0; i < aPlugins.Count(); i++)
		{
		User::LeaveIfError(iPlugins.Append(aPlugins[i]));
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfilePlugins::InternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPProfilePlugins* CSIPProfilePlugins::InternalizeL(
    RReadStream& aReadStream)
	{
	CSIPProfilePlugins* self = new (ELeave) CSIPProfilePlugins();
	CleanupStack::PushL(self);
	self->DoInternalizeL (aReadStream);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPProfilePlugins::ExternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPProfilePlugins::ExternalizeL(RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteUint32L(iPlugins.Count());
	for (TInt i = 0; i < iPlugins.Count(); i++)
		{
		aWriteStream.WriteUint8L(iPlugins[i].iSIPProfileClass);
		const TPtrC8& value = iPlugins[i].iSIPProfileName;
		aWriteStream.WriteUint32L(value.Length());
		aWriteStream.WriteL(value);
		}
	}

// -----------------------------------------------------------------------------
// CSIPProfilePlugins::ExternalizedSizeL
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CSIPProfilePlugins::ExternalizedSize() const
	{
	TInt i = 0;
	TUint size = 4; 

	for (i = 0; i < iPlugins.Count(); i++)
		{
		const TPtrC8& value = iPlugins[i].iSIPProfileName;
		size = size + 5; // 4 + 1 bytes
		size = size + value.Length();
		}

	return size;
	}
		
// -----------------------------------------------------------------------------
// CSIPProfilePlugins::DoInternalizeL
// -----------------------------------------------------------------------------
//
void CSIPProfilePlugins::DoInternalizeL(RReadStream& aReadStream)
	{
	TInt count = aReadStream.ReadUint32L();
	for (TInt i = 0; i < count; i++)
		{
		TInt type = aReadStream.ReadUint8L();
		TInt valueLen= aReadStream.ReadUint32L();
		HBufC8* valueBuf = HBufC8::NewLC(valueLen);
		TPtr8 value = valueBuf->Des();
		aReadStream.ReadL (value, valueLen);
		TSIPProfileTypeInfo info;
		info.iSIPProfileClass = (enum TSIPProfileTypeInfo::TSIPProfileClass) type;
		info.iSIPProfileName = *valueBuf;
		CleanupStack::PopAndDestroy(valueBuf);
		iPlugins.AppendL(info);
		}
	}
