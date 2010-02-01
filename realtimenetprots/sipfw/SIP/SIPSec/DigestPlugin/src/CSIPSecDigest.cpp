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
// Name          : CSIPSecDigest.cpp
// Part of       : SIPDigestPlugin
// Version       : SIP/6.0
//



#include "sipsecurityserverheader.h"
#include "CSIPSecDigest.h"
#include "sipsecdigestcache.h"
#include "CSIPSecChallenge.h"
#include "RSIPSecChallengeResolver.h"
#include "CSIPSecCredentials.h"
#include "sipsecdigestcontext.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "MSIPSecUser.h"
#include <hash.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPSecDigest::NewL
// -----------------------------------------------------------------------------
//
CSIPSecDigest* CSIPSecDigest::NewL()
    {
	CSIPSecDigest* self = new( ELeave )CSIPSecDigest();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
    }

// -----------------------------------------------------------------------------
// CSIPSecDigest::CSIPSecDigest
// -----------------------------------------------------------------------------
//
CSIPSecDigest::CSIPSecDigest()
    {
    }

// -----------------------------------------------------------------------------
// CSIPSecDigest::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPSecDigest::ConstructL()
    {
    SIPStrings::OpenL();
    iStringPoolOpened = ETrue;

    iAlgorithm = SIPStrings::StringF( SipStrConsts::EEmpty );

    iMessageDigest = CMD5::NewL();
    }

// -----------------------------------------------------------------------------
// CSIPSecDigest::~CSIPSecDigest
// -----------------------------------------------------------------------------
//
CSIPSecDigest::~CSIPSecDigest()
    {
    delete iMessageDigest;
    iAlgorithm.Close();

    if ( iStringPoolOpened )
    	{
    	SIPStrings::Close();
    	}
    }

// -----------------------------------------------------------------------------
// CSIPSecDigest::Name
// -----------------------------------------------------------------------------
//
const TDesC8& CSIPSecDigest::Name() const
    {
    return KSIPSecDigestName;
    }

// -----------------------------------------------------------------------------
// CSIPSecDigest::InitializeL
// -----------------------------------------------------------------------------
//
void CSIPSecDigest::InitializeL( CSIPSecurityServerHeader& aSecurityServer )
    {
    SetDefaultAlgorithm( aSecurityServer.ParamValue(
    	SIPStrings::StringF( SipStrConsts::EDigestAlgorithm ) ) );
    }

// -----------------------------------------------------------------------------
// CSIPSecDigest::UpdateCacheL
// -----------------------------------------------------------------------------
//
TBool CSIPSecDigest::UpdateCacheL( TSIPSecPluginCtxResponse& aContext )
    {
    TBool mustWait( EFalse );
    // EFalse = all challenges in the SIP response have found credentials from
    // the cache, or are already waiting credentials from application.
    TBool askCredentials( EFalse );
    CSIPSecChallenge* challenge( NULL );
    CSIPSecCredentials* credentials( NULL );

	RSIPSecChallengeResolver challenges( aContext, *this );
    CleanupClosePushL( challenges );
    challenges.OpenL( aContext.SIPSecUser() );

    // Create one cache entry (CSIPSecCredentials) for each challenge.
    while ( ( challenge = challenges.Pop() ) != NULL )
        {
        CleanupStack::PushL( challenge );
		credentials = CSIPSecCredentials::NewL( challenge, *this, aContext );
		CleanupStack::Pop( challenge );
        aContext.Cache().AddEntry( credentials );

        TSIPSecDigestCtxSetup ctx( *credentials,
        						   aContext.TransactionId(),
        						   &aContext );
		TBool ask( EFalse );
		// AKA challenges set "ask" to EFalse, as they use SIM card
        mustWait = credentials->DigestUpdateL( ctx, ask ) || mustWait;
        askCredentials = askCredentials || ask;
        }

	MSIPSecUser& sipSecUser = aContext.SIPSecUser();
	MSIPSecUser* trustedUser = const_cast< MSIPSecUser* >
		( sipSecUser.TrustedUser( aContext.RegistrationId() ) );
	MSIPSecUser* user = trustedUser ? trustedUser : &sipSecUser;
	if ( mustWait && !user->PassOnlyRealmsToUser() && askCredentials )
		{
		user->RequestCredentialsL( aContext.SIPResponse(),
								  aContext.TransactionId(),
								  KEmptyRefreshId );
		}

    CleanupStack::PopAndDestroy(); // challenges
    return mustWait;
    }

// -----------------------------------------------------------------------------
// CSIPSecDigest::CSIPSecDigest
// -----------------------------------------------------------------------------
//
CMD5& CSIPSecDigest::MessageDigest()
    {
    return *iMessageDigest;
    }

// -----------------------------------------------------------------------------
// CSIPSecDigest::Algorithm
// -----------------------------------------------------------------------------
//
RStringF CSIPSecDigest::Algorithm() const
    {
    return iAlgorithm;
    }

// -----------------------------------------------------------------------------
// CSIPSecDigest::SetDefaultAlgorithm
// -----------------------------------------------------------------------------
//
void CSIPSecDigest::SetDefaultAlgorithm( RStringF aAlgorithm )
	{
	iAlgorithm.Close();
	iAlgorithm = aAlgorithm.Copy();
	}
