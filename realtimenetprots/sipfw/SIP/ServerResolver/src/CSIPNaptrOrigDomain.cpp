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
// Name          : CSIPNaptrOrigDomain.cpp
// Part of       : ServerResolver
// Version       : SIP/4.0 
//



#include "sipstrings.h"
#include "sipstrconsts.h"
#include "CSIPNaptrOrigDomain.h"
#include "CSIPQueryBase.h"
#include "CSIPQueryNaptr.h"
#include "CSIPQuerySrv.h"
#include "CSIPSRVOrigDomainUDP.h"
#include "CSIPSRVNaptrDomain.h"
#include "MSIPServerQuery.h"
#include "CSIPQueryData.h"

// ----------------------------------------------------------------------------
// CSIPNaptrOrigDomain::CSIPNaptrOrigDomain
// ----------------------------------------------------------------------------
//
CSIPNaptrOrigDomain::CSIPNaptrOrigDomain( MSIPServerQuery& aServerQuery )
:CSIPQueryConditionBase(), iServerQuery ( aServerQuery )
	{
	}

// ----------------------------------------------------------------------------
// CSIPNaptrOrigDomain::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPNaptrOrigDomain::ConstructL()
	{
	iQueryBase = 
		     CSIPQueryNaptr::NewL( iServerQuery.QueryData().OriginalTarget(),
		     					   iServerQuery.QueryData().IsSipsUri() );
	}

// ----------------------------------------------------------------------------
// CSIPNaptrOrigDomain::NewL
// ----------------------------------------------------------------------------
//
CSIPNaptrOrigDomain* CSIPNaptrOrigDomain::NewL( MSIPServerQuery& aServerQuery )
	{
	CSIPNaptrOrigDomain* self =
						   new ( ELeave ) CSIPNaptrOrigDomain( aServerQuery );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPNaptrOrigDomain::~CSIPNaptrOrigDomain
// ----------------------------------------------------------------------------
//
CSIPNaptrOrigDomain::~CSIPNaptrOrigDomain()
	{
	delete iQueryBase;
	}

// ----------------------------------------------------------------------------
// CSIPNaptrOrigDomain::QueryBase
// ----------------------------------------------------------------------------
//	
CSIPQueryBase& CSIPNaptrOrigDomain::QueryBase() 
	{
	return *iQueryBase;
	}

// ----------------------------------------------------------------------------
// CSIPNaptrOrigDomain::HandleQueryResultL
// ----------------------------------------------------------------------------
//
CSIPQueryConditionBase* CSIPNaptrOrigDomain::HandleQueryResultL( TInt aStatus ) 
	{
	if ( aStatus == KErrNone )
		{		
		CheckIfOnlyUDPResultsL();

		iQueryBase->HandleQueryResultL( iServerQuery.QueryData().Resolver() );
		if ( iQueryBase->ArrayCount() > 0 )
			{
			iServerQuery.QueryData().SetProtocol(
											iQueryBase->ResultProtocolL() );
			iServerQuery.QueryData().SetTargetL( iQueryBase->ResultTargetL() );
			CSIPQueryConditionBase* conditionbase = 
					     CSIPSRVNaptrDomain::NewL( iServerQuery, iQueryBase );
			iQueryBase = NULL;
			return conditionbase;
			}
		}
	iServerQuery.QueryData().SetPort( 0 );
	if (iServerQuery.QueryData().IsSipsUri())
		{
		iServerQuery.QueryData().SetProtocol(SIPStrings::StringF(
											SipStrConsts::ETLS) );
		}
	else
		{
		iServerQuery.QueryData().SetProtocol(SIPStrings::StringF(
											SipStrConsts::EUDP) );
		}
	
	HBufC8* temp = CSIPQuerySrv::SetTargetProtocolL(
									iServerQuery.QueryData().OriginalTarget(),
					 				iServerQuery.QueryData().Protocol(),
					 				iServerQuery.QueryData().IsSipsUri());
	CleanupStack::PushL( temp );
	iServerQuery.QueryData().SetTargetL( *temp );
	CleanupStack::PopAndDestroy( temp );
	return ( CSIPSRVOrigDomainUDP::NewL( iServerQuery ) );	
	}

// ----------------------------------------------------------------------------
// CSIPNaptrOrigDomain::CheckIfOnlyUDPResultsL
// If the result array has only records where protocol is UDP, the "force UDP"
// flag is set to guarantee the protocol won't be changed by later stages in
// the SIP stack.
// Since the "force UDP" can be set only if CSIPNaptrOrigDomain query returned
// at least one result, the further resolving phases do not change the
// protocol.
// ----------------------------------------------------------------------------
//
void CSIPNaptrOrigDomain::CheckIfOnlyUDPResultsL() const
	{
	__ASSERT_ALWAYS( iQueryBase, User::Leave( KErrNotFound ) );

	//CSIPNaptrOrigDomain::iQueryBase is always CSIPQueryNaptr.
	CSIPQueryNaptr& queryNaptr = static_cast<CSIPQueryNaptr&>( *iQueryBase );
	TBool udpOnly = ( queryNaptr.ArrayCount() > 0 );

	for ( TInt i = 0; i < queryNaptr.ArrayCount() && udpOnly; ++i )
		{
		TDnsRespNAPTR& dnsResp = queryNaptr.NAPTRRecordL( i );
		udpOnly = ( queryNaptr.NaptrProtocol( dnsResp ) ==
			 		SIPStrings::StringF( SipStrConsts::EUDP ) );
		}

	//There is only one query data instance for a single GetByUriL request
	if ( udpOnly )
		{
		iServerQuery.QueryData().SetForceUDP();
		}
	}
