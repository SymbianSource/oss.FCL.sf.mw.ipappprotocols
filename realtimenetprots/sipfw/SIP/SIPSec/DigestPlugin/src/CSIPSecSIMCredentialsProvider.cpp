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
// Name          : CSIPSecSIMCredentialsProvider.cpp
// Part of       : SIPSec
// Version       : SIP/6.0
//



#include <commsdat.h>
#include <commsdattypesv1_1.h>
#include <mmretrieve.h>
#include <etelmmerr.h>
#include <cdblen.h>

#include "SipLogs.h"
#include "SipAssert.h"
#include "CSIPSecSIMCredentialsProvider.h"
#include "CSIPSecSIMRecord.h"
#include "CSIPSecDigestPlugin.h"
#include "sipsecdigestcontext.h"

// CONSTANTS
_LIT8( KSIPSecCK, "ck" );
_LIT8( KSIPSecIK, "ik" );

// ============================ MEMBER FUNCTIONS ===============================


// ----------------------------------------------------------------------------
// CSIPSecIMSAuthentication::NewL
// ----------------------------------------------------------------------------
//
CSIPSecIMSAuthentication* CSIPSecIMSAuthentication::NewL(
         MSIPSecIMSAuthenticationDataObserver& aObserver )
    {
    return new ( ELeave ) CSIPSecIMSAuthentication( aObserver );    
    }

// ----------------------------------------------------------------------------
// CSIPSecIMSAuthentication::CSIPSecIMSAuthentication
// ----------------------------------------------------------------------------
//
CSIPSecIMSAuthentication::CSIPSecIMSAuthentication( 
            MSIPSecIMSAuthenticationDataObserver& aObserver ) :
    CActive( CActive::EPriorityStandard ),
    iObserver( aObserver ),
    iPhone( NULL ),    
    iMessage( iData )
    {
    CActiveScheduler::Add( this );
    }
        
// ----------------------------------------------------------------------------
// CSIPSecIMSAuthentication::~CSIPSecIMSAuthentication
// ----------------------------------------------------------------------------
//
CSIPSecIMSAuthentication::~CSIPSecIMSAuthentication()
    {
    Cancel();
    }

#ifndef USE_TEST_STUB
// ----------------------------------------------------------------------------
// CSIPSecIMSAuthentication::Request
// ----------------------------------------------------------------------------
//
void CSIPSecIMSAuthentication::Request( RMobilePhone& aPhone )
    {
    __SIP_LOG( "CSIPSecIMSAuthen:Request" )
	__ASSERT_ALWAYS( !IsActive(),
					 User::Panic( _L( "IMSAuthentic:Req" ), KErrInUse ) );

	iMessage.Copy( RMobilePhone::TImsAuthenticateDataV5Pckg( iData ) );

    __SIP_LOG( "CSIPSecIMSAuthen:Request Asking ISIM to Authenticate" )
	aPhone.ImsAuthenticate( iStatus, iMessage );
    SetActive();
    iPhone = &aPhone;	
    }
#endif // #ifndef USE_TEST_STUB

// ----------------------------------------------------------------------------
// CSIPSecIMSAuthentication::Response
// ----------------------------------------------------------------------------
//
RMobilePhone::TImsAuthenticateDataV5& CSIPSecIMSAuthentication::Response()
    {
    return iData;
    }

// ----------------------------------------------------------------------------
// CSIPSecIMSAuthentication::RunL
// ----------------------------------------------------------------------------
//
void CSIPSecIMSAuthentication::RunL()
    {
	__SIP_INT_LOG1( "SIPSecIMSAuthen:RunL status", iStatus.Int() )    

    if ( iStatus.Int() == KErrNone )
        {
        iObserver.AuthenticatedL();
        }
    else
        {
        iObserver.AuthenticationFailedL( iStatus.Int() );        
        }
   	iPhone = NULL;
    }

// -----------------------------------------------------------------------------
// CSIPSecIMSAuthentication::RunError
// -----------------------------------------------------------------------------
//
TInt CSIPSecIMSAuthentication::RunError( TInt aError )
    {
	return iObserver.PassErrorToFramework( aError, ETrue );
	}

// ----------------------------------------------------------------------------
// CSIPSecIMSAuthentication::DoCancel
// As there is a pending request, iPhone must be defined.
// ----------------------------------------------------------------------------
//
void CSIPSecIMSAuthentication::DoCancel()
    {
#if defined( USE_TEST_STUB )
    // If CSIPSecDigestTest::AKAAuthWWWChallengeFailuresTestL leaves before it
    // called iSIMStub->CallBack, this is needed to cancel the active object.
	TRequestStatus* status = &iStatus;
   	User::RequestComplete( status, KErrCancel );
#else
 	__SIP_LOG( "CSIPSecIMSAuthen:DoCancel" )

    __SIP_ASSERT_RETURN( iPhone, KErrNotFound );    

	iPhone->CancelAsyncRequest( EMobilePhoneIMSAuthenticate );	
#endif
    }

// ============================ MEMBER FUNCTIONS ===============================


// ----------------------------------------------------------------------------
// CSIPSecIMSAuthorization::NewL
// ----------------------------------------------------------------------------
//
CSIPSecIMSAuthorization*
CSIPSecIMSAuthorization::NewL( MSIPSecIMSAuthenticationDataObserver& aObserver )
    {
    CSIPSecIMSAuthorization* self = 
        new ( ELeave ) CSIPSecIMSAuthorization( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;    
    }

// ----------------------------------------------------------------------------
// CSIPSecIMSAuthorization::CSIPSecIMSAuthorization
// ----------------------------------------------------------------------------
//
CSIPSecIMSAuthorization::CSIPSecIMSAuthorization( 
        MSIPSecIMSAuthenticationDataObserver& aObserver ) :
    CActive( CActive::EPriorityStandard ),
    iObserver( aObserver )
    {
    }
        
// ----------------------------------------------------------------------------
// CSIPSecIMSAuthorization::~CSIPSecIMSAuthorization
// ----------------------------------------------------------------------------
//
CSIPSecIMSAuthorization::~CSIPSecIMSAuthorization()
    {
    Cancel();
    delete iRetriever;
    delete iData;
    }

// ----------------------------------------------------------------------------
// CSIPSecIMSAuthorization::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPSecIMSAuthorization::ConstructL()
    {    
    iData = RMobilePhone::CImsAuthorizationInfoV5::NewL();
    CActiveScheduler::Add( this );    
    }

#ifndef USE_TEST_STUB
// ----------------------------------------------------------------------------
// CSIPSecIMSAuthorization::RequestL
// ----------------------------------------------------------------------------
//
void CSIPSecIMSAuthorization::RequestL( RMobilePhone& aPhone )
    {
    delete iRetriever;
    iRetriever = NULL;
    iRetriever = CAsyncRetrieveAuthorizationInfo::NewL( aPhone, *iData );

    iRetriever->Start( iStatus );
    SetActive();
    }
#endif // #ifndef USE_TEST_STUB

// ----------------------------------------------------------------------------
// CSIPSecIMSAuthorization::Response
// ----------------------------------------------------------------------------
//
RMobilePhone::CImsAuthorizationInfoV5& CSIPSecIMSAuthorization::Response()
    {
    return *iData;
    }   

#ifndef USE_TEST_STUB
// ----------------------------------------------------------------------------
// CSIPSecIMSAuthorization::RunL
// ----------------------------------------------------------------------------
//
void CSIPSecIMSAuthorization::RunL()
    {
	if ( iStatus.Int() == KErrNone &&
		 iRetriever &&
		 iRetriever->AuthInfoValid() )
        {
        iObserver.Authorized();
        }
    else
        {
        iObserver.AuthorizationFailedL();
        }
    }
#endif // #ifndef USE_TEST_STUB

// -----------------------------------------------------------------------------
// CSIPSecIMSAuthorization::RunError
// -----------------------------------------------------------------------------
//
TInt CSIPSecIMSAuthorization::RunError( TInt aError )	
    {
	return iObserver.PassErrorToFramework( aError, EFalse );	
	}
        
// ----------------------------------------------------------------------------
// CSIPSecIMSAuthorization::DoCancel
// ----------------------------------------------------------------------------
//
void CSIPSecIMSAuthorization::DoCancel()
    {
#if defined( USE_TEST_STUB )
    // If using test stub, must cancel own request as iRetriever is always NULL
	TRequestStatus* status = &iStatus;
   	User::RequestComplete( status, KErrCancel );
#endif
    __SIP_LOG( "CSIPSecIMSAuthor:DoCancel" )

	if ( iRetriever )
		{
		iRetriever->Cancel();
		}
    delete iRetriever;
    iRetriever = NULL;
    }


// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CSIPSecSIMCredentialsProvider::NewL
// ----------------------------------------------------------------------------
//
CSIPSecSIMCredentialsProvider*
CSIPSecSIMCredentialsProvider::NewL( CSIPSecSIMRecord& aRecord )
    {
    CSIPSecSIMCredentialsProvider* self =
		new ( ELeave ) CSIPSecSIMCredentialsProvider( aRecord );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CSIPSecSIMCredentialsProvider::CSIPSecSIMCredentialsProvider
// ----------------------------------------------------------------------------
//
CSIPSecSIMCredentialsProvider::CSIPSecSIMCredentialsProvider(
	CSIPSecSIMRecord& aRecord ) :
	CSIPSecCredentialsProvider( aRecord )
	{
	}

#ifndef USE_TEST_STUB
// ----------------------------------------------------------------------------
// CSIPSecSIMCredentialsProvider::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPSecSIMCredentialsProvider::ConstructL()
    {
	TBuf<KCommsDbSvrMaxFieldLength> tsyModuleName;
	GetTsyModuleNameL( tsyModuleName );
	PreparePhoneL( tsyModuleName );

    iAuthentication = CSIPSecIMSAuthentication::NewL( *this );
    iAuthorization = CSIPSecIMSAuthorization::NewL( *this );
    }
#endif // #ifndef USE_TEST_STUB

// ----------------------------------------------------------------------------
// CSIPSecSIMCredentialsProvider::~CSIPSecSIMCredentialsProvider
// ----------------------------------------------------------------------------
//
CSIPSecSIMCredentialsProvider::~CSIPSecSIMCredentialsProvider()
    {
    __SIP_LOG( "~SIPSecSIMCredProv" )

    delete iAuthentication;
    delete iAuthorization;
    delete iOutboundProxy;

    iPhone.Close();
    iEtelServer.Close();

	__SIP_LOG( "~SIPSecSIMCredProv ends" )
    }

// ----------------------------------------------------------------------------
// CSIPSecSIMCredentialsProvider::GetTsyModuleNameL
// ----------------------------------------------------------------------------
//
void
CSIPSecSIMCredentialsProvider::GetTsyModuleNameL( TDes& aTsyModuleName ) const
    {
    __ASSERT_ALWAYS( aTsyModuleName.MaxSize() >= KCommsDbSvrMaxFieldLength,
		User::Leave( KErrArgument ) );

    using namespace CommsDat;

    CMDBSession* db = CMDBSession::NewLC( KCDVersion1_1 ); 

    CMDBField<TUint32>* globalSettingField =
    	new ( ELeave ) CMDBField<TUint32>( KCDTIdModemPhoneServicesSMS );
    CleanupStack::PushL( globalSettingField );

    globalSettingField->SetRecordId( 1 );
    globalSettingField->LoadL( *db );
    TUint32 modemId = *globalSettingField; 

    CMDBField<TDesC>* tsyField =
    	new ( ELeave ) CMDBField<TDesC>( KCDTIdTsyName );
    CleanupStack::PushL( tsyField );

    tsyField->SetRecordId( modemId ); 
    tsyField->LoadL( *db );
    aTsyModuleName = *tsyField;

    CleanupStack::PopAndDestroy( tsyField );
    CleanupStack::PopAndDestroy( globalSettingField );
    CleanupStack::PopAndDestroy( db );
    }

// ----------------------------------------------------------------------------
// CSIPSecSIMCredentialsProvider::PreparePhoneL
// ----------------------------------------------------------------------------
//
void CSIPSecSIMCredentialsProvider::PreparePhoneL( const TDes& aTsyModuleName )
	{
    User::LeaveIfError( iEtelServer.Connect() );
    User::LeaveIfError( iEtelServer.LoadPhoneModule( aTsyModuleName ) );

    // Get number of phones supported by all currently loaded TSY modules
    TInt phoneCount( 0 );
    User::LeaveIfError( iEtelServer.EnumeratePhones( phoneCount ) );

    RTelServer::TPhoneInfo phoneInfo;
	TName searchTsyName;
	TInt phoneIndex( KErrNotFound );

    while ( phoneIndex == KErrNotFound && phoneCount-- > 0 )
        {
        // Check if this phone belongs to the loaded TSY
        if ( ( iEtelServer.GetTsyName( phoneCount, searchTsyName ) == KErrNone )
             &&
             ( searchTsyName.CompareF( aTsyModuleName ) == 0 ) )
        	{
        	phoneIndex = phoneCount;        	
        	}
        }
	if ( phoneIndex == KErrNotFound )
		{
		User::Leave( KErrNotFound );
		}

    User::LeaveIfError( iEtelServer.GetPhoneInfo( phoneIndex, phoneInfo ) );
    User::LeaveIfError( iPhone.Open( iEtelServer, phoneInfo.iName ) );	

    RPhone::TStatus phoneStatus;
    User::LeaveIfError( iPhone.GetStatus( phoneStatus ) );

    // Check if the phone is already initialised
    if ( phoneStatus.iMode == RPhone::EModeUnknown )
        {
        TInt err = iPhone.Initialise();

        if ( err != KErrNone )
            {
            iPhone.Close();
            User::Leave( err );
            }
        }
    }

// ----------------------------------------------------------------------------
// CSIPSecSIMCredentialsProvider::RequestL
// ----------------------------------------------------------------------------
//
void CSIPSecSIMCredentialsProvider::RequestL( TSIPSecDigestCtxSetup& aContext )
    {
    iPlugin = &aContext.Parent().Plugin();

    delete iOutboundProxy;
    iOutboundProxy = NULL;
    iOutboundProxy = aContext.Parent().OutboundProxy().AllocL();

    static_cast<CSIPSecSIMRecord&>( UserRecord() ).SetAuthenticationDataL(
    	aContext.Nonce() );

    if ( UserRecord().UserName().Length() == 0 )
        {
        RequestUserNameL();
        }
    else
        {
        RequestPassword();
        }
    }
    
// ----------------------------------------------------------------------------
// CSIPSecSIMCredentialsProvider::RequestUserNameL
// ----------------------------------------------------------------------------
//
void CSIPSecSIMCredentialsProvider::RequestUserNameL()
    {
    RMobilePhone::CImsAuthorizationInfoV5& data = iAuthorization->Response();

    data.iIMPI.Zero();
    data.iIMPUArray.Reset();
    data.iHomeNetworkDomainName.Zero();

    iAuthorization->RequestL( iPhone );
    }

// ----------------------------------------------------------------------------
// CSIPSecSIMCredentialsProvider::RequestPassword
// ----------------------------------------------------------------------------
//
void CSIPSecSIMCredentialsProvider::RequestPassword()
    {
    CSIPSecSIMRecord& record = static_cast<CSIPSecSIMRecord&>( UserRecord() );

    RMobilePhone::TImsAuthenticateDataV5& data = iAuthentication->Response();
    data.iRAND.Zero();
    data.iAUTN.Zero();
    data.iRES.Zero();
	data.iIK.Zero();
    data.iCK.Zero();
    data.iAUTS.Zero();

    data.iRAND.Copy( record.RAND() );
	data.iAUTN.Copy( record.AUTN() );

	__SIP_INT_LOG2( "SIPSecSIMCredProv:RequestPassword, RAND len, AUTN len",
	                record.RAND().Length(), record.AUTN().Length() )
	HexDumpToLog( _L8( "RAND=" ), record.RAND() );	
	HexDumpToLog( _L8( "AUTN=" ), record.AUTN() );

    iAuthentication->Request( iPhone );
    }

// ----------------------------------------------------------------------------
// CSIPSecSIMCredentialsProvider::Cancel
// ----------------------------------------------------------------------------
//
void CSIPSecSIMCredentialsProvider::Cancel()
    {
    iAuthentication->Cancel();
    iAuthorization->Cancel();
    }
    
// ----------------------------------------------------------------------------
// CSIPSecSIMCredentialsProvider::AuthenticatedL
// For AKA, use the challenge's realm, not the SIM's realm. AUTS does not exist.
// ----------------------------------------------------------------------------
//
void CSIPSecSIMCredentialsProvider::AuthenticatedL()
    {
    __SIP_LOG( "SIPSecSIMCredProv:AuthenticatedL" )
    __SIP_ASSERT_LEAVE( iPlugin && iOutboundProxy, KErrNotFound );

    RMobilePhone::CImsAuthorizationInfoV5& authorization =
    	iAuthorization->Response();
    RMobilePhone::TImsAuthenticateDataV5& authentication =
		iAuthentication->Response();

	__SIP_INT_LOG2( "Lengths: IK, CK",
	                authentication.iIK.Length(),
			        authentication.iCK.Length() )
	__SIP_INT_LOG2( "Lengths: IMPI, RES",       
			        authorization.iIMPI.Length(),
			        authentication.iRES.Length() )
	__SIP_INT_LOG1( "Lengths: AUTS", authentication.iAUTS.Length() )

	HexDumpToLog( _L8( "IK=" ), authentication.iIK );
	HexDumpToLog( _L8( "CK=" ), authentication.iCK );
	HexDumpToLog( _L8( "RES=" ), authentication.iRES );

    iPlugin->EngineContext().RemoveValue( KSIPSecIK );
    iPlugin->EngineContext().RemoveValue( KSIPSecCK );
   	iPlugin->EngineContext().SetValueL( KSIPSecIK, authentication.iIK );
   	iPlugin->EngineContext().SetValueL( KSIPSecCK, authentication.iCK );
    iPlugin->SetCredentialsL( UserRecord().SIPSecUser(),
                              UserRecord().Realm(),
                              *iOutboundProxy,
                              authorization.iIMPI,   // username
                              authentication.iRES ); // password
    }

// ----------------------------------------------------------------------------
// CSIPSecSIMCredentialsProvider::AuthenticationFailedL
// If SQN failure, AUTS is set. Compute credentials using an empty password.
// ----------------------------------------------------------------------------
//
void CSIPSecSIMCredentialsProvider::AuthenticationFailedL( TInt aError )
    {
	__SIP_LOG( "SIPSecSIMCredProv:AuthenticFailedL" )
	__SIP_ASSERT_RETURN( aError != KErrNone, KErrArgument );
    __SIP_ASSERT_RETURN( iPlugin && iOutboundProxy, KErrNotFound );    

	const TDesC8& res = iAuthentication->Response().iRES;	
	const TDesC8& auts = iAuthentication->Response().iAUTS;

	__SIP_INT_LOG2( "Lengths RES, AUTS", res.Length(), auts.Length() )
	HexDumpToLog( _L8( "AUTS=" ), auts );

	__ASSERT_ALWAYS( res.Length() == 0, User::Leave( KErrGeneral ) );

	CSIPSecSIMRecord& record = static_cast<CSIPSecSIMRecord&>( UserRecord() );
	switch ( aError )
		{
		case KErrMMEtelMacVerificationFailed:
			record.UserCredentialsFailed( CSIPSecSIMRecord::EMAC );
			break;

		case KErrMMEtelSqnVerificationFailed:			
			__ASSERT_ALWAYS( auts.Length() > 0, User::Leave( KErrGeneral ) );

			record.UserCredentialsFailed( CSIPSecSIMRecord::ESQN );
        	record.SetResynchronizationDataL( auts );
			break;

		case KErrMMEtelAuthenticateFailed: // Authenticate command failed		
		default: // System wide errors
			User::Leave( aError );
		}

	iPlugin->EngineContext().RemoveValue( KSIPSecIK );
    iPlugin->EngineContext().RemoveValue( KSIPSecCK );

	// Though using record.TransactionId() works too, it is better to use
	// userRecord().SIPSecUser() as transaction id variant of SetCredentialsL
	// will be removed.
	iPlugin->SetCredentialsL( record.SIPSecUser(),
	                          record.Realm(),
                              *iOutboundProxy,
                              iAuthorization->Response().iIMPI, // username
                              KNullDesC8 ); // empty password ("")
    }

// ----------------------------------------------------------------------------
// CSIPSecSIMCredentialsProvider::Authorized
// ----------------------------------------------------------------------------
//
void CSIPSecSIMCredentialsProvider::Authorized()
    {    
    RequestPassword();    
    }

// ----------------------------------------------------------------------------
// CSIPSecSIMCredentialsProvider::AuthorizationFailedL
// ----------------------------------------------------------------------------
//
void CSIPSecSIMCredentialsProvider::AuthorizationFailedL()
    {
    __SIP_LOG( "SIPSecSIMCredProv:AuthorizationFailedL" )
    __SIP_ASSERT_RETURN( iPlugin && iOutboundProxy, KErrNotFound );

    CSIPSecSIMRecord& record = static_cast<CSIPSecSIMRecord&>( UserRecord() );

    record.UserCredentialsFailed( CSIPSecSIMRecord::EPrivateIdentity );
    iPlugin->SetCredentialsL( record.SIPSecUser(),
                              record.Realm(),
                              *iOutboundProxy,
                              iAuthorization->Response().iIMPI,   // username
                              iAuthentication->Response().iRES ); // password
    }

// ----------------------------------------------------------------------------
// CSIPSecSIMCredentialsProvider::PassErrorToFramework
// If aAuthenticate == EFalse, UserCredentialsFailed( EPrivateIdentity ) has
// already been done in AuthorizationFailedL.
// ----------------------------------------------------------------------------
//
TInt CSIPSecSIMCredentialsProvider::PassErrorToFramework( TInt aError,
	TBool aAuthenticate ) const
	{
	__SIP_ASSERT_RETURN_VALUE( iPlugin, KErrNotFound );

	__SIP_INT_LOG2( "SIPSecSIMCredPr:PassErrorToFramework err, authen",
	                aError,	aAuthenticate )

	if ( aError == KErrNoMemory )
		{
		return aError;
		}

	CSIPSecSIMRecord& record = static_cast<CSIPSecSIMRecord&>( UserRecord() );

	if ( aAuthenticate )
		{		
		// Sets state -> ECredentialsFailed and failure type -> ENoFailure
		record.Invalidate( CSIPSecUserRecord::ECredentialsFailed );

		// Sets failure type -> EPrivateIdentity. Does not change state.
	    record.UserCredentialsFailed( CSIPSecSIMRecord::EPrivateIdentity );
		}

	// Inform SIPSecFW that the asynchronous request has ended
	record.Updated();

	// Don't remove anything from cache, but use CleanObservers
	iPlugin->ClearCache( NULL );

	return KErrNone;
	}

// ----------------------------------------------------------------------------
// CSIPSecSIMCredentialsProvider::HexDumpToLog
// ----------------------------------------------------------------------------
//
void CSIPSecSIMCredentialsProvider::HexDumpToLog( const TDesC8& aMessage,
												  const TDesC8& aData ) const
	{
	__SIP_DES8_LOG( "CSIPSecSIMCredentialsProvider::HexDumpToLog", aMessage )

	// Prevent compiler warning in UREL build
	aMessage.Length();

	// Each byte uses 3 characters (e.g. 0x12 is written as "12 ")
	const TInt KLengthOfByte = 3;
	const TInt KMaxLineLength = 60;
	TBuf8< KMaxLineLength > logBuf;

	TInt inputPos( 0 );
	while ( inputPos < aData.Length() )
		{
		logBuf.Zero();

		TInt outputPos( 0 );
		while ( ( outputPos < KMaxLineLength - KLengthOfByte ) &&
		        ( inputPos < aData.Length() ) )
			{
			logBuf.AppendFormat( _L8( "%02x " ), aData[ inputPos++ ] );
			outputPos += KLengthOfByte;
			}
		__SIP_DES8_LOG( "CSIPSecSIMCredentialsProvider::HexDumpToLog", logBuf )
		}
	}

// End of File
