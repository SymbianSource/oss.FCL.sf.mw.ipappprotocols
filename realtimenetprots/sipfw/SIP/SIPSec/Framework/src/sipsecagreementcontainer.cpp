// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : sipsecagreementcontainer.cpp
// Part of       : SIPSec
// Version       : SIP/6.0
//



#include "sipsecagreementcontainer.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "SipSecUtils.h"
#include "sipstrings.h"

// ----------------------------------------------------------------------------
// CSIPSecAgreementContainer::NewL
// ----------------------------------------------------------------------------
//
CSIPSecAgreementContainer* CSIPSecAgreementContainer::NewL(
				MSIPSecMechanismStore& aMechanismStore )
	{
	CSIPSecAgreementContainer* self = NewLC( aMechanismStore );
	CleanupStack::Pop( self );
	return self;
	}
	
// ----------------------------------------------------------------------------
// CSIPSecAgreementContainer::NewLC
// ----------------------------------------------------------------------------
//			
CSIPSecAgreementContainer* CSIPSecAgreementContainer::NewLC(
				MSIPSecMechanismStore& aMechanismStore )
	{
	CSIPSecAgreementContainer* self = 
		new ( ELeave ) CSIPSecAgreementContainer( aMechanismStore );
	CleanupStack::PushL( self );
	return self;
	}
				
// ----------------------------------------------------------------------------
// CSIPSecAgreementContainer::~CSIPSecAgreementContainer
// ----------------------------------------------------------------------------
//	
CSIPSecAgreementContainer::~CSIPSecAgreementContainer()
	{
	EmptyAgreementList();
	}

// ----------------------------------------------------------------------------
// CSIPSecAgreementContainer::CSIPSecAgreementContainer
// ----------------------------------------------------------------------------
//	
CSIPSecAgreementContainer::CSIPSecAgreementContainer(
						MSIPSecMechanismStore& aMechanismStore ) :
	iMechanismStore( aMechanismStore ),
 	iAgreementList( CSIPSecAgreement::iOffset ),
  	iAgreementListIter( iAgreementList )
	{
	}

// ----------------------------------------------------------------------------
// CSIPSecAgreementContainer::ResponseReceivedL
// ----------------------------------------------------------------------------
//	
void
CSIPSecAgreementContainer::ResponseReceivedL( CSIPResponse& aResponse,
							                  TInetAddr& aNextHopAddr,
							                  const TDesC8& aNextHop,
							                  const MSIPSecUser& aUser,
							                  TBool aUseSecurityServerHeaders )
	{	
	CSIPSecAgreement* agreement = Agreement( aNextHopAddr );
	if ( !agreement &&
		 CSIPSecAgreement::IsSecAgreeResponse( aResponse.ResponseCode() ) &&
		 aResponse.HasHeader(
			SIPStrings::StringF( SipStrConsts::ESecurityServerHeader) ) )
		{
		// Server initiated Sec-Agree
		agreement = CreateAgreementL( aNextHopAddr, aNextHop, aUser );
		}

	if ( agreement )
		{
		agreement->ResponseReceivedL( aResponse,
									  aUser,
									  aUseSecurityServerHeaders );
		}
	}

// ----------------------------------------------------------------------------
// CSIPSecAgreementContainer::AddSecurityParamsL
// Security agreement is not done for ACK.
// If a security agreement has been formed for the next-hop, pass the event to
// the agreement for handling.
// ----------------------------------------------------------------------------
//
void CSIPSecAgreementContainer::AddSecurityParamsL(
	TSIPTransportParams& aTransportParams,
	CSIPRequest& aRequest,
	TInetAddr& aNextHopAddr,
	const TDesC8& aNextHop,
	const CUri8& aRemoteTarget,
	const TDesC8& aOutboundProxy,
	MSIPSecUser& aUser,
	TRegistrationId aRegistrationId )
	{
	if ( !SipSecUtils::Match( SipStrConsts::EAck, aRequest.Method() ) )
		{
		if ( aOutboundProxy.Length() > 0 )
		    {
		    SipSecUtils::AddSupportedHeaderL( aRequest,
		    								  SipStrConsts::ESecAgreeTag );
		    }

		CSIPSecAgreement* agreement = Agreement( aNextHopAddr );
		if ( !agreement &&
			 aRequest.HasHeader(
				SIPStrings::StringF( SipStrConsts::ESecurityClientHeader) ) )
			{
			// Client initiated Sec-Agree
			agreement = CreateAgreementL( aNextHopAddr, aNextHop, aUser );
			}

		if ( agreement )
			{		
			agreement->HandleRequestL( aTransportParams,
									   aRequest,
								   	   aRemoteTarget,
								   	   aOutboundProxy,
								   	   aUser,
								   	   aRegistrationId );
			}

		AddRequiredHeadersIfNeededL( aRequest );
		}
	}

// ----------------------------------------------------------------------------
// CSIPSecAgreementContainer::ClearCache
// ----------------------------------------------------------------------------
//
void CSIPSecAgreementContainer::ClearCache( const MSIPSecUser& aUser )
	{
	if ( !iAgreementList.IsEmpty() )
		{
		CSIPSecAgreement* agreement = NULL;
		iAgreementListIter.SetToFirst();

		while ( ( agreement = iAgreementListIter++ ) != NULL )
			{
			if ( agreement->RemoveUser( aUser ) )
				{
				iAgreementList.Remove( *agreement );
				delete agreement;
				}
			}
		}
	}

// ----------------------------------------------------------------------------
// CSIPSecAgreementContainer::CancelPendingOperations
// ----------------------------------------------------------------------------
//
void CSIPSecAgreementContainer::CancelPendingOperations(
									MSIPSecObserver* /*aObserver*/ )
	{
	}

// ----------------------------------------------------------------------------
// CSIPSecAgreementContainer::NegotiatedSecurityMechanismL
// ----------------------------------------------------------------------------
//
HBufC8*
CSIPSecAgreementContainer::NegotiatedSecurityMechanismL( const TDesC8& aHop )
	{
	if ( !iAgreementList.IsEmpty() )
		{
		CSIPSecAgreement* agreement = NULL;
		iAgreementListIter.SetToFirst();
		while ( ( agreement = iAgreementListIter++ ) != NULL )
			{
			if ( agreement->NextHop( aHop ) )
				{
				return agreement->MechanismNameL();
				}
			}
		}

	return NULL;
	}

// ----------------------------------------------------------------------------
// CSIPSecAgreementContainer::Agreement
// ----------------------------------------------------------------------------
//	
CSIPSecAgreement*
CSIPSecAgreementContainer::Agreement( const TInetAddr& aNextHopAddr )
	{
	if ( !iAgreementList.IsEmpty() )
		{
		CSIPSecAgreement* agreement = NULL;
		iAgreementListIter.SetToFirst();
		while ( ( agreement = iAgreementListIter++ ) != NULL )
			{
			if ( agreement->NextHop( aNextHopAddr ) )
				{
				return agreement;
				}
			}
		}
	return NULL;
	}

// ----------------------------------------------------------------------------
// CSIPSecAgreementContainer::CreateAgreementL
// ----------------------------------------------------------------------------
//
CSIPSecAgreement*
CSIPSecAgreementContainer::CreateAgreementL( TInetAddr& aNextHopAddr,
							                 const TDesC8& aNextHop,
							                 const MSIPSecUser& aUser )
	{
	CSIPSecAgreement* agreement = CSIPSecAgreement::NewLC( aNextHopAddr,
														   aNextHop,
														   aUser,
														   iMechanismStore );
	iAgreementList.AddLast( *agreement );
	CleanupStack::Pop( agreement );
	return agreement;
	}

// ----------------------------------------------------------------------------
// CSIPSecAgreementContainer::EmptyAgreementList
// ----------------------------------------------------------------------------
//		
void CSIPSecAgreementContainer::EmptyAgreementList()
	{
	if ( !iAgreementList.IsEmpty() )
		{
		CSIPSecAgreement* agreement = NULL;
		iAgreementListIter.SetToFirst();
		while ( ( agreement = iAgreementListIter++ ) != NULL )
			{
			iAgreementList.Remove( *agreement );
			delete agreement;
			}
		}
	}
	
// ----------------------------------------------------------------------------
// CSIPSecAgreementContainer::AddRequiredHeadersIfNeededL
// ----------------------------------------------------------------------------
//
void CSIPSecAgreementContainer::AddRequiredHeadersIfNeededL(
	CSIPRequest& aSIPRequest )
	{
	if ( aSIPRequest.HasHeader(
			SIPStrings::StringF( SipStrConsts::ESecurityClientHeader ) )
		||
		aSIPRequest.HasHeader(
			SIPStrings::StringF( SipStrConsts::ESecurityVerifyHeader ) ) )
		{
		SipSecUtils::AddRequireHeadersL( aSIPRequest,
										 SipStrConsts::ESecAgreeTag );
		}
	}
