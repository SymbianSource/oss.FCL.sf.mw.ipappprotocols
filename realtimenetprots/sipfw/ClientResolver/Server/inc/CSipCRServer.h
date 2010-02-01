/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSipCRServer.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPCRSERVER_H
#define CSIPCRSERVER_H

#include <e32base.h>
#include "SipCRServer.pan"

class CSIPCRITCUtility;
class CSIPCRRoutingTable;
class CSIPClientResolver;
class CSIPCRServerCloseTimer;
class CSIPResolvedClient;
class CSIPResolvedClient2;

class CSIPCRServer : public CPolicyServer
	{
public : 
    
    static CSIPCRServer* NewL ();
    static CSIPCRServer* NewLC ();
    ~CSIPCRServer ();
    
   	void IncrementSessions ();
	void DecrementSessions ();
	CSIPCRITCUtility& ITCUtility ();
	CSIPCRRoutingTable& RoutingTable ();
	CSIPClientResolver& ClientResolver ();
	CSIPResolvedClient* LoadPlugin1LC(const TUid& aClientUid,
	                                  const TUid& aResolverUid);
    
    CSIPResolvedClient2* LoadPlugin2LC(const TUid& aImplementationUid);                                
                                                                          
	void RoutingEntryAddedL(const TUid& aUid);
	
protected: // From CActive
    
	TInt RunError (TInt aError);

private: // From CServer2
    
    CSession2* NewSessionL(const TVersion &aVersion,
                           const RMessage2& aMessage) const;

private: // Second phase constructors

    CSIPCRServer (TInt aPriority);
    void ConstructL ();
    
private: // Data

	CSIPCRITCUtility*       iItcUtility;
	CSIPCRRoutingTable*     iRoutingTable;
	CSIPClientResolver*     iClientResolver;
	CSIPCRServerCloseTimer* iCloseTimer;
    CActiveScheduler*       iActiveScheduler;
    TUint                   iSessionCount;
	};

#endif // CSIPCRSERVER_H

// End of File
