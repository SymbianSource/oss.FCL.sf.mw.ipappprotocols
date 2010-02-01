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
// Name          : CSIPA_AAAASRVDomainOrig.cpp
// Part of       : ServerResolver
// Version       : SIP/4.0 
//



#include "sipstrings.h"
#include "sipstrconsts.h"
#include "CSIPA_AAAASRVDomainOrig.h"
#include "CSIPA_AAAAOrigDomain.h"
#include "CSIPSRVOrigDomainTCP.h"
#include "CSIPQueryBase.h"
#include "CSIPQuerySrv.h"
#include "CSIPQueryA_AAAA.h"
#include "MSIPServerQuery.h"
#include "CSIPQueryData.h"

// ----------------------------------------------------------------------------
// CSIPA_AAAASRVDomainOrig::CSIPA_AAAASRVDomainOrig
// ----------------------------------------------------------------------------
//
CSIPA_AAAASRVDomainOrig::CSIPA_AAAASRVDomainOrig( MSIPServerQuery& aServerQuery )
:CSIPQueryConditionBase(), iServerQuery ( aServerQuery )
	{
	}

// ----------------------------------------------------------------------------
// CSIPA_AAAASRVDomainOrig::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPA_AAAASRVDomainOrig::ConstructL()
	{
	iQueryBase = CSIPQueryA_AAAA::NewL( 
								  iServerQuery.QueryData().Target() );
	}

// ----------------------------------------------------------------------------
// CSIPA_AAAASRVDomainOrig::NewL
// ----------------------------------------------------------------------------
//
CSIPA_AAAASRVDomainOrig* 
	CSIPA_AAAASRVDomainOrig::NewL( MSIPServerQuery& aServerQuery )
	{
	CSIPA_AAAASRVDomainOrig* self = 
				new ( ELeave ) CSIPA_AAAASRVDomainOrig( aServerQuery );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPA_AAAASRVDomainOrig::~CSIPA_AAAASRVDomainOrig
// ----------------------------------------------------------------------------
//
CSIPA_AAAASRVDomainOrig::~CSIPA_AAAASRVDomainOrig()
	{
	delete iQueryBase;
	}

// ----------------------------------------------------------------------------
// CSIPA_AAAASRVDomainOrig::QueryBase
// ----------------------------------------------------------------------------
//	
CSIPQueryBase& CSIPA_AAAASRVDomainOrig::QueryBase() 
	{
	return *iQueryBase;
	}

// ----------------------------------------------------------------------------
// CSIPA_AAAASRVDomainOrig::HandleQueryL
// ----------------------------------------------------------------------------
//	
CSIPQueryConditionBase* 
	CSIPA_AAAASRVDomainOrig::HandleQueryResultL( TInt aStatus ) 
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
	if (iServerQuery.QueryData().IsSipsUri())
		{
		iServerQuery.QueryData().SetPort( KSIPsDefaultPort );
		iServerQuery.QueryData().SetProtocol(
				SIPStrings::StringF( SipStrConsts::ETLS ) );
		}
	else
		{
		iServerQuery.QueryData().SetPort( KSIPDefaultPort );
		}	
	iServerQuery.QueryData().SetTargetL(
								 iServerQuery.QueryData().OriginalTarget() );
	return ( CSIPA_AAAAOrigDomain::NewL( iServerQuery ));
	}
