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
// Name          : CSipCRServer.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



// INCLUDES
#include "sipCRclientserver.h"
#include "CSipCRServer.h"
#include "CSipCRServerSession.h"
#include "CSIPCRITCUtility.h"
#include "CSIPCRRoutingTable.h"
#include "CSIPCRServerCloseTimer.h"
#include "CSIPClientResolver.h"
#include "sipresolvedclient.h"
#include "sipresolvedclient2.h"
#include "sipstrings.h"
#include "SIPHeaderLookup.h"
#include "TSipCRPlatSecPolicy.h"

// CONSTANTS
const TUint KServerCloseWaitTime = 500; // milliseconds

// -----------------------------------------------------------------------------
// CSIPCRServer::NewL
// -----------------------------------------------------------------------------
//
CSIPCRServer* CSIPCRServer::NewL ()
	{
    CSIPCRServer* self = CSIPCRServer::NewLC();
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPCRServer::NewLC
// -----------------------------------------------------------------------------
//
CSIPCRServer* CSIPCRServer::NewLC ()
	{
	CSIPCRServer* self = new(ELeave)CSIPCRServer(EPriorityHigh);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPCRServer::CSipCRServer
// -----------------------------------------------------------------------------
//
CSIPCRServer::CSIPCRServer (TInt aPriority)
 : CPolicyServer(aPriority, TSIPCRPlatSecPolicy)
	{
	}

// -----------------------------------------------------------------------------
// CSIPCRServer::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPCRServer::ConstructL ()
	{
	SIPStrings::OpenL();
	SIPHeaderLookup::OpenL();
	SIPHeaderLookup::SetConvertToSIPURI();
	
    iActiveScheduler = new (ELeave) CActiveScheduler ();
    CActiveScheduler::Install (iActiveScheduler);

    iItcUtility = CSIPCRITCUtility::NewL();
    iCloseTimer = CSIPCRServerCloseTimer::NewL();
    iRoutingTable = CSIPCRRoutingTable::NewL();
    iClientResolver = CSIPClientResolver::NewL();
    
    StartL(KSipClientResolverServerName);
	}

// -----------------------------------------------------------------------------
// CSIPCRServer::~CSIPCRServer
// -----------------------------------------------------------------------------
//
CSIPCRServer::~CSIPCRServer ()
	{
	delete iClientResolver;
	delete iRoutingTable;
	delete iCloseTimer;
	delete iItcUtility;
	delete iActiveScheduler;
	CActiveScheduler::Install (NULL);
	SIPHeaderLookup::Close();
	SIPStrings::Close();
	REComSession::FinalClose();
	
#ifdef _BullseyeCoverage
    // Store coverage measurement
    cov_write();
#endif
	}

// -----------------------------------------------------------------------------
// CSIPCRServer::IncrementSessions
// -----------------------------------------------------------------------------
//
void CSIPCRServer::IncrementSessions ()
	{
	if (iCloseTimer->IsActive())
        {
        iCloseTimer->Cancel();
        }
    iSessionCount++;
	}

// -----------------------------------------------------------------------------
// CSIPCRServer::DecrementSessions
// -----------------------------------------------------------------------------
//
void CSIPCRServer::DecrementSessions ()
	{
    iSessionCount--;
	if (iSessionCount == 0)
		{
		iCloseTimer->StopActiveSchedulerAfter(KServerCloseWaitTime);
		}
	}

// -----------------------------------------------------------------------------
// CSIPCRServer::ITCUtility
// -----------------------------------------------------------------------------
//
CSIPCRITCUtility& CSIPCRServer::ITCUtility ()
    {
    return *iItcUtility;
    }
    
// -----------------------------------------------------------------------------
// CSIPCRServer::RoutingTable
// -----------------------------------------------------------------------------
//
CSIPCRRoutingTable& CSIPCRServer::RoutingTable ()
    {
    return *iRoutingTable;
    }
    
// -----------------------------------------------------------------------------
// CSIPCRServer::ClientResolver
// -----------------------------------------------------------------------------
//
CSIPClientResolver& CSIPCRServer::ClientResolver ()
    {
    return *iClientResolver;
    }
    
// ----------------------------------------------------------------------------
// CSIPCRServer::LoadPlugin1LC
// ----------------------------------------------------------------------------
//    
CSIPResolvedClient* CSIPCRServer::LoadPlugin1LC( 
    const TUid& aClientUid,
    const TUid& aResolverUid )
    {
    const TInt KUidWidth = 8;
    TBuf8<KUidWidth> clientUidBuf;
    clientUidBuf.AppendNumFixedWidthUC( aClientUid.iUid, EHex, KUidWidth );
    TEComResolverParams resolverParams;
    resolverParams.SetDataType( clientUidBuf );
    CSIPResolvedClient* resolvedClient = NULL;
    
    if (aResolverUid == TUid::Null())
        {
        resolvedClient = reinterpret_cast< CSIPResolvedClient* >(
            REComSession::CreateImplementationL( KSIPResolvedClientIFUid,
                _FOFF( CSIPResolvedClient, iInstanceKey ),
                resolverParams ) );
        }
    else
        {
        resolvedClient = reinterpret_cast< CSIPResolvedClient* >(
            REComSession::CreateImplementationL( KSIPResolvedClientIFUid,
                _FOFF( CSIPResolvedClient, iInstanceKey ),
                resolverParams, aResolverUid ) );        
        }
        
    CleanupStack::PushL(resolvedClient);
    return resolvedClient;    
    }
    
// ----------------------------------------------------------------------------
// CSIPCRServer::LoadPlugin2LC
// ----------------------------------------------------------------------------
//    
CSIPResolvedClient2* CSIPCRServer::LoadPlugin2LC(
    const TUid& aImplementationUid )
    {
    CSIPResolvedClient2* resolvedClient = 
        reinterpret_cast< CSIPResolvedClient2* >(
            REComSession::CreateImplementationL( 
                aImplementationUid,
                _FOFF( CSIPResolvedClient2, iInstanceKey ) ) );
    CleanupStack::PushL(resolvedClient);
    return resolvedClient;    
    }
   
// ----------------------------------------------------------------------------
// CSIPCRServer::RoutingEntryAddedL
// ----------------------------------------------------------------------------
//    
void CSIPCRServer::RoutingEntryAddedL(const TUid& aUid)
    {
	// Inform all the sessions
	iSessionIter.SetToFirst();
    CSIPCRServerSession* session = NULL;
	for (CSession2* s=iSessionIter; s!=0; s=iSessionIter++)
		{
		session = static_cast<CSIPCRServerSession*>(s);
		session->RoutingEntryAddedL(aUid);
		}
    }    
    
// -----------------------------------------------------------------------------
// CSIPCRServer::RunError
// -----------------------------------------------------------------------------
//
TInt CSIPCRServer::RunError(TInt aError)
	{
	if (aError == KErrBadDescriptor)
		{
        // A bad descriptor error implies a badly programmed client, 
        // so panic it; otherwise report the error to the client
		Message().Panic(KSipCRServerPanic, EBadDescriptor);
		}
	else
		{
		Message().Complete(aError);
		}
	//
	// The leave will result in an early return from CServer::RunL(), skipping
	// the call to request another message. So do that now in order to keep the
	// server running.
	ReStart();
	return KErrNone; // handled the error fully
	}
	
// -----------------------------------------------------------------------------
// CSIPCRServer::NewSessionL
// -----------------------------------------------------------------------------
//
CSession2* CSIPCRServer::NewSessionL(const TVersion &aVersion,
                                     const RMessage2& /*aMessage*/) const
	{
	// check we're the right version
	if (!User::QueryVersionSupported(TVersion(KSipCRServerMajorVersionNumber,
                                              KSipCRServerMinorVersionNumber,
                                              KSipCRServerBuildVersionNumber),
                                              aVersion))
		{
		User::Leave(KErrNotSupported);
		}
	// make new session
    return CSIPCRServerSession::NewL(const_cast<CSIPCRServer&>(*this));
  	}

//  End of File
