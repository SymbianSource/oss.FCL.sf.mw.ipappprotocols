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
// Name          : CSIPSecChallengeMD5.cpp
// Part of       : SIPSec/DigestPlugin
// Version       : SIP/6.0
//



#include <hash.h>
#include <e32math.h>
#include "SipLogs.h"
#include "CSIPSecChallengeMD5.h"
#include "sipsecrequestdata.h"
#include "CSIPSecUserRecord.h"
#include "sipsecdigestcontext.h"
#include "sipstrings.h"
#include "sipstrconsts.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPSecChallengeMD5::CSIPSecChallengeMD5
// -----------------------------------------------------------------------------
//
CSIPSecChallengeMD5::CSIPSecChallengeMD5( CSIPSecDigest::TChallengeType aType,
										  CSIPSecRequestData::TQop aQop ) :
	CSIPSecChallenge( aType, aQop )
    {
    }

// -----------------------------------------------------------------------------
// CSIPSecChallengeMD5::NewLC
// -----------------------------------------------------------------------------
//
CSIPSecChallengeMD5*
CSIPSecChallengeMD5::NewLC( CSIPSecDigest::TChallengeType aType,
                            CSIPAuthenticateHeaderBase& aAuthenticateHeader,
                            CSIPSecRequestData::TQop aQop )
    {
	CSIPSecChallengeMD5* self = new( ELeave )CSIPSecChallengeMD5( aType, aQop );
	CleanupStack::PushL( self );
	self->ConstructL( aAuthenticateHeader );	
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPSecChallengeMD5::~CSIPSecChallengeMD5
// -----------------------------------------------------------------------------
//
CSIPSecChallengeMD5::~CSIPSecChallengeMD5()
    {
    }

// -----------------------------------------------------------------------------
// CSIPSecChallengeMD5::VerifyTypeL
// -----------------------------------------------------------------------------
//
void CSIPSecChallengeMD5::VerifyTypeL( CSIPSecUserRecord::TType aType ) const
	{
	__ASSERT_ALWAYS( aType == CSIPSecUserRecord::EIETF,
        			 User::Leave( KErrPermissionDenied ) );
	}

// -----------------------------------------------------------------------------
// CSIPSecChallengeMD5::CreateUserRecordL
// -----------------------------------------------------------------------------
//
CSIPSecUserRecord*
CSIPSecChallengeMD5::CreateUserRecordL( const TDesC8& aRealm,
										const MSIPSecUser& aUser,
										TRegistrationId aRegistrationId,
									    TTransactionId aTransactionId,
									    TUint /*aResponseCode*/ ) const
	{
	return CSIPSecUserRecord::NewL( aRealm,
									aUser,
									aRegistrationId,
									aTransactionId );
	}
			
// -----------------------------------------------------------------------------
// CSIPSecChallengeMD5::SupportedAlgorithm
// -----------------------------------------------------------------------------
//
RStringF CSIPSecChallengeMD5::SupportedAlgorithm()
    {    
    return SIPStrings::StringF( SipStrConsts::EMD5 );
    }

// -----------------------------------------------------------------------------
// CSIPSecChallengeMD5::AlgorithmName
// -----------------------------------------------------------------------------
//
RStringF CSIPSecChallengeMD5::AlgorithmName() const
    {
    return CSIPSecChallengeMD5::SupportedAlgorithm();
    }

// -----------------------------------------------------------------------------
// CSIPSecChallengeMD5::ProcessResponseL
// -----------------------------------------------------------------------------
//
TBool CSIPSecChallengeMD5::ProcessResponseL( TSIPSecDigestCtxSetup& aContext,
									   		 TBool& aAskCredentials ) const
    {
    aAskCredentials = EFalse;
    if ( aContext.UserData().IsValid() )
        {
        if ( aContext.UserData().SecretData().Length() == 0 )
            {
            CalculateSecretDataA1L( aContext );
            }
        return EFalse;
        }

    aAskCredentials = aContext.RequestUserCredentialsL();
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CSIPSecChallengeMD5::ProcessRequestL
// -----------------------------------------------------------------------------
//
void
CSIPSecChallengeMD5::ProcessRequestL( TSIPSecDigestCtxProcess& aContext ) const
    {
    __SIP_LOG( "SIPSecChallMD5:ProcessRequestL" )

    HBufC8* response = RequestDigestL( aContext );
    CleanupStack::PushL( response );

	__SIP_LOG( "response computed" )

    aContext.SetResponseL( response->Des() );
    CleanupStack::PopAndDestroy( response );
    
    __SIP_LOG( "SIPSecChallMD5:ProcessRequestL ends" )
    }

// -----------------------------------------------------------------------------
// CSIPSecChallengeMD5::RequestDigestL
// -----------------------------------------------------------------------------
//
HBufC8*
CSIPSecChallengeMD5::RequestDigestL( TSIPSecDigestCtxProcess& aContext ) const
    {
    CMD5& messageDigest = aContext.Mechanism().MessageDigest();            
    HBufC8* a2buf = CalculateA2L( aContext );
    CleanupStack::PushL( a2buf );

    const TDesC8& secretData = aContext.UserData().SecretData();    
    const TDesC8& nonce 	 = aContext.Nonce();
    const TDesC8& nonceCount = aContext.NonceCount();    
    const TDesC8& cnonce 	 = aContext.CNonce();
    const TDesC8& a2 		 = a2buf->Des();        
	const TDesC8& qop 		 = QopDescriptor();

	const TInt KSeparatorCharacters = 2;
	TInt dataSize = secretData.Length() +
					nonce.Length() +
					a2.Length() +
					KSeparatorCharacters;
    if ( HasQop() )
        {
        const TInt KAdditionalSeparatorCharacters = 3;

        dataSize += nonceCount.Length();
        dataSize += cnonce.Length();
        dataSize += qop.Length();        
        dataSize += KAdditionalSeparatorCharacters;
        }

    // Create response for hashing
    HBufC8* data = HBufC8::NewLC( dataSize );

    if ( HasQop() )
        {
        _LIT8( KSIPSecResponsePattern, "%S:%S:%S:%S:%S:%S" );
        data->Des().AppendFormat( KSIPSecResponsePattern,
         					      &secretData,
         					      &nonce,
         					      &nonceCount,
         					      &cnonce,
         					      &qop,
         					      &a2 );
        }
    else
        {
        _LIT8( KSIPSecResponsePatternAssumed, "%S:%S:%S" );
        data->Des().AppendFormat( KSIPSecResponsePatternAssumed,
        						  &secretData,
        						  &nonce,
        						  &a2 );
        }

    // Create response hash
    HBufC8* response = HashL( messageDigest, data->Des() );

    CleanupStack::PopAndDestroy( data );
    CleanupStack::PopAndDestroy( a2buf );

    return response;        
    }

// -----------------------------------------------------------------------------
// CSIPSecChallengeMD5::GenerateCNonceL
// -----------------------------------------------------------------------------
//
void
CSIPSecChallengeMD5::GenerateCNonceL( TSIPSecDigestCtxSetup& aContext ) const
    {
#ifdef CPPUNIT_TEST
	// Use a known CNonce value in unit testing
	_LIT8( KDummyCNonce, "0a4f113b" );
    aContext.SetCNonceL( KDummyCNonce );
#else
    TTime time;
    time.HomeTime();

    TInt64 timeKey = time.Int64();
    CMD5& messageDigest = aContext.Mechanism().MessageDigest();

    // Append also some other data to keyBuf and increase size of KBufLength

    // Long enough to hold the decimal representation of TInt64
    const TInt KBufLength = 20;
    TBuf8< KBufLength > keyBuf;
	keyBuf.AppendNum( static_cast< TInt64 >( Math::Rand( timeKey ) ) );

    HBufC8* cnonce = HashL( messageDigest, keyBuf );
    CleanupStack::PushL( cnonce );

    aContext.SetCNonceL( cnonce->Des() );
    CleanupStack::PopAndDestroy( cnonce );
#endif
    }

// -----------------------------------------------------------------------------
// CSIPSecChallengeMD5::CalculateA2L
// -----------------------------------------------------------------------------
//
HBufC8*
CSIPSecChallengeMD5::CalculateA2L( TSIPSecDigestCtxProcess& aContext ) const
    {
    CMD5& messageDigest = aContext.Mechanism().MessageDigest();
    CSIPSecRequestData* requestData = aContext.RequestDataL( Qop() );
    CleanupStack::PushL( requestData );

    HBufC8* dataA2 = HBufC8::NewLC( requestData->Size() );
    
    while ( !requestData->EndOfData() )
        {
        RSIPSecRequestDataField field = requestData->NextL();
        CleanupClosePushL( field );

        if ( field.NeedsHashing() )
            {
            HBufC8* hashBuf = HashL( messageDigest, field.Value() );
            dataA2->Des().Append( hashBuf->Des() );
            delete hashBuf;
            }
        else
            {
            dataA2->Des().Append( field.Value() );
            }

        dataA2->Des().Append( requestData->Separator() );

        CleanupStack::PopAndDestroy(); // field
        }

    HBufC8* a2 = HashL( messageDigest, dataA2->Des() );

    CleanupStack::PopAndDestroy( dataA2 );
    CleanupStack::PopAndDestroy( requestData );
    return a2;
    }

// -----------------------------------------------------------------------------
// CSIPSecChallengeMD5::CalculateSecretDataA1L
// -----------------------------------------------------------------------------
//
void CSIPSecChallengeMD5::CalculateSecretDataA1L(
	TSIPSecDigestCtxSetup& aContext ) const
    {
    _LIT8( KSIPSecA1Pattern, "%S:%S:%S" );

    CMD5& messageDigest    = aContext.Mechanism().MessageDigest();
    const TDesC8& realm    = aContext.UserData().Realm();
    const TDesC8& username = aContext.UserData().UserName();
    const TDesC8& password = aContext.UserData().Password();

	TInt dataSize = realm.Length() +
					username.Length() +
         	        password.Length() +
         	        KSIPSecA1Pattern().Length();
    HBufC8* data = HBufC8::NewLC( dataSize );
    data->Des().AppendFormat( KSIPSecA1Pattern, &username, &realm, &password );

    HBufC8* secretData = HashL( messageDigest, data->Des() );

    CleanupStack::PopAndDestroy( data );
    aContext.UserData().SetSecretData( secretData );
    }

// -----------------------------------------------------------------------------
// CSIPSecChallengeMD5::HashL
// -----------------------------------------------------------------------------
//
HBufC8* CSIPSecChallengeMD5::HashL( CMD5& aDigest, TPtrC8 aData ) const
    {
    const TInt KHexSize = 2;
    TPtrC8 hash = aDigest.Hash( aData );
    TInt hashSize = aDigest.HashSize();
    TInt bufLength = hashSize * KHexSize;

    HBufC8* buf = HBufC8::NewL( bufLength );
    TPtr8 ptr( buf->Des() );
    TBuf8< KHexSize > hexChar;

    for ( TInt i = 0; i < hashSize; i++ )
		{
		hexChar.NumFixedWidth( hash[ i ], EHex, KHexSize );
		ptr.Append( hexChar );
		}

    aDigest.Reset();
	return buf;
    }
