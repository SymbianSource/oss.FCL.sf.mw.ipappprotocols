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

#include "CMSRPMessageChunkState.h"


EXPORT_C CMSRPMessageChunkState* CMSRPMessageChunkState::NewL( )
    {
    CMSRPMessageChunkState* self = new (ELeave) CMSRPMessageChunkState( );
    CleanupStack::PushL( self );
   // self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }


EXPORT_C CMSRPMessageChunkState::~CMSRPMessageChunkState()
    {
  //  MSRPStrings::Close();
    }

CMSRPMessageChunkState::CMSRPMessageChunkState( )
    {
    }
/*
void CMSRPMessageChunkState::ConstructL( ) 
    {
  
    }
*/
EXPORT_C void CMSRPMessageChunkState::SetStartPos(TInt aStartPos)
    {
    iStartPos = aStartPos ;
    }

EXPORT_C TInt CMSRPMessageChunkState::GetStartPos( )
    {
    return iStartPos;
    }

EXPORT_C void CMSRPMessageChunkState::SetEndPos(TInt aEndPos)
    {
    iEndPos = aEndPos ;
    }

EXPORT_C TInt CMSRPMessageChunkState::GetEndPos( )
    {
    return iEndPos;
    }

EXPORT_C TBool CMSRPMessageChunkState::GetResponseReceived()
    {
   if(iResponseReceived)
       return ETrue;
   else
       return EFalse;
    
    }
       
EXPORT_C void CMSRPMessageChunkState::SetResponseReceived(TBool aResponseReceived)
    {
    iResponseReceived = aResponseReceived;
    }

EXPORT_C TDesC8& CMSRPMessageChunkState::GetTransactionId( )
    {
    return iTransactionId;
    }

EXPORT_C void CMSRPMessageChunkState::SetTransactionId(TDesC8& aTransactionId )
    {
    iTransactionId = aTransactionId ;
    }

EXPORT_C void CMSRPMessageChunkState::SetResponseSent(TBool aResponseSent)
    {
    iResponseSent = aResponseSent;
    }

EXPORT_C TBool CMSRPMessageChunkState::GetResponseSent( )
    {
    if(iResponseSent)
        return ETrue;
    else
        return EFalse;
    }
