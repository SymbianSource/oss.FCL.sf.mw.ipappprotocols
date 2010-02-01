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
// Name          : CSIPRegistrarStore.cpp
// Part of       : Registration
// Version       : SIP/4.0 
//



#include "CSIPRegistrarStore.h"
#include "CSIPRegistrar.h"
#include "uricontainer.h"
#include "DeleteMgr.h"

// -----------------------------------------------------------------------------
// CSIPRegistrarStore::NewL
// -----------------------------------------------------------------------------
//
CSIPRegistrarStore* CSIPRegistrarStore::NewL()
	{
	CSIPRegistrarStore* self = CSIPRegistrarStore::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrarStore::NewLC
// -----------------------------------------------------------------------------
//
CSIPRegistrarStore* CSIPRegistrarStore::NewLC()
	{
    CSIPRegistrarStore* self = new (ELeave) CSIPRegistrarStore();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrarStore::CSIPRegistrarStore
// -----------------------------------------------------------------------------
//
CSIPRegistrarStore::CSIPRegistrarStore()
	{
	}

// -----------------------------------------------------------------------------
// CSIPRegistrarStore::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPRegistrarStore::ConstructL()
	{
	iDeleteMgr = CDeleteMgr::NewL();
	}

// -----------------------------------------------------------------------------
// CSIPRegistrarStore::~CSIPRegistrarStore
// -----------------------------------------------------------------------------
//
CSIPRegistrarStore::~CSIPRegistrarStore()
	{
	iRegistrars.ResetAndDestroy();
	delete iDeleteMgr;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrarStore::CreateRegistrarL
// -----------------------------------------------------------------------------
//
CSIPRegistrar* CSIPRegistrarStore::CreateRegistrarL(
	const CURIContainer&        aRegistrarName, 
	const TDesC8&         aCallId,
	TUint                 aCSeq,
	const TRegistrationId aRegistrationId)
	{
	CSIPRegistrar* registrar = CSIPRegistrar::NewLC(
		aRegistrarName, aCallId, aCSeq, aRegistrationId, *this);
	iRegistrars.AppendL(registrar);
	CleanupStack::Pop(registrar);
	return registrar;
	}

// -----------------------------------------------------------------------------
// CSIPRegistrarStore::FindRegistrar
// -----------------------------------------------------------------------------
//
CSIPRegistrar* CSIPRegistrarStore::FindRegistrar(
	const CURIContainer& aRegistrarName)
	{
	CSIPRegistrar* returnValue = NULL;

	for (TInt i=iRegistrars.Count()-1; (i>=0 && !returnValue); i--)
		{
		if ((iRegistrars[i])->Registrar() == aRegistrarName)
			{
			returnValue = iRegistrars[i];
			}
		}

	return returnValue;
	}	

// -----------------------------------------------------------------------------
// CSIPRegistrarStore::RemoveRegistrar
// -----------------------------------------------------------------------------
//
TInt CSIPRegistrarStore::RemoveRegistrar(CSIPRegistrar* aRegistrar)
	{
	TInt err   = KErrGeneral;
	TInt index = iRegistrars.Find(aRegistrar);

	if (index != KErrNotFound)
		{
		err = iDeleteMgr->AddDeleteRequest(aRegistrar);

		if (err == KErrNone)
			{
			// remove it from registrars array
			iRegistrars.Remove(index);
			}
		}

	return err;
	}

