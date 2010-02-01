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
// Name          : CSIPA_AAAASRVDomainUDP.cpp
// Part of       : ServerResolver
// Version       : SIP/4.0 
//



#include "sipstrings.h"
#include "sipstrconsts.h"
#include "CSIPA_AAAASRVDomainUDP.h"
#include "CSIPA_AAAAOrigDomain.h"
#include "CSIPSRVOrigDomainTCP.h"
#include "CSIPQueryBase.h"
#include "CSIPQuerySrv.h"
#include "CSIPQueryA_AAAA.h"
#include "MSIPServerQuery.h"
#include "CSIPQueryData.h"

// ----------------------------------------------------------------------------
// CSIPA_AAAASRVDomainUDP::CSIPA_AAAASRVDomainUDP
// ----------------------------------------------------------------------------
//
CSIPA_AAAASRVDomainUDP::CSIPA_AAAASRVDomainUDP( MSIPServerQuery& aServerQuery )
:CSIPQueryConditionBase(), iServerQuery ( aServerQuery )
	{
	}

// ----------------------------------------------------------------------------
// CSIPA_AAAASRVDomainUDP::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPA_AAAASRVDomainUDP::ConstructL( CSIPQueryBase* aSrvQuery )
	{
	iQueryBase = CSIPQueryA_AAAA::NewL( iServerQuery.QueryData().Target() );
	iSrvQuery = aSrvQuery;
	}

// ----------------------------------------------------------------------------
// CSIPA_AAAASRVDomainUDP::NewL
// ----------------------------------------------------------------------------
//
CSIPA_AAAASRVDomainUDP* 
	CSIPA_AAAASRVDomainUDP::NewL( MSIPServerQuery& aServerQuery,
								CSIPQueryBase* aSrvQuery )
	{
	CSIPA_AAAASRVDomainUDP* self = 
				new ( ELeave ) CSIPA_AAAASRVDomainUDP( aServerQuery );
	CleanupStack::PushL( self );
	self->ConstructL( aSrvQuery );
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPA_AAAASRVDomainUDP::~CSIPA_AAAASRVDomainUDP
// ----------------------------------------------------------------------------
//
CSIPA_AAAASRVDomainUDP::~CSIPA_AAAASRVDomainUDP()
	{
	delete iQueryBase;
	delete iSrvQuery;
	}

// ----------------------------------------------------------------------------
// CSIPA_AAAASRVDomainUDP::QueryBase
// ----------------------------------------------------------------------------
//	
CSIPQueryBase& CSIPA_AAAASRVDomainUDP::QueryBase() 
	{
	return *iQueryBase;
	}

// ----------------------------------------------------------------------------
// CSIPA_AAAASRVDomainUDP::HandleQueryL
// ----------------------------------------------------------------------------
//	
CSIPQueryConditionBase* 
	CSIPA_AAAASRVDomainUDP::HandleQueryResultL( TInt aStatus ) 
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
		HBufC8* temp = 0;	
		if (iServerQuery.QueryData().IsSipsUri())
			{
			iServerQuery.QueryData().SetProtocol(
				SIPStrings::StringF( SipStrConsts::ETLS ) );
			temp = CSIPQuerySrv::SetTargetTLSL( 
									iServerQuery.QueryData().OriginalTarget());
			}
		else
			{
			iServerQuery.QueryData().SetPort( 0 );
			iServerQuery.QueryData().SetProtocol(
				SIPStrings::StringF( SipStrConsts::ETCP ) );
			temp = CSIPQuerySrv::SetTargetProtocolL(
				iServerQuery.QueryData().OriginalTarget(),
				SIPStrings::StringF( SipStrConsts::ETCP ),
				iServerQuery.QueryData().IsSipsUri() );
			}
		CleanupStack::PushL( temp );
		iServerQuery.QueryData().SetTargetL( *temp );
		CleanupStack::PopAndDestroy( temp );
		return ( CSIPSRVOrigDomainTCP::NewL( iServerQuery ));
		}
	iServerQuery.QueryData().SetPort( iSrvQuery->ResultPortL() );
	iServerQuery.QueryData().SetTargetL( iSrvQuery->ResultTargetL() );
	iQueryBase->SetTargetL( iServerQuery.QueryData().Target() );
	return this;	
	}
