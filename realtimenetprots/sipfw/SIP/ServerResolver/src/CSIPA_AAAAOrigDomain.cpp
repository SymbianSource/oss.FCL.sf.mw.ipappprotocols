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
// Name          : CSIPA_AAAAOrigDomain.cpp
// Part of       : ServerResolver
// Version       : SIP/4.0 
//



#include "CSIPA_AAAAOrigDomain.h"
#include "CSIPQueryBase.h"
#include "CSIPQueryA_AAAA.h"
#include "MSIPServerQuery.h"
#include "CSIPQueryData.h"
#include "MSIPServerResolverObserver.h"
#include "MSIPHostResolver.h"

// ----------------------------------------------------------------------------
// CSIPA_AAAAOrigDomain::CSIPA_AAAAOrigDomain
// ----------------------------------------------------------------------------
//
CSIPA_AAAAOrigDomain::CSIPA_AAAAOrigDomain( MSIPServerQuery& aServerQuery )
:CSIPQueryConditionBase(), iServerQuery ( aServerQuery )
	{
	}

// ----------------------------------------------------------------------------
// CSIPA_AAAAOrigDomain::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPA_AAAAOrigDomain::ConstructL()
	{
	iQueryBase = CSIPQueryA_AAAA::NewL( iServerQuery.QueryData().Target() );
	}

// ----------------------------------------------------------------------------
// CSIPA_AAAAOrigDomain::NewL
// ----------------------------------------------------------------------------
//
CSIPA_AAAAOrigDomain* 
	CSIPA_AAAAOrigDomain::NewL( MSIPServerQuery& aServerQuery )
	{
	CSIPA_AAAAOrigDomain* self =
						   new ( ELeave ) CSIPA_AAAAOrigDomain( aServerQuery );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPA_AAAAOrigDomain::~CSIPA_AAAAOrigDomain
// ----------------------------------------------------------------------------
//
CSIPA_AAAAOrigDomain::~CSIPA_AAAAOrigDomain()
	{
	delete iQueryBase;
	}

// ----------------------------------------------------------------------------
// CSIPA_AAAAOrigDomain::QueryBase
// ----------------------------------------------------------------------------
//	
CSIPQueryBase& CSIPA_AAAAOrigDomain::QueryBase() 
	{
	return *iQueryBase;
	}

// ----------------------------------------------------------------------------
// CSIPA_AAAAOrigDomain::HandleQueryResultL
// ----------------------------------------------------------------------------
//	
CSIPQueryConditionBase* 
	CSIPA_AAAAOrigDomain::HandleQueryResultL( TInt aStatus ) 
	{
	if ( aStatus == KErrNone )
		{
		if ( iServerQuery.QueryData().CreateResultL(
							  *static_cast<CSIPQueryA_AAAA*> ( iQueryBase ) ) )
			{
			iServerQuery.QueryData().CompleteQueryL();
			}
		else
			{
			iServerQuery.QueryData().Resolver().SetIPListFailed(ETrue);
			}					  
		}
	return NULL;	
	}
