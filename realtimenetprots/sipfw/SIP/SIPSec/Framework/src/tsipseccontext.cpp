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
// Name          : tsipseccontext.cpp
// Part of       : SIPSec
// Version       : SIP/4.0 
//



#include "tsipseccontext.h"


// ----------------------------------------------------------------------------
// TSIPSecContext::TSIPSecContext
// ----------------------------------------------------------------------------
//
TSIPSecContext::TSIPSecContext(TInetAddr& aNextHop,
					  		   const CUri8& aRemoteTarget,
					  		   const TDesC8& aOutboundProxy,
					  		   MSIPSecUser& aUser) :
	iNextHop(aNextHop),
	iRemoteTarget(aRemoteTarget),
	iOutboundProxy(aOutboundProxy),
	iUser(aUser)
	{			
	}
 			
// ----------------------------------------------------------------------------
// TSIPSecContext::NextHop
// ----------------------------------------------------------------------------
//
TInetAddr& TSIPSecContext::NextHop()
	{
	return iNextHop;
	}

// ----------------------------------------------------------------------------
// TSIPSecContext::RemoteTarget
// ----------------------------------------------------------------------------
//
const CUri8& TSIPSecContext::RemoteTarget() const
	{
	return iRemoteTarget;
	}

// ----------------------------------------------------------------------------
// TSIPSecContext::OutboundProxy
// ----------------------------------------------------------------------------
//	
const TDesC8& TSIPSecContext::OutboundProxy() const
	{
	return iOutboundProxy;
	}

// ----------------------------------------------------------------------------
// TSIPSecContext::SIPSecUser
// ----------------------------------------------------------------------------
//	
MSIPSecUser& TSIPSecContext::SIPSecUser()
	{
	return iUser;
	}

// ----------------------------------------------------------------------------
// TSIPSecResponseContext::TSIPSecResponseContext
// ----------------------------------------------------------------------------
//		
TSIPSecResponseContext::TSIPSecResponseContext(TInetAddr& aNextHop,
							  				   const CUri8& aRemoteTarget,
							  				   const TDesC8& aOutboundProxy,
							  				   MSIPSecUser& aUser,
							  				   MSIPSecObserver& aObserver,
							  				   CSIPResponse& aSIPResponse) :
	TSIPSecContext(aNextHop, aRemoteTarget, aOutboundProxy, aUser),
	iObserver(aObserver),
	iSIPResponse(aSIPResponse)
	{
	}

// ----------------------------------------------------------------------------
// TSIPSecResponseContext::SIPResponse
// ----------------------------------------------------------------------------
//	
CSIPResponse& TSIPSecResponseContext::SIPResponse()
	{
	return iSIPResponse;	
	}

// ----------------------------------------------------------------------------
// TSIPSecResponseContext::Observer
// ----------------------------------------------------------------------------
//	
MSIPSecObserver& TSIPSecResponseContext::Observer()
	{
	return iObserver;
	}
