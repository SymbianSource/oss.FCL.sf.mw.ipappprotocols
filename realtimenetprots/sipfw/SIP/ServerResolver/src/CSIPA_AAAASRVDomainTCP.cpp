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
// Name          : CSIPA_AAAASRVDomainTCP.cpp
// Part of       : ServerResolver
// Version       : SIP/4.0 
//



#include "sipstrings.h"
#include "sipstrconsts.h"
#include "CSIPA_AAAASRVDomainTCP.h"
#include "CSIPA_AAAAOrigDomain.h"
#include "CSIPQueryBase.h"
#include "CSIPQuerySrv.h"
#include "CSIPQueryA_AAAA.h"
#include "MSIPServerQuery.h"
#include "CSIPQueryData.h"

// ----------------------------------------------------------------------------
// CSIPA_AAAASRVDomainTCP::CSIPA_AAAASRVDomainTCP
// ----------------------------------------------------------------------------
//
CSIPA_AAAASRVDomainTCP::CSIPA_AAAASRVDomainTCP( MSIPServerQuery& aServerQuery )
:CSIPQueryConditionBase(), iServerQuery ( aServerQuery )
	{
	}

// ----------------------------------------------------------------------------
// CSIPA_AAAASRVDomainTCP::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPA_AAAASRVDomainTCP::ConstructL( CSIPQueryBase* aSrvQuery )
	{
	iQueryBase = CSIPQueryA_AAAA::NewL( iServerQuery.QueryData().Target() );
	iSrvQuery = aSrvQuery;
	}

// ----------------------------------------------------------------------------
// CSIPA_AAAASRVDomainTCP::NewL
// ----------------------------------------------------------------------------
//
CSIPA_AAAASRVDomainTCP* CSIPA_AAAASRVDomainTCP::NewL( 
												MSIPServerQuery& aServerQuery,
												CSIPQueryBase* aSrvQuery )
	{
	CSIPA_AAAASRVDomainTCP* self = 
						new ( ELeave ) CSIPA_AAAASRVDomainTCP( aServerQuery );
	CleanupStack::PushL( self );
	self->ConstructL( aSrvQuery );
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPA_AAAASRVDomainTCP::~CSIPA_AAAASRVDomainTCP
// ----------------------------------------------------------------------------
//
CSIPA_AAAASRVDomainTCP::~CSIPA_AAAASRVDomainTCP()
	{
	delete iQueryBase;
	delete iSrvQuery;
	}

// ----------------------------------------------------------------------------
// CSIPA_AAAASRVDomainTCP::QueryBase
// ----------------------------------------------------------------------------
//	
CSIPQueryBase& CSIPA_AAAASRVDomainTCP::QueryBase() 
	{
	return *iQueryBase;
	}

// ----------------------------------------------------------------------------
// CSIPA_AAAASRVDomainTCP::HandleQueryResultL
// ----------------------------------------------------------------------------
//	
CSIPQueryConditionBase* 
	CSIPA_AAAASRVDomainTCP::HandleQueryResultL( TInt aStatus ) 
	{
	if ( aStatus == KErrNone )
		{
		if ( iServerQuery.QueryData().CreateResultL(
								*static_cast<CSIPQueryA_AAAA*> (iQueryBase) ) )
			{
			iServerQuery.QueryData().CompleteQueryL();
			return NULL;
			}
		}
	//more srvrecords
	iSrvQuery->RemoveElementL( 0 );
	if ( iSrvQuery->ArrayCount() == 0 )
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
		return ( CSIPA_AAAAOrigDomain::NewL( iServerQuery ) );
		}
	iServerQuery.QueryData().SetPort( iSrvQuery->ResultPortL() );
	iServerQuery.QueryData().SetTargetL( iSrvQuery->ResultTargetL() );
	iQueryBase->SetTargetL( iServerQuery.QueryData().Target() );
	return this;	
	}



