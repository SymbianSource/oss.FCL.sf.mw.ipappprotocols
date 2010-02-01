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
// Name          : CSIPSRVOrigDomainUDP.cpp
// Part of       : ServerResolver
// Version       : SIP/4.0 
//



#include "sipstrings.h"
#include "sipstrconsts.h"
#include "CSIPSRVOrigDomainUDP.h"
#include "CSIPSRVOrigDomainTCP.h"
#include "CSIPA_AAAASRVDomainUDP.h"
#include "CSIPA_AAAAOrigDomain.h"
#include "CSIPQueryBase.h"
#include "CSIPQuerySrv.h"
#include "MSIPServerQuery.h"
#include "CSIPQueryData.h"
#include "MSIPHostResolver.h"

// ----------------------------------------------------------------------------
// CSIPSRVOrigDomainUDP::CSIPSRVOrigDomainUDP
// ----------------------------------------------------------------------------
//
CSIPSRVOrigDomainUDP::CSIPSRVOrigDomainUDP( MSIPServerQuery& aServerQuery )
:CSIPQueryConditionBase(), iServerQuery ( aServerQuery )
	{
	}

// ----------------------------------------------------------------------------
// CSIPSRVOrigDomainUDP::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPSRVOrigDomainUDP::ConstructL()
	{
	iQueryBase = CSIPQuerySrv::NewL( iServerQuery.QueryData().Target(), 
									 iServerQuery.QueryData().IsSipsUri() );
	}

// ----------------------------------------------------------------------------
// CSIPSRVOrigDomainUDP::NewL
// ----------------------------------------------------------------------------
//
CSIPSRVOrigDomainUDP* 
	CSIPSRVOrigDomainUDP::NewL( MSIPServerQuery& aServerQuery )
	{
	CSIPSRVOrigDomainUDP* self =
					       new ( ELeave ) CSIPSRVOrigDomainUDP( aServerQuery );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPSRVOrigDomainUDP::~CSIPSRVOrigDomainUDP
// ----------------------------------------------------------------------------
//
CSIPSRVOrigDomainUDP::~CSIPSRVOrigDomainUDP()
	{
	delete iQueryBase;
	}

// ----------------------------------------------------------------------------
// CSIPSRVOrigDomainUDP::QueryBase
// ----------------------------------------------------------------------------
//	
CSIPQueryBase& CSIPSRVOrigDomainUDP::QueryBase() 
	{
	return *iQueryBase;
	}

// ----------------------------------------------------------------------------
// CSIPSRVOrigDomainUDP::HandleQueryL
// ----------------------------------------------------------------------------
//	
CSIPQueryConditionBase* 
	CSIPSRVOrigDomainUDP::HandleQueryResultL( TInt aStatus ) 
	{
	if ( aStatus == KErrNone )
		{
		iQueryBase->HandleQueryResultL( iServerQuery.QueryData().Resolver() );
		if ( iQueryBase->ArrayCount() > 0	)
			{
			iServerQuery.QueryData().SetPort( iQueryBase->ResultPortL() );
		    iServerQuery.QueryData().SetTargetL( iQueryBase->ResultTargetL() );
		    CSIPQueryConditionBase* conditionbase = 
		    	( CSIPA_AAAASRVDomainUDP::NewL( iServerQuery, iQueryBase ));
		    iQueryBase = NULL;//ownership is transferred
		    return conditionbase;
			}
		}
		
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
		iServerQuery.QueryData().SetProtocol(
			SIPStrings::StringF( SipStrConsts::ETCP ) );
		temp = CSIPQuerySrv::SetTargetProtocolL( 
									iServerQuery.QueryData().OriginalTarget(),
									iServerQuery.QueryData().Protocol(),
									iServerQuery.QueryData().IsSipsUri() );
		}	
	CleanupStack::PushL( temp );
	iServerQuery.QueryData().SetTargetL( *temp );
	CleanupStack::PopAndDestroy( temp );
	return ( CSIPSRVOrigDomainTCP::NewL( iServerQuery ));
	}



