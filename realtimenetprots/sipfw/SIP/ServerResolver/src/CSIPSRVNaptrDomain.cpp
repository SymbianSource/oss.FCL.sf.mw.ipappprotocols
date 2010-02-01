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
// Name          : CSIPSRVNaptrDomain.cpp
// Part of       : ServerResolver
// Version       : SIP/4.0 
//



#include "sipstrings.h"
#include "sipstrconsts.h"
#include "CSIPSRVNaptrDomain.h"
#include "CSIPA_AAAAOrigDomain.h"
#include "CSIPA_AAAASRVDomain.h"
#include "CSIPQueryBase.h"
#include "CSIPQuerySrv.h"
#include "MSIPServerQuery.h"
#include "CSIPQueryData.h"

// ----------------------------------------------------------------------------
// CSIPSRVNaptrDomain::CSIPSRVNaptrDomain
// ----------------------------------------------------------------------------
//
CSIPSRVNaptrDomain::CSIPSRVNaptrDomain( MSIPServerQuery& aServerQuery )
:CSIPQueryConditionBase(), iServerQuery ( aServerQuery )
	{
	}

// ----------------------------------------------------------------------------
// CSIPSRVNaptrDomain::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPSRVNaptrDomain::ConstructL( CSIPQueryBase* aNaptrQuery )
	{
	iQueryBase = CSIPQuerySrv::NewL( iServerQuery.QueryData().Target(),
									 iServerQuery.QueryData().IsSipsUri() );
	iQueryNaptr = aNaptrQuery;
	}

// ----------------------------------------------------------------------------
// CSIPSRVNaptrDomain::NewL
// ----------------------------------------------------------------------------
//
CSIPSRVNaptrDomain* CSIPSRVNaptrDomain::NewL( MSIPServerQuery& aServerQuery,
											  CSIPQueryBase* aNaptrQuery )
	{
	CSIPSRVNaptrDomain* self = new ( ELeave ) CSIPSRVNaptrDomain( aServerQuery );
	CleanupStack::PushL( self );
	self->ConstructL( aNaptrQuery );
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPSRVNaptrDomain::~CSIPSRVNaptrDomain
// ----------------------------------------------------------------------------
//
CSIPSRVNaptrDomain::~CSIPSRVNaptrDomain()
	{
	delete iQueryBase;
	delete iQueryNaptr;
	}

// ----------------------------------------------------------------------------
// CSIPSRVNaptrDomain::QueryBase
// ----------------------------------------------------------------------------
//	
CSIPQueryBase& CSIPSRVNaptrDomain::QueryBase() 
	{
	return *iQueryBase;
	}

// ----------------------------------------------------------------------------
// CSIPSRVNaptrDomain::HandleQueryResultL
// ----------------------------------------------------------------------------
//	
CSIPQueryConditionBase* CSIPSRVNaptrDomain::HandleQueryResultL( TInt aStatus ) 
	{
	if ( aStatus == KErrNone )
		{
		iQueryBase->HandleQueryResultL( iServerQuery.QueryData().Resolver() );
		if ( iQueryBase->ArrayCount() != 0 )
			{
			iServerQuery.QueryData().SetPort( iQueryBase->ResultPortL() );
		    //protocol has already been chosen
		    iServerQuery.QueryData().SetTargetL( iQueryBase->ResultTargetL() );
		
		    CSIPQueryConditionBase* conditionbase = CSIPA_AAAASRVDomain::NewL( 
												iServerQuery,
								   				iQueryNaptr,
								   				iQueryBase );
		    iQueryNaptr = NULL;//ownership is transferred
		    iQueryBase = NULL;//ownership is transferred
		    return conditionbase;
			}
		}
	iQueryNaptr->RemoveElementL( 0 );
	if ( iQueryNaptr->ArrayCount() == 0 )
		{
		if (iServerQuery.QueryData().IsSipsUri())
			{
			iServerQuery.QueryData().SetPort( KSIPsDefaultPort );
			iServerQuery.QueryData().SetProtocol(
				SIPStrings::StringF( SipStrConsts::ETLS ) );
			}
		else
			{
			iServerQuery.QueryData().SetPort( KSIPDefaultPort );
			iServerQuery.QueryData().SetProtocol(
				SIPStrings::StringF( SipStrConsts::EUDP ) );
			}
	
		iServerQuery.QueryData().SetTargetL(
								iServerQuery.QueryData().OriginalTarget() );
		//original target
		return ( CSIPA_AAAAOrigDomain::NewL( iServerQuery ) );
		}
	else
		{
		iServerQuery.QueryData().SetProtocol( iQueryNaptr->ResultProtocolL() );
		iServerQuery.QueryData().SetTargetL( iQueryNaptr->ResultTargetL() );
		iQueryBase->SetTargetL( iServerQuery.QueryData().Target() );
		return this;
		}	
	}



