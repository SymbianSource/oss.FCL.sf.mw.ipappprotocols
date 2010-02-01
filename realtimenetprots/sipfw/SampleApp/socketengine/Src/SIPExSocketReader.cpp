
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
//



#include    "SIPExSocketReader.h"
#include    "SIPEXReaderNotifier.h"
#include    <in_sock.h>
#include    <e32svr.h>

// -----------------------------------------------------------------------------
// CSIPExSocketReader::NewL
// Static constructor
// -----------------------------------------------------------------------------
//
CSIPExSocketReader* CSIPExSocketReader::NewL( 
    RSocket& aSocket, 
    MSIPExReaderNotifier& aNotifier )
    {
    CSIPExSocketReader* self = NewLC( aSocket, aNotifier );
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPExSocketReader::NewLC
// Static constructor
// -----------------------------------------------------------------------------
//    
CSIPExSocketReader* CSIPExSocketReader::NewLC( 
    RSocket& aSocket, 
    MSIPExReaderNotifier& aNotifier )
    {
    CSIPExSocketReader* self = new (ELeave) CSIPExSocketReader( aSocket, aNotifier );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPExSocketReader::CSIPExSocketReader
// Constructor
// -----------------------------------------------------------------------------
//
CSIPExSocketReader::CSIPExSocketReader( 
    RSocket& aSocket, 
    MSIPExReaderNotifier& aNotifier )
    : CActive(CActive::EPriorityStandard), 
      iNotifier( aNotifier ), 
      iSocket( aSocket )
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CSIPExSocketReader::~CSIPExSocketReader
// Destructor
// -----------------------------------------------------------------------------
//
CSIPExSocketReader::~CSIPExSocketReader()
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CSIPExSocketReader::ConstructL
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CSIPExSocketReader::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CSIPExSocketReader::DoCancel
// Cancels outstanding request.
// -----------------------------------------------------------------------------
//
void CSIPExSocketReader::DoCancel()
    {
    iSocket.CancelRead();
    }

// -----------------------------------------------------------------------------
// CSIPExSocketReader::RunL
// Buffers the characters read from socket and notifies the observer when buffer
// if full. If error notifies the observer.
// -----------------------------------------------------------------------------
//
void CSIPExSocketReader::RunL()
    {
    if( iStatus.Int() == KErrNone )
        {
        iMessage.Append( iBuffer );
        iBuffer.Zero();
        if( iMessage.Length() == 4 )
            {
            iNotifier.MessageReceived( iMessage );
            iMessage.Zero();
            }
        Read();
        }
    else
        {
        iMessage.FillZ();
        iMessage.Zero();
        iNotifier.ErrorInReading( iStatus.Int() );
        }
    }

// -----------------------------------------------------------------------------
// CSIPExSocketReader::Read
// Asynchronous
// Starts the asynchoronous reading if not already active.
// -----------------------------------------------------------------------------
//
void CSIPExSocketReader::Read()
    {
    if( !IsActive() )
        {
        iSocket.Read( iBuffer, iStatus );
        SetActive();
        }
    }

// End of file
