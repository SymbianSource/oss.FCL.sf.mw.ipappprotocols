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
// Name          : CRequestQueue.cpp
// Part of       : ServerResolver
// Version       : SIP/4.0 
//



#include "CRequestQueue.h"
#include "CServerQuery.h"
#include "MSIPServerResolverObserver.h"

// ----------------------------------------------------------------------------
// CRequestQueue::CRequestQueue
// ----------------------------------------------------------------------------
//
CRequestQueue::CRequestQueue(): iList( CServerQuery::iOffset ),
							    iListIter( iList )
	{}

// ----------------------------------------------------------------------------
// CRequestQueue::NewL
// ----------------------------------------------------------------------------
//
CRequestQueue* CRequestQueue::NewL()
	{
	CRequestQueue* self = NewLC();
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CRequestQueue::NewLC
// ----------------------------------------------------------------------------
//
CRequestQueue* CRequestQueue::NewLC()
	{
	CRequestQueue* self = new ( ELeave ) CRequestQueue();
	CleanupStack::PushL( self );
	return self;	
	}

// ----------------------------------------------------------------------------
// CRequestQueue::~CRequestQueue
// ----------------------------------------------------------------------------
//
CRequestQueue::~CRequestQueue()
	{
	CServerQuery* query;
	iListIter.SetToFirst();
	query = iListIter++;
	while ( query )
		{
		iList.Remove( *query );
		delete query;
		query = iListIter++;
		}
	}

// ----------------------------------------------------------------------------
// CRequestQueue::CancelQuery
// ----------------------------------------------------------------------------
//
void CRequestQueue::CancelQuery( MSIPServerResolverObserver* aObserver )
	{
	if ( !iList.IsEmpty() )
		{
		CServerQuery* query;
		iListIter.SetToFirst();
		query = iListIter++;	
		while( query )
			{
			if( query->ServerResolverObserver() == aObserver )
				{
				iList.Remove( *query );
				delete query;
				}
			query = iListIter++;
			}
		}
	}

// ----------------------------------------------------------------------------
// CRequestQueue::AddL
// ----------------------------------------------------------------------------
//
void CRequestQueue::AddL( CServerQuery& aQuery )
	{
	if ( !iList.IsEmpty() )
		{
		CServerQuery* query;
		iListIter.SetToFirst();
		query = iListIter++;
		while ( query )
			{
			if ( query->ServerResolverObserver() ==
											 aQuery.ServerResolverObserver() )
				{
				User::Leave( KErrAlreadyExists );
				}
			query = iListIter++;
			}
		}
	iList.AddLast( aQuery );
	}

// ----------------------------------------------------------------------------
// CRequestQueue::NextQuery
// ----------------------------------------------------------------------------
//
CServerQuery* CRequestQueue::NextQuery()
	{
	if ( !iList.IsEmpty() )
		{
		CServerQuery* query = iList.First();
		iList.Remove( *query );
		return query;
		}
	return 0;
	}

