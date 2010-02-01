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
// Name          : CSIPA_AAAASRVDomain.cpp
// Part of       : ServerResolver
// Version       : SIP/4.0 
//



#include "sipstrings.h"
#include "sipstrconsts.h"
#include "CSIPA_AAAASRVDomain.h"
#include "CSIPSRVNaptrDomain.h"
#include "CSIPA_AAAAOrigDomain.h"
#include "CSIPQueryA_AAAA.h"
#include "CSIPQueryBase.h"
#include "MSIPServerQuery.h"
#include "CSIPQueryData.h"

// ----------------------------------------------------------------------------
// CSIPA_AAAASRVDomain::CSIPA_AAAASRVDomain
// ----------------------------------------------------------------------------
//
CSIPA_AAAASRVDomain::CSIPA_AAAASRVDomain( MSIPServerQuery& aServerQuery )
:CSIPQueryConditionBase(), iServerQuery ( aServerQuery )
	{
	}

// ----------------------------------------------------------------------------
// CSIPA_AAAASRVDomain::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPA_AAAASRVDomain::ConstructL( CSIPQueryBase* aNaptrQuery,
                               		  CSIPQueryBase* aSrvQuery )
	{
	iQueryBase = CSIPQueryA_AAAA::NewL( iServerQuery.QueryData().Target() );
	iQueryNaptr = aNaptrQuery;
	iSrvQuery = aSrvQuery;
	}

// ----------------------------------------------------------------------------
// CSIPA_AAAASRVDomain::NewL
// ----------------------------------------------------------------------------
//
CSIPA_AAAASRVDomain* 
	CSIPA_AAAASRVDomain::NewL( MSIPServerQuery& aServerQuery,
                               CSIPQueryBase* aNaptrQuery,
                               CSIPQueryBase* aSrvQuery )
	{
	CSIPA_AAAASRVDomain* self = 
						new ( ELeave ) CSIPA_AAAASRVDomain( aServerQuery );
	CleanupStack::PushL( self );
	self->ConstructL( aNaptrQuery, aSrvQuery );
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPA_AAAASRVDomain::~CSIPA_AAAASRVDomain
// ----------------------------------------------------------------------------
//
CSIPA_AAAASRVDomain::~CSIPA_AAAASRVDomain()
	{
	delete iQueryBase;
	delete iQueryNaptr;
	delete iSrvQuery;
	}

// ----------------------------------------------------------------------------
// CSIPA_AAAASRVDomain::QueryBase
// ----------------------------------------------------------------------------
//	
CSIPQueryBase& CSIPA_AAAASRVDomain::QueryBase() 
	{
	return *iQueryBase;
	}

// ----------------------------------------------------------------------------
// CSIPA_AAAASRVDomain::HandleQueryResultL
// ----------------------------------------------------------------------------
//	
CSIPQueryConditionBase* CSIPA_AAAASRVDomain::HandleQueryResultL( TInt aStatus ) 
	{
	if ( aStatus == KErrNone )
		{
		if ( iServerQuery.QueryData().CreateResultL(
							*static_cast<CSIPQueryA_AAAA*> ( iQueryBase ) ) )
			{
			iServerQuery.QueryData().CompleteQueryL();
			return NULL;
			}
		}

	//more srvrecords
	iSrvQuery->RemoveElementL( 0 );
	if ( iSrvQuery->ArrayCount() == 0 )
		{		
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
			
		iServerQuery.QueryData().SetProtocol( iQueryNaptr->ResultProtocolL() );										
		iServerQuery.QueryData().SetTargetL( iQueryNaptr->ResultTargetL() );
											
		CSIPQueryConditionBase* conditionbase = 
						CSIPSRVNaptrDomain::NewL( iServerQuery, iQueryNaptr );
						
		//naptrQuery ownership is transferred to the new condition
		iQueryNaptr = NULL;
		return conditionbase;
			
		}
	iServerQuery.QueryData().SetPort( iSrvQuery->ResultPortL() );
	iServerQuery.QueryData().SetTargetL( iSrvQuery->ResultTargetL() );
	iQueryBase->SetTargetL( iServerQuery.QueryData().Target() );
	return this;
	}

