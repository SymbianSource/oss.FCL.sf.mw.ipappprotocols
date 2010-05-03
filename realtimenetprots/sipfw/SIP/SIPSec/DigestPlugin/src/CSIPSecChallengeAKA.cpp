// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CSIPSecChallengeAKA.cpp
// Part of       : SIPSec/DigestPlugin
// Version       : SIP/6.0
//



#include "SipLogs.h"
#include "CSIPSecChallengeAKA.h"
#include "CSIPSecDigest.h"
#include "CSIPSecSIMRecord.h"
#include "CSIPSecDigestPlugin.h"
#include "sipsecdigestcontext.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include <tconvbase64.h>

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CSIPSecChallengeAKA::CSIPSecAlgAKA
// -----------------------------------------------------------------------------
//
CSIPSecChallengeAKA::CSIPSecChallengeAKA( CSIPSecDigest::TChallengeType aType,
										  CSIPSecRequestData::TQop aQop ) :
    CSIPSecChallengeMD5( aType, aQop )
    {
    }

// -----------------------------------------------------------------------------
// CSIPSecChallengeAKA::NewLC
// -----------------------------------------------------------------------------
//
CSIPSecChallengeAKA*
CSIPSecChallengeAKA::NewLC( CSIPSecDigest::TChallengeType aType,
                            CSIPAuthenticateHeaderBase& aAuthenticateHeader,
                            CSIPSecRequestData::TQop aQop )
    {
	CSIPSecChallengeAKA* self = new ( ELeave ) CSIPSecChallengeAKA( aType,
																	aQop );
	CleanupStack::PushL( self );
	self->ConstructL( aAuthenticateHeader );
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPSecChallengeAKA::~CSIPSecChallengeAKA
// -----------------------------------------------------------------------------
//
CSIPSecChallengeAKA::~CSIPSecChallengeAKA()
    {
    }

// -----------------------------------------------------------------------------
// CSIPSecChallengeAKA::VerifyTypeL
// -----------------------------------------------------------------------------
//
void CSIPSecChallengeAKA::VerifyTypeL( CSIPSecUserRecord::TType aType ) const
	{
	__ASSERT_ALWAYS( aType == CSIPSecUserRecord::EIMS,
        			 User::Leave( KErrPermissionDenied ) );
	}

// -----------------------------------------------------------------------------
// CSIPSecChallengeAKA::CreateUserRecordL
// -----------------------------------------------------------------------------
//
CSIPSecUserRecord*
CSIPSecChallengeAKA::CreateUserRecordL( const TDesC8& aRealm,
										const MSIPSecUser& aUser,
										TRegistrationId aRegistrationId,
									    TTransactionId aTransactionId,
									    TUint aResponseCode ) const
	{
	CSIPSecSIMRecord* record = CSIPSecSIMRecord::NewL( aRealm,
													   aUser,
													   aRegistrationId,
													   aTransactionId );
	record->UpdateMessageCounter( aResponseCode );
	return record;
	}

// -----------------------------------------------------------------------------
// CSIPSecChallengeAKA::SupportedAlgorithm
// -----------------------------------------------------------------------------
//
RStringF CSIPSecChallengeAKA::SupportedAlgorithm()
    {
    return SIPStrings::StringF( SipStrConsts::EAKAv1MD5 );
    }

// -----------------------------------------------------------------------------
// CSIPSecChallengeAKA::AlgorithmName
// -----------------------------------------------------------------------------
//
RStringF CSIPSecChallengeAKA::AlgorithmName() const
    {
    return CSIPSecChallengeAKA::SupportedAlgorithm();
    }

// -----------------------------------------------------------------------------
// CSIPSecChallengeAKA::ProcessResponseL
// -----------------------------------------------------------------------------
//
TBool CSIPSecChallengeAKA::ProcessResponseL( TSIPSecDigestCtxSetup& aContext,
									   	     TBool& aAskCredentials ) const
    {
    // AKA gets credentials from SIM card, not from MSIPSecUser
    aAskCredentials = EFalse;

	CSIPSecSIMRecord& userData =
		static_cast< CSIPSecSIMRecord& >( aContext.UserData() );
    if ( userData.IsValid() )
    	{
    	if ( userData.Failure() == CSIPSecSIMRecord::ESQN )
            {
            const TInt KBase64EncodedAutsLength = 20;
			HBufC8* auts = HBufC8::NewLC( KBase64EncodedAutsLength );
			TPtr8 autsPtr( auts->Des() );

			TBase64 encoder;
			User::LeaveIfError( encoder.Encode( userData.AUTS(), autsPtr ) );
            aContext.SetParamValueL( SipStrConsts::EAuts, *auts );

            CleanupStack::PopAndDestroy( auts );
            }
        if ( userData.SecretData().Length() == 0 )
            {
            CalculateSecretDataA1L( aContext );
            }
        return EFalse;
    	}

    aContext.RequestUserCredentialsL();
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CSIPSecChallengeAKA::ProcessRequestL
// -----------------------------------------------------------------------------
//
void
CSIPSecChallengeAKA::ProcessRequestL( TSIPSecDigestCtxProcess& aContext ) const
    {
    __SIP_LOG( "SIPSecChallAKA:ProcessRequestL" )

    CSIPSecSIMRecord& userData =
    	static_cast< CSIPSecSIMRecord& >( aContext.UserData() );
    __ASSERT_ALWAYS ( userData.Failure() != CSIPSecSIMRecord::EPrivateIdentity,
    				  User::Leave( KErrGeneral ) );

    if ( userData.Failure() == CSIPSecSIMRecord::EMAC )
        {
        __SIP_LOG( "MAC failure, set empty nonce&response" )

		aContext.SetParamWithEmptyValueL( SipStrConsts::ENonce );
        aContext.SetParamWithEmptyValueL( SipStrConsts::EResponse );
        }
    else
    	{
    	if ( userData.Failure() == CSIPSecSIMRecord::ESQN &&
    		 aContext.Parent().Plugin().EmptyResponseAfterSqnFailure() )
    		{
    		__SIP_LOG( "set empty response parameter after SQN" )
    		aContext.SetParamWithEmptyValueL( SipStrConsts::EResponse );
    		}
    	else
    		{
    		CSIPSecChallengeMD5::ProcessRequestL( aContext );
    		}
        }
    }
