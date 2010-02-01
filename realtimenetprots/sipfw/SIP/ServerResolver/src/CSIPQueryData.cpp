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
// Name          : CSIPQueryData.cpp
// Part of       : ServerResolver
// Version       : SIP/4.0 
//



#include <utf.h>
#include "CSIPQueryData.h"
#include "CSIPQueryBase.h"
#include "CSIPQueryA_AAAA.h"
#include "MSIPHostResolver.h"
#include "MSIPResolvingResult.h"
#include "CSIPResolvingResult.h"
#include "MSIPServerResolverObserver.h"

// ----------------------------------------------------------------------------
// CSIPQueryData::CSIPQueryData
// ----------------------------------------------------------------------------
//
CSIPQueryData::CSIPQueryData( MSIPHostResolver& aResolver,
							  RPointerArray< MSIPResolvingResult >& aResult,
							  TBool aSipsUri,
							  TBool aForceUDP ) :
	iResolver ( aResolver ),
	iResultArray ( aResult ),
	iSipsUri ( aSipsUri ),
	iForceUDP ( aForceUDP )
	{
	}

// ----------------------------------------------------------------------------
// CSIPQueryData::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPQueryData::ConstructL( MSIPServerResolverObserver* aObserver,
                                TUint aPort,
                                RStringF aProtocol,
                                const TDesC8& aOriginalTarget )
	{
	iPort = aPort;
	iProtocol = aProtocol;
	iTarget = aOriginalTarget.AllocL();
	iOriginalTarget = aOriginalTarget.AllocL();
	iObserver = aObserver;
	}

// ----------------------------------------------------------------------------
// CSIPQueryData::NewL
// ----------------------------------------------------------------------------
//
CSIPQueryData* CSIPQueryData::NewL( MSIPServerResolverObserver* aObserver,
                               TUint aPort,
                               RStringF aProtocol,
                               const TDesC8& aOriginalTarget,
							   MSIPHostResolver& aResolver,
							   RPointerArray< MSIPResolvingResult >& aResult,
							   TBool aSipsUri,
							   TBool aForceUDP )
	{
	CSIPQueryData* self =
		new ( ELeave ) CSIPQueryData( aResolver, aResult, aSipsUri, aForceUDP );
	CleanupStack::PushL( self );
	self->ConstructL( aObserver, aPort, aProtocol, aOriginalTarget );
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPQueryData::~CSIPQueryData
// ----------------------------------------------------------------------------
//
CSIPQueryData::~CSIPQueryData()
	{	
	delete iTarget;
	delete iOriginalTarget;
	iProtocol.Close();
	}

// ----------------------------------------------------------------------------
// CSIPQueryData::ResultArray
// ----------------------------------------------------------------------------
//	
RPointerArray<MSIPResolvingResult>& CSIPQueryData::ResultArray() 
	{
	return iResultArray;
	}

// ----------------------------------------------------------------------------
// CSIPQueryData::Observer
// ----------------------------------------------------------------------------
//	
MSIPServerResolverObserver* CSIPQueryData::Observer() 
	{
	return iObserver;
	}

// ----------------------------------------------------------------------------
// CSIPQueryData::Port
// ----------------------------------------------------------------------------
//	
TUint CSIPQueryData::Port() 
	{
	return iPort;
	}

// ----------------------------------------------------------------------------
// CSIPQueryData::Protocol
// ----------------------------------------------------------------------------
//	
RStringF CSIPQueryData::Protocol() 
	{
	return iProtocol;
	}

// ----------------------------------------------------------------------------
// CSIPQueryData::Target
// ----------------------------------------------------------------------------
//	
const TDesC8& CSIPQueryData::Target() const
	{
	return *iTarget;
	}

// ----------------------------------------------------------------------------
// CSIPQueryData::OriginalTarget
// ----------------------------------------------------------------------------
//	
const TDesC8& CSIPQueryData::OriginalTarget() const
	{
	return *iOriginalTarget;
	}

// ----------------------------------------------------------------------------
// CSIPQueryData::Resolver
// ----------------------------------------------------------------------------
//	
MSIPHostResolver& CSIPQueryData::Resolver()
	{
	return iResolver;
	}

// ----------------------------------------------------------------------------
// CSIPQueryData::SetPort
// ----------------------------------------------------------------------------
//	
void CSIPQueryData::SetPort( TUint aPort )
	{
	iPort = aPort;
	}

// ----------------------------------------------------------------------------
// CSIPQueryData::SetProtocol
// ----------------------------------------------------------------------------
//	
void CSIPQueryData::SetProtocol( RStringF aProtocol )
	{	
	iProtocol.Close();
	iProtocol = aProtocol.Copy();	
	}

// ----------------------------------------------------------------------------
// CSIPQueryData::SetTargetL
// ----------------------------------------------------------------------------
//	
void CSIPQueryData::SetTargetL( const TDesC8& aTarget )
	{
	HBufC8* temp = aTarget.AllocL();
	delete iTarget;
	iTarget = temp;
	}

// ----------------------------------------------------------------------------
// CSIPQueryData::SetOriginalTargetL
// ----------------------------------------------------------------------------
//	
void CSIPQueryData::SetOriginalTargetL( const TDesC8& aOriginalTarget )
	{
	HBufC8* temp = aOriginalTarget.AllocLC();
	delete iOriginalTarget;
	iOriginalTarget = temp;
	CleanupStack::Pop( temp );
	}

// ----------------------------------------------------------------------------
// CSIPQueryData::CreateResultL
// ----------------------------------------------------------------------------
//
TBool CSIPQueryData::CreateResultL( CSIPQueryA_AAAA& aQuery )
	{
	TBool result = EFalse;
	iQuery = &aQuery;
	if ( AddResultToArrayL() )
		{
		result = ETrue;
		}
	while ( iQuery->QueryNext( iResolver ) )
		{
		if ( AddResultToArrayL() )
			{
			result = ETrue;
			}
		}
	return result;
	}

// ----------------------------------------------------------------------------
// CSIPQueryData::AddResultToArrayL
// ----------------------------------------------------------------------------
//
TBool CSIPQueryData::AddResultToArrayL()
    {
    if ( iQuery->DnsNoRoute() )
    	{
    	return EFalse;
    	}
	TInetAddr addr( iQuery->QueryResultBuf() );
	addr.SetPort( iPort );
	for ( TInt i=0;i < ResultArray().Count();i++ )
		{
		if ( ResultArray()[i]->Address().CmpAddr( addr ) 
				&& ResultArray()[i]->Protocol() == iProtocol )
			{
			return EFalse;
			}
		}

	CSIPResolvingResult* result =
		CSIPResolvingResult::NewLC( addr, iProtocol, iForceUDP );
	User::LeaveIfError( ResultArray().Append( result ) );
	CleanupStack::Pop( result );
	return ETrue;
    }

// ----------------------------------------------------------------------------
// CSIPQueryData::IsSipsUri
// ----------------------------------------------------------------------------
//
TBool CSIPQueryData::IsSipsUri()
    {
    return iSipsUri;
    }

// ----------------------------------------------------------------------------
// CSIPQueryData::SetForceUDP
// ----------------------------------------------------------------------------
//
void CSIPQueryData::SetForceUDP()
	{
	iForceUDP = ETrue;
	}

// ----------------------------------------------------------------------------
// CSIPQueryData::CompleteQueryL
// ----------------------------------------------------------------------------
//
void CSIPQueryData::CompleteQueryL()
    {
	iObserver->CompletedL();
    }
