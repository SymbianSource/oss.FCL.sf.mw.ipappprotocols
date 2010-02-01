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
// Name          : CSIPSRVOrigDomain.cpp
// Part of       : ServerResolver
// Version       : SIP/4.0 
//



#include "sipstrings.h"
#include "sipstrconsts.h"
#include "CSIPSRVOrigDomain.h"
#include "CSIPA_AAAAOrigDomain.h"
#include "CSIPA_AAAASRVDomainOrig.h"
#include "CSIPSRVOrigDomainTCP.h"
#include "CSIPQueryBase.h"
#include "CSIPQuerySrv.h"
#include "MSIPServerQuery.h"
#include "CSIPQueryData.h"
#include "MSIPHostResolver.h"

// ----------------------------------------------------------------------------
// CSIPSRVOrigDomain::CSIPSRVOrigDomain
// ----------------------------------------------------------------------------
//
CSIPSRVOrigDomain::CSIPSRVOrigDomain( MSIPServerQuery& aServerQuery )
:CSIPQueryConditionBase(), iServerQuery ( aServerQuery )
	{
	}

// ----------------------------------------------------------------------------
// CSIPSRVOrigDomain::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPSRVOrigDomain::ConstructL()
	{
	HBufC8* temp = CSIPQuerySrv::SetTargetProtocolL(
									iServerQuery.QueryData().OriginalTarget(), 
									iServerQuery.QueryData().Protocol(),
									iServerQuery.QueryData().IsSipsUri());
	CleanupStack::PushL( temp );
	iServerQuery.QueryData().SetTargetL( *temp );
	iQueryBase = CSIPQuerySrv::NewL( *temp, iServerQuery.QueryData().IsSipsUri() );
	CleanupStack::PopAndDestroy( temp );
	}

// ----------------------------------------------------------------------------
// CSIPSRVOrigDomain::NewL
// ----------------------------------------------------------------------------
//
CSIPSRVOrigDomain* CSIPSRVOrigDomain::NewL( MSIPServerQuery& aServerQuery )
	{
	CSIPSRVOrigDomain* self = new ( ELeave ) CSIPSRVOrigDomain( aServerQuery );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPSRVOrigDomain::~CSIPSRVOrigDomain
// ----------------------------------------------------------------------------
//
CSIPSRVOrigDomain::~CSIPSRVOrigDomain()
	{
	delete iQueryBase;
	}

// ----------------------------------------------------------------------------
// CSIPSRVOrigDomain::QueryBase
// ----------------------------------------------------------------------------
//	
CSIPQueryBase& CSIPSRVOrigDomain::QueryBase() 
	{
	return *iQueryBase;
	}

// ----------------------------------------------------------------------------
// CSIPSRVOrigDomain::HandleQueryResultL
// ----------------------------------------------------------------------------
//	
CSIPQueryConditionBase* CSIPSRVOrigDomain::HandleQueryResultL( TInt aStatus ) 
	{	
	if( aStatus == KErrNone )
		{
		iQueryBase->HandleQueryResultL( iServerQuery.QueryData().Resolver() );
		if ( iQueryBase->ArrayCount() > 0	)
			{
			iServerQuery.QueryData().SetPort( iQueryBase->ResultPortL() );
		    iServerQuery.QueryData().SetTargetL( iQueryBase->ResultTargetL() );
		    return ( CSIPA_AAAASRVDomainOrig::NewL( iServerQuery) );
			}
		}
	if (iServerQuery.QueryData().IsSipsUri())
		{
		iServerQuery.QueryData().SetProtocol(
				SIPStrings::StringF( SipStrConsts::ETLS ) );
		HBufC8* temp = CSIPQuerySrv::SetTargetTLSL( 
								iServerQuery.QueryData().OriginalTarget());
		CleanupStack::PushL( temp );
		iServerQuery.QueryData().SetTargetL( *temp );
		CleanupStack::PopAndDestroy( temp );
		return ( CSIPSRVOrigDomainTCP::NewL( iServerQuery ));
		}
	iServerQuery.QueryData().SetPort( KSIPDefaultPort );
	iServerQuery.QueryData().SetTargetL(
								iServerQuery.QueryData().OriginalTarget() );	
	return ( CSIPA_AAAAOrigDomain::NewL( iServerQuery) );
	}

