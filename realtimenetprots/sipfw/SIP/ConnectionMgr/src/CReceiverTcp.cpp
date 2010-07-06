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
// Name          : CReceiverTcp.cpp
// Part of       : ConnectionMgr
// Version       : SIP/4.0 
//




#include "CReceiverTcp.h"
#include "SipLogs.h"
#include "CSipConnection.h"
#include "MContext.h"
#include "CErrorHandler.h"
#include "CSIPMsgAssembler.h"
#include "CSocketContainer.h"

// -----------------------------------------------------------------------------
// CReceiverTcp::NewL
// -----------------------------------------------------------------------------
//
CReceiverTcp* CReceiverTcp::NewL(MContext& aContext)
	{
	CReceiverTcp* self = NewLC(aContext);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CReceiverTcp::NewLC
// -----------------------------------------------------------------------------
//
CReceiverTcp* CReceiverTcp::NewLC(MContext& aContext)
	{
	CReceiverTcp* self = new (ELeave) CReceiverTcp(aContext);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
	}

// -----------------------------------------------------------------------------
// CReceiverTcp::ConstructL
// -----------------------------------------------------------------------------
//
void CReceiverTcp::ConstructL()
	{
	iNextLength = CSIPMsgAssembler::EMsgBufferSize;
	CActiveScheduler::Add(this);
	iMsgAssembler = CSIPMsgAssembler::NewL(*this, *iContext.SigCompHandler());
	}

// -----------------------------------------------------------------------------
// CReceiverTcp::RunL
// -----------------------------------------------------------------------------
//
void CReceiverTcp::RunL()
	{
	TInt status( iStatus.Int() );
	
	TBool disconnected( EFalse );
	
	switch ( iReceiverState )
	    {
	    case EConnecting:
	        {
	        if ( status == KErrNone )
    			{
    			iReceiverState = EConnected;
    			
    			if ( iContext.ConnectionOpenL() )
    			    {
    			    // Context allowed receiving after connection was established
    			    ReceiveDataL();
    			    }
    			}
    		else
    			{
    			disconnected = ETrue;
    			}
	        break;
	        }
	    case EShuttingDown:
	        {
	        disconnected = ETrue;
	        break;
	        }
	    default: // EIdle, EConnected
	        {
	        if ( status == KErrEof )
    			{
    			disconnected = ETrue;
    			}
    		if ( status == KErrNone )
    			{
    			__SIP_MESSAGE_LOG( "Connection Manager::ReceivedFromNetwork",
    							   *iReceivedMsg )
    			__SIP_DES8_LOG( "Received data via TCP", *iReceivedMsg )
    			
    			iMsgAssembler->InputL( iReceivedMsg, iNextLength );
    			iReceivedMsg = 0; // Ownership was transferred to assembler	
    			
    			ReceiveDataL(); // May lead to self deletion
    			}
	        break;
	        }
	    }
	    
	if ( disconnected )
	    {
	    __SIP_INT_LOG1( "TCP connection disconnected, status:", status )
	    
	    iContext.DisconnectedL(); // Leads to self deletion
	    }
	}

// -----------------------------------------------------------------------------
// CReceiverTcp::MessageCompleteL
// -----------------------------------------------------------------------------
//
void CReceiverTcp::MessageCompleteL(CSIPMessage* aSIPMessage,
									TInt aError,
									TBool aCompressed)
	{
	TInetAddr addr;
	iContext.SocketContainer().RemoteName(addr);
	iContext.ReceivedDataL(aSIPMessage, addr, aError, aCompressed);
	}

// -----------------------------------------------------------------------------
// CReceiverTcp::DoCancel
// -----------------------------------------------------------------------------
//
void CReceiverTcp::DoCancel()
	{
	if ( iReceiverState == EConnected )
		{
		iContext.SocketContainer().CancelRecv();
		}
	else
		{        
		iContext.SocketContainer().CancelAll();        
		}
	RemoveReceived();
	}

// -----------------------------------------------------------------------------
// CReceiverTcp::RunError
// -----------------------------------------------------------------------------
//
TInt CReceiverTcp::RunError(TInt /*aError*/)
	{
	return iContext.Remove();
	}

// -----------------------------------------------------------------------------
// CReceiverTcp::ReceiveDataL
// -----------------------------------------------------------------------------
//
void CReceiverTcp::ReceiveDataL()
	{
	if (!IsActive())
		{
		RemoveReceived();

		// Trying to allocate KMaxTInt/2 or more bytes causes panic
		__ASSERT_ALWAYS( iNextLength < KMaxTInt/2, User::Leave(KErrOverflow) );

		iReceivedMsg = HBufC8::NewL(iNextLength);
		iReceivedMsgPtr.Set(iReceivedMsg->Des());
		iContext.SocketContainer().RecvOneOrMore(iReceivedMsgPtr,
                                                 0,
                                                 iStatus,
                                                 iSockLenght);
		SetActive();
		}
	}

// -----------------------------------------------------------------------------
// CReceiverTcp::Connect
// -----------------------------------------------------------------------------
//
void CReceiverTcp::Connect(TInetAddr& aRemoteAddr)
	{
	if(!IsActive())
		{
		iContext.SocketContainer().Connect(aRemoteAddr, iStatus);
		iReceiverState = EConnecting;
		SetActive();
		}
	}
	
// -----------------------------------------------------------------------------
// CReceiverTcp::Shutdown
// -----------------------------------------------------------------------------
//
void CReceiverTcp::Shutdown()
	{
	Cancel();
	iContext.SocketContainer().Socket().Shutdown( RSocket::ENormal, iStatus );
	iReceiverState = EShuttingDown;
	SetActive();
	}

// -----------------------------------------------------------------------------
// CReceiverTcp::~CReceiverTcp
// -----------------------------------------------------------------------------
//
CReceiverTcp::~CReceiverTcp()
	{
	Cancel();
	RemoveReceived();
	delete iMsgAssembler;
	}

// -----------------------------------------------------------------------------
// CReceiverTcp::CReceiverTcp
// -----------------------------------------------------------------------------
//
CReceiverTcp::CReceiverTcp(MContext& aContext) :
	CActive(CActive::EPriorityStandard),
	iContext(aContext),
	iReceivedMsgPtr(0,0,0),
	iReceiverState( EIdle )
	{
	}

// -----------------------------------------------------------------------------
// CReceiverTcp::RemoveReceived
// -----------------------------------------------------------------------------
//	
void CReceiverTcp::RemoveReceived()
    {
    delete iReceivedMsg;
    iReceivedMsg = 0;
    }
    
// End of file
