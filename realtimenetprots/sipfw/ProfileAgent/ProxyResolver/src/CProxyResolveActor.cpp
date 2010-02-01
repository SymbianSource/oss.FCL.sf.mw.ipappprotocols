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
// Name        : CProxyResolveActor.cpp
// Part of     : ProxyResolver
// Version     : SIP/4.0
//



#include "CProxyResolveActor.h"
#include "CProxyResolvingQueue.h"
#include "CProxyQuery.h"
#include <nifman.h>


// -----------------------------------------------------------------------------
// CProxyResolveActor::NewL
// -----------------------------------------------------------------------------
//
CProxyResolveActor* CProxyResolveActor::NewL()
    {
    CProxyResolveActor* self = CProxyResolveActor::NewLC();
    CleanupStack::Pop( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// CProxyResolveActor::NewLC
// -----------------------------------------------------------------------------
//
CProxyResolveActor* CProxyResolveActor::NewLC()
    {
    CProxyResolveActor* self = new( ELeave )CProxyResolveActor();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
// -----------------------------------------------------------------------------
// CProxyResolveActor::~CProxyResolveActor
// -----------------------------------------------------------------------------
//
CProxyResolveActor::~CProxyResolveActor()
    {
    Cancel();
    if ( iCurrentQuery )
        {
        delete iCurrentQuery;       
        }
    delete iRequestQueue;
    // close socket server
    iServer.Close();

    }

// -----------------------------------------------------------------------------
// CProxyResolveActor::ConstructL
// -----------------------------------------------------------------------------
//
void CProxyResolveActor::ConstructL ()
    {
    iRequestQueue = CProxyResolvingQueue::NewL();
    // connect the socket server
    User::LeaveIfError( iServer.Connect() );
    }
    
    
// -----------------------------------------------------------------------------
// CProxyResolveActor::CProxyResolveActor
// -----------------------------------------------------------------------------
//
CProxyResolveActor::CProxyResolveActor()
    : CActive( EPriorityStandard )
    {
    CActiveScheduler::Add( this );  
    }
    
    
// -----------------------------------------------------------------------------
// CProxyResolveActor::ResolveProxyL
// -----------------------------------------------------------------------------
//
void CProxyResolveActor::ResolveProxyL( TUint&                     aRequestId, 
                                        TUint32                    aIapId, 
                                        MSIPProxyResolverObserver& aObserver )
    {
    CProxyQuery* query = CProxyQuery::NewL( iRequestQueue->NextRequestId(), 
                                            aIapId, aObserver, *this, 
                                            iServer );
    
    // add the query to the queue
    iRequestQueue->AddQuery( *query );
        
    // and return the ID to the user
    aRequestId = query->RequestId();    
    }


// -----------------------------------------------------------------------------
// void CProxyResolveActor::ConnectionReady(CProxyQuery* aQuery)
//
// Callback from CProxyQuery, indicating that opening RConnection for
// this query is successful and completed
//
// -----------------------------------------------------------------------------
//  
void CProxyResolveActor::ConnectionReady( CProxyQuery* aQuery )
    {
    if( !IsActive() )
        {
        iCurrentQuery = aQuery;
        // detach this query from the queue (but do not delete)
        iRequestQueue->DetachByRequestId( aQuery->RequestId() );
        // perform the actual query
        PerformDHCPQuery(); 
        }
    }

// -----------------------------------------------------------------------------
// void CProxyResolveActor::PerformDHCPQuery()
//
// Perform the actual DHCP query using RConnection API call (Ioctl)
//
// -----------------------------------------------------------------------------
//
void CProxyResolveActor::PerformDHCPQuery()
    {
    if ( iCurrentQuery && !IsActive() )  
        {   
         //Make sure that the connection has been opened for this Query object
         if (iCurrentQuery->ConnectionOpened())
            {
            iCurrentQuery->Connection().Ioctl( KCOLConfiguration, 
                                               iCurrentQuery->CurrentQueryType(),
                                               iStatus, 
                                               iCurrentQuery->GetBuf() );      
            SetActive();
            }
        }
    }


// -----------------------------------------------------------------------------
// CProxyResolveActor::RunL
// -----------------------------------------------------------------------------
//
void CProxyResolveActor::RunL()
    {
    TInt err = iStatus.Int();
    
    switch( err )
        {
        case KErrNone: 
            {
            // store results of this query
            iCurrentQuery->AddResultsL();            
            //make a new query (SIP addresses or domain names)
            iCurrentQuery->SetNextIndex();
            PerformDHCPQuery();        
            return;        
            }
            
        case KErrCancel: // ignore the cancel error code
            break;
        
        default: 
            {
            if ( iCurrentQuery->CurrentQueryType() == KConnGetSipServerDomain )
                {
                // Completed domain names. Now try IP addresses.
                // Note: index is already 0.
                iCurrentQuery->SetQueryType( KConnGetSipServerAddr );
                PerformDHCPQuery();                                
                return;        
                }
            else
                {
                // No more data available, send results to the owner
                iCurrentQuery->ResolvingRequestComplete();           
                }
            }
            break;
        }
    
    // Finally, delete this CProxyQuery and process the next one
    delete iCurrentQuery;
    iCurrentQuery = NULL;
    ResolveNext();
    }

// -----------------------------------------------------------------------------
// CProxyResolveActor::ResolveNext
// Delete the current query and process the next one with opened RConnection
// -----------------------------------------------------------------------------
//
void CProxyResolveActor::ResolveNext()
    {
    if(!IsActive())
        {
        if ( iCurrentQuery )
            {
            delete iCurrentQuery; 
            iCurrentQuery = NULL;
            }
            
        // get next CProxyQuery object, whose RConnection is opened
        iCurrentQuery = iRequestQueue->NextReadyQuery();
        PerformDHCPQuery();
        }
    }

// -----------------------------------------------------------------------------
// CProxyResolveActor::RunError
// -----------------------------------------------------------------------------
//
TInt CProxyResolveActor::RunError( TInt aError )
    {
    TInt err = KErrNone;
    // delete the query, as RunL leaved with aError.
    if ( iCurrentQuery )
        {
        iCurrentQuery->ResolvingRequestFailed( aError );
        delete iCurrentQuery ; 
        iCurrentQuery  = NULL;
        }
    if( aError == KErrNoMemory )
        {
        err = aError;
        }
    ResolveNext();
    return err;
    }

// -----------------------------------------------------------------------------
// CProxyResolveActor::CancelById
// -----------------------------------------------------------------------------
//                     
void CProxyResolveActor::CancelById( TUint aRequestId )
    {
    if ( iCurrentQuery && iCurrentQuery->RequestId() == aRequestId )
        {
        Cancel();
        iRequestQueue->DetachByRequestId( aRequestId );
        delete iCurrentQuery;
        iCurrentQuery = NULL;
        ResolveNext();
        }
    else
        {
        iRequestQueue->DeleteByRequestId( aRequestId );
        }
    }

// -----------------------------------------------------------------------------
// CProxyResolveActor::DoCancel
// -----------------------------------------------------------------------------
//  
void CProxyResolveActor::DoCancel()
    {   
    // if iCurrentQuery is not null, request has been issued for 
    // the iCurrentQuery, do the cancel action
    if ( iCurrentQuery )
        {
        iCurrentQuery->Connection().CancelIoctl(); 
        }
    }

// -----------------------------------------------------------------------------
// CProxyResolveActor::ErrorOccured
// Callback from CProxyQuery, indicating that opening RConnection failed
//
// -----------------------------------------------------------------------------
//  
void CProxyResolveActor::ErrorOccured( TInt aError, CProxyQuery* aQuery )
    {
    aQuery->ResolvingRequestFailed( aError );
    iRequestQueue->DeleteByRequestId( aQuery->RequestId() );
    }
