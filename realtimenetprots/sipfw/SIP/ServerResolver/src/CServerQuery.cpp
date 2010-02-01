// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CServerQuery.cpp
// Part of       : ServerResolver
// Version       : SIP/4.0 
//



#include "CServerQuery.h"
#include "CSIPQueryConditionBase.h"
#include "CSIPQueryBase.h"
#include "CSIPQueryData.h"

const TInt CServerQuery::iOffset = _FOFF(CServerQuery, iLink);

// ----------------------------------------------------------------------------
// CServerQuery::CServerQuery
// ----------------------------------------------------------------------------
//
CServerQuery::CServerQuery()
	{
	}

// ----------------------------------------------------------------------------
// CServerQuery::ConstructL
// ----------------------------------------------------------------------------
//
void CServerQuery::ConstructL( MSIPServerResolverObserver* aObserver, 
							   CSIPQueryData* aQueryData )
	{
	iObserver = aObserver;
	iQueryData = aQueryData;
	}
	
// ----------------------------------------------------------------------------
// CServerQuery::NewL
// ----------------------------------------------------------------------------
//
CServerQuery* CServerQuery::NewL( MSIPServerResolverObserver* aObserver,
							      CSIPQueryData* aQueryData )
	{
	CServerQuery* self = NewLC( aObserver, aQueryData );
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CServerQuery::NewLC
// ----------------------------------------------------------------------------
//
CServerQuery* CServerQuery::NewLC( MSIPServerResolverObserver* aObserver,
							       CSIPQueryData* aQueryData )
	{
	CServerQuery* self = new ( ELeave ) CServerQuery();
	CleanupStack::PushL( self );
	self->ConstructL( aObserver, aQueryData );
	return self;	
	}
	
// ----------------------------------------------------------------------------
// CServerQuery::~CServerQuery
// ----------------------------------------------------------------------------
//
CServerQuery::~CServerQuery()
	{
	delete iQueryCondition;
	delete iQueryData;
	}
	
// ----------------------------------------------------------------------------
// CServerQuery::Query
// ----------------------------------------------------------------------------
//	
void CServerQuery::Query( MSIPHostResolver& aResolver ) 
	{
	iQueryCondition->QueryBase().Query( aResolver );
	}

// ----------------------------------------------------------------------------
// CServerQuery::HandleQueryResultL
// ----------------------------------------------------------------------------
//	
TBool CServerQuery::HandleQueryResultL( TInt aStatus ) 
	{
	CSIPQueryConditionBase* querycondition =
								iQueryCondition->HandleQueryResultL( aStatus );
	TBool result = EFalse;
	if ( !querycondition )
		{
		delete iQueryCondition;
		iQueryCondition = NULL;
		result = EFalse;
		}
	else if ( querycondition == iQueryCondition )
			{
			result = ETrue;
			}
		 else
		 	{
			delete iQueryCondition;
			iQueryCondition = querycondition;
			result = ETrue;
			}
	return result;
	}

// ----------------------------------------------------------------------------
// CServerQuery::MSIPServerResolverObserver
// ----------------------------------------------------------------------------
//	
MSIPServerResolverObserver* CServerQuery::ServerResolverObserver() 
	{
	return iObserver;
	}

// ----------------------------------------------------------------------------
// CServerQuery::QueryData
// ----------------------------------------------------------------------------
//	
CSIPQueryData& CServerQuery::QueryData() 
	{
	return *iQueryData;
	}

// ----------------------------------------------------------------------------
// CServerQuery::SetQueryCondition
// ----------------------------------------------------------------------------
//	
void CServerQuery::SetQueryConditionL( CSIPQueryConditionBase* aQueryCondition ) 
	{
	__ASSERT_ALWAYS ( aQueryCondition, User::Leave( KErrArgument ) );
	delete iQueryCondition;
	iQueryCondition = aQueryCondition;
	}

