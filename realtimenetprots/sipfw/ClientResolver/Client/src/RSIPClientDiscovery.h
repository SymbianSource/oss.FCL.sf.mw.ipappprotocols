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
* Name          : RSIPClientDiscovery.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef RSIPCLIENTDISCOVERY_H
#define RSIPCLIENTDISCOVERY_H

//  INCLUDES
#include <e32std.h>
#include <stringpool.h>
#include "sipCRclientserver.h"
#include "MSIPClientDiscoveryITC.h"


// CLASS DECLARATION

/**
* Client class of client-server SIP Client Resolver, derived from
* RSessionBase
* 
*  @lib siprsvclient.lib
*/

class RSIPClientDiscovery : public RSessionBase, 
                            public MSIPClientDiscoveryITC
	{
public:

	RSIPClientDiscovery();
	TInt Connect(TUid aSelf);
	void Close();
    TInt SendReceive(TSIPCRIpcFunctions aFunction,
                     const TIpcArgs& aArgs) const;	
	
public: // From MSIPClientDiscoveryITC

    TInt Send (TSIPCRIpcFunctions aFunction, TIpcArgs& aArgs);
    void Receive (TIpcArgs& aArgs, TRequestStatus& aStatus);
    TInt Receive (TIpcArgs& aArgs);
    void CancelReceive ();

private:

    TInt SendClientUid (const TUid& aUid) const;
    TVersion Version(void) const;
    void Panic (TInt aReason) const;

    TBool iConnected;
    
#ifdef CPPUNIT_TEST
    // For testing purposes only:
    TBool iReceiveCalled;
    TRequestStatus* iStatus;
#endif
	};

#endif // RSIPCLIENTDISCOVERY_H

// End of File
