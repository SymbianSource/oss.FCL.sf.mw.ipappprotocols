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
// Name          : CSIPQueryA_AAAA.cpp
// Part of       : ServerResolver
// Version       : SIP/4.0 
//



#include <utf.h>
#include "CSIPQueryA_AAAA.h"
#include "MSIPResolvingResult.h"
#include "MSIPHostResolver.h"

// ----------------------------------------------------------------------------
// CSIPQueryA_AAAA::CSIPQueryA_AAAA
// ----------------------------------------------------------------------------
//
CSIPQueryA_AAAA::CSIPQueryA_AAAA()
:CSIPQueryBase()
	{
	}

// ----------------------------------------------------------------------------
// CSIPQueryA_AAAA::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPQueryA_AAAA::ConstructL( const TDesC8& aTarget )
	{
	SetTargetL( aTarget );
	}

// ----------------------------------------------------------------------------
// CSIPQueryA_AAAA::NewL
// ----------------------------------------------------------------------------
//
CSIPQueryA_AAAA* CSIPQueryA_AAAA::NewL( const TDesC8& aTarget )
	{
	CSIPQueryA_AAAA* self = new ( ELeave ) CSIPQueryA_AAAA();
	CleanupStack::PushL( self );
	self->ConstructL( aTarget );
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPQueryA_AAAA::~CSIPQueryA_AAAA
// ----------------------------------------------------------------------------
//
CSIPQueryA_AAAA::~CSIPQueryA_AAAA()
	{
	delete iTarget;
	}

// ----------------------------------------------------------------------------
// CSIPQueryA_AAAA::SetTargetL
// ----------------------------------------------------------------------------
//
void CSIPQueryA_AAAA::SetTargetL( const TDesC8& aTarget )
	{
	__ASSERT_ALWAYS ( aTarget.Length() > 0, User::Leave( KErrArgument ) );
	HBufC* temp = HBufC::NewL( aTarget.Length() );
	TPtr targetPtr( temp->Des() );
	CleanupStack::PushL( temp );
	User::LeaveIfError(
		CnvUtfConverter::ConvertToUnicodeFromUtf8( targetPtr, aTarget ) );
	delete iTarget;
	iTarget = temp;
	CleanupStack::Pop( temp );
	}

// ----------------------------------------------------------------------------
// CSIPQueryA_AAAA::QueryResultBuf
// ----------------------------------------------------------------------------
//
TInetAddr CSIPQueryA_AAAA::QueryResultBuf()
	{
	return iResult = TInetAddr::Cast( iNResult().iAddr );
	}
		
// ----------------------------------------------------------------------------
// CSIPQueryA_AAAA::Query
// ----------------------------------------------------------------------------
//
void CSIPQueryA_AAAA::Query( MSIPHostResolver& aResolver )
	{
	aResolver.Resolver().GetByName( *iTarget,
									 iNResult, 
									 aResolver.RequestStatus() );		
	}

// ----------------------------------------------------------------------------
// CSIPQueryA_AAAA::QueryNext
// ----------------------------------------------------------------------------
//
TBool CSIPQueryA_AAAA::QueryNext( MSIPHostResolver& aResolver )
	{	
	TBool result = EFalse;
	if( aResolver.Resolver().Next( iNResult ) == KErrNone	)
		{
		result = ETrue;
		}
	return result;
	}

// ----------------------------------------------------------------------------
// CSIPQueryA_AAAA::HandleQueryResultL
// ----------------------------------------------------------------------------
//
void CSIPQueryA_AAAA::HandleQueryResultL( MSIPHostResolver& /*aResolver*/ )
	{
	__ASSERT_ALWAYS ( EFalse, User::Leave( KErrNotSupported ) );	
	}

// ----------------------------------------------------------------------------
// CSIPQueryA_AAAA::DnsNoRoute
// ----------------------------------------------------------------------------
//
TBool CSIPQueryA_AAAA::DnsNoRoute() const
	{
	TInt flags = iNResult().iFlags;
	return ( flags & EDnsNoRoute );
	}
	
