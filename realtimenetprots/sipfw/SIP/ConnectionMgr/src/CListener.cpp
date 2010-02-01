// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CListener.cpp
// Part of       : ConnectionMgr
// Version       : SIP/4.0 
//




#include "CListener.h"
#include "MListenerOwner.h"
#include "CommonConsts.h"
#include "siperr.h"


// -----------------------------------------------------------------------------
// CListener::NewL
// -----------------------------------------------------------------------------
//
CListener* CListener::NewL(MListenerOwner& aContext,
						   RSocketServ& aServer,
						   TUint aPort)
	{
	CListener* self = NewLC(aContext, aServer, aPort);
	CleanupStack::Pop();
	return self;
	}

// -----------------------------------------------------------------------------
// CListener::NewLC
// -----------------------------------------------------------------------------
//
CListener* CListener::NewLC(MListenerOwner& aContext,
							RSocketServ& aServer,
							TUint aPort)
	{
	CListener* self = new (ELeave) CListener();
	CleanupStack::PushL(self);
	self->ConstructL(aContext, aServer, aPort);
	return self;
	}

// -----------------------------------------------------------------------------
// CListener::ConstructL
// -----------------------------------------------------------------------------
//
void CListener::ConstructL(MListenerOwner& aContext,
						   RSocketServ& aServer,
						   TUint aPort)
	{
	iContext = &aContext;
	iServer = &aServer;
	iPort = aPort;
	CActiveScheduler::Add(this);
	User::LeaveIfError(iListen.Open(*iServer, KAfInet, KSockStream,
									KProtocolInetTcp));

    iListen.SetOpt(KSoReuseAddr, KSolInetIp, 1);
    
	TInetAddr address;
	iContext->GetLocalAddrL(address);
	address.SetPort(iPort);
	address.SetFamily(0);
	TInt err = iListen.Bind(address);
	if(err == KErrInUse)
		{
		User::Leave(KErrSIPTransportFailure);
		}
	else
		{
		User::LeaveIfError(err);
		}
	iListen.Listen(5);
	}

// -----------------------------------------------------------------------------
// CListener::NewL
// -----------------------------------------------------------------------------
//
CListener* CListener::NewL(MListenerOwner& aContext,
						   RSocketServ& aServer,
						   TUint aPort,
						   RConnection& aConnection)
	{
	CListener* self = NewLC(aContext, aServer, aPort, aConnection);
	CleanupStack::Pop();
	return self;
	}

// -----------------------------------------------------------------------------
// CListener::NewLC
// -----------------------------------------------------------------------------
//
CListener* CListener::NewLC(MListenerOwner& aContext,
							RSocketServ& aServer,
							TUint aPort,
							RConnection& aConnection)
	{
	CListener* self = new (ELeave) CListener();
	CleanupStack::PushL(self);
	self->ConstructL(aContext, aServer, aPort, aConnection);
	return self;
	}

// -----------------------------------------------------------------------------
// CListener::ConstructL
// -----------------------------------------------------------------------------
//
void CListener::ConstructL(MListenerOwner& aContext,
						   RSocketServ& aServer,
						   TUint aPort,
						   RConnection& aConnection)
	{
	iContext = &aContext;
	iServer = &aServer;
	iPort = aPort;

	iConnection = &aConnection;

	CActiveScheduler::Add(this);
	User::LeaveIfError(iListen.Open(*iServer, KAfInet, KSockStream,
									KProtocolInetTcp, *iConnection));
	if(aPort == KDefaultSipPort)
		{
		iListen.SetOpt(KSoReuseAddr, KSolInetIp, 1);
		}
	TInetAddr address;
	iContext->GetLocalAddrL(address);
	address.SetPort(iPort);
	address.SetFamily(0);
	TInt err = iListen.Bind(address);
	if(err == KErrInUse)
		{
		User::Leave(KErrSIPTransportFailure);
		}
	else
		{
		User::LeaveIfError(err);
		}
	iListen.Listen(5);
	}

// -----------------------------------------------------------------------------
// CListener::RunL
// -----------------------------------------------------------------------------
//
void CListener::RunL()
	{
	if(iStatus == KErrNone)
		{
		iContext->AcceptedL();
		}
	else
		{
		iContext->AcceptErrorL(iStatus.Int());
		}
	}

// -----------------------------------------------------------------------------
// CListener::DoCancel
// -----------------------------------------------------------------------------
//
void CListener::DoCancel()
	{
	iListen.CancelAccept();
	}

// -----------------------------------------------------------------------------
// CListener::RunError
// -----------------------------------------------------------------------------
//
TInt CListener::RunError(TInt aError)
	{
	if(aError != KErrNoMemory)
		{
		return KErrNone;
		}
	return aError;
	}

// -----------------------------------------------------------------------------
// CListener::Accept
// -----------------------------------------------------------------------------
//
void CListener::Accept(RSocket& aSocket)
	{
	if(!IsActive())
		{
		iListen.Accept(aSocket, iStatus);
		SetActive();
		}
	}

// -----------------------------------------------------------------------------
// CListener::~CListener
// -----------------------------------------------------------------------------
//
CListener::~CListener()
	{
	Cancel();
	iListen.Close();
	}

// -----------------------------------------------------------------------------
// CListener::CListener
// -----------------------------------------------------------------------------
//
CListener::CListener() : CActive(CActive::EPriorityStandard)
	{
	}
