/*
* Copyright (c) 2002-2003 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/




#include "rtpcommsend.h"

// CONSTANTS

/////////////////////Class CRtpCommSend///////////////////////////////////////
// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// Set to address
// ---------------------------------------------------------------------------
// 
void CRtpCommSend::SetToAddress(TInetAddr& aAddr)
    {
    iToAddr.SetAddress( aAddr.Address() ); 
    iToAddr.SetPort( aAddr.Port() );     
    if (aAddr.Family() == KAfInet6 )
    	{
    	iToAddr.SetAddress( aAddr.Ip6Address() ); 
    	}  
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CRtpCommSend::CRtpCommSend( RSocket& aSocket, MRtpErrNotify& aErrNotify, TInetAddr& aAddr ) 
    : CActive( EPriorityStandard ), 
      iSocket( aSocket ),
      iToAddr(aAddr),      
      iWriteStatus( EWaiting ),
      iBufPtr( NULL, 0 ),
      iRtpErrNotify( aErrNotify ),
      iSendQue(CRtpSendItem::iOffset ),
      iQueueIter(iSendQue)
    {
    CActiveScheduler::Add( this );
    }
  
// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
// 
void CRtpCommSend::ConstructL( /*TInt aBufSize */)
    {

   	}

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CRtpCommSend* CRtpCommSend::NewL( RSocket& aSocket, MRtpErrNotify& aErrNotify,
                                  /*TInt aBufSize,*/ TInetAddr& aAddr )
    {
    CRtpCommSend* self = new ( ELeave ) CRtpCommSend( aSocket, aErrNotify, aAddr );
    CleanupStack::PushL( self );
    self->ConstructL( /*aBufSize*/ );
    CleanupStack::Pop(); //self
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// 
// ---------------------------------------------------------------------------
//
CRtpCommSend::~CRtpCommSend()
    {
    Cancel();
    CompleteAndRemoveRequests( KErrAbort );
    }

// ---------------------------------------------------------------------------
// CRtpCommSend::RunL()
// Called when request has completed.
// ---------------------------------------------------------------------------
//
void CRtpCommSend::RunL()
    {
    // Active object request complete handler
    if ( iStatus == KErrNone )
        {
        RemoveFromQueue();
		
        switch( iWriteStatus )
            {
            case ESending:
                SendNextPacket();
                break;
            default:
                RTP_DEBUG_DETAIL( "Tx state error" );
                User::Leave( KErrInUse );
                break;
            }
        }
    else 
        {
        RTP_DEBUG_DETAIL_DVALUE( "CRtpCommSend::RunL, ERR: ", iStatus.Int() );
        
        // Client originated cancelling is handled in CancelAsyncSend() function.
        if ( KErrCancel != iStatus.Int() )
            {
            CompleteAndRemoveRequests( iStatus.Int() );
            }
        
        iRtpErrNotify.ErrorNotify( iStatus.Int() );
        }
    }

// ---------------------------------------------------------------------------
// CRtpCommSend::RemoveFromQueue()
// Remove Item from Queue
// ---------------------------------------------------------------------------
//
void CRtpCommSend::RemoveFromQueue()
	{
	if ( !( iSendQue.IsEmpty() ) )
		{
		CRtpSendItem* item = iSendQue.First();
		CompleteClientRequest( *item, KErrNone );
		iSendQue.Remove(*item);
		delete item;
		}
	}
// ---------------------------------------------------------------------------
// CRtpCommSend::DoCancel()
// Cancel ongoing requests.
// ---------------------------------------------------------------------------
//
void CRtpCommSend::DoCancel()
    {	
    // Cancel asychronous send request
    CancelAsyncSend();
    } 

// ---------------------------------------------------------------------------
// CRtpCommSend::CancelAsyncSend()
// 
// ---------------------------------------------------------------------------
//
void CRtpCommSend::CancelAsyncSend()
	{
    RTP_DEBUG_DETAIL( "CRtpCommSend::CancelAsyncSend" );

    iSocket.CancelSend();
    CompleteAndRemoveRequests( KErrCancel );
	}

// ---------------------------------------------------------------------------
// CRtpCommSend::Send()
// 
// ---------------------------------------------------------------------------
//
void CRtpCommSend::Send( const TDesC8& aPacket, TRequestStatus& aStatus )
    {
    RTP_DEBUG_DETAIL( "CRtpCommSend::Send -- ASynchronous" );
    
    aStatus = KRequestPending;
    
    TInt err( KErrNone );
    TRAP( err, 
        iSendQue.AddLast( *CRtpSendItem::NewL( aPacket, iStatus, aStatus ) ) );
    
    if ( KErrNone == err )
        {
        if ( !IsActive() )
            {
            SendNextPacket();
            }
        }
    else
        {
        // Probably out of memory, so don't send any more packets
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, err );
        CompleteAndRemoveRequests( err );
        RTP_DEBUG_DETAIL( "Error Occured while put Packet to Send Item Que" );
        iRtpErrNotify.ErrorNotify( err );
        }
    }

// ---------------------------------------------------------------------------
// CRtpCommSend::Send()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpCommSend::Send( const TDesC8& aPacket )
    {
    RTP_DEBUG_DETAIL( "CRtpCommSend::Send -- Synchronous Entry" );
    
    TInt err( KErrNone );
    if ( ( iWriteStatus != EWaiting ) && ( iWriteStatus != ESending ) )
        {
        RTP_DEBUG_DETAIL( "Write status is not correct" );
        return KErrDisconnected;
        }
    
    // Put in in our buffer
    TRAP( err, iSendQue.AddLast(*CRtpSendItem::NewL(aPacket, iStatus)) );
	RTP_DEBUG_DETAIL( "Put Packet to Send Item Que" );
    
    if ( err != KErrNone )
        {
        // Probably out of memory, so don't send any more packets
        CompleteAndRemoveRequests( err );
        
		RTP_DEBUG_DETAIL( "Error Occured while put Packet to Send Item Que" );
        return err;
        }
    
    if ( !IsActive() )
        {
        SendNextPacket();
		}
	
	RTP_DEBUG_DETAIL( "CRtpCommSend::Send -- Synchronous Exit" );
    return err;
    }

// ---------------------------------------------------------------------------
// CRtpCommSend::SendNextPacket()
// Write data from buffer to socket.
// ---------------------------------------------------------------------------
//
void CRtpCommSend::SendNextPacket()
    {
    RTP_DEBUG_DETAIL( "CRtpCommSend::SendNextPacket Entry" );
    
    iWriteStatus = EWaiting;
    iQueueIter.SetToFirst();
	CRtpSendItem* item;
    if ( ( item=iQueueIter++ ) != NULL )
        {
        iSocket.SendTo( item->GetData(), iToAddr, 0, item->iStatus );
        SetActive();
        iWriteStatus = ESending;
        }

    RTP_DEBUG_DETAIL( "CRtpCommSend::SendNextPacket Exit" );
    }

// ---------------------------------------------------------------------------
// CRtpCommSend::CompleteAndRemoveRequests()
// Removes all requests in the queue.
// ---------------------------------------------------------------------------
//
void CRtpCommSend::CompleteAndRemoveRequests( TInt aError )
    {
    RTP_DEBUG_DETAIL( "CRtpCommSend::CompleteAndRemoveRequests Entry" );
    
    CRtpSendItem* item;
    iQueueIter.SetToFirst();
    item = iQueueIter++;
    while ( item )
        {
        CompleteClientRequest( *item, aError );
        iSendQue.Remove( *item );
        delete item;
        item = iQueueIter++;
        }
    
    RTP_DEBUG_DETAIL( "CRtpCommSend::CompleteAndRemoveRequests Exit" );
    }

// ---------------------------------------------------------------------------
// CRtpCommSend::CompleteClientRequest()
// Completes the request.
// ---------------------------------------------------------------------------
//
void CRtpCommSend::CompleteClientRequest( CRtpSendItem& aItem, 
    TInt aReason ) const
    {
    RTP_DEBUG_DETAIL( "CRtpCommSend::CompleteClientRequest Entry" );

    TRequestStatus* status = aItem.ClientStatus();
    if ( NULL != status )
        {
        User::RequestComplete( status, aReason );
        }
    }

// End of file


