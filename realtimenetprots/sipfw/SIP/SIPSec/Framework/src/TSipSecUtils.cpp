// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : SipSecUtils.cpp
// Part of       : SIPSec
// Version       : SIP/4.0 
//



#include <in_sock.h>
#include <e32math.h>
#include <commdbconnpref.h>

#include "sipmessage.h"
#include "sipstrings.h"
#include "sipsupportedheader.h"
#include "siprequireheader.h"
#include "sipproxyrequireheader.h"
#include "sipsecurityheaderbase.h"

#include "SipSecUtils.h"

_LIT8(KComma, ",");

// ----------------------------------------------------------------------------
// SipSecUtils::Match
// ----------------------------------------------------------------------------
//
TBool SipSecUtils::Match( SipStrConsts::TStrings aStringConst,
				    	  const RStringF& aString )
	{
	return ( _STRINGF( aStringConst ) ) == aString;
	}

// ----------------------------------------------------------------------------
// SipSecUtils::SecHeaders
// ----------------------------------------------------------------------------
//
TSglQueIter<CSIPHeaderBase>
SipSecUtils::SecHeaders( CSIPMessage& aMessage,
					     SipStrConsts::TStrings aHeaderStrConst,
						 TBool& aFound )
	{
	RStringF str = _STRINGF( aHeaderStrConst );
	TSglQueIter< CSIPHeaderBase > headers = aMessage.Headers( str );

	aFound = aMessage.HasHeader( str );
	return headers;
	}

// ----------------------------------------------------------------------------
// SipSecUtils::SingleHeader
// ----------------------------------------------------------------------------
//
CSIPHeaderBase*
SipSecUtils::SingleHeader( CSIPMessage& aMessage,
					       SipStrConsts::TStrings aHeaderStrConst )
	{
	TBool found = EFalse;
	TSglQueIter< CSIPHeaderBase > headers =
		SipSecUtils::SecHeaders( aMessage, aHeaderStrConst, found );
	if ( found )
		{
		headers.SetToFirst();
		return &( *headers );
		}
	return NULL;
	}
	
// ----------------------------------------------------------------------------
// SipSecUtils::HeaderWithMechanism
// ----------------------------------------------------------------------------
//	
CSIPSecurityHeaderBase*
SipSecUtils::HeaderWithMechanism( CSIPMessage& aMessage,
								  SipStrConsts::TStrings aHeaderStrConst,
								  const TDesC8& aMechanism )
	{
	TBool found = EFalse;
	TSglQueIter<CSIPHeaderBase> sh = SipSecUtils::SecHeaders( aMessage, 
								  						      aHeaderStrConst,
						          							  found );
	if ( found )
		{
		sh.SetToFirst();
		CSIPSecurityHeaderBase* 
			sch = static_cast< CSIPSecurityHeaderBase *>( &( *sh ) );
		while ( sch )
			{
			const TDesC8& mech = sch->MechanismName();
			if ( mech.Compare( aMechanism ) == 0 )
				{
				return sch;
				}
			//TSglQueIter doesn't have pre-increment, so silence pc-lint warning
			//lint -e1757
			sh++;
			sch = static_cast<CSIPSecurityHeaderBase*>( &( *sh ) );
			}
		}
	return NULL;
	}
	
// ----------------------------------------------------------------------------
// SipSecUtils::HeadersWithMechanismLC
// ----------------------------------------------------------------------------
//	
RPointerArray< CSIPSecurityHeaderBase >*
SipSecUtils::HeadersWithMechanismLC(    
    CSIPMessage& aMessage,
	SipStrConsts::TStrings aHeaderStrConst,
	const TDesC8& aMechanism )
	{
	const TInt KHeadersArrGranularity = 2;
	RPointerArray< CSIPSecurityHeaderBase >* headers = 
	    new ( ELeave ) RPointerArray< CSIPSecurityHeaderBase >(
	                                            KHeadersArrGranularity );
	
	TCleanupItem cleanupItem( ResetAndDeleteArray, headers );
	CleanupStack::PushL( cleanupItem );
	
	TBool found( EFalse );
	TSglQueIter<CSIPHeaderBase> sh = SipSecUtils::SecHeaders( aMessage, 
								  						      aHeaderStrConst,
						          							  found );
	if ( found )
		{
		sh.SetToFirst();
		CSIPSecurityHeaderBase* 
			sch = static_cast< CSIPSecurityHeaderBase *>( &( *sh ) );
		while ( sch )
			{
			const TDesC8& mech = sch->MechanismName();
			if ( mech.Compare( aMechanism ) == 0 )
				{
				headers->AppendL( sch );
				}
			//TSglQueIter doesn't have pre-increment, so silence pc-lint warning
			//lint -e1757
			sh++;
			sch = static_cast<CSIPSecurityHeaderBase*>( &( *sh ) );
			}
		}
		
    return headers;
	}

// ----------------------------------------------------------------------------
// SipSecUtils::ResetAndDeleteArray
// ----------------------------------------------------------------------------
//		
void SipSecUtils::ResetAndDeleteArray( TAny* aHeaders )
	{
	if ( !aHeaders )
	    {
	    return;
	    }
    RPointerArray< CSIPSecurityHeaderBase >* headers =
	    reinterpret_cast< RPointerArray< CSIPSecurityHeaderBase >* >( 
	        aHeaders );
	        
	headers->Reset();
	
	delete headers;
	}

// ----------------------------------------------------------------------------
// SipSecUtils::HeaderWithHighestVaL
// ----------------------------------------------------------------------------
//	
CSIPSecurityHeaderBase* SipSecUtils::HeaderWithHighestVaL(
	RPointerArray<CSIPSecurityHeaderBase>& aHeaders,
	const TDesC8& aMechanism,
	const TDesC8& aParameter )
	{
	CSIPSecurityHeaderBase* h = NULL;
	TReal64 sv = 0;
	TReal64 cv = 0;

	for ( TInt i = 0; i < aHeaders.Count(); i++ )
		{		
		if ( aHeaders[ i ]->MechanismName().Compare( aMechanism ) == 0 )
			{
			ValueL( *( aHeaders[ i ] ), aParameter, cv );
			if ( cv > sv )
				{
				sv = cv;
				cv = 0;
				h = aHeaders[ i ];
				}
			}
		}
	return h;
	}				
				
// ----------------------------------------------------------------------------
// SipSecUtils::HeaderWithParamVaL
// ----------------------------------------------------------------------------
//	
CSIPSecurityHeaderBase* SipSecUtils::HeaderWithParamVaL( 
	RPointerArray<CSIPSecurityHeaderBase>& aHeaders,
	const TDesC8& aMechanism,
	const TDesC8& aParameter,
	const TDesC8& aValue )
	{
	for ( TInt i = 0; i < aHeaders.Count(); i++ )
		{
		const TDesC8& mech = aHeaders[ i ]->MechanismName();
		if ( mech.Compare( aMechanism ) == 0 && 
			 SipSecUtils::HasParamL( *( aHeaders[ i ] ), aParameter ) )
			{
			RStringF v = SipSecUtils::ValueFL( *( aHeaders[ i ] ), aParameter);
			if ( v.DesC() == aValue )
				{
				return aHeaders[ i ];
				}
			}
		}
	User::Leave( KErrNotFound );
	return NULL;
	}

// ----------------------------------------------------------------------------
// SipSecUtils::HasHeaderWithValue
// ----------------------------------------------------------------------------
//
TBool SipSecUtils::HasHeaderWithValue( CSIPMessage& aMessage,
								 	   SipStrConsts::TStrings aHeader,
								 	   SipStrConsts::TStrings aValue )
	{
	TBool found = EFalse;
	TSglQueIter< CSIPHeaderBase > headers =
		SecHeaders( aMessage, aHeader, found );

	if ( found )
		{
		headers.SetToFirst();
		CSIPHeaderBase* header = headers;
		while ( header )
			{
			if ( Match(aValue,
					   static_cast<CSIPTokenHeaderBase*>( header )->Value() ) )
				{
				return ETrue;
				}
			header = headers++;
			}
		}
	return EFalse;
	}

// ----------------------------------------------------------------------------
// SipSecUtils::AddSupportedHeaderL
// Add "sec-agree" option tag to Supported-header, unless already present.
// ----------------------------------------------------------------------------
//	
void SipSecUtils::AddSupportedHeaderL( CSIPMessage& aMessage,
								  	   SipStrConsts::TStrings aValue )
	{
	if ( !HasHeaderWithValue( aMessage, SipStrConsts::ESupportedHeader, aValue) )
		{
		aMessage.AddHeaderL( CSIPSupportedHeader::NewLC( _STRINGF( aValue ) ) );
		CleanupStack::Pop(); //CSIPSupportedHeader
		}
	}
	
// ----------------------------------------------------------------------------
// SipSecUtils::AddRequireHeadersL
// Add Require and Proxy-Require headers with value "sec-agree", unless already
// present.
// ----------------------------------------------------------------------------
//	
void SipSecUtils::AddRequireHeadersL( CSIPMessage& aMessage,
								  	  SipStrConsts::TStrings aValue )
	{
	if ( !HasHeaderWithValue( aMessage, SipStrConsts::ERequireHeader, aValue) )
		{
		aMessage.AddHeaderL( CSIPRequireHeader::NewLC( _STRINGF( aValue ) ) );
		CleanupStack::Pop(); //CSIPRequireHeader
		}

	if ( !HasHeaderWithValue( aMessage,
							  SipStrConsts::EProxyRequireHeader,
							  aValue) )
		{
		aMessage.AddHeaderL(
			CSIPProxyRequireHeader::NewLC( _STRINGF( aValue ) ) );
		CleanupStack::Pop(); //CSIPProxyRequireHeader
		}	
	}

// ----------------------------------------------------------------------------
// SipSecUtils::RandomNumber
// ----------------------------------------------------------------------------
//	
TUint
SipSecUtils::RandomNumber( TInt64& aSeed, TUint aMinLimit, TUint aMaxLimit )
	{	
	TReal range = aMaxLimit - aMinLimit;
	TReal random = Math::Rand( aSeed );
	TReal res = 0;
	Math::Mod( res, random, range );
	return static_cast< TUint >( aMinLimit + res );	
	}

// ----------------------------------------------------------------------------
// SipSecUtils::ValueL
// ----------------------------------------------------------------------------
//
TUint SipSecUtils::ValueL( const CSIPParameterHeaderBase& aHeader, 
					  	   const TDesC8& aParameter )
	{
	RStringF p = SIPStrings::Pool().OpenFStringL( aParameter );
	CleanupClosePushL( p );
	TLex8 lx( aHeader.ParamValue( p ).DesC() );
	TUint val = 0;
	User::LeaveIfError( lx.Val( val ) );
	CleanupStack::PopAndDestroy(); //p
	return val;
	}

// ----------------------------------------------------------------------------
// SipSecUtils::ValueL
// ----------------------------------------------------------------------------
//
void SipSecUtils::ValueL( const CSIPParameterHeaderBase& aHeader, 
					  	  const TDesC8& aParameter,
					  	  TReal64& aValue )
	{
	RStringF p = SIPStrings::Pool().OpenFStringL( aParameter );
	CleanupClosePushL( p );
	TLex8 lx( aHeader.ParamValue( p ).DesC() );
	// SIP always uses dot as the decimal separator
	const TChar KDotChr = '.';
	User::LeaveIfError( lx.Val( aValue , KDotChr ) ); 
	CleanupStack::PopAndDestroy(); //p
	}
	
// ----------------------------------------------------------------------------
// SipSecUtils::ValueFL
// ----------------------------------------------------------------------------
//
RStringF SipSecUtils::ValueFL( const CSIPParameterHeaderBase& aHeader, 
					   		   const TDesC8& aParameter )
	{
	RStringF p = SIPStrings::Pool().OpenFStringL( aParameter );
	CleanupClosePushL( p );
	RStringF v = aHeader.ParamValue( p );
	CleanupStack::PopAndDestroy(); //p
	return v;
	}
	
// ----------------------------------------------------------------------------
// SipSecUtils::SetParamL
// ----------------------------------------------------------------------------
//
void SipSecUtils::SetParamL( CSIPParameterHeaderBase& aSecHeader,
						 	  const TDesC8& aParameter,
						 	  const TDesC8& aValue )
	{
	RStringF p = SIPStrings::Pool().OpenFStringL( aParameter );
	CleanupClosePushL( p );
	RStringF v = SIPStrings::Pool().OpenFStringL( aValue );
	CleanupClosePushL( v );
	aSecHeader.SetParamL( p, v );
	CleanupStack::PopAndDestroy( 2 ); //v, p
	}

// ----------------------------------------------------------------------------
// SipSecUtils::SetParamL
// ----------------------------------------------------------------------------
//
void SipSecUtils::SetParamL( CSIPParameterHeaderBase& aSecHeader,
							 const TDesC8& aParameter,
						 	 TUint aValue )
	{
	//Long enough to hold aValue as descriptor
	const TInt KNumberBufLength = 11;
	TBuf8<KNumberBufLength> val;

	TInt64 value = aValue;
	val.Num( value );
	SetParamL( aSecHeader, aParameter, val );
	}
	
// ----------------------------------------------------------------------------
// SipSecUtils::HasParamL
// ----------------------------------------------------------------------------
//
TBool SipSecUtils::HasParamL( CSIPParameterHeaderBase& aSecHeader,
 					     	  const TDesC8& aParameter )
	{
	RStringF param = SIPStrings::Pool().OpenFStringL( aParameter );
	CleanupClosePushL( param );
	TBool hasParam = aSecHeader.HasParam( param );
	CleanupStack::PopAndDestroy(); //param
	return hasParam;
	}

// ----------------------------------------------------------------------------
// SipSecUtils::EncodeHeadersL
// ----------------------------------------------------------------------------
//
CBufBase* SipSecUtils::EncodeHeadersL( TSglQueIter< CSIPHeaderBase >& aIter )
    {
    const TInt KHeaderBufExpandSize = 50;
    CBufFlat* encodedHeaders = CBufFlat::NewL( KHeaderBufExpandSize );
    CleanupStack::PushL( encodedHeaders );
    TInt encodedLength = 0;
    TInt headerCountInList = 0;
    while ( aIter )
        {
	    CSIPHeaderBase* header = aIter++;
        HBufC8* headerAsText = NULL;
        if ( ++headerCountInList == 1 )
            {
	        headerAsText = header->ToTextLC();
            }
        else
            {
            headerAsText = header->ToTextValueLC();
            }
        encodedHeaders->InsertL( encodedLength, *headerAsText );
        encodedLength += headerAsText->Length();
        CleanupStack::PopAndDestroy( headerAsText );
        if ( aIter )
            {
            encodedHeaders->InsertL( encodedLength, KComma );
            encodedLength += KComma().Length();
            }
        }
    CleanupStack::Pop( encodedHeaders );
    return encodedHeaders;    
    }
