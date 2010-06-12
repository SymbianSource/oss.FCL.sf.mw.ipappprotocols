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
#include "CMSRPParser.h"
#include "MSRPCommon.h"

// -----------------------------------------------------------------------------
// CMSRPParser::NewL
// Static constructor
// -----------------------------------------------------------------------------
//
/*static*/MMSRPParser* CMSRPParser::NewL(MMSRPParserObserver& aConnection)
    {
    MSRPLOG( "CMSRPParser::NewL enter" )
    CMSRPParser* self = new (ELeave) CMSRPParser(aConnection);
    CleanupStack::PushL(self);
    self->ConstructL(aConnection);
    CleanupStack::Pop(self);
    MSRPLOG( "CMSRPParser::NewL exit" )
    return self;
    }

// -----------------------------------------------------------------------------
// CMSRPParser::CMSRPParser
// Constructor
// -----------------------------------------------------------------------------
//
CMSRPParser::CMSRPParser(MMSRPParserObserver& aConnection)        
    : CActive(CActive::EPriorityStandard), iConnection( aConnection )
    {  
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CMSRPParser::~CMSRPParser
// Destructor
// -----------------------------------------------------------------------------
//
CMSRPParser::~CMSRPParser()
    {
    MSRPLOG( "CMSRPParser::~CMSRPParser enter" )
    Cancel();    
    delete iParser;
    MSRPLOG( "CMSRPParser::~CMSRPParser exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPParser::ConstructL
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CMSRPParser::ConstructL(MMSRPParserObserver& aConnection)
    {
    //iState = EIdle;
    iParser = CMSRPMsgParser::NewL(aConnection);
    }

// -----------------------------------------------------------------------------
// CMSRPParser::DoCancel
// Cancels outstanding request.
// -----------------------------------------------------------------------------
//
void CMSRPParser::DoCancel()
    {
    MSRPLOG( "CMSRPParser::DoCancel enter" )
    //TRequestStatus* status = &iStatus;
    //User::RequestComplete( status, KErrCancel );
    MSRPLOG( "CMSRPParser::DoCancel exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPParser::Parse
// -----------------------------------------------------------------------------
//
void CMSRPParser::ParseL(RMsrpBuf& aBuf)
    { 
    MSRPLOG( "CMSRPParser::ParseL enter" )
    
    if(!IsActive())
        {
        TRequestStatus* status = &iStatus;
        SetActive();
        User::RequestComplete( status, KErrNone );
        }
    
    //collate with last element in array, else claim ownership
    if(iParser->iParseBuffers.Count())
        {
        if(iParser->iParseBuffers[iParser->iParseBuffers.Count()-1]->Collate(aBuf))
            return;
        }
        
    RMsrpBuf* buf = new (ELeave) RMsrpBuf(aBuf);
    CleanupDeletePushL(buf);
    iParser->iParseBuffers.AppendL(buf);
    CleanupStack::Pop( );
    MSRPLOG( "CMSRPParser::ParseL exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPParser::CancelReceiving
// -----------------------------------------------------------------------------
//
/*void CMSRPParser::CancelReceiving()
    {
    Cancel();
    }*/


// -----------------------------------------------------------------------------
// CMSRPParser::RunL
// -----------------------------------------------------------------------------
//
void CMSRPParser::RunL()
    {     
    MSRPLOG( "CMSRPParser::RunL enter" )
          
    TBool reschedule = iParser->ParseL();
    
    //count could be > 0 but data could be insufficient to parse
    if(reschedule && iParser->iParseBuffers.Count())
       {
       TRequestStatus* status = &iStatus;
       SetActive();
       User::RequestComplete( status, KErrNone );
       }
    MSRPLOG( "CMSRPParser::RunL exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPParser::RunError
// -----------------------------------------------------------------------------
//
TInt CMSRPParser::RunError(TInt /*aError*/)
    {
    MSRPLOG( "CMSRPParser::RunError enter" )    
    //aError = KErrNone;
    //ParseError//non leaving introduce
    TRAPD(err,iConnection.ParseStatusL(NULL, KErrCorrupt));
    MSRPLOG( "CMSRPParser::RunError exit" )
    return err;
    }
    

// End of File

