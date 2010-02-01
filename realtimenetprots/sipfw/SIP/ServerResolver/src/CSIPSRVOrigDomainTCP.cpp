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
// Name          : CSIPSRVOrigDomainTCP.cpp
// Part of       : ServerResolver
// Version       : SIP/4.0 
//



#include "sipstrings.h"
#include "sipstrconsts.h"
#include "CSIPSRVOrigDomainTCP.h"
#include "CSIPA_AAAAOrigDomain.h"
#include "CSIPA_AAAASRVDomainTCP.h"
#include "CSIPQueryBase.h"
#include "CSIPQuerySrv.h"
#include "MSIPServerQuery.h"
#include "CSIPQueryData.h"
#include "MSIPHostResolver.h"

// ----------------------------------------------------------------------------
// CSIPSRVOrigDomainTCP::CSIPSRVOrigDomainTCP
// ----------------------------------------------------------------------------
//
CSIPSRVOrigDomainTCP::CSIPSRVOrigDomainTCP( MSIPServerQuery& aServerQuery )
:CSIPQueryConditionBase(), iServerQuery ( aServerQuery )
	{
	}

// ----------------------------------------------------------------------------
// CSIPSRVOrigDomainTCP::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPSRVOrigDomainTCP::ConstructL()
	{
	iQueryBase = CSIPQuerySrv::NewL( iServerQuery.QueryData().Target(),
									 iServerQuery.QueryData().IsSipsUri() );
	}

// ----------------------------------------------------------------------------
// CSIPSRVOrigDomainTCP::NewL
// ----------------------------------------------------------------------------
//
CSIPSRVOrigDomainTCP* 
	CSIPSRVOrigDomainTCP::NewL( MSIPServerQuery& aServerQuery )
	{
	CSIPSRVOrigDomainTCP* self = 
						new ( ELeave ) CSIPSRVOrigDomainTCP( aServerQuery );
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPSRVOrigDomainTCP::~CSIPSRVOrigDomainTCP
// ----------------------------------------------------------------------------
//
CSIPSRVOrigDomainTCP::~CSIPSRVOrigDomainTCP()
	{
	delete iQueryBase;
	}

// ----------------------------------------------------------------------------
// CSIPSRVOrigDomainTCP::QueryBase
// ----------------------------------------------------------------------------
//	
CSIPQueryBase& CSIPSRVOrigDomainTCP::QueryBase() 
	{
	return *iQueryBase;
	}

// ----------------------------------------------------------------------------
// CSIPSRVOrigDomainTCP::HandleQueryL
// ----------------------------------------------------------------------------
//	
CSIPQueryConditionBase* CSIPSRVOrigDomainTCP::HandleQueryResultL( TInt aStatus ) 
	{
	if ( aStatus == KErrNone )
		{
		iQueryBase->HandleQueryResultL( iServerQuery.QueryData().Resolver() );
		if( iQueryBase->ArrayCount() > 0 )
			{
			//iProtocol is tcp
		    iServerQuery.QueryData().SetPort( iQueryBase->ResultPortL() );
		    iServerQuery.QueryData().SetTargetL( iQueryBase->ResultTargetL() );
		    CSIPQueryConditionBase* conditionbase = 
		    	( CSIPA_AAAASRVDomainTCP::NewL( iServerQuery, iQueryBase ));
		    iQueryBase = NULL;//ownership is transferred
		    return conditionbase;
			}
		}
		
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
	return ( CSIPA_AAAAOrigDomain::NewL( iServerQuery ));
	}

