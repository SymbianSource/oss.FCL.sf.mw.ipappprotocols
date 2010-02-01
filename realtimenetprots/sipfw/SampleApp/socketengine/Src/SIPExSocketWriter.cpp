
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



// INCLUDES
#include    "SIPExSocketWriter.h"
#include    "SIPExWriterNotifier.h"
#include    <in_sock.h>

// -----------------------------------------------------------------------------
// CSIPExSocketWriter::NewL
// Static constructor
// -----------------------------------------------------------------------------
//
CSIPExSocketWriter* CSIPExSocketWriter::NewL( 
    RSocket& aSocket, 
    MSIPExWriterNotifier& aNotifier )
    {
    CSIPExSocketWriter* self = NewLC( aSocket, aNotifier );
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPExSocketWriter::NewLC
// Static constructor
// -----------------------------------------------------------------------------
//    
CSIPExSocketWriter* CSIPExSocketWriter::NewLC( 
    RSocket& aSocket, 
    MSIPExWriterNotifier& aNotifier )
    {
    CSIPExSocketWriter* self = new (ELeave) CSIPExSocketWriter( aSocket, aNotifier );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPExSocketWriter::CSIPExSocketWriter
// Constructor
// -----------------------------------------------------------------------------
//
CSIPExSocketWriter::CSIPExSocketWriter( 
    RSocket& aSocket, 
    MSIPExWriterNotifier& aNotifier )
    : CActive(CActive::EPriorityStandard), 
      iNotifier( aNotifier ), 
      iSocket( aSocket )
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CSIPExSocketWriter::~CSIPExSocketWriter
// Destructor
// -----------------------------------------------------------------------------
//
CSIPExSocketWriter::~CSIPExSocketWriter()
    {
    Cancel();
    delete iBuffer;
    }

// -----------------------------------------------------------------------------
// CSIPExSocketWriter::ConstructL
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CSIPExSocketWriter::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CSIPExSocketWriter::DoCancel
// From CActive
// -----------------------------------------------------------------------------
//
void CSIPExSocketWriter::DoCancel()
    {
    iSocket.CancelWrite();
    }

// -----------------------------------------------------------------------------
// CSIPExSocketWriter::RunL
// From CActive
// -----------------------------------------------------------------------------
//
void CSIPExSocketWriter::RunL()
    {
    if( iBuffer )
        {
        delete iBuffer;
        iBuffer = NULL;
        }

    if( iStatus.Int() == KErrNone )
        {
        iNotifier.WriteDone();
        }
    else
        {
        iNotifier.ErrorInWriting( iStatus.Int() );
        }
    }

// -----------------------------------------------------------------------------
// CSIPExSocketWriter::Write
// Asynchronous
// Copy the parameter to the member variable and start asynchronous writing.
// -----------------------------------------------------------------------------
//
void CSIPExSocketWriter::Write( const TDesC8& aBuffer )
    {
    if( iBuffer )
        {
        delete iBuffer;
        iBuffer = NULL;
        }

    TRAPD( err, iBuffer = aBuffer.AllocL() );

    if( !IsActive() && err == KErrNone )
        {
        iSocket.Write( *iBuffer, iStatus );
        SetActive();
        }
    }

// End of file
