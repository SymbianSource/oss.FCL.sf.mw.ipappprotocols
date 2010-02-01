// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CSIPServerResolver.cpp
// Part of       : ServerResolver
// Version       : SIP/4.0 
//



#include <uriutils.h>
#include "CSIPServerResolver.h"
#include "CSIPHostResolver.h"
#include "CServerQuery.h"
#include "sipuri.h"
#include "uricontainer.h"
#include "siphostport.h"
#include "sipviaheader.h"

#include "MSIPResolvingResult.h"
#include "CSIPNaptrOrigDomain.h"
#include "CSIPSRVOrigDomain.h"
#include "CSIPQuerySrv.h"
#include "MSIPHostResolver.h"

#include "CSIPQueryA_AAAA.h"
#include "CSIPA_AAAAOrigDomain.h"
#include "CSIPQueryData.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "siperr.h"

// ----------------------------------------------------------------------------
// CSIPServerResolver::NewL
// ----------------------------------------------------------------------------
//
CSIPServerResolver* CSIPServerResolver::NewL( RSocketServ& aServer, 
                                              RConnection& aConnection )
	{
	CSIPServerResolver* self = NewLC( aServer, aConnection );
	CleanupStack::Pop( self );
	return self;	
	}

// ----------------------------------------------------------------------------
// CSIPServerResolver::NewLC
// ----------------------------------------------------------------------------
//
CSIPServerResolver* CSIPServerResolver::NewLC( RSocketServ& aServer, 
                                               RConnection& aConnection )
	{
	CSIPServerResolver* self = new ( ELeave ) CSIPServerResolver();
	CleanupStack::PushL( self );
	self->ConstructL( aServer, aConnection );
	return self;	
	}

// ----------------------------------------------------------------------------
// CSIPServerResolver::CSIPServerResolver
// ----------------------------------------------------------------------------
//
CSIPServerResolver::CSIPServerResolver()
	{
	}

// ----------------------------------------------------------------------------
// CSIPServerResolver::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPServerResolver::ConstructL( RSocketServ& aServer, 
                                     RConnection& aConnection )
	{
	iResolver = CSIPHostResolver::NewL( aServer, aConnection );
	}

// ----------------------------------------------------------------------------
// CSIPServerResolver::~CSIPServerResolver
// ----------------------------------------------------------------------------
//
CSIPServerResolver::~CSIPServerResolver()
	{
	delete iResolver;
	}

// ----------------------------------------------------------------------------
// CSIPServerResolver::GetByURIL
// ----------------------------------------------------------------------------
//
void CSIPServerResolver::GetByURIL( 
    const CURIContainer& aURI,
    TBool aSkipNaptrAndSrvQueries,
    RPointerArray< MSIPResolvingResult >& aResult, 
    MSIPServerResolverObserver* aObserver )
	{
	__ASSERT_ALWAYS( aURI.IsSIPURI(), User::Leave( KErrSIPInvalidURIType ) );
	
	const CSIPURI& uri = *(aURI.SIPURI());
	
	TBool forceUDP( EFalse );
	RStringF protocol = TransportProtocolL( uri, forceUDP );  

	TUint port( uri.HostPort().Port() );
		
	HBufC8* host = NULL;
	if ( uri.HasParam( SIPStrings::StringF( SipStrConsts::EMaddr ) ) )
		{
		host = uri.ParamValue( 
		    SIPStrings::StringF( SipStrConsts::EMaddr ) ).DesC().AllocL();
		}
	else
		{
		host = uri.HostPort().Host().AllocL();
		}
    CleanupStack::PushL( host );
			
	if ( !( UriUtils::HostType( *host ) == UriUtils::ETextHost ) && port == 0 )		
		{
		// The host is an IP address and there is no explicit port is in the URI
		TBool useTls = 
		    ( protocol == SIPStrings::StringF( SipStrConsts::ETLS ) );
		port = DefaultPort( useTls );
		}
	
	if ( ( UriUtils::HostType( *host ) == UriUtils::ETextHost ) &&
	     aSkipNaptrAndSrvQueries )			
		{
        if ( protocol == SIPStrings::StringF( SipStrConsts::EEmpty ) )
            {
            protocol.Close();
            protocol = DefaultTransport( uri.IsSIPSURI() );
            }
        if ( port == 0 )
            {
            port = DefaultPort( uri.IsSIPSURI() );
			}
		}
		
	ChooseQueryL( aObserver, *host, aResult, uri.IsSIPSURI(), 
	              port, protocol, forceUDP, aSkipNaptrAndSrvQueries );
	              
	CleanupStack::PopAndDestroy( host );
	}

// ----------------------------------------------------------------------------
// CSIPServerResolver::GetByViaL
// ----------------------------------------------------------------------------
//
void CSIPServerResolver::GetByViaL( 
    CSIPViaHeader& aVia,
	RPointerArray< MSIPResolvingResult >& aResult, 
    MSIPServerResolverObserver* aObserver )
    {
    TUint port( aVia.SentByHostPort().Port() );
		
	HBufC8* host = NULL;
	
	if ( aVia.HasParam(SIPStrings::StringF(SipStrConsts::EMaddr)) )
		{
		host = aVia.ParamValue( SIPStrings::StringF( 
									SipStrConsts::EMaddr ) ).DesC().AllocLC();
		}
	else
		{
		host = aVia.SentByHostPort().Host().AllocLC();
		}
	
	if ( !( UriUtils::HostType( *host ) == UriUtils::ETextHost ) )		
		{
		if ( port == 0 )
			{
			port = KSIPDefaultPort;
			}
		}
		
	ChooseQueryL( aObserver, *host, aResult, EFalse, port, aVia.Transport() );
	
	CleanupStack::PopAndDestroy( host );
	}

// ----------------------------------------------------------------------------
// CSIPServerResolver::Cancel
// ----------------------------------------------------------------------------
//
void CSIPServerResolver::Cancel( MSIPServerResolverObserver* aObserver )
	{
	iResolver->CancelGetByURI( aObserver );
	}

// ----------------------------------------------------------------------------
// CSIPServerResolver::ChooseQueryL
// ----------------------------------------------------------------------------
//
void CSIPServerResolver::ChooseQueryL( 
    MSIPServerResolverObserver* aObserver,
    const TDesC8& aTarget,
    RPointerArray< MSIPResolvingResult >& aResult,
    TBool aSipsUri,
    TUint aPort,
    RStringF aProtocol,
    TBool aForceUDP,
    TBool aSkipNaptrAndSrvQueries )
	{	
	CSIPQueryData* querydata = CSIPQueryData::NewL( aObserver,
												    aPort, 
												    aProtocol,
												    aTarget,
												    *iResolver,
												    aResult,
												    aSipsUri,
												    aForceUDP );
	CleanupStack::PushL( querydata );	
	CServerQuery* query = CServerQuery::NewL( aObserver, querydata );
	CleanupStack::Pop( querydata );
	CleanupStack::PushL( query );
		
	if ( aPort || aSkipNaptrAndSrvQueries )
		{
		SetA_AAAAQueryL( query );
		}
	else
		{
		if ( aProtocol != SIPStrings::StringF( SipStrConsts::EEmpty ) )
			{
			SetSRVQueryL( query );
			}
		else
			{
			// NAPTR query is done only when resolving by URI, and neither the
			// transport protocol nor port was specified in the URI.
			SetNaptrQueryL( query );
			}
		}
		
	CleanupStack::Pop( query );
	}

// ----------------------------------------------------------------------------
// CSIPServerResolver::SetA_AAAAQueryL
// ----------------------------------------------------------------------------
//
void CSIPServerResolver::SetA_AAAAQueryL( CServerQuery* aQuery )
	{
	CSIPA_AAAAOrigDomain* querycondition =
										 CSIPA_AAAAOrigDomain::NewL( *aQuery );																
	CleanupStack::PushL( querycondition );
	aQuery->SetQueryConditionL( querycondition );
	CleanupStack::Pop( querycondition );		 								 
	iResolver->GetByQueryL( aQuery );
	}

// ----------------------------------------------------------------------------
// CSIPServerResolver::SetSRVQueryL
// ----------------------------------------------------------------------------
//
void CSIPServerResolver::SetSRVQueryL( CServerQuery* aQuery )
	{
	CSIPSRVOrigDomain* querycondition = CSIPSRVOrigDomain::NewL( *aQuery );													
	CleanupStack::PushL( querycondition );
	aQuery->SetQueryConditionL( querycondition );
	CleanupStack::Pop( querycondition );							 
	iResolver->GetByQueryL( aQuery );
	}
	
// ----------------------------------------------------------------------------
// CSIPServerResolver::SetNaptrQueryL
// ----------------------------------------------------------------------------
//
void CSIPServerResolver::SetNaptrQueryL( CServerQuery* aQuery )
	{
	CSIPNaptrOrigDomain* querycondition = CSIPNaptrOrigDomain::NewL( *aQuery );												
	CleanupStack::PushL( querycondition );
	aQuery->SetQueryConditionL( querycondition );
	CleanupStack::Pop( querycondition );		 								 
	iResolver->GetByQueryL( aQuery );
	}

// ----------------------------------------------------------------------------
// CSIPServerResolver::TransportProtocolL
// ----------------------------------------------------------------------------
//
RStringF CSIPServerResolver::TransportProtocolL( 
    const CSIPURI& aURI,
    TBool& aForceUdp ) const
    {
    aForceUdp = EFalse;
    
    RStringF protocol =
        aURI.ParamValue( SIPStrings::StringF( SipStrConsts::ETransport ) );    
    
    if ( protocol == SIPStrings::StringF( SipStrConsts::EEmpty ) )
        {
        // Add explicit transport protocol 
        // if there is a port in the URI or the host is an IP address
        const CSIPHostPort& hostPort( aURI.HostPort() );
        if ( hostPort.HasPort() || 
             !( UriUtils::HostType( hostPort.Host() ) == UriUtils::ETextHost ) )
            {
            protocol = DefaultTransport( aURI.IsSIPSURI() );
            }
        }
    else
        {
        if ( aURI.IsSIPSURI() && 
             protocol == SIPStrings::StringF( SipStrConsts::EUDP ) )
            {
            User::Leave( KErrNotSupported );
            }
        
        __ASSERT_ALWAYS( 
            ( protocol == SIPStrings::StringF( SipStrConsts::ETCP ) ||
		      protocol == SIPStrings::StringF( SipStrConsts::EUDP ) ||		
		      protocol == SIPStrings::StringF( SipStrConsts::ETLS ) ),
		    User::Leave( KErrNotSupported ) );
		    
		aForceUdp = ( protocol == SIPStrings::StringF( SipStrConsts::EUDP ) ); 
        }
    return protocol;
    }

// ----------------------------------------------------------------------------
// CSIPServerResolver::DefaultTransport
// ----------------------------------------------------------------------------
//
RStringF CSIPServerResolver::DefaultTransport( TBool aSipsUri ) const
    {
    RStringF protocol( SIPStrings::StringF( SipStrConsts::EUDP ) );
    if ( aSipsUri )
        {
        protocol = SIPStrings::StringF( SipStrConsts::ETLS );
        }
    return protocol; 
    }

// ----------------------------------------------------------------------------
// CSIPServerResolver::DefaultPort
// ----------------------------------------------------------------------------
//        
TUint CSIPServerResolver::DefaultPort( TBool aUseTls ) const
    {
    TUint port( KSIPDefaultPort );
    if ( aUseTls )
        {
        port = KSIPsDefaultPort;
        }
    return port;
    }
