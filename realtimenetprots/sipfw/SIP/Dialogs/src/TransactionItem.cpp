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
// Name          : TransactionItem.cpp
// Part of       : SIPDialogs
// Version       : SIP/4.0 
//



#include "TransactionItem.h"
#include "MTransactionUser.h"
#include "TSIPTransportParams.h"


// -----------------------------------------------------------------------------
// CTransactionItem::NewL
// -----------------------------------------------------------------------------
//
CTransactionItem* CTransactionItem::NewL (MTransactionUser& aTU,
                                          MTransactionOwner* aTransactionOwner,
                                          TTransactionId aTransactionId)
    {
	CTransactionItem* self = 
	    CTransactionItem::NewLC(aTU,aTransactionOwner,aTransactionId);
    CleanupStack::Pop(self);
	return self;
    }

// -----------------------------------------------------------------------------
// CTransactionItem::NewLC
// -----------------------------------------------------------------------------
//
CTransactionItem* CTransactionItem::NewLC (MTransactionUser& aTU,
                                           MTransactionOwner* aTransactionOwner,
                                           TTransactionId aTransactionId)
    {
	CTransactionItem* self = 
	    new(ELeave)CTransactionItem(aTU,aTransactionOwner,aTransactionId);
	CleanupStack::PushL(self);
	return self;
    }

// -----------------------------------------------------------------------------
// CTransactionItem::NewL
// -----------------------------------------------------------------------------
//
CTransactionItem* CTransactionItem::NewL (MTransactionUser& aTU,
                                          MTransactionOwner* aTransactionOwner,
                                          RStringF aMethod)
    {
	CTransactionItem* self = 
	    CTransactionItem::NewLC(aTU,aTransactionOwner,aMethod);
    CleanupStack::Pop(self);
	return self;
    }

// -----------------------------------------------------------------------------
// CTransactionItem::NewLC
// -----------------------------------------------------------------------------
//
CTransactionItem* CTransactionItem::NewLC (MTransactionUser& aTU,
                                           MTransactionOwner* aTransactionOwner,
                                           RStringF aMethod)
    {
	CTransactionItem* self = 
	    new(ELeave)CTransactionItem(
	        aTU,aTransactionOwner,KEmptyTransactionId,aMethod);
	CleanupStack::PushL(self);
	return self;
    }

// -----------------------------------------------------------------------------
// CTransactionItem::NewL
// -----------------------------------------------------------------------------
//
CTransactionItem* CTransactionItem::NewL (MTransactionUser& aTU,
                                          MTransactionOwner* aTransactionOwner,
                                          TTransactionId aTransactionId,
                                          RStringF aMethod)
    {
	CTransactionItem* self = 
	    CTransactionItem::NewLC(aTU,aTransactionOwner,aTransactionId,aMethod);
    CleanupStack::Pop(self);
	return self;
    }

// -----------------------------------------------------------------------------
// CTransactionItem::NewLC
// -----------------------------------------------------------------------------
//
CTransactionItem* CTransactionItem::NewLC (MTransactionUser& aTU,
                                           MTransactionOwner* aTransactionOwner,
                                           TTransactionId aTransactionId,
                                           RStringF aMethod)
    {
	CTransactionItem* self = 
	    new(ELeave)CTransactionItem(
	        aTU,aTransactionOwner,aTransactionId,aMethod);
	CleanupStack::PushL(self);
	return self;
    }

// -----------------------------------------------------------------------------
// CTransactionItem::NewL
// -----------------------------------------------------------------------------
//
CTransactionItem* CTransactionItem::NewL (MTransactionUser& aTU,
                                          MTransactionOwner* aTransactionOwner,
                                          const CTransactionItemBase& aItem)
    {
    CTransactionItem* self = 
        CTransactionItem::NewLC(aTU, aTransactionOwner, aItem);
	CleanupStack::Pop(self);
	return self;
    }

// -----------------------------------------------------------------------------
// CTransactionItem::NewLC
// -----------------------------------------------------------------------------
//
CTransactionItem* CTransactionItem::NewLC (MTransactionUser& aTU,
                                           MTransactionOwner* aTransactionOwner,
                                           const CTransactionItemBase& aItem)
    {
	CTransactionItem* self = 
	    new(ELeave)CTransactionItem(aTU,aTransactionOwner,aItem);
	CleanupStack::PushL(self);
	return self;
    }

// -----------------------------------------------------------------------------
// CTransactionItem::CTransactionItem
// -----------------------------------------------------------------------------
//
CTransactionItem::CTransactionItem (MTransactionUser& aTU,
                                    MTransactionOwner* aTransactionOwner,
                                    const CTransactionItemBase& aItem)
 : iTU(aTU),
   iTransactionOwner(aTransactionOwner)
	{
	Copy(aItem);
	}

// -----------------------------------------------------------------------------
// CTransactionItem::CTransactionItem
// -----------------------------------------------------------------------------
//
CTransactionItem::CTransactionItem (MTransactionUser& aTU,
                                    MTransactionOwner* aTransactionOwner,
                                    TTransactionId aTransactionId)
 : CTransactionItemBase(aTransactionId),
   iTU(aTU),
   iTransactionOwner(aTransactionOwner)
	{
	}
  
// -----------------------------------------------------------------------------
// CTransactionItem::CTransactionItem
// -----------------------------------------------------------------------------
//
CTransactionItem::CTransactionItem (MTransactionUser& aTU,
                                    MTransactionOwner* aTransactionOwner,
                                    TTransactionId aTransactionId,
                                    RStringF aMethod)
 : CTransactionItemBase(aTransactionId,aMethod),
   iTU(aTU),
   iTransactionOwner(aTransactionOwner)
	{
	}  
  
// -----------------------------------------------------------------------------
// CTransactionItem::~CTransactionItem
// -----------------------------------------------------------------------------
//
CTransactionItem::~CTransactionItem ()
    {
	}

// -----------------------------------------------------------------------------
// CTransactionItem::IsRefreshed
// From CTransactionItemBase:
// -----------------------------------------------------------------------------
//
TBool CTransactionItem::IsRefreshed () const
	{
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CTransactionItem::ResponseReceived
// From CTransactionItemBase:
// -----------------------------------------------------------------------------
//
void CTransactionItem::ResponseReceived (TUint /*aResponseCode*/)
	{
	}

// -----------------------------------------------------------------------------
// CTransactionItem::SendAndGetHeadersL
// From CTransactionItemBase:
// -----------------------------------------------------------------------------
//
MTransactionHeaders* CTransactionItem::SendAndGetHeadersL (
    const TSIPTransportParams& aTransportParams,
    TTransactionId& aTransactionId,
    TRegistrationId aRegistrationId,
    CSIPRequest* aRequest,
    CURIContainer& aRemoteTarget)
    {
    MTransactionHeaders* headers = 
        iTU.SendAndGetHeadersL (aTransactionId,aRegistrationId,aRequest,
                                iTransactionOwner,aRemoteTarget,
                                aTransportParams,ETrue);
    SetTransactionId (aTransactionId);
    return headers;
    }

// -----------------------------------------------------------------------------
// CTransactionItem::SendL
// From CTransactionItemBase:
// -----------------------------------------------------------------------------
//
void CTransactionItem::SendL (const TSIPTransportParams& aTransportParams,
                              TTransactionId& aTransactionId,
                              TRegistrationId aRegistrationId,
                              CSIPRequest* aRequest,
                              CURIContainer& aRemoteTarget)
    {
    iTU.SendL (aTransactionId,aRegistrationId,aRequest,iTransactionOwner,
               aRemoteTarget,aTransportParams,ETrue);
    SetTransactionId (aTransactionId);
    }

// -----------------------------------------------------------------------------
// CTransactionItem::SendL
// -----------------------------------------------------------------------------
//
void CTransactionItem::SendL (const TSIPTransportParams& aTransportParams,
                              CSIPResponse* aResponse,
                              TBool aAddOwner)
    {
    if (aAddOwner)
        {   
        iTU.SendL(Id(),aResponse,iTransactionOwner,aTransportParams);
        }
    else
        {
        iTU.SendL(Id(),aResponse,aTransportParams);
        }
    }
