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
// Name          : CSIPSecSIMRecord.cpp
// Part of       : SIPSec
// Version       : SIP/6.0
//



#include "SipLogs.h"
#include "CSIPSecSIMRecord.h"
#include "CSIPSecSIMCredentialsObserver.h"
#include "CSIPSecSIMCredentialsProvider.h"

// Base64 decoding related constants

const TInt KAsciiToBase64TableSize = 80;

const TInt8 KAsciiToBase64Table[ KAsciiToBase64TableSize ] =
	{
	 62, -1, -1, -1, 63, 52, 53, 54, 55, 56,
	 57, 58, 59, 60, 61, -1, -1, -1, 64, -1,
	 -1, -1,  0,  1,  2,  3,  4,  5,  6,  7,
	  8,  9, 10, 11, 12, 13, 14, 15, 16, 17,
	 18, 19, 20, 21, 22, 23, 24, 25, -1, -1,
	 -1, -1, -1, -1, 26, 27, 28, 29, 30, 31,
	 32, 33, 34, 35, 36, 37, 38, 39, 40, 41,
	 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
	};


// ============================ MEMBER FUNCTIONS ===============================


// ----------------------------------------------------------------------------
// CSIPSecSIMRecord::NewL
// ----------------------------------------------------------------------------
//
CSIPSecSIMRecord* CSIPSecSIMRecord::NewL( const TDesC8& aRealm,
										  const MSIPSecUser& aUser,
										  TRegistrationId aRegistrationId,
										  TTransactionId aTransactionId )
	{
	CSIPSecSIMRecord* self =
		NewLC( aRealm, aUser, aRegistrationId, aTransactionId );
	CleanupStack::Pop( self );
	return self;
	}
	
// ----------------------------------------------------------------------------
// CSIPSecSIMRecord::NewLC
// ----------------------------------------------------------------------------
//	
CSIPSecSIMRecord* CSIPSecSIMRecord::NewLC( const TDesC8& aRealm,
										   const MSIPSecUser& aUser,
										   TRegistrationId aRegistrationId,
										   TTransactionId aTransactionId )
	{
	CSIPSecSIMRecord* self = new ( ELeave ) CSIPSecSIMRecord( aUser,
															  aRegistrationId,
															  aTransactionId );
	CleanupStack::PushL( self );
	self->ConstructL( aRealm );
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPSecSIMRecord::CSIPSecSIMRecord
// ----------------------------------------------------------------------------
//
CSIPSecSIMRecord::CSIPSecSIMRecord( const MSIPSecUser& aUser,
									TRegistrationId aRegistrationId,
									TTransactionId aTransactionId ) :
	CSIPSecUserRecord( CSIPSecUserRecord::EIMS,
					   aUser,
					   aRegistrationId,
					   aTransactionId ),
    iFailureType( ENoFailure )
	{
	}

// ----------------------------------------------------------------------------
// CSIPSecSIMRecord::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPSecSIMRecord::ConstructL( const TDesC8& aRealm )
    {    
    CSIPSecUserRecord::ConstructL( aRealm );       
    }

// ----------------------------------------------------------------------------
// CSIPSecSIMRecord::~CSIPSecSIMRecord
// ----------------------------------------------------------------------------
//
CSIPSecSIMRecord::~CSIPSecSIMRecord()
	{
	delete iRAND;
	delete iAUTN;
	delete iAUTS;
	}

// ----------------------------------------------------------------------------
// CSIPSecSIMRecord::SetUserCredentialsL
// ----------------------------------------------------------------------------
//
TBool CSIPSecSIMRecord::SetUserCredentialsL( const TDesC8& aUsername,
											 const TDesC8& aPassword )
    {
    __SIP_LOG( "CSIPSecSIMRecord::SetUserCredL" )

    if ( iFailureType == ENoFailure )
    	{
    	iFailureCount = 0;
    	}

    iValid = ( iFailureType != EPrivateIdentity );
    if ( iValid )
    	{
		CSIPSecUserRecord::SetUserCredentialsL( aUsername, aPassword );
		}

	if ( !iValid || iFailureCount > ESIPSecMaxAuthenticationFailuresAllowed )
        {
        iState = ECredentialsFailed;
        iValid = EFalse;
        }

	__SIP_INT_LOG1( "SIPSecSIMRecord:SetUserCredL return", iValid )
    return iValid;
    }

// ----------------------------------------------------------------------------
// CSIPSecSIMRecord::UserCredentialsFailed
// ----------------------------------------------------------------------------
//
void CSIPSecSIMRecord::UserCredentialsFailed( TFailureType aFailure )
    {
    __ASSERT_ALWAYS( aFailure != ENoFailure,
    	User::Panic( _L( "SIPSecSIMRec:UserCredFailed" ), KErrArgument ) );

    Invalidate( CSIPSecUserRecord::ENoState );
    iFailureType = aFailure;
    ++iFailureCount;
    }

// ----------------------------------------------------------------------------
// CSIPSecSIMRecord::Failure
// ----------------------------------------------------------------------------
//
CSIPSecSIMRecord::TFailureType CSIPSecSIMRecord::Failure() const
    {
    return iFailureType;
    }

// ----------------------------------------------------------------------------
// CSIPSecSIMRecord::SetAuthenticationDataL
// ----------------------------------------------------------------------------
//
void CSIPSecSIMRecord::SetAuthenticationDataL( const TDesC8& aNonce )
    {
    HBufC8* decoded = HBufC8::NewLC( aNonce.Length() );
    TPtr8 decodedPtr( decoded->Des() );
    User::LeaveIfError( DecodeBase64Encoded( aNonce, decodedPtr ) );    
    __ASSERT_ALWAYS( decodedPtr.Length() >= 
                     ( RMobilePhone::KRandLength + RMobilePhone::KAutnLength ),
                     User::Leave( KErrCorrupt ) );

    TPtrC8 rand = decodedPtr.Left( RMobilePhone::KRandLength );
    TPtrC8 autn = decodedPtr.Mid( RMobilePhone::KRandLength,
                                  RMobilePhone::KAutnLength );

	HBufC8* newRAND = rand.AllocLC();
	HBufC8* newAUTN = autn.AllocL();
	CleanupStack::Pop( newRAND );

	CleanupStack::PopAndDestroy( decoded );
	
    delete iRAND;
    iRAND = newRAND;
    
    delete iAUTN;
    iAUTN = newAUTN;
    }
    
// ----------------------------------------------------------------------------
// CSIPSecSIMRecord::Invalidate
// ----------------------------------------------------------------------------
//
void CSIPSecSIMRecord::Invalidate( CSIPSecUserRecord::TUpdateState aState )
    {
    CSIPSecUserRecord::Invalidate( aState );

    delete iAUTS;
    iAUTS = NULL;

    iFailureType = ENoFailure;
    }

// ----------------------------------------------------------------------------
// CSIPSecSIMRecord::DidAuthenticationFail
// ----------------------------------------------------------------------------
//
TBool CSIPSecSIMRecord::DidAuthenticationFail() const
	{
	return iFailureType != ENoFailure;
	}

// ----------------------------------------------------------------------------
// CSIPSecSIMRecord::RAND
// ----------------------------------------------------------------------------
//
const TDesC8& CSIPSecSIMRecord::RAND() const
    {
    return ValueOrEmptyDescriptor( iRAND );
    }

// ----------------------------------------------------------------------------
// CSIPSecSIMRecord::AUTN
// ----------------------------------------------------------------------------
//
const TDesC8& CSIPSecSIMRecord::AUTN() const
    {
    return ValueOrEmptyDescriptor( iAUTN );
    }

// ----------------------------------------------------------------------------
// CSIPSecSIMRecord::SetResynchronizationDataL
// ----------------------------------------------------------------------------
//
void CSIPSecSIMRecord::SetResynchronizationDataL( const TDesC8& aAUTS )
    {    
    __ASSERT_ALWAYS( aAUTS.Length() > 0, User::Leave( KErrArgument ) );

    HBufC8* newAUTS = aAUTS.AllocL();    
    delete iAUTS;
    iAUTS = newAUTS;    
    }

// ----------------------------------------------------------------------------
// CSIPSecSIMRecord::AUTS
// ----------------------------------------------------------------------------
//
const TDesC8& CSIPSecSIMRecord::AUTS() const
    {    
    return ValueOrEmptyDescriptor( iAUTS );
    }

// ----------------------------------------------------------------------------
// CSIPSecSIMRecord::UpdateMessageCounter
// ----------------------------------------------------------------------------
//
TBool CSIPSecSIMRecord::UpdateMessageCounter( TUint aResponseCode )
	{
	TBool is401 = ( aResponseCode == 401 );
	TBool error = is401 &&
		( ++iSuccessive401Responses > EMaxSuccessive401ResponsesAllowed );

	if ( !is401 || error )
		{
		iSuccessive401Responses = 0;
		}
	return error;
	}

// ----------------------------------------------------------------------------
// CSIPSecSIMRecord::CreateObserverL
// ----------------------------------------------------------------------------
//
CSIPSecDigestObserver*
CSIPSecSIMRecord::CreateObserverL( MSIPSecSecurityMechanismObserver& aObserver )
    {
    return new ( ELeave ) CSIPSecSIMCredentialsObserver( aObserver );
    }

// ----------------------------------------------------------------------------
// CSIPSecSIMRecord::CreateProviderL
// ----------------------------------------------------------------------------
//
CSIPSecCredentialsProvider* CSIPSecSIMRecord::CreateProviderL()
    {
    return CSIPSecSIMCredentialsProvider::NewL( *this );
    }

// ----------------------------------------------------------------------------
// CSIPSecSIMRecord::DecodeBase64Encoded
// ----------------------------------------------------------------------------
//  
TInt CSIPSecSIMRecord::DecodeBase64Encoded( const TDesC8& aEncoded,
											TDes8& aDecoded )
    {
    const TUint KAsciiToBase64Offset = 43;
	const TInt KMaskShiftZero = 0;
	const TInt KMaskShiftTwo  = 2;
	const TInt KMaskShiftFour = 4;
	const TInt KMaskShiftSix  = 6;
	const TInt KBase64PadChar = 64;
	const TInt KEightBitMask  = 0xff;

    aDecoded.Zero();
    TLex8 encodedLex( aEncoded );
	TInt maskShift = KMaskShiftSix;
	TUint shiftStorage = KMaskShiftZero;

	TChar chr = encodedLex.Get();
	while ( chr )
		{
		TInt offsetChar = ( chr - KAsciiToBase64Offset );
		if ( offsetChar < 0 || offsetChar >= KAsciiToBase64TableSize )
			{
	        return KErrCorrupt; // Not a valid Base64 character
	        }

		// Read in next character and Base64 decode
		TInt decodedInt = KAsciiToBase64Table[ offsetChar ];

		// Return when a PAD char is reached
		if ( decodedInt == KBase64PadChar )
			{
			return KErrNone;
			}

		// Ensures the first 2 chars of 4 are received before processing
		if ( maskShift == KMaskShiftSix )
		    {
		    maskShift = KMaskShiftFour;
		    }
		else
			{
			shiftStorage = ( shiftStorage << KMaskShiftSix ) | decodedInt;
			TChar decodedChr = ( shiftStorage >> maskShift ) & KEightBitMask;
			maskShift -= KMaskShiftTwo;
			if ( maskShift < KMaskShiftZero )
			    {
			    maskShift = KMaskShiftSix;
			    }
			aDecoded.Append( decodedChr );
			}

		shiftStorage = decodedInt;
	    chr = encodedLex.Get();
		}

    if ( maskShift < KMaskShiftSix )
        {
	    return KErrCorrupt;
	    }

	return KErrNone;
    }
