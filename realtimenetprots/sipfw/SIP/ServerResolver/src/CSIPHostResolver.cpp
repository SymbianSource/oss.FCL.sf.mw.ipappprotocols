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
// Name          : CSIPHostResolver.cpp
// Part of       : ServerResolver
// Version       : SIP/4.0 
//



#include "CSIPHostResolver.h"
#include "CServerQuery.h"
#include "CRequestQueue.h"
#include "MSIPServerResolverObserver.h"
#include "siperr.h"

// ----------------------------------------------------------------------------
// CSIPHostResolver::CSIPHostResolver
// ----------------------------------------------------------------------------
//
CSIPHostResolver::CSIPHostResolver() 
 : CActive( EPriorityStandard )
	{
	CActiveScheduler::Add( this );
	}

// ----------------------------------------------------------------------------
// CSIPHostResolver::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPHostResolver::ConstructL ( RSocketServ& aServer, 
                                    RConnection& aConnection )
	{
	iObserverCancel = EFalse;
	iFailed = EFalse;
	User::LeaveIfError( iResolver.Open( aServer, KAfInet, KProtocolInetUdp, 
		                                aConnection ) );
	iRequestQueue = CRequestQueue::NewL();
	
	}

// ----------------------------------------------------------------------------
// CSIPHostResolver::NewL
// ----------------------------------------------------------------------------
//
CSIPHostResolver* CSIPHostResolver::NewL( RSocketServ& aServer, 
                                          RConnection& aConnection )
	{
	CSIPHostResolver* self = NewLC( aServer, aConnection );
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPHostResolver::NewLC
// ----------------------------------------------------------------------------
//
CSIPHostResolver* CSIPHostResolver::NewLC( RSocketServ& aServer, 
                                           RConnection& aConnection )
	{
	CSIPHostResolver* self = new ( ELeave ) CSIPHostResolver();
	CleanupStack::PushL( self );
	self->ConstructL( aServer, aConnection );
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPHostResolver::~CSIPHostResolver
// ----------------------------------------------------------------------------
//
CSIPHostResolver::~CSIPHostResolver()
	{
	Cancel();
	iResolver.Close();
	delete iRequestQueue;
	delete iQuery;
	}

// ----------------------------------------------------------------------------
// CSIPHostResolver::GetByQueryL
// ----------------------------------------------------------------------------
//
void CSIPHostResolver::GetByQueryL ( CServerQuery* aQuery )
	{
	if( !IsActive() && !iQuery )
		{
		iQuery = aQuery;
		iQuery->Query( *this );
		SetActive();
		}
	else
		{
		if ( iQuery->ServerResolverObserver() == 
										    aQuery->ServerResolverObserver() )
			{
			User::Leave( KErrAlreadyExists );
			}
		iRequestQueue->AddL( *aQuery );
		}
	}

// ----------------------------------------------------------------------------
// CSIPHostResolver::CancelGetByURI
// ----------------------------------------------------------------------------
//
void CSIPHostResolver::CancelGetByURI( MSIPServerResolverObserver* aObserver )
	{
	if ( iQuery != 0 && iQuery->ServerResolverObserver() == aObserver )
		{
		Cancel();
		ResolveNext();
		}
	else
		{
		iRequestQueue->CancelQuery( aObserver );
		}
	}

// ----------------------------------------------------------------------------
// CSIPHostResolver::RunL
// ----------------------------------------------------------------------------
//
void CSIPHostResolver::RunL()
	{
	iFailed = EFalse;
	TBool subquery = EFalse;
	TInt status = iStatus.Int();
	
	MSIPServerResolverObserver* observer = NULL;
	
	if(iQuery != 0)
	{
		observer = iQuery->ServerResolverObserver();

		if ( iQuery && !iObserverCancel )
		{
			subquery = iQuery->HandleQueryResultL( iStatus.Int() );
		}

		if ( subquery )
		{
			iQuery->Query( *this );
			SetActive();
		}
		else
		{
			delete iQuery;
			iQuery = NULL;
			ResolveNext();
		}
	}
	else
	{
		delete iQuery;
		iQuery = NULL;
		ResolveNext();
	}

		
	if ( !subquery && ( status != KErrNone || iFailed ) && !iObserverCancel)
		{
		if ( status != KErrNoMemory )
			{
			status = KErrSIPResolvingFailure;
			}
		if(observer != 0)
			observer->ErrorOccured( status );
		}
	else
		{
		iObserverCancel = EFalse;
		}
	}

// ----------------------------------------------------------------------------
// CSIPHostResolver::RunError
// ----------------------------------------------------------------------------
//
TInt CSIPHostResolver::RunError ( TInt aError )
	{
	TInt err = KErrNone;
	// Complete the query, as RunL has not completed it.
	if ( iQuery )
		{
		MSIPServerResolverObserver* observer = iQuery->ServerResolverObserver();
		delete iQuery; 
		iQuery = NULL;
		observer->ErrorOccured( aError );
		}
		
	if ( aError == KErrNoMemory )
		{
		err = aError;
		}
		
	return err;
	}

// ----------------------------------------------------------------------------
// CSIPHostResolver::ResolveNext
// ----------------------------------------------------------------------------
//
void CSIPHostResolver::ResolveNext()
	{
	if ( !IsActive() )
		{
		iQuery = iRequestQueue->NextQuery();
		if ( iQuery )
			{
			iQuery->Query( *this );
			SetActive();
			}
		}
	}

// ----------------------------------------------------------------------------
// CSIPHostResolver::DoCancel
// ----------------------------------------------------------------------------
//
void CSIPHostResolver::DoCancel ()
	{
	iResolver.Cancel();
	if ( iQuery )
		{
		iObserverCancel = ETrue;
		delete iQuery; 
		iQuery = NULL;
		}
	}

// ----------------------------------------------------------------------------
// CSIPHostResolver::Resolver
// ----------------------------------------------------------------------------
//
RHostResolver& CSIPHostResolver::Resolver ()
	{
	return iResolver;
	}

// ----------------------------------------------------------------------------
// CSIPHostResolver::RequestStatus
// ----------------------------------------------------------------------------
//
TRequestStatus& CSIPHostResolver::RequestStatus ()
	{
	return iStatus;
	}

// ----------------------------------------------------------------------------
// CSIPHostResolver::SetIPListFailed
// ----------------------------------------------------------------------------
//
void CSIPHostResolver::SetIPListFailed( TBool aFailed )
	{
	iFailed = aFailed;
	}
