// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CReceiver.cpp
// Part of       : ConnectionMgr
// Version       : SIP/5.0
//



#include "CReceiver.h"
#include "SipLogs.h"
#include "CSipConnection.h"
#include "CommonConsts.h"
#include "MContext.h"
#include "MSigCompController.h"
#include "CSocketContainer.h"

// -----------------------------------------------------------------------------
// CReceiver::NewL
// -----------------------------------------------------------------------------
//
CReceiver* CReceiver::NewL( MContext& aContext, TUint aIPAddrFamily )
	{
	CReceiver* self = NewLC( aContext, aIPAddrFamily );
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CReceiver::NewLC
// -----------------------------------------------------------------------------
//
CReceiver* CReceiver::NewLC( MContext& aContext, TUint aIPAddrFamily )
	{
	CReceiver* self = new ( ELeave ) CReceiver( aContext, aIPAddrFamily );
	CleanupStack::PushL( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CReceiver::CReceiver
// -----------------------------------------------------------------------------
//
CReceiver::CReceiver( MContext& aContext, TUint aIPAddrFamily ) : 
    CActive( EPriorityStandard ), 
    iContext( aContext ),
    iIPAddrFamily( aIPAddrFamily ),
    iState( EIdle ),
    iReceivedMsgPtr( 0, 0, 0 )
	{
	CActiveScheduler::Add( this );
	}

// -----------------------------------------------------------------------------
// CReceiver::~CReceiver
// -----------------------------------------------------------------------------
//
CReceiver::~CReceiver()
	{
	Cancel();
	delete iReceivedMsg;
	}

// -----------------------------------------------------------------------------
// CReceiver::DoCancel
// -----------------------------------------------------------------------------
//
void CReceiver::DoCancel()
	{
	CSocketContainer* socketContainer = 
	    iContext.SocketContainer( iIPAddrFamily );
	if ( iState == EListening )
	    {
	    socketContainer->Socket().CancelIoctl();
	    }
	else
        {
        socketContainer->Socket().CancelRecv();
        }
	}

// -----------------------------------------------------------------------------
// CReceiver::Listen
// -----------------------------------------------------------------------------
//
void CReceiver::Listen()
	{
	CSocketContainer* socketContainer = 
	    iContext.SocketContainer( iIPAddrFamily );	
	if ( socketContainer && !IsActive() )
		{	
		iState = EListening;
		
		iFlags() = KSockSelectRead | KSockSelectExcept;
	    socketContainer->Socket().Ioctl( 
	        KIOctlSelect, 
	        iStatus, 
	        &iFlags, 
            KSOLSocket );	
		SetActive();
		}
	}

// -----------------------------------------------------------------------------
// CReceiver::RunL
// -----------------------------------------------------------------------------
//
void CReceiver::RunL()
	{
	switch ( iStatus.Int() )
		{
		case KErrNone:
			{
			if ( iState == EListening )
			    {
			    ReceivePendingL();
			    }
			else if ( iState == EReceiving )
			    {
    			__SIP_MESSAGE_LOG( "Connection Manager::ReceivedFromNetwork",
    							   *iReceivedMsg )
    			__SIP_DES8_LOG( "Received data via UDP", *iReceivedMsg )	
    			// Give away ownership of the received message
    			HBufC8* tmpReceivedMsg = iReceivedMsg;
    			iReceivedMsg = 0; 
    			
    			Listen();
    			ReceivedDataL( tmpReceivedMsg );
			    }
			else
			    {
			    // Required by PC-Lint
			    }
			break;
			}
		case KErrCancel:
			break;
		default:
			{
			__SIP_INT_LOG1( "CReceiver::RunL() status", iStatus.Int() )
			iContext.Remove();
			break;
			}
		}
	}

// -----------------------------------------------------------------------------
// CReceiver::ReceivedDataL
// -----------------------------------------------------------------------------
//
void CReceiver::ReceivedDataL( HBufC8* aData )
	{
	if ( iContext.SigCompHandler()->IsSupported() &&		
		 iContext.SigCompHandler()->IsSigCompMsg( *aData ) )
		{
		__SIP_LOG( "IsSigComp complete == ETrue" )

		CleanupStack::PushL( aData );
		TUint bytesconsumed = 0;
		CBufBase* decompressedmessage =				  
			iContext.SigCompHandler()->DecompressL( *aData,
													bytesconsumed,
													EFalse );
		CleanupStack::PopAndDestroy( aData );
		CleanupStack::PushL( decompressedmessage );

		HBufC8* receiveddata = HBufC8::NewL( decompressedmessage->Size() );
		receiveddata->Des().Copy( decompressedmessage->Ptr( 0 ) );
		CleanupStack::PopAndDestroy( decompressedmessage );

		iContext.ReceivedDataL( receiveddata, iAddr, ETrue );
		}
	else
		{
		iContext.ReceivedDataL( aData, iAddr, EFalse );
		}
	}

// -----------------------------------------------------------------------------
// CReceiver::ReceivePendingL
// -----------------------------------------------------------------------------
//	
void CReceiver::ReceivePendingL()
    {
    __ASSERT_ALWAYS( !IsActive(), User::Leave( KErrInUse ) );
    
    CSocketContainer* socketContainer = 
	    iContext.SocketContainer( iIPAddrFamily );
    __ASSERT_ALWAYS( socketContainer, User::Leave( KErrNotFound ) );
    
    iState = EReceiving;
			    
	TInt bytesPending( 0 );
	User::LeaveIfError( 
	    socketContainer->Socket().GetOpt( 
	        KSOReadBytesPending, 
            KSOLSocket, 
            bytesPending ) );

    __SIP_INT_LOG1( "CReceiver::ReceivePendingL() bytes", bytesPending )

	// Don't allow excessive buffer
	if ( bytesPending > KMaxMessageLength )
	    {
	    bytesPending = KMaxMessageLength;
	    }
	    
    delete iReceivedMsg;
	iReceivedMsg = 0;
	iReceivedMsg = HBufC8::NewL( bytesPending );
	iReceivedMsgPtr.Set( iReceivedMsg->Des() );
	socketContainer->Socket().RecvFrom( iReceivedMsgPtr, iAddr, 0, iStatus );     	
	SetActive();
    }

// -----------------------------------------------------------------------------
// CReceiver::RunError
// -----------------------------------------------------------------------------
//
TInt CReceiver::RunError( TInt aError )
	{
	Listen();
	if ( aError != KErrNoMemory )
		{
		return KErrNone;
		}
	return aError;
	}
