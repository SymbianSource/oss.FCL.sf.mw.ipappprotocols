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
// Name          : sipsecplugincontext.cpp
// Part of       : SIPSec/DigestPlugin
// Version       : SIP/6.0
//



#include "sipsecplugincontext.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "sipsecdigestcache.h"
#include "MSIPSecSecurityMechanismObserver.h"
#include "sipsecurityserverheader.h"

const TInt KSIPSecHeaderOffset = _FOFF( CSIPHeaderBase,iLink );

// ============================ MEMBER FUNCTIONS ==============================


// ----------------------------------------------------------------------------
// TSIPSecPluginContext::TSIPSecPluginContext
// ----------------------------------------------------------------------------
//
TSIPSecPluginContext::TSIPSecPluginContext( CSIPSecDigestPlugin& aPlugin,                                            
                                            const CUri8& aRemoteTarget,
                                            const TDesC8& aOutboundProxy,
                                            MSIPSecUser& aUser,
                                            TRegistrationId aRegistrationId,
                                            TTransactionId aTransactionId ) :
    iPlugin( aPlugin ),
    iRemoteTarget( aRemoteTarget ),
    iOutboundProxy( aOutboundProxy ),
    iUser( aUser ),
    iRegistrationId( aRegistrationId ),
    iTransactionId( aTransactionId )
    {
    }

// ----------------------------------------------------------------------------
// TSIPSecPluginContext::AlsoTrustedUser
// ----------------------------------------------------------------------------
//
TBool TSIPSecPluginContext::AlsoTrustedUser()
	{
	return ETrue;
	}

// ----------------------------------------------------------------------------
// TSIPSecPluginContext::RemoteTarget
// ----------------------------------------------------------------------------
//	
const CUri8& TSIPSecPluginContext::RemoteTarget() const
    {
    return iRemoteTarget;
    }

// ----------------------------------------------------------------------------
// TSIPSecPluginContext::OutboundProxy
// ----------------------------------------------------------------------------
//	
const TDesC8& TSIPSecPluginContext::OutboundProxy() const
    {
    return iOutboundProxy;
    }

// ----------------------------------------------------------------------------
// TSIPSecPluginContext::SIPSecUser
// ----------------------------------------------------------------------------
//
MSIPSecUser& TSIPSecPluginContext::SIPSecUser()
    {
    return iUser;
    }

// ----------------------------------------------------------------------------
// TSIPSecPluginContext::Plugin
// ----------------------------------------------------------------------------
//
CSIPSecDigestPlugin& TSIPSecPluginContext::Plugin() const
    {
    return iPlugin;
    }

// ----------------------------------------------------------------------------
// TSIPSecPluginContext::RegistrationId
// ----------------------------------------------------------------------------
//
TRegistrationId TSIPSecPluginContext::RegistrationId() const
	{
	return iRegistrationId;
	}

// ----------------------------------------------------------------------------
// TSIPSecPluginContext::TransactionId
// ----------------------------------------------------------------------------
//
TTransactionId TSIPSecPluginContext::TransactionId() const
	{
	return iTransactionId;	
	}

// ============================ MEMBER FUNCTIONS ===============================
	
// ----------------------------------------------------------------------------
// TSIPSecPluginCtxRequest::TSIPSecPluginCtxRequest
// ----------------------------------------------------------------------------
//		
TSIPSecPluginCtxRequest::TSIPSecPluginCtxRequest( CSIPSecDigestPlugin& aPlugin,                                                  
	const CUri8& aRemoteTarget,
	const TDesC8& aOutboundProxy,
	CSIPRequest& aSIPRequest,
	MSIPSecUser& aUser,
	TRegistrationId aRegistrationId,
	TTransactionId aTransactionId ) :
    TSIPSecPluginContext( aPlugin,
    					  aRemoteTarget,
    					  aOutboundProxy,
    					  aUser,
    					  aRegistrationId,
    					  aTransactionId ),
    iSIPRequest( aSIPRequest )
    {
    }
	  					
// ----------------------------------------------------------------------------
// TSIPSecPluginCtxRequest::SIPRequest
// ----------------------------------------------------------------------------
//		
CSIPRequest& TSIPSecPluginCtxRequest::SIPRequest()
    {
    return iSIPRequest;	
    }



// ============================ MEMBER FUNCTIONS ==============================
	
// ----------------------------------------------------------------------------
// TSIPSecPluginCtxResponse::TSIPSecPluginCtxResponse
// ----------------------------------------------------------------------------
//		
TSIPSecPluginCtxResponse::TSIPSecPluginCtxResponse( 
                                CSIPSecDigestPlugin& aPlugin,                                    
                                const CUri8& aRemoteTarget,
                                const TDesC8& aOutboundProxy,
                                CSIPResponse& aSIPResponse,
                                CSIPSecDigestCache& aCache,
                                MSIPSecSecurityMechanismObserver& aObserver,
                                MSIPSecUser& aUser,
                                TRegistrationId aRegistrationId,
                                TTransactionId aTransactionId ) :
                                                    
    TSIPSecPluginContext( aPlugin,
    					  aRemoteTarget,
    					  aOutboundProxy,
    					  aUser,
    					  aRegistrationId,
    					  aTransactionId ),
    iCache( aCache ),
    iObserver( aObserver ),
    iSIPResponse( aSIPResponse )
    {
    }

// ----------------------------------------------------------------------------
// TSIPSecPluginCtxResponse::Cache
// ----------------------------------------------------------------------------
//	
CSIPSecDigestCache& TSIPSecPluginCtxResponse::Cache()
    {
    return iCache;	
    }

// ----------------------------------------------------------------------------
// TSIPSecPluginCtxResponse::Observer
// ----------------------------------------------------------------------------
//	
MSIPSecSecurityMechanismObserver& TSIPSecPluginCtxResponse::Observer()
    {
    return iObserver;
    }


// ----------------------------------------------------------------------------
// TSIPSecPluginCtxResponse::SIPResponse
// ----------------------------------------------------------------------------
//	
CSIPResponse& TSIPSecPluginCtxResponse::SIPResponse()
    {
    return iSIPResponse;	
    }



// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// TSIPSecPluginCtxVerify::TSIPSecPluginCtxVerify
// ----------------------------------------------------------------------------
//	
TSIPSecPluginCtxVerify::TSIPSecPluginCtxVerify( 
                    CSIPSecDigestPlugin& aPlugin,                    
                    const CUri8& aRemoteTarget,
                    const TDesC8& aOutboundProxy,
                    CSIPRequest& aSIPRequest, 
                    CSIPSecurityVerifyHeader& aVerifyHeader,
                    RPointerArray<CSIPSecurityServerHeader>& aServerHeaders,
                    MSIPSecUser& aUser,
                    TRegistrationId aRegistrationId ) :
    TSIPSecPluginCtxRequest( aPlugin,
    						 aRemoteTarget, 
                             aOutboundProxy,
                             aSIPRequest,
                             aUser,
                             aRegistrationId,
                             KEmptyTransactionId ),
    iVerifyHeader( aVerifyHeader ),
    iServerHeaders( KSIPSecHeaderOffset )
    {
    for ( TInt i = 0; i < aServerHeaders.Count() ; ++i )
        {
        iServerHeaders.AddLast( *aServerHeaders[ i ] );
        }
    }
	
// ----------------------------------------------------------------------------
// TSIPSecPluginCtxVerify::SecurityVerifyHeader
// ----------------------------------------------------------------------------
//	
CSIPSecurityVerifyHeader& TSIPSecPluginCtxVerify::SecurityVerifyHeader()
    {
    return iVerifyHeader;
    }

// ----------------------------------------------------------------------------
// TSIPSecPluginCtxVerify::SecurityServerHeaders
// ----------------------------------------------------------------------------
//	
TSglQue<CSIPHeaderBase>& TSIPSecPluginCtxVerify::SecurityServerHeaders()
    {
    return iServerHeaders;    
    }



// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// TSIPSecPluginCtxInfo::TSIPSecPluginCtxInfo
// ----------------------------------------------------------------------------
//	
TSIPSecPluginCtxInfo::TSIPSecPluginCtxInfo( 
	CSIPSecDigestPlugin& aPlugin,     
	const CUri8& aRemoteTarget,
	const TDesC8& aOutboundProxy,
	CSIPResponse& aSIPResponse,
	CSIPAuthenticationInfoHeader& aAuthInfo,
	CSIPSecDigestCache& aCache,
	MSIPSecSecurityMechanismObserver& aObserver,
	MSIPSecUser& aUser,
	TRegistrationId aRegistrationId ) :
    TSIPSecPluginCtxResponse( aPlugin, aRemoteTarget, aOutboundProxy,
    						  aSIPResponse, aCache, aObserver, aUser,
    						  aRegistrationId, KEmptyTransactionId ),
    iAuthInfo( aAuthInfo )
    {
    }

// ----------------------------------------------------------------------------
// TSIPSecPluginCtxInfo::AuthenticationInfoHeader
// ----------------------------------------------------------------------------
//	
CSIPAuthenticationInfoHeader& TSIPSecPluginCtxInfo::AuthenticationInfoHeader()
    {
    return iAuthInfo;
    }

// ----------------------------------------------------------------------------
// TSIPSecPluginCtxInfo::AlsoTrustedUser
// Let only the exact SIPSec user to process the Authentication-Info.
// ----------------------------------------------------------------------------
//
TBool TSIPSecPluginCtxInfo::AlsoTrustedUser()
	{
	return EFalse;
	}

// End of File
