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
// Name          : CSIPSecUserCredentialsProvider.cpp
// Part of       : SIPSec
// Version       : SIP/6.0
//



#include "CSIPSecUserCredentialsProvider.h"
#include "CSIPSecUserRecord.h"
#include "sipsecdigestcache.h"
#include "sipsecdigestcontext.h"
#include "CSIPSecDigestPlugin.h"
#include "MSIPSecUser.h"


// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CSIPSecCredentialsProvider::CSIPSecCredentialsProvider
// ----------------------------------------------------------------------------
//
CSIPSecCredentialsProvider::CSIPSecCredentialsProvider(
	CSIPSecUserRecord& aUserRecord ) :
	iUserRecord( aUserRecord )
    {
    }

// ----------------------------------------------------------------------------
// CSIPSecCredentialsProvider::~CSIPSecCredentialsProvider
// ----------------------------------------------------------------------------
//
CSIPSecCredentialsProvider::~CSIPSecCredentialsProvider() 
    {
    }

// ----------------------------------------------------------------------------
// CSIPSecCredentialsProvider::RequestAgainL
// IMS credentials are never asked again, so no action.
// ----------------------------------------------------------------------------
//
void CSIPSecCredentialsProvider::RequestAgainL()
	{
	}

// ----------------------------------------------------------------------------
// CSIPSecCredentialsProvider::UserRecord
// ----------------------------------------------------------------------------
//
CSIPSecUserRecord& CSIPSecCredentialsProvider::UserRecord() const
    {
    return iUserRecord;
    }

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CSIPSecUserCredentialsProvider::CSIPSecUserCredentialsProvider
// ----------------------------------------------------------------------------
//
CSIPSecUserCredentialsProvider::CSIPSecUserCredentialsProvider(
	CSIPSecUserRecord& aUserRecord ) :
	CSIPSecCredentialsProvider( aUserRecord ),
	iTimer( NULL ),
	iUser( NULL ),
    iTimerId( 0 )
    {    
    }    

// ----------------------------------------------------------------------------
// CSIPSecUserCredentialsProvider::~CSIPSecUserCredentialsProvider
// ----------------------------------------------------------------------------
//
CSIPSecUserCredentialsProvider::~CSIPSecUserCredentialsProvider()
    {
    CSIPSecUserCredentialsProvider::Cancel();
    }

// ----------------------------------------------------------------------------
// CSIPSecUserCredentialsProvider::TimerExpiredL
// ----------------------------------------------------------------------------
//
void CSIPSecUserCredentialsProvider::TimerExpiredL( TTimerId /*aTimerId*/,
													TAny* /*aTimerParam*/ )
    {
  	TRAPD( error, iUser->RequestCredentialsL( UserRecord().Realm() ) )
  	if ( error != KErrNone )
  	    {
  	    UserRecord().Invalidate( CSIPSecUserRecord::ECredentialsFailed );
		// Timer is used only when PassOnlyRealmsToUser()==ETrue, so use
		// KEmptyTransactionId returned by TransactionIdPassedToClient().
  	    iPlugin->SetCredentialsL( UserRecord().TransactionIdPassedToClient(),
  	                              UserRecord().Realm(),
  	                              KNullDesC8,
  	                              KNullDesC8,
  	                              KNullDesC8 );
  	    }
    }

// ----------------------------------------------------------------------------
// CSIPSecUserCredentialsProvider::RequestL
// If application wants the whole SIP response, timer is not started and
// credentials are not requested here.
// ----------------------------------------------------------------------------
//
void CSIPSecUserCredentialsProvider::RequestL( TSIPSecDigestCtxSetup& aContext )
    {
    TSIPSecPluginCtxResponse& parent =
    	static_cast<TSIPSecPluginCtxResponse&>( aContext.Parent() );

    iPlugin = &parent.Plugin();
    iTimer = &parent.Cache().Timer();

	RequestFromUserL();
    }

// ----------------------------------------------------------------------------
// CSIPSecUserCredentialsProvider::Cancel
// ----------------------------------------------------------------------------
//
void CSIPSecUserCredentialsProvider::Cancel()
    {
    if ( iTimer )
        {
    	iTimer->Stop( iTimerId );
        }        
    }    

// ----------------------------------------------------------------------------
// CSIPSecUserCredentialsProvider::RequestAgainL
// ----------------------------------------------------------------------------
//
void CSIPSecUserCredentialsProvider::RequestAgainL()
	{
	// Cancel previous timer, though it should not be running anymore
	Cancel();

	RequestFromUserL();
	}

// ----------------------------------------------------------------------------
// CSIPSecUserCredentialsProvider::RequestFromUserL
// ----------------------------------------------------------------------------
//
void CSIPSecUserCredentialsProvider::RequestFromUserL()
	{
	iUser = const_cast< MSIPSecUser* >( &UserRecord().SIPSecUser() );

    if ( iUser->PassOnlyRealmsToUser() )
    	{
    	iTimerId = iTimer->StartL( this, KSIPSecWaitBeforeRequestUC );
    	}
	}

// End of File
