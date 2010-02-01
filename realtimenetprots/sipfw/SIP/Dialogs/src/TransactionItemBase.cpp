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
// Name          : TransactionItemBase.cpp
// Part of       : SIPDialogs
// Version       : SIP/4.0 
//



#include "TransactionItemBase.h"

// -----------------------------------------------------------------------------
// CTransactionItemBase::Offset
// -----------------------------------------------------------------------------
//
TInt CTransactionItemBase::Offset ()
	{
	return _FOFF(CTransactionItemBase,iLink);
	}

// -----------------------------------------------------------------------------
// CTransactionItemBase::CTransactionItemBase
// -----------------------------------------------------------------------------
//
CTransactionItemBase::CTransactionItemBase ()
 : iTerminate(EFalse),
   iPassResponseToOwner(ETrue),
   iTransactionId (KEmptyTransactionId)
	{
	}

// -----------------------------------------------------------------------------
// CTransactionItemBase::CTransactionItemBase
// -----------------------------------------------------------------------------
//
CTransactionItemBase::CTransactionItemBase (TTransactionId aTransactionId)
 : iTerminate(EFalse),
   iPassResponseToOwner(ETrue),
   iTransactionId(aTransactionId)
	{
	}
    
// -----------------------------------------------------------------------------
// CTransactionItemBase::CTransactionItemBase
// -----------------------------------------------------------------------------
//
CTransactionItemBase::CTransactionItemBase (TTransactionId aTransactionId,
                                            RStringF aMethod)
 : iTerminate(EFalse),
   iPassResponseToOwner(ETrue),
   iTransactionId (aTransactionId)
    {
    iMethod = aMethod.Copy();
    }
    
// -----------------------------------------------------------------------------
// CTransactionItemBase::Copy
// -----------------------------------------------------------------------------
//
void CTransactionItemBase::Copy (const CTransactionItemBase& aItem)
    {
    iTransactionId = aItem.iTransactionId;
    iIsTargetRefresh = aItem.iIsTargetRefresh;
    iOverlappingRestricted = aItem.iOverlappingRestricted;
    iMethod = aItem.Method().Copy();
    }

// -----------------------------------------------------------------------------
// CTransactionItemBase::~CTransactionItemBase
// -----------------------------------------------------------------------------
//
CTransactionItemBase::~CTransactionItemBase()
	{
    iMethod.Close();
	}

// -----------------------------------------------------------------------------
// CTransactionItemBase::TransactionId
// -----------------------------------------------------------------------------
//
TTransactionId CTransactionItemBase::Id () const
	{
	return iTransactionId;
	}

// -----------------------------------------------------------------------------
// CTransactionItemBase::Method
// -----------------------------------------------------------------------------
//
RStringF CTransactionItemBase::Method () const
	{
	return iMethod;
	}

// -----------------------------------------------------------------------------
// CTransactionItemBase::TargetRefresh
// -----------------------------------------------------------------------------
//
TBool CTransactionItemBase::TargetRefresh () const
	{
	return iIsTargetRefresh;
	}

// -----------------------------------------------------------------------------
// CTransactionItemBase::SetTargetRefresh
// -----------------------------------------------------------------------------
//
void CTransactionItemBase::SetTargetRefresh (TBool aIsTargetRefresh)
	{
	iIsTargetRefresh = aIsTargetRefresh;	
	}

// -----------------------------------------------------------------------------
// CTransactionItemBase::SetOverlappingRestricted
// -----------------------------------------------------------------------------
//
void CTransactionItemBase::SetOverlappingRestricted (TBool aOverlappingRestricted)
	{	
	iOverlappingRestricted = aOverlappingRestricted;	
	}

// -----------------------------------------------------------------------------
// CTransactionItemBase::OverlappingRestricted
// -----------------------------------------------------------------------------
//
TBool CTransactionItemBase::OverlappingRestricted () const
	{
	return iOverlappingRestricted;
	}

// -----------------------------------------------------------------------------
// CTransactionItemBase::PassResponseToOwner
// -----------------------------------------------------------------------------
//
TBool CTransactionItemBase::PassResponseToOwner () const
	{
	return iPassResponseToOwner;
	}

// -----------------------------------------------------------------------------
// CTransactionItemBase::Terminate
// -----------------------------------------------------------------------------
//
TBool CTransactionItemBase::Terminate () const
    {
    return iTerminate;
    }

// -----------------------------------------------------------------------------
// CTransactionItemBase::SetTransactionId
// -----------------------------------------------------------------------------
//
void CTransactionItemBase::SetTransactionId (TTransactionId aTransactionId)
	{
	iTransactionId = aTransactionId;
	}
