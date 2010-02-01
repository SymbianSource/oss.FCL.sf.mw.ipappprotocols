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
// Name        : CProxyResolvingQueue.cpp
// Part of     : ProxyResolver
// Version     : SIP/4.0
//



#include "CProxyResolvingQueue.h"
#include "CProxyQuery.h"

        
// -----------------------------------------------------------------------------
// CProxyResolvingQueue::NewL
// -----------------------------------------------------------------------------
//
CProxyResolvingQueue* CProxyResolvingQueue::NewL ()
    {
    CProxyResolvingQueue* self = CProxyResolvingQueue::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CProxyResolvingQueue::NewLC
// -----------------------------------------------------------------------------
//  
CProxyResolvingQueue* CProxyResolvingQueue::NewLC ()
    {
    CProxyResolvingQueue* self = new (ELeave) CProxyResolvingQueue();
    CleanupStack::PushL( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CProxyResolvingQueue::~CProxyResolvingQueue
// -----------------------------------------------------------------------------
//  
CProxyResolvingQueue::~CProxyResolvingQueue ()
    {
    CProxyQuery* query = iList.First();
    while (iList.IsFirst(query) && !iList.IsEmpty())
        {
        iList.Remove(*query);
        delete query;
        query = iList.First();
        }
    
    }
    
// -----------------------------------------------------------------------------
// CProxyResolvingQueue::CProxyResolvingQueue
// -----------------------------------------------------------------------------
//  
CProxyResolvingQueue::CProxyResolvingQueue () 
    : iList          ( CProxyQuery::iOffset ), 
      iListIter      ( iList ),
      iNextRequestId ( KMinProxyResolveRequestId )
    {
    }
    
// -----------------------------------------------------------------------------
// CProxyResolvingQueue::AddQuery
// -----------------------------------------------------------------------------
//  
void CProxyResolvingQueue::AddQuery ( CProxyQuery& aQuery )
    {
    iList.AddLast( aQuery );
    }
    
// -----------------------------------------------------------------------------
// CProxyResolvingQueue::NextQuery
// -----------------------------------------------------------------------------
//
CProxyQuery* CProxyResolvingQueue::NextQuery ()
    {
    CProxyQuery* query = NULL;
    
    if(!iList.IsEmpty())
        {
        query = iList.First();
        iList.Remove(*query);
        }
        
    return query;
    }
  
// -----------------------------------------------------------------------------
// CProxyResolvingQueue::DeleteByRequestId
// -----------------------------------------------------------------------------
//  
void CProxyResolvingQueue::DeleteByRequestId ( TUint aRequestId )
    {
    CProxyQuery* query = FindQueryByRequestId( aRequestId );
    
    if (query)
        {
        iList.Remove(*query);
        delete query;
        }
    }

// -----------------------------------------------------------------------------
// CProxyResolvingQueue::DetachByRequestId
// -----------------------------------------------------------------------------
//  
void CProxyResolvingQueue::DetachByRequestId ( TUint aRequestId )
    {
    CProxyQuery* query = FindQueryByRequestId( aRequestId );
    
    if (query)
        {
        iList.Remove(*query);
        }
    }


    
// -----------------------------------------------------------------------------
// CProxyResolvingQueue::NextRequestId
// -----------------------------------------------------------------------------
//
TUint CProxyResolvingQueue::NextRequestId ()
    {
    TUint nextRequestId = iNextRequestId;

    if (iNextRequestId == KMaxProxyResolveRequestId)
        {
        iNextRequestId = KMinProxyResolveRequestId;
        }
    else
        {
        iNextRequestId++;
        }
    return nextRequestId;
    }
    
// -----------------------------------------------------------------------------
// CProxyResolvingQueue::FindQueryByRequestId
// -----------------------------------------------------------------------------
//
CProxyQuery* CProxyResolvingQueue::FindQueryByRequestId ( TUint aRequestId )
    {
    TSglQueIter<CProxyQuery> iter(iList);
    CProxyQuery* query = iter++;
    
    while ( query )
        {
        if ( query->RequestId() == aRequestId )
            {
            return query;
            }
        query = iter++;
        }
        
    return NULL;
    }

// -----------------------------------------------------------------------------
// CProxyResolvingQueue::NextReadyQuery
// -----------------------------------------------------------------------------
//
CProxyQuery* CProxyResolvingQueue::NextReadyQuery ()
    {
    TSglQueIter<CProxyQuery> iter(iList);
    CProxyQuery* query = iter++;
    CProxyQuery* nextReadyQuery = NULL;
    
    while( query && !nextReadyQuery )
        {
        if( query->ConnectionOpened())
            {
            nextReadyQuery = query;     
            }
        else
            {
            query = iter++;     
            }
        }

    if( nextReadyQuery )
        {
        iList.Remove( *query ); 
        }
        
    return nextReadyQuery;
    }
