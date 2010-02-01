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
// Name          : sipsecdigestcontext.cpp
// Part of       : SIPSec/DigestPlugin
// Version       : SIP/6.0
//



#include "sipresponse.h"
#include "siprequest.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "sipauthorizationheaderbase.h"
#include "sipsecurityverifyheader.h"
#include "sipsecdigestcontext.h"
#include "sipsecdigestcache.h"
#include "sipsecrequestdata.h"
#include "CSIPSecDigest.h"
#include "CSIPSecCredentials.h"
#include "CSIPSecUserRecord.h"
#include "SipSecUtils.h"
#include "CSIPSecDigestObserver.h"

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// TSIPSecDigestContext::TSIPSecDigestContext
// -----------------------------------------------------------------------------
//
TSIPSecDigestContext::TSIPSecDigestContext( CSIPSecCredentials& aCredentials,
                                            TSIPSecPluginContext* aParent ) :
    iCredentials( aCredentials ),
    iParent( aParent )
    {
    }

// -----------------------------------------------------------------------------
// TSIPSecDigestContext::Mechanism
// -----------------------------------------------------------------------------
//
CSIPSecDigest& TSIPSecDigestContext::Mechanism() const
    {
    return iCredentials.Mechanism();
    }

// -----------------------------------------------------------------------------
// TSIPSecDigestContext::Parent
// -----------------------------------------------------------------------------
//
TSIPSecPluginContext& TSIPSecDigestContext::Parent()
    {
    __ASSERT_ALWAYS( iParent,
    	User::Panic( _L( "SIPSecDigestCtx:Parent" ), KErrNotFound ) );

    return *iParent;
    }

// -----------------------------------------------------------------------------
// TSIPSecDigestContext::UserData
// -----------------------------------------------------------------------------
//
CSIPSecUserRecord& TSIPSecDigestContext::UserData()
	{
	return iCredentials.UserData();
	}

// -----------------------------------------------------------------------------
// TSIPSecDigestContext::SetUserData
// -----------------------------------------------------------------------------
//
void TSIPSecDigestContext::SetUserData( CSIPSecUserRecord& aUserData )
	{
	iCredentials.SetUserData( aUserData );
	}

// -----------------------------------------------------------------------------
// TSIPSecDigestContext::GetParam
// -----------------------------------------------------------------------------
//
const TDesC8& TSIPSecDigestContext::GetParam( const CSIPAuthHeaderBase& aHeader,
											  TInt aParam )
    {
    return aHeader.ParamValue( SIPStrings::StringF( aParam ) ).DesC();
    }

// -----------------------------------------------------------------------------
// TSIPSecDigestContext::GetDesParam
// -----------------------------------------------------------------------------
//
const TDesC8&
TSIPSecDigestContext::GetDesParam( const CSIPAuthHeaderBase& aHeader,
								   TInt aParam )
    {
    return aHeader.DesParamValue( SIPStrings::StringF( aParam ) );
    }

// -----------------------------------------------------------------------------
// TSIPSecDigestContext::SetParamValueL
// -----------------------------------------------------------------------------
//
void TSIPSecDigestContext::SetParamValueL( CSIPAuthHeaderBase& aHeader,
										   TInt aParam,
										   const TDesC8& aValue )
    {
    if ( aValue.Length() > 0 )
        {
		RStringF value = SIPStrings::Pool().OpenFStringL( aValue );
		CleanupClosePushL( value );
		aHeader.SetParamL( SIPStrings::StringF( aParam ), value );
		CleanupStack::PopAndDestroy(); //value
        }
    }

// -----------------------------------------------------------------------------
// TSIPSecDigestContext::SetDesParamValueL
// -----------------------------------------------------------------------------
//
void TSIPSecDigestContext::SetDesParamValueL( CSIPAuthHeaderBase& aHeader,
										   	  TInt aParam,
										      const TDesC8& aValue )
    {
    if ( aValue.Length() > 0 )
        {
		aHeader.SetDesParamL( SIPStrings::StringF( aParam ), aValue );			
        }
    }

// -----------------------------------------------------------------------------
// TSIPSecDigestContext::SetParamWithEmptyValueL
// -----------------------------------------------------------------------------
//
void TSIPSecDigestContext::SetParamWithEmptyValueL( TInt aParam )
	{
	iCredentials.Content().SetParamAndAddQuotesL( SIPStrings::StringF( aParam ),
												  KNullDesC8 );
	}

// -----------------------------------------------------------------------------
// TSIPSecDigestContext::GetParam
// -----------------------------------------------------------------------------
//
const TDesC8& TSIPSecDigestContext::GetParam( TInt aParam ) const
    {
    return GetParam( iCredentials.Content(), aParam );
    }

// -----------------------------------------------------------------------------
// TSIPSecDigestContext::GetDesParam
// -----------------------------------------------------------------------------
//
const TDesC8& TSIPSecDigestContext::GetDesParam( TInt aParam ) const
    {
    return GetDesParam( iCredentials.Content(), aParam );
    }

// -----------------------------------------------------------------------------
// TSIPSecDigestContext::SetParamValueL
// -----------------------------------------------------------------------------
//
void TSIPSecDigestContext::SetParamValueL( TInt aParam, const TDesC8& aValue )
    {
    SetParamValueL( iCredentials.Content(), aParam, aValue );
    }

// -----------------------------------------------------------------------------
// TSIPSecDigestContext::SetDesParamValueL
// -----------------------------------------------------------------------------
//
void
TSIPSecDigestContext::SetDesParamValueL( TInt aParam, const TDesC8& aValue )
    {
    SetDesParamValueL( iCredentials.Content(), aParam, aValue );
    }

// -----------------------------------------------------------------------------
// TSIPSecDigestContext::Nonce
// -----------------------------------------------------------------------------
//
const TDesC8& TSIPSecDigestContext::Nonce() const
    {
    return GetDesParam( SipStrConsts::ENonce );
    }

// -----------------------------------------------------------------------------
// TSIPSecDigestContext::NonceCount
// -----------------------------------------------------------------------------
//
const TDesC8& TSIPSecDigestContext::NonceCount() const
	{
	return GetParam( SipStrConsts::ENonceCount );
	}

// -----------------------------------------------------------------------------
// TSIPSecDigestContext::CNonce
// -----------------------------------------------------------------------------
//
const TDesC8& TSIPSecDigestContext::CNonce() const
	{
    return GetDesParam( SipStrConsts::ECNonce );
	}

// -----------------------------------------------------------------------------
// TSIPSecDigestContext::DigestURI
// -----------------------------------------------------------------------------
//
const TDesC8& TSIPSecDigestContext::DigestURI() const
    {
    return GetParam( SipStrConsts::EUri );
    }



// ============================ MEMBER FUNCTIONS ===============================
	
	
// ----------------------------------------------------------------------------
// TSIPSecDigestCtxSetup::TSIPSecDigestCtxSetup
// ----------------------------------------------------------------------------
//
TSIPSecDigestCtxSetup::TSIPSecDigestCtxSetup(
	CSIPSecCredentials& aCredentials,
	TTransactionId aTransactionId,
	TSIPSecPluginCtxResponse* aParent ) :
	TSIPSecDigestContext( aCredentials, aParent ),
	iTransactionId( aTransactionId )
	{
	}
	  
// -----------------------------------------------------------------------------
// TSIPSecDigestCtxSetup::RequestUserCredentialsL
// -----------------------------------------------------------------------------
//
TBool TSIPSecDigestCtxSetup::RequestUserCredentialsL()
	{
    TSIPSecPluginCtxResponse& parent =
    	static_cast<TSIPSecPluginCtxResponse&>( Parent() );
    CSIPSecDigestObserver* observer =
    	parent.Cache().RegisterObserverL( parent.Observer(), UserData() );
	return UserData().RequestL( *this, *observer );
    }

// -----------------------------------------------------------------------------
// TSIPSecDigestCtxSetup::SetCNonceL
// -----------------------------------------------------------------------------
//
void TSIPSecDigestCtxSetup::SetCNonceL( const TDesC8& aCnonce )
	{
    SetDesParamValueL( SipStrConsts::ECNonce, aCnonce );
	}

// ----------------------------------------------------------------------------
// TSIPSecDigestCtxSetup::TransactionId
// ----------------------------------------------------------------------------
//
TTransactionId TSIPSecDigestCtxSetup::TransactionId() const
	{
	return iTransactionId;
	}


// ============================ MEMBER FUNCTIONS ===============================
	
	
// ----------------------------------------------------------------------------
// TSIPSecDigestCtxProcess::TSIPSecDigestCtxProcess
// ----------------------------------------------------------------------------
//
TSIPSecDigestCtxProcess::TSIPSecDigestCtxProcess(
									CSIPSecCredentials& aCredentials,
                                    TSIPSecPluginContext* aParent ) :
    TSIPSecDigestContext( aCredentials, aParent )
	{	
	}

// -----------------------------------------------------------------------------
// TSIPSecDigestCtxProcess::Method
// -----------------------------------------------------------------------------
//
const TDesC8& TSIPSecDigestCtxProcess::Method()
	{	
	return static_cast<TSIPSecPluginCtxRequest&>( Parent() ).
		SIPRequest().Method().DesC();
	}

// -----------------------------------------------------------------------------
// TSIPSecDigestCtxProcess::Message
// -----------------------------------------------------------------------------
//
const TDesC8& TSIPSecDigestCtxProcess::Message()
	{
	return static_cast<TSIPSecPluginCtxRequest&>( Parent() ).
		SIPRequest().Content();
	}

// -----------------------------------------------------------------------------
// TSIPSecDigestCtxProcess::SetResponseL
// -----------------------------------------------------------------------------
//
void TSIPSecDigestCtxProcess::SetResponseL( const TDesC8& aResponse )
	{
    SetDesParamValueL( SipStrConsts::EResponse, aResponse );
	}

// -----------------------------------------------------------------------------
// TSIPSecDigestCtxProcess::RequestDataL
// -----------------------------------------------------------------------------
//
CSIPSecRequestData*
TSIPSecDigestCtxProcess::RequestDataL( CSIPSecRequestData::TQop aQop )
    {
    return CSIPSecRequestData::NewL( *this, aQop );
    }

    
// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// TSIPSecDigestVerifyContext::RequestData
// -----------------------------------------------------------------------------
//
TSIPSecDigestVerifyContext::TSIPSecDigestVerifyContext(
	CSIPSecCredentials& aCredentials,
	const TDesC8& aUri,
	TSIPSecPluginCtxVerify* aParent ) :
    TSIPSecDigestCtxProcess( aCredentials, aParent ),
    iUri( aUri )
    {
    }

// -----------------------------------------------------------------------------
// TSIPSecDigestVerifyContext::DigestURI
// -----------------------------------------------------------------------------
//
const TDesC8& TSIPSecDigestVerifyContext::DigestURI() const
	{
    return iUri;
	}

// -----------------------------------------------------------------------------
// TSIPSecDigestVerifyContext::SetResponseL
// -----------------------------------------------------------------------------
//
void TSIPSecDigestVerifyContext::SetResponseL( const TDesC8& aResponse )
	{
    RStringF value = SIPStrings::Pool().OpenFStringL( aResponse );
    CleanupClosePushL( value );
    static_cast< TSIPSecPluginCtxVerify& >( Parent() ).
    	SecurityVerifyHeader().SetParamL( SIPStrings::StringF(
    		SipStrConsts::EDigestVerify ), value );
    CleanupStack::PopAndDestroy(); // value
	}

// -----------------------------------------------------------------------------
// TSIPSecDigestVerifyContext::RequestDataL
// -----------------------------------------------------------------------------
//
CSIPSecRequestData*
TSIPSecDigestVerifyContext::RequestDataL( CSIPSecRequestData::TQop aQop )
    {
    return CSIPSecDigestVerifyData::NewL( *this, aQop );
    }



// ----------------------------------------------------------------------------
// TSIPSecDigestVerifyContext::SecurityServerHeaderL
// Example
// 	Security-Server: ipsec-ike;q=0.1
// 	Security-Server: tls;q=0.2
// Result
// 	Security-Server:ipsec-ike;q=0.1,tls;q=0.2
// ----------------------------------------------------------------------------
//	
HBufC8* TSIPSecDigestVerifyContext::SecurityServerHeaderL()
    {
    HBufC8* serverHeader( NULL );

    TSglQueIter<CSIPHeaderBase> headers(
    	static_cast< TSIPSecPluginCtxVerify& >( Parent() ).
    		SecurityServerHeaders() );

    if ( headers )
        {
        CBufBase* encoded = SipSecUtils::EncodeHeadersL( headers );
        CleanupStack::PushL( encoded );
        TPtr8 encodedPtr( encoded->Ptr( 0 ) );
        serverHeader = encodedPtr.AllocL();
        CleanupStack::PopAndDestroy( encoded );
        }
        
    return serverHeader;    
    }


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// TSIPSecDigestAuthInfoContext::TSIPSecDigestAuthInfoContext
// -----------------------------------------------------------------------------
//
TSIPSecDigestAuthInfoContext::TSIPSecDigestAuthInfoContext(
										CSIPSecCredentials& aCredentials,
                      	                TDes8& aResponse,
                      	                TSIPSecPluginCtxResponse* aParent ) :
    TSIPSecDigestCtxProcess( aCredentials, aParent ),
    iResponse( aResponse )
    {    
    }

// -----------------------------------------------------------------------------
// TSIPSecDigestAuthInfoContext::SetResponseL
// -----------------------------------------------------------------------------
//
void TSIPSecDigestAuthInfoContext::SetResponseL( const TDesC8& aResponse )
    {
	__ASSERT_ALWAYS( iResponse.MaxLength() >= aResponse.Length(),
	 				 User::Leave( KErrArgument ) );
    iResponse.Copy( aResponse );
    }

// -----------------------------------------------------------------------------
// TSIPSecDigestAuthInfoContext::Method
// -----------------------------------------------------------------------------
//
const TDesC8& TSIPSecDigestAuthInfoContext::Method()
    {
    return KNullDesC8;
    }

// -----------------------------------------------------------------------------
// TSIPSecDigestAuthInfoContext::Message
// -----------------------------------------------------------------------------
//
const TDesC8& TSIPSecDigestAuthInfoContext::Message()
    {
	return static_cast< TSIPSecPluginCtxResponse& >( Parent() ).
		SIPResponse().Content();
    }

// End of File
