/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Name          : tsipseccontext.h
* Part of       : SIPSec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/
#ifndef __SIPSEC_CONTEXT_H__
#define __SIPSEC_CONTEXT_H__

// INCLUDES
#include <e32base.h>
#include <in_sock.h>

// FORWARD DECLARATIONS
class CUri8;
class MSIPSecObserver;
class MSIPSecUser;
class CSIPResponse;

// CLASS DECLARATION

/**
*
*  @lib SipServer.exe
*/
class TSIPSecContext
	{
public: // Constructor and destructor
	TSIPSecContext(TInetAddr& aNextHop,
	  			   const CUri8& aRemoteTarget,
	  			   const TDesC8& aOutboundProxy,
	  			   MSIPSecUser& aUser);

	virtual ~TSIPSecContext() {}

public: // New functions

	TInetAddr& NextHop();

	const CUri8& RemoteTarget() const;

	const TDesC8& OutboundProxy() const;

	MSIPSecUser& SIPSecUser();

private: // Data

	TInetAddr& iNextHop;
	const CUri8& iRemoteTarget;
	const TDesC8& iOutboundProxy;
  	MSIPSecUser& iUser;				    
	};



// CLASS DECLARATION

/**
*
*  @lib SipServer.exe
*/
class TSIPSecResponseContext : public TSIPSecContext
	{	
public: // Constructor and destructor

	TSIPSecResponseContext(TInetAddr& aNextHop,
	  					   const CUri8& aRemoteTarget,
	  					   const TDesC8& aOutboundProxy,
	  					   MSIPSecUser& aUser,
	  					   MSIPSecObserver& aObserver,
	  					   CSIPResponse& aSIPResponse);

	~TSIPSecResponseContext() {}

public: // New functions

	CSIPResponse& SIPResponse();
	
	MSIPSecObserver& Observer();	
	
private: // Data

	MSIPSecObserver& iObserver;
	CSIPResponse& iSIPResponse;
	};

#endif
