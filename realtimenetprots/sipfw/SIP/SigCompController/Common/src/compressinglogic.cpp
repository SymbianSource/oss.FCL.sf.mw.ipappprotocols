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
// Name          : compressinglogic.cpp
// Part of       : SIP SigComp Controller
// Version       : SIP/3.0
//



#include <e32base.h>
#include <in_sock.h>
#include "compressinglogic.h"
#include "sipmessage.h"
#include "sipresponse.h"
#include "siprequest.h"
#include "siphostport.h"
#include "sipviaheader.h"
#include "sipuri.h"
#include "siprouteheader.h"
#include "sipaddress.h"
#include "msiGcompowner.h"
#include "sipcontactheader.h"
#include "sipheaderbase.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "uricontainer.h"
#include "TSIPTransportParams.h"

// -----------------------------------------------------------------------------
// CSigCompCompressingLogic::NewL
// -----------------------------------------------------------------------------
//
CSigCompCompressingLogic* CSigCompCompressingLogic::NewL(
	MSigCompOwner& aSigCompOwner )
	{
	CSigCompCompressingLogic* self = NewLC( aSigCompOwner );
	CleanupStack::Pop();
	return self;
	}

// -----------------------------------------------------------------------------
// CSigCompCompressingLogic::NewLC
// -----------------------------------------------------------------------------
//
CSigCompCompressingLogic* CSigCompCompressingLogic::NewLC(
	MSigCompOwner& aSigCompOwner )
	{
	CSigCompCompressingLogic* self =
		new (ELeave) CSigCompCompressingLogic( aSigCompOwner );
	CleanupStack::PushL( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CSigCompCompressingLogic::EncodeL
// -----------------------------------------------------------------------------
//
CBufBase* CSigCompCompressingLogic::EncodeL(
    const TSIPTransportParams& aTransportParams,
    CSIPRequest& aRequest,
	const TInetAddr& aAddress,
	TBool aStreambasedProtocol,
	TBool& aCompressed )
	{
	if ( ShouldBeCompressL( aTransportParams, aRequest ) )
		{
		AddSigCompToContactsL( aRequest );
		aCompressed = ETrue;
		SetCompToTopViaL( aRequest );
		
		return iSigCompOwner.EncodeSipAndCompressL( aTransportParams,
		                                            aRequest,
													aAddress,
													aStreambasedProtocol );
		}
	else
		{
		aCompressed = EFalse;
		return iSigCompOwner.EncodeSipL( aRequest, aStreambasedProtocol );
		}
	}

// -----------------------------------------------------------------------------
// CSigCompCompressingLogic::EncodeL
// -----------------------------------------------------------------------------
//
CBufBase* CSigCompCompressingLogic::EncodeL(
    const TSIPTransportParams& aTransportParams,
    CSIPResponse& aResponse,
	const TInetAddr& aAddress,
	TBool aStreambasedProtocol,
	TBool& aCompressed )
	{
	if ( ShouldBeCompressL( aResponse ) )
		{
		AddSigCompToContactsL( aResponse );
		aCompressed = ETrue;
		
		return iSigCompOwner.EncodeSipAndCompressL( aTransportParams,
		                                            aResponse,
													aAddress,
													aStreambasedProtocol );
		}
	else
		{
		aCompressed = EFalse;
		return iSigCompOwner.EncodeSipL( aResponse, aStreambasedProtocol );
		}
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CSigCompCompressingLogic::~CSigCompCompressingLogic()
	{

	}

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CSigCompCompressingLogic::CSigCompCompressingLogic( MSigCompOwner& aSigCompOwner )
	: iSigCompOwner( aSigCompOwner )
	{
	}

// -----------------------------------------------------------------------------
// CSigCompCompressingLogic::CompInVia
// -----------------------------------------------------------------------------
//
TBool CSigCompCompressingLogic::CompInVia( CSIPMessage& aMessage )
	{
	CSIPViaHeader* viaheader;
	viaheader = TopViaHeader( aMessage );
	if ( viaheader )
		{
		if ( viaheader->HasParam( SIPStrings::StringF( SipStrConsts::EComp ) ) )
			{
			if ( viaheader->ParamValue(
			        SIPStrings::StringF( SipStrConsts::EComp ) ) ==
			        SIPStrings::StringF( SipStrConsts::ESigComp ) )
				{
				return ETrue;
				}
			}
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSigCompCompressingLogic::SetCompToTopVia
// -----------------------------------------------------------------------------
//
void CSigCompCompressingLogic::SetCompToTopViaL( CSIPMessage& aMessage )
	{
	if ( !CompInVia( aMessage ) )
		{
		CSIPViaHeader* viaheader = TopViaHeader( aMessage );
		if ( viaheader )
			{
			viaheader->SetParamL( SIPStrings::StringF( SipStrConsts::EComp ),
			                      SIPStrings::StringF( SipStrConsts::ESigComp ) );
			}
		}
	}

// -----------------------------------------------------------------------------
// CSigCompCompressingLogic::DeleteCompFromTopVia
// -----------------------------------------------------------------------------
//
void CSigCompCompressingLogic::DeleteCompFromTopVia( CSIPMessage& aMessage )
	{
	if ( CompInVia( aMessage ) )
		{
		CSIPViaHeader* viaheader = TopViaHeader( aMessage );
		viaheader->DeleteParam( SIPStrings::StringF( SipStrConsts::EComp ) );
		}
	}

// -----------------------------------------------------------------------------
// CSigCompCompressingLogic::TopViaHeader
// -----------------------------------------------------------------------------
//
CSIPViaHeader* CSigCompCompressingLogic::TopViaHeader( CSIPMessage& aMessage )
	{
	if ( aMessage.HasHeader( 
	        SIPStrings::StringF( SipStrConsts::EViaHeader ) ) )
		{
		TSglQueIter<CSIPHeaderBase> iter = 
		    aMessage.Headers( SIPStrings::StringF( SipStrConsts::EViaHeader ) );
		CSIPHeaderBase* header = iter;
		return static_cast<CSIPViaHeader*>( header );
		}

	return 0;
	}

// -----------------------------------------------------------------------------
// CSigCompCompressingLogic::HasSigCompParam
// -----------------------------------------------------------------------------
//
TBool CSigCompCompressingLogic::HasSigCompParam( CSIPURI& aURI ) const
	{
	if ( aURI.HasParam( SIPStrings::StringF( SipStrConsts::EComp ) ) )
		{
		return aURI.ParamValue( SIPStrings::StringF( SipStrConsts::EComp ) ) ==
			                    SIPStrings::StringF( SipStrConsts::ESigComp );
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSigCompCompressingLogic::SetCompEqualSigCompL
// -----------------------------------------------------------------------------
//
void CSigCompCompressingLogic::SetCompEqualSigCompL( CSIPURI& aURI )
	{
	if ( !HasSigCompParam( aURI ) )
		{
		aURI.SetParamL( SIPStrings::StringF( SipStrConsts::EComp ),
		                SIPStrings::StringF( SipStrConsts::ESigComp ) );
		}
	}

// -----------------------------------------------------------------------------
// CSigCompCompressingLogic::RemoveCompEqualSigComp
// -----------------------------------------------------------------------------
//
void CSigCompCompressingLogic::RemoveCompEqualSigComp( CSIPURI& aURI )
	{
	aURI.DeleteParam( SIPStrings::StringF( SipStrConsts::EComp ) );
	}

// -----------------------------------------------------------------------------
// CSigCompCompressingLogic::SipUri
// -----------------------------------------------------------------------------
//
CSIPURI* CSigCompCompressingLogic::SigCompUriFromRequest( CSIPRequest& aRequest )
	{
	if ( aRequest.HasHeader( 
	        SIPStrings::StringF( SipStrConsts::ERouteHeader ) ) )
		{
        TSglQueIter<CSIPHeaderBase> iter = 
            aRequest.Headers(SIPStrings::StringF( SipStrConsts::ERouteHeader ) );

		CSIPHeaderBase* header = iter;
		CSIPRouteHeader* routeheader = static_cast<CSIPRouteHeader*>( header );
		
		if ( routeheader->SIPAddress().URI().IsSIPURI() &&
		     routeheader->SIPAddress().URI().SIPURI()->HasParam(
		        SIPStrings::StringF( SipStrConsts::ELr ) ) )
			{
			CSIPURI* sipuri = routeheader->SIPAddress().URI().SIPURI();
			if ( HasSigCompParam( *sipuri ) )
				{
				return sipuri;
				}
			else
				{
				return 0;
				}
			}
		}

	if ( aRequest.RequestURI()->IsSIPURI() && 
	     HasSigCompParam( *aRequest.RequestURI()->SIPURI() ) )
		{
		return aRequest.RequestURI()->SIPURI();
		}
	return 0;
	}

// -----------------------------------------------------------------------------
// CSigCompCompressingLogic::ShouldBeCompressL
// -----------------------------------------------------------------------------
//
TBool CSigCompCompressingLogic::ShouldBeCompressL(
    const TSIPTransportParams& aTransportParams,
    CSIPRequest& aRequest )
	{
	if ( iSigCompOwner.HasCompartmentUser( aTransportParams ) )
	    {
		return ETrue;
    	}
    	
	DeleteCompFromTopVia( aRequest );
	CSIPURI* sipuri = SigCompUriFromRequest( aRequest );
	if ( sipuri )
		{
		RemoveCompEqualSigComp( *sipuri );
		}
	return EFalse;
	}


// -----------------------------------------------------------------------------
// CSigCompCompressingLogic::ShouldBeCompressL
// -----------------------------------------------------------------------------
//
TBool CSigCompCompressingLogic::ShouldBeCompressL( CSIPResponse& aResponse )
	{
	return CompInVia( aResponse );
	}

// -----------------------------------------------------------------------------
// CSigCompCompressingLogic::AddSigCompToContactsL
// -----------------------------------------------------------------------------
//
void CSigCompCompressingLogic::AddSigCompToContactsL( CSIPMessage& aMessage )
	{
	CSIPHeaderBase* headerbase;
	CSIPContactHeader* header;
	if ( aMessage.HasHeader( 
	        SIPStrings::StringF( SipStrConsts::EContactHeader ) ) )
		{
		TSglQueIter<CSIPHeaderBase> headeriter =
			aMessage.Headers( SIPStrings::StringF( SipStrConsts::EContactHeader ) );
		headeriter.SetToFirst();
		while ( ( headerbase = headeriter++ ) != NULL )
			{
			header = static_cast<CSIPContactHeader*>( headerbase );
			if ( !header->Star() && 
			     header->SIPAddress()->URI().IsSIPURI() &&
				 iSigCompOwner.Match(
				 header->SIPAddress()->URI().SIPURI()->HostPort().Host() ) )
				{
				SetCompEqualSigCompL( *header->SIPAddress()->URI().SIPURI() );
				}
			}
		}
	}
