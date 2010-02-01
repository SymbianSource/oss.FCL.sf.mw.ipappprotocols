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
// Name          : DeleteMgr.cpp
// Part of       : SIPCommon
// Version       : SIP/3.0
//



#include "DeleteMgr.h"

// -----------------------------------------------------------------------------
// CDeleteMgr::NewL
// Start listening for requests
// -----------------------------------------------------------------------------
//
CDeleteMgr* CDeleteMgr::NewL()
	{
	CDeleteMgr* self = new (ELeave) CDeleteMgr;
	CActiveScheduler::Add(self);
	self->WaitForRequests();
	return self;
	}

// -----------------------------------------------------------------------------
// CDeleteMgr::CDeleteMgr
// -----------------------------------------------------------------------------
//
CDeleteMgr::CDeleteMgr() : CActive(EPriorityStandard)
#ifdef CPPUNIT_TEST
    //For unit tests the granularity of arrays is set to 1 to cause them to
    //allocate memory every time an item is appended to array
    , iDeleteRequests(1)    
#endif
	{
	}

// -----------------------------------------------------------------------------
// CDeleteMgr::~CDeleteMgr
// -----------------------------------------------------------------------------
//
CDeleteMgr::~CDeleteMgr()
	{
	iDeleteRequests.ResetAndDestroy();

	//Make sure CDeleteMgr isn't active anymore
	Cancel();
	}

// -----------------------------------------------------------------------------
// CDeleteMgr::WaitForRequests
// -----------------------------------------------------------------------------
//
void CDeleteMgr::WaitForRequests()
	{
	__ASSERT_DEBUG(!IsActive(),
		User::Panic(_L("CDeleteMgr::WaitForRequests() already active"),
		EDeleteMgrFncPrecondNotMet));	

	if (!IsActive())
		{
		iStatus = KRequestPending;
		SetActive();
		}
	}

// -----------------------------------------------------------------------------
// CDeleteMgr::AddDeleteRequest
// -----------------------------------------------------------------------------
//
TInt CDeleteMgr::AddDeleteRequest(const CBase* aItemToDelete)
	{
	if (!aItemToDelete)
        {
		return KErrArgument;
        }

	//It is not allowed to delete CDeleteMgr itself
	if (aItemToDelete == this)
        {
		return KErrArgument;
        }

	//Duplicate entries are not allowed
	if (iDeleteRequests.Find(aItemToDelete) != KErrNotFound)
        {
		return KErrAlreadyExists;
        }

	//CActive::iActive won't tell whether User::RequestComplete() has already
	//been called, so iStatus is inspected to see whether a previous call to
	//AddDeleteRequestL() has already called User::RequestComplete().
	if (iStatus == KRequestPending)
		{
		//Wake up to process the request
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		}
	
	return iDeleteRequests.Append(aItemToDelete);
	}

// -----------------------------------------------------------------------------
// CDeleteMgr::DoCancel
// -----------------------------------------------------------------------------
//
void CDeleteMgr::DoCancel()
	{
	//Complete the request
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrCancel);
	}

// -----------------------------------------------------------------------------
// CDeleteMgr::RunL
// Handle all requests and then start waiting for the next request
// -----------------------------------------------------------------------------
//
void CDeleteMgr::RunL()
	{
	iDeleteRequests.ResetAndDestroy();
	WaitForRequests();
	}

// -----------------------------------------------------------------------------
// CDeleteMgr::RunError
// Since RunL() won't leave this shouldn't be called.
// -----------------------------------------------------------------------------
//
TInt CDeleteMgr::RunError(TInt aError)
	{
	if (aError == KErrNoMemory)
		{
		return aError;
		}

    return KErrNone;
	}
