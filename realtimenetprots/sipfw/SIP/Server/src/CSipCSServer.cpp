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
// Name          : CSipCSServer.cpp
// Part of       : SIPServerCore
// Version       : SIP/3.0 
//




#include "sipclientserver.h"
#include "CSipCSServer.h"
#include "CSipServerCore.h"
#include "CSipCSSession.h"
#include "TSipCSPlatSecPolicy.h"

// -----------------------------------------------------------------------------
// CSipCSServer::NewL
// -----------------------------------------------------------------------------
//
CSipCSServer* CSipCSServer::NewL(CSipServerCore& aServerCore)
	{
    CSipCSServer* self = CSipCSServer::NewLC(aServerCore);
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSipCSServer::NewLC
// -----------------------------------------------------------------------------
//
CSipCSServer* CSipCSServer::NewLC(CSipServerCore& aServerCore)
	{
	CSipCSServer* self = new(ELeave)CSipCSServer(EPriorityHigh,aServerCore);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
	}

// -----------------------------------------------------------------------------
// CSipCSServer::ConstructL
// -----------------------------------------------------------------------------
//
void CSipCSServer::ConstructL()
	{
    StartL(KSipServerName);
	}

// -----------------------------------------------------------------------------
// CSipCSServer::CSipCSServer
// -----------------------------------------------------------------------------
//
CSipCSServer::CSipCSServer(TInt aPriority, CSipServerCore& aServerCore)
 : CPolicyServer(aPriority, TSipCSPlatSecPolicy),
   iServerCore(aServerCore)
	{
	iSecurityInfo.SetToCurrentInfo();
	iSecurityInfo.iCaps.RemoveCapability(ECapabilityProtServ);
	}

// -----------------------------------------------------------------------------
// CSipCSServer::~CSipCSServer
// -----------------------------------------------------------------------------
//
CSipCSServer::~CSipCSServer()
	{
	}

// -----------------------------------------------------------------------------
// CSipCSServer::NewSessionL
// -----------------------------------------------------------------------------
//
CSession2* CSipCSServer::NewSessionL(const TVersion &aVersion,
                                     const RMessage2& /*aMessage*/) const
	{
	// check we're the right version
	if (!User::QueryVersionSupported (TVersion (KSipServerMajorVersionNumber,
                                                KSipServerMinorVersionNumber,
                                                KSipServerBuildVersionNumber),
                                                aVersion))
		{
		User::Leave(KErrNotSupported);
		}
    return CSipCSSession::NewL(iServerCore);
	}

// -----------------------------------------------------------------------------
// CSipCSServer::ConnectionStateChangedL
// -----------------------------------------------------------------------------
//
void CSipCSServer::ConnectionStateChangedL(TUint32 aIapId,
                                           CSIPConnection::TState aState)
	{
	// inform all the sessions
	iSessionIter.SetToFirst();
    CSipCSSession* session = 0;
	for (CSession2* s=iSessionIter; s!=0; s=iSessionIter++)
		{
		session = static_cast<CSipCSSession*>(s);
		session->ConnectionStateChangedL (aIapId,aState);
		}
	}
	
// -----------------------------------------------------------------------------
// CSipCSServer::FindOwner
// -----------------------------------------------------------------------------
//    
MTransactionOwner* CSipCSServer::FindOwner(TUint32 aIapId, const TUid& aUid)
	{
	MTransactionOwner* transactionOwner = NULL;
	iSessionIter.SetToFirst();
    CSipCSSession* session = 0;
	for (CSession2* s=iSessionIter; s!=0 && !transactionOwner; s=iSessionIter++)
		{
		session = static_cast<CSipCSSession*>(s);
		if (session->Uid() == aUid)
			{
			transactionOwner = &(session->FindOwner(aIapId));
			}
		}
	return transactionOwner;
	}	

// -----------------------------------------------------------------------------
// CSipCSServer::RunError
// -----------------------------------------------------------------------------
//
TInt CSipCSServer::RunError(TInt aError)
	{
	if (aError == KErrBadDescriptor)
		{
        // A bad descriptor error implies a badly programmed client, 
        // so panic it; otherwise report the error to the client
		Message().Panic(KSipCSServerPanic, EBadDescriptor);
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
// CSipCSServer::CustomSecurityCheckL
// -----------------------------------------------------------------------------
//	
CPolicyServer::TCustomResult CSipCSServer::CustomSecurityCheckL(
    const RMessage2& aMsg,
    TInt& /*aAction*/,
    TSecurityInfo& /*aMissing*/)
    {
    // Profile server is allowed to set opts regardless of its capabilities 
    const TUint32 KProfileServerUidVal = 0x101F413C;
    TUid clientUid = aMsg.SecureId();
        
    if ( ( aMsg.Function() == ESipItcSetSIPSockOpt || 
           aMsg.Function() == ESipItcSetSIPSockOptWithDes ) &&
          clientUid.iUid != KProfileServerUidVal )
        {
        TSecurityInfo clientSecurityInfo(aMsg);
        TCapabilitySet sipServerCaps;
        sipServerCaps.SetEmpty();
        sipServerCaps.Union(iSecurityInfo.iCaps);
        sipServerCaps.RemoveCapability(ECapabilityReadUserData); // Maintain SC
        if (!clientSecurityInfo.iCaps.HasCapabilities(sipServerCaps))
            {
            return CPolicyServer::EFail;
            }
        }
    return CPolicyServer::EPass;
    }
