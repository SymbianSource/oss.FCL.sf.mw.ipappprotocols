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
// Name          : CSIPSecUserRecord.cpp
// Part of       : SIPSec
// Version       : SIP/6.0
//



#include "SipLogs.h"
#include "MSIPSecUser.h"
#include "CSIPSecUserRecord.h"
#include "CSIPSecDigestObserver.h"
#include "CSIPSecUserCredentialsProvider.h"
#include "sipsecdigestcontext.h"
#include "CSIPSecDigestCacheEntry.h"
#include "RecordCleanup.h"
#include "SipAssert.h"
#include "sipresponse.h"


const TInt CSIPSecUserRecord::iSIPSecOffset = _FOFF( CSIPSecUserRecord, iLink );


// ============================ MEMBER FUNCTIONS ===============================


// ----------------------------------------------------------------------------
// CSIPSecUserRecord::NewL
// ----------------------------------------------------------------------------
//
CSIPSecUserRecord* CSIPSecUserRecord::NewL( const TDesC8& aRealm,
											const MSIPSecUser& aUser,
											TRegistrationId aRegistrationId,
											TTransactionId aTransactionId ) 
	{
	CSIPSecUserRecord* self =
		NewLC( aRealm, aUser, aRegistrationId, aTransactionId );
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::NewLC
// ----------------------------------------------------------------------------
//	
CSIPSecUserRecord* CSIPSecUserRecord::NewLC( const TDesC8& aRealm,
											 const MSIPSecUser& aUser,
											 TRegistrationId aRegistrationId,
											 TTransactionId aTransactionId )
	{
	CSIPSecUserRecord* self =
		new ( ELeave ) CSIPSecUserRecord( EIETF,
									      aUser,
									      aRegistrationId,
									      aTransactionId );
	CleanupStack::PushL( self );
	self->ConstructL( aRealm );
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::CSIPSecUserRecord
// ----------------------------------------------------------------------------
//
CSIPSecUserRecord::CSIPSecUserRecord( CSIPSecUserRecord::TType aType,
									  const MSIPSecUser& aUser,
									  TRegistrationId aRegistrationId,
									  TTransactionId aTransactionId ) :
	iType( aType ),
    iState( ENoState ),
    iValid( EFalse ),
    iUser( &aUser ),
    iSecretData( NULL ),
#ifdef CPPUNIT_TEST
	// Use granularity 1 to get all append operations to allocate memory
  	iObservers( 1 ),
#endif
	iRegistrationId( aRegistrationId ),
	iTransactionId( aTransactionId )
	{
	}

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPSecUserRecord::ConstructL( const TDesC8& aRealm )
	{
	__SIP_ASSERT_LEAVE( aRealm.Length() > 0, KErrArgument );

	iRealm = aRealm.AllocL();
	iProvider = CreateProviderL();
	}

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::~CSIPSecUserRecord
// ----------------------------------------------------------------------------
//
CSIPSecUserRecord::~CSIPSecUserRecord()
	{
    Updated();

	delete iRealm;
	delete iUserName;
	delete iPassword;
	delete iSecretData;
	delete iOutboundProxy;

	if ( iProvider )
		{
		iProvider->Cancel();
		delete iProvider;
		}

    iObservers.Reset();
    iObservers.Close();
	}

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::Type
// ----------------------------------------------------------------------------
//
CSIPSecUserRecord::TType CSIPSecUserRecord::Type() const
    {
    return iType;
    }

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::Realm
// ----------------------------------------------------------------------------
//
const TDesC8& CSIPSecUserRecord::Realm() const
	{
	return *iRealm;
	}

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::IsSameRealm
// ----------------------------------------------------------------------------
//
TBool CSIPSecUserRecord::IsSameRealm( const TDesC8& aRealm ) const
	{
	return Realm().Compare( aRealm ) == 0;
	}

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::CompareUser
// ----------------------------------------------------------------------------
//
TBool CSIPSecUserRecord::CompareUser( const MSIPSecUser& aUser,
									  TRegistrationId aRegistrationId ) const
	{	
	return CSIPSecDigestCacheEntry::CompareUsers( aUser,
												  *iUser,
												  ETrue,
												  aRegistrationId );
	}

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::TransactionId
// ----------------------------------------------------------------------------
//
TTransactionId CSIPSecUserRecord::TransactionId() const
	{
	return iTransactionId;
	}

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::TransactionIdPassedToClient
// ----------------------------------------------------------------------------
//
TTransactionId CSIPSecUserRecord::TransactionIdPassedToClient() const
	{
	return SIPSecUser().PassOnlyRealmsToUser() ? KEmptyTransactionId :
												 iTransactionId;
	}

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::UserName
// ----------------------------------------------------------------------------
//
const TDesC8& CSIPSecUserRecord::UserName() const
	{
	return ValueOrEmptyDescriptor( iUserName );
	}

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::Password
// ----------------------------------------------------------------------------
//
const TDesC8& CSIPSecUserRecord::Password() const
	{
	return ValueOrEmptyDescriptor( iPassword );
	}

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::SetSecretData
// ----------------------------------------------------------------------------
//
void CSIPSecUserRecord::SetSecretData( HBufC8* aSecretData )
	{	
    delete iSecretData;
	iSecretData = aSecretData;
	}

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::SecretData
// ----------------------------------------------------------------------------
//
const TDesC8& CSIPSecUserRecord::SecretData() const
	{
    return ValueOrEmptyDescriptor( iSecretData );
	}

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::SetOutboundProxyL
// ----------------------------------------------------------------------------
//	
void CSIPSecUserRecord::SetOutboundProxyL( const TDesC8& aOutboundProxy )
	{
	HBufC8* newProxy = aOutboundProxy.AllocL();
	delete iOutboundProxy;
	iOutboundProxy = newProxy;
	}

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::OutboundProxy
// ----------------------------------------------------------------------------
//
const TDesC8& CSIPSecUserRecord::OutboundProxy() const
	{	
	return ValueOrEmptyDescriptor( iOutboundProxy );
	}

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::ValueOrEmptyDescriptor
// ----------------------------------------------------------------------------
//
const TDesC8&
CSIPSecUserRecord::ValueOrEmptyDescriptor( HBufC8* aDescriptor ) const
	{
	if ( aDescriptor )
		{
		return *aDescriptor;
		}

	return KNullDesC8;
	}

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::CredentialsAttach
// ----------------------------------------------------------------------------
//
void CSIPSecUserRecord::CredentialsAttach()
	{
	++iCredentialsAttachedCount;
	}
    
// ----------------------------------------------------------------------------
// CSIPSecUserRecord::CredentialsDetach
// ----------------------------------------------------------------------------
//
void CSIPSecUserRecord::CredentialsDetach()
	{
	__SIP_ASSERT_RETURN( iCredentialsAttachedCount > 0, KErrUnderflow );

	--iCredentialsAttachedCount;
	}

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::CredentialsAttached
// ----------------------------------------------------------------------------
//
TBool CSIPSecUserRecord::CredentialsAttached() const
    {
    return iCredentialsAttachedCount > 0;
    }

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::CreateObserverL
// ----------------------------------------------------------------------------
//
CSIPSecDigestObserver* CSIPSecUserRecord::CreateObserverL(
	MSIPSecSecurityMechanismObserver& aObserver )
    {
    return new ( ELeave ) CSIPSecDigestObserver( aObserver );
    }

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::CreateProviderL
// ----------------------------------------------------------------------------
//
CSIPSecCredentialsProvider* CSIPSecUserRecord::CreateProviderL()
    {
    return new ( ELeave ) CSIPSecUserCredentialsProvider( *this );
    }

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::RequestL
// Update transaction id if asking credentials.
// If SIPSec user sends 2 requests (2 transactions), and both are challenged
// with the same realm, credentials are asked just once. iTransactionId must be
// the one that is passed to client, so when client sets credentials and gives
// transaction id, it matches iTransactionId.
// ----------------------------------------------------------------------------
//
TBool CSIPSecUserRecord::RequestL( TSIPSecDigestCtxSetup& aContext,
								   CSIPSecDigestObserver& aObserver )
	{
	TBool askCredentials( EFalse );

    if ( iObservers.Find( &aObserver ) == KErrNotFound )
        {
    	iObservers.AppendL( &aObserver );

    	__SIP_INT_LOG1( "SIPSecUserRec:Req state", iState )
    	if ( iState == ENoState )
    	    {
			TSIPSecPluginCtxResponse ctx =
				static_cast< TSIPSecPluginCtxResponse& >( aContext.Parent() );
			iUser = &ctx.SIPSecUser();
			iRegistrationId = ctx.RegistrationId();
			iTransactionId = aContext.TransactionId();

			if ( TrustedUser() && !iUser->PassOnlyRealmsToUser() )
				{
				iSIPResponse = &ctx.SIPResponse();
				}

    	    // Removes aObserver from array if iProvider->RequestL leaves
			TRecordCleanup cleanupInfo( *this, aObserver );
    		CleanupStack::PushL(
    			TCleanupItem( CSIPSecUserRecord::RemoveObserver,
    						  &cleanupInfo ) );
			iProvider->RequestL( aContext );
    		CleanupStack::Pop(); // cleanupItem

        	iState = EReceivingCredentials;
        	askCredentials = ETrue;        	
    	    }
	    aObserver.OperationStarted();
        }
    return askCredentials;
	}

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::RemoveObserver
// ----------------------------------------------------------------------------
//
void CSIPSecUserRecord::RemoveObserver( TAny* aRecordCleanup )
    {
	__SIP_ASSERT_RETURN( aRecordCleanup, KErrArgument );

    TRecordCleanup* restore =
        reinterpret_cast< TRecordCleanup* >( aRecordCleanup );
	TInt pos = restore->iRecord.iObservers.Find( &restore->iObserver );

	__SIP_ASSERT_RETURN( pos >= 0, KErrArgument );

    restore->iRecord.iObservers.Remove( pos );
	}

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::IsUpdating
// ----------------------------------------------------------------------------
//
TBool CSIPSecUserRecord::IsUpdating() const
	{
	return iState == EReceivingCredentials;
	}

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::SetUserCredentialsL
// ----------------------------------------------------------------------------
//
TBool CSIPSecUserRecord::SetUserCredentialsL( const TDesC8& aUsername,
											  const TDesC8& aPassword )
    {
    if ( iState == ECredentialsFailed )
        {
        __SIP_LOG( "SIPSecUserRec:SetUserCredL failed" )
        return EFalse;
        }

    if ( !iValid &&
         UserName().Compare( aUsername ) == 0 &&
         Password().Compare( aPassword ) == 0 )
        {
        __SIP_LOG( "SIPSecUserRec:SetUserCredL wrong username&passwd" )

        iState = ECredentialsFailed;
        return EFalse;
        }

	SetUsernameAndPasswordL( aUsername, aPassword );

    iState = ECredentialsReceived;
    iValid = ETrue;

	if ( TrustedUser() )
		{
		__SIP_LOG( "SIPSecUserRec:SetUserCredL trusted user sets credentials" )
		// Got credentials from trusted user, associate this record with it
		iUser = TrustedUser();
		}
	iRegistrationId = KEmptyRegistrationId;

	__SIP_LOG( "SIPSecUserRec:SetUserCredL return ETrue" )
    return ETrue;
    }

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::ClearUsernameAndPassword
// ----------------------------------------------------------------------------
//
void CSIPSecUserRecord::ClearUsernameAndPassword()
	{
	delete iUserName;
	iUserName = NULL;
	delete iPassword;
	iPassword = NULL;
	}

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::SIPSecUser
// ----------------------------------------------------------------------------
//
const MSIPSecUser& CSIPSecUserRecord::SIPSecUser() const
	{
	return TrustedUser() ? *TrustedUser() : *iUser;
	}

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::Invalidate
// ----------------------------------------------------------------------------
//
void CSIPSecUserRecord::Invalidate( CSIPSecUserRecord::TUpdateState aState )
    {
    SetSecretData( NULL );
    iValid = EFalse;
    if ( aState != ENoState )
        {
        iState = aState;
        }
    }

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::IsValid
// ----------------------------------------------------------------------------
//
TBool CSIPSecUserRecord::IsValid() const
    {
    return iValid;    
    }

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::Updated
// Also used by destructor, so iState can be EReceivingCredentials, if leave
// occurred in SetUserCredentialsL.
// ----------------------------------------------------------------------------
//
void CSIPSecUserRecord::Updated()
	{
	__SIP_LOG( "SIPSecUserRecord:Updated" )

    CSIPSecDigestObserver::TStatus reason = CSIPSecDigestObserver::EComplete;
	if ( iState == ECredentialsFailed )
	    {
	    __SIP_LOG( "credentials failed" )
	    reason = CSIPSecDigestObserver::EFailure;
	    }

    CSIPSecDigestObserver* observer = NULL;
    while ( iObservers.Count() > 0 )
    	{
    	observer = iObservers[ 0 ];
    	iObservers.Remove( 0 );
    	observer->OperationCompleted( reason, !DidAuthenticationFail() );
    	}

    iObservers.Reset();
    iState = ENoState;
	}

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::DidAuthenticationFail
// ----------------------------------------------------------------------------
//
TBool CSIPSecUserRecord::DidAuthenticationFail() const
	{
	return EFalse;
	}

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::CancelPendingOperations
// If trusted user ignores challenge, ask from iUser. If iUser ignores challenge
// or UAC cancels SIPSec processing, don't re-request credentials.
// ----------------------------------------------------------------------------
//
void CSIPSecUserRecord::CancelPendingOperations(
	MSIPSecSecurityMechanismObserver* aObserver )
	{
    TBool cancelAll = ( aObserver == NULL );

	if ( Type() == EIETF &&
		 cancelAll &&
		 TrustedUser() &&
		 RequestCredentialsFromOtherUser() )
		{
		return; // Don't complete observers. Wait credentials to arrive.
		}

	CSIPSecDigestObserver* observer( NULL );
    TInt i( 0 );
	TBool ready( EFalse );

	while ( i < iObservers.Count() && !ready )
    	{
    	observer = iObservers[ i ];
    	if ( cancelAll || observer->HasObserver( *aObserver ) )
    	    {
    	    // Remove observer before OperationCompleted, which may lead UA to
    	    // cancel its SIPSec requests, and this function to be re-entered
    	    // before OperationCompleted returns.
            iObservers.Remove( i );
            observer->OperationCompleted( CSIPSecDigestObserver::ECancelled,
            							  ETrue );
            ready = !cancelAll;
    	    }
        else
            {
            // Observer not removed, proceed to next observer
            i++;
            }
    	}

	if ( iObservers.Count() == 0 )
	    {
    	iProvider->Cancel();
	    }
    iState = ENoState;
	}

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::User
// ----------------------------------------------------------------------------
//
const MSIPSecUser& CSIPSecUserRecord::User() const
	{
	return *iUser;
	}

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::SetUser
// ----------------------------------------------------------------------------
//
void CSIPSecUserRecord::SetUser( const MSIPSecUser& aUser )
	{
	__SIP_ASSERT_RETURN( &aUser != iUser, KErrAlreadyExists );

	iUser = &aUser;
	}

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::SetUsernameAndPasswordL
// Don't delete old username and password until new ones have been allocated.
// ----------------------------------------------------------------------------
//
void CSIPSecUserRecord::SetUsernameAndPasswordL( const TDesC8& aUsername,
											     const TDesC8& aPassword )
	{
	HBufC8* newUserName = aUsername.AllocL();
	CleanupStack::PushL( newUserName );
	HBufC8* newPassword = aPassword.AllocL();
	CleanupStack::Pop( newUserName );

	delete iUserName;
	iUserName = newUserName;

	delete iPassword;
	iPassword = newPassword;
	}

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::RequestCredentialsFromOtherUser
// Trusted user ignored challenge, ask credentials from iUser.
// Use existing observers. They won't know that iUser (not trusted user) calls
// SetCredentialsL/IgnoreChallenge later.
// ----------------------------------------------------------------------------
//
TBool CSIPSecUserRecord::RequestCredentialsFromOtherUser()
	{
	__SIP_INT_LOG1( "SIPSecUserRec:ReqCredsFromOtherUser pass realm",
					iUser->PassOnlyRealmsToUser() )
	__SIP_ASSERT_RETURN_VALUE( Type() != EIMS, EFalse );

	// Clear Registration id, to avoid asking again from trusted user
	iRegistrationId = KEmptyRegistrationId;

	TInt err( KErrNone );
	if ( iUser->PassOnlyRealmsToUser() )
		{
		TRAP( err, iProvider->RequestAgainL() );
		}
	else
		{
		__SIP_ASSERT_RETURN_VALUE( iSIPResponse != NULL, EFalse );
		MSIPSecUser* user = const_cast< MSIPSecUser* >( iUser );
		TRAP( err, user->RequestCredentialsL( *iSIPResponse,
						  					  iTransactionId,
						  					  KEmptyRefreshId ) );
		iSIPResponse = NULL;
		}

	__SIP_INT_LOG1( "SIPSecUserRec:ReqCredsFromOtherUser err=", err )
	return err == KErrNone;
	}

// ----------------------------------------------------------------------------
// CSIPSecUserRecord::TrustedUser
// ----------------------------------------------------------------------------
//
const MSIPSecUser* CSIPSecUserRecord::TrustedUser() const
	{
	return const_cast< MSIPSecUser* >( iUser )->TrustedUser( iRegistrationId );
	}
