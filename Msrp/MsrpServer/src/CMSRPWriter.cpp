/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html."
* Initial Contributors:
* Nokia Corporation - initial contribution.
* Contributors:
*
* Description:
* MSRP Implementation
*
*/

// INCLUDES

// CLASS HEADER
#include "CMSRPWriter.h"
#include "MSRPCommon.h"

// -----------------------------------------------------------------------------
// CMSRPParser::NewL
// Static constructor
// -----------------------------------------------------------------------------
//
/*static*/MMSRPWriter* CMSRPWriter::NewL(RSocket& aSocket, MMSRPWriterErrorObserver& aConnection)
    {
    MSRPLOG( "CMSRPWriter::NewL enter" )
    CMSRPWriter* self = new (ELeave) CMSRPWriter( aSocket, aConnection );
    CleanupStack::PushL(self);
    //self->ConstructL();
    CleanupStack::Pop(self);
    MSRPLOG( "CMSRPWriter::NewL exit" )
    return self;
    }

// -----------------------------------------------------------------------------
// CMSRPWriter::CMSRPWriter
// Constructor
// -----------------------------------------------------------------------------
//
CMSRPWriter::CMSRPWriter(RSocket& aSocket, MMSRPWriterErrorObserver& aConnection)        
    : CActive(CActive::EPriorityStandard), iConnection( aConnection ), iSocket(aSocket)
    {  
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CMSRPWriter::~CMSRPWriter
// Destructor
// -----------------------------------------------------------------------------
//
CMSRPWriter::~CMSRPWriter()
    {
    MSRPLOG( "CMSRPWriter::~CMSRPWriter enter" )
    Cancel();    
    iSendQueue.Reset();
    iSendQueue.Close();
    MSRPLOG( "CMSRPWriter::~CMSRPWriter exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPWriter::ConstructL
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
/*
void CMSRPWriter::ConstructL()
    {
    //iWriteIssued = FALSE;
    //iState = EIdle;
    }
*/

// -----------------------------------------------------------------------------
// CMSRPWriter::DoCancel
// Cancels outstanding request.
// -----------------------------------------------------------------------------
//
void CMSRPWriter::DoCancel()
    {
    MSRPLOG( "CMSRPWriter::DoCancel enter" )
    
    if(iWriteIssued)
        {
        MSRPLOG( "CMSRPWriter::write issued cancelled" )
        iSocket.CancelWrite();      
        }
    else
        {
        //MSRPLOG( "CMSRPWriter::self complete cancelled" )
        //TRequestStatus* status = &iStatus;
        //User::RequestComplete( status, KErrCancel );
        }
    MSRPLOG( "CMSRPWriter::DoCancel exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPWriter::Parse
// -----------------------------------------------------------------------------
//
void CMSRPWriter::RequestSendL(MMSRPWriterObserver& aMsg)
    { 
    MSRPLOG( "CMSRPWriter::RequestSendL enter" )
    // making sure the msg is not already in queue
    for ( TInt i = 0; i < iSendQueue.Count(); i++ )
        {
        if ( iSendQueue[ i ] == &aMsg )
            {
            // already included
            return;
            }
        }
    
    iSendQueue.AppendL(&aMsg);
    if(!IsActive())
        {
        /*avoid invoking send in other active object context*/
        //SendL
        TRequestStatus* status = &iStatus;
        SetActive();
        User::RequestComplete( status, KErrNone );
        }
    MSRPLOG( "CMSRPWriter::RequestSendL exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPWriter::CancelSendingL
// -----------------------------------------------------------------------------
//
void CMSRPWriter::CancelSendingL( const MMSRPWriterObserver* aMsg )
    {
    MSRPLOG( "CMSRPWriter::CancelSendingL enter" )
    if ( iSendQueue.Count() )
        {
        if ( iSendQueue[ 0 ] == aMsg )
            {
            if( iWriteIssued )
                {
                MSRPLOG( "CMSRPWriter::CancelSendingL Write Canceled" )
                iSocket.CancelWrite();      
                }
            }
        }
    }   

// -----------------------------------------------------------------------------
// CMSRPWriter::CancelReceiving
// -----------------------------------------------------------------------------
//
/*void CMSRPWriter::CancelReceiving()
    {
    Cancel();
    }*/


// -----------------------------------------------------------------------------
// CMSRPWriter::RunL
// -----------------------------------------------------------------------------
//
void CMSRPWriter::RunL()
    { 
    MSRPLOG( "CMSRPWriter::RunL enter" )    
        
    TInt status = iStatus.Int();
        
    if(status != KErrNone)
        {
        //socket error
        iConnection.WriteSocketError(status);
        return;
        }
    
    if(iWriteIssued)
        {
        iWriteIssued = FALSE;
        MMSRPWriterObserver::TMsgStatus msgState = MMSRPWriterObserver::EComplete;
        msgState = iSendQueue[0]->WriteDoneL(status); 
        
        if( msgState != MMSRPWriterObserver::EPending && 
                msgState != MMSRPWriterObserver::ESendingReport )    
            {
            MMSRPWriterObserver* obs = iSendQueue[0];
            iSendQueue.Remove(0);
            if(msgState == MMSRPWriterObserver::EInterrupted)
                iSendQueue.AppendL(obs);              
            }
        }
        
    SendL();
                    
    MSRPLOG( "CMSRPWriter::RunL exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPWriter::RunError
// -----------------------------------------------------------------------------
//
TInt CMSRPWriter::RunError(TInt aError)
    {
    MSRPLOG( "CMSRPWriter::RunError enter" )
    MSRPLOG( "CMSRPWriter::RunError exit" )
    /*get send buffer errors shud b handled in SendL 
     * by deleting msg obs from writer queue, if part of msg written to conn is 0 or complete
     * send failed msg error to client  in msghandler b4 getsend returns*/
    iConnection.WriteSocketError(aError);
    return KErrNone;    
    }
    
// -----------------------------------------------------------------------------
// CMSRPWriter::SendL
// -----------------------------------------------------------------------------
//
void CMSRPWriter::SendL()
    {
    TBool interruptSend = FALSE;

    if(iSendQueue.Count())        
        {
        MMSRPWriterObserver::TWriteStatus ret = MMSRPWriterObserver::EMsrpSocketWrite;
        
        if(iSendQueue.Count()>1)
            interruptSend = TRUE;
        
        const TDesC8& writeBuffer = iSendQueue[0]->GetSendBufferL( ret, interruptSend);
        
        Deque();
        CActiveScheduler::Add(this);
        SetActive();

        if( ret == MMSRPWriterObserver::EMsrpSocketWrite ) //KErrNone == 0
           {
           iSocket.Write( writeBuffer, iStatus );
           iWriteIssued = TRUE;           
           }   
        else if (ret == MMSRPWriterObserver::EMsrpAvoidSocketWrite)//EAvoidSocketWrite
            {
            TRequestStatus* status = &iStatus;                       
            User::RequestComplete( status, KErrNone );
            iWriteIssued = FALSE;        
            }
        

         /* check in run_error: getsendbufferl leaves are recoverable
          * if msg state equivalent to nothing written to connection
          * ,remove observer and try to recover
          * if writedone leaves, then we can be sure that msg state will reflect socket write issued
          * and so no attempt will be made to recover
          */
        
        //iSendQueue.Remove(0);
        
        }            
    }


// End of File

