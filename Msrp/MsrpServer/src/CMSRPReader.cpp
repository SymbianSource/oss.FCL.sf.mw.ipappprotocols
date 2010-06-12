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
#include "CMSRPReader.h"
#include "MSRPCommon.h"

// -----------------------------------------------------------------------------
// CMSRPReader::NewL
// Static constructor
// -----------------------------------------------------------------------------
//
/*static*/MMSRPReader* CMSRPReader::NewL(RSocket& aSocket, MMSRPReaderObserver& aConnection)
    {
    MSRPLOG( "CMSRPReader::NewL enter" )
    CMSRPReader* self = new (ELeave) CMSRPReader( aSocket, aConnection );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    MSRPLOG( "CMSRPReader::NewL exit" )
    return self;
    }

// -----------------------------------------------------------------------------
// CMSRPReader::CMSRPReader
// Constructor
// -----------------------------------------------------------------------------
//
CMSRPReader::CMSRPReader(RSocket& aSocket, MMSRPReaderObserver& aConnection)        
    : CActive(CActive::EPriorityStandard), iConnection( aConnection ), iSocket(aSocket)
    {  
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CMSRPReader::~CMSRPReader
// Destructor
// -----------------------------------------------------------------------------
//
CMSRPReader::~CMSRPReader()
    {
    MSRPLOG( "CMSRPReader::~CMSRPReader enter" )
    Cancel();    
    delete iBuf;
    MSRPLOG( "CMSRPReader::~CMSRPReader exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPReader::ConstructL
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CMSRPReader::ConstructL()
    {
    //iState = EIdle;
    }

// -----------------------------------------------------------------------------
// CMSRPReader::DoCancel
// Cancels outstanding request.
// -----------------------------------------------------------------------------
//
void CMSRPReader::DoCancel()
    {
    MSRPLOG( "CMSRPReader::DoCancel enter" )
    iSocket.CancelRecv();
    MSRPLOG( "CMSRPReader::DoCancel exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPReader::Connect
// -----------------------------------------------------------------------------
//
void CMSRPReader::StartReceivingL(RMsrpBuf aBuf)
    {
    MSRPLOG( "CMSRPReader::StartReceivingL enter" )
    //claim ownership, 
    delete iBuf;
    iBuf = NULL;
    iBuf = new (ELeave) RMsrpBuf(aBuf);
    //make sure that iBuf' ptr length is  updated and not some local copy
    iSocket.RecvOneOrMore( *iBuf, 0, iStatus, iNumberOfBytesRead );
    SetActive();
    MSRPLOG( "CMSRPReader::StartReceivingL exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPReader::IsMessageMSRPReport
// -----------------------------------------------------------------------------
//
/*void CMSRPReader::CancelReceiving()
    {
    Cancel();
    }*/


// -----------------------------------------------------------------------------
// CMSRPReader::RunL
// -----------------------------------------------------------------------------
//
void CMSRPReader::RunL()
    {   
    MSRPLOG( "CMSRPReader::RunL enter" )
    Deque();
    CActiveScheduler::Add(this);
    iConnection.ReadStatusL(*iBuf, iStatus.Int());
    MSRPLOG( "CMSRPReader::RunL exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPReader::RunError
// -----------------------------------------------------------------------------
//
TInt CMSRPReader::RunError(TInt /*aError*/)
    {
    MSRPLOG( "CMSRPReader::RunError enter" )
    MSRPLOG( "CMSRPReader::RunError exit" )
    //iBuf cud b invalid, if iBuf alloc leaves
    //iConnection.ReadStatus(/**iBuf*/, aError);
    return KErrNone;
    }
    

// End of File

