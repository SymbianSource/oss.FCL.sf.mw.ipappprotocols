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
// Name          : CSIPMsgAssembler.cpp
// Part of       : ConnectionMgr
// Version       : SIP/4.0 
//




#include "SipLogs.h"
#include "SipAssert.h"
#include "sipmessage.h"

#include "TTCPMsgInit.h"
#include "TTCPMsgHeaderStart.h"
#include "TTCPMsgHeaderEnd.h"
#include "TTCPMsgContentStart.h"
#include "TTCPMsgContentEnd.h"
#include "TTCPCompMsgStart.h"
#include "TTCPCompMsgEnd.h"
#include "TTCPMsgComplete.h"

#include "CSIPMsgAssembler.h"
#include "MSIPMsgAssemblerOwner.h"
#include "MSigCompController.h"



// -----------------------------------------------------------------------------
// CSIPMsgAssembler::NewL
// -----------------------------------------------------------------------------
//
CSIPMsgAssembler* CSIPMsgAssembler::NewL ( MSIPMsgAssemblerOwner& aMsgOwner,
		                                   MSigCompController&    aSigComp )
	{
	CSIPMsgAssembler* self = CSIPMsgAssembler::NewLC(aMsgOwner, aSigComp);
	CleanupStack::Pop();
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPMsgAssembler::NewLC
// -----------------------------------------------------------------------------
//
CSIPMsgAssembler* CSIPMsgAssembler::NewLC ( MSIPMsgAssemblerOwner& aMsgOwner,
		                                    MSigCompController&    aSigComp )
	{
	CSIPMsgAssembler* self = new (ELeave) CSIPMsgAssembler(aMsgOwner, aSigComp);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPMsgAssembler::~CSIPMsgAssembler
// -----------------------------------------------------------------------------
//
CSIPMsgAssembler::~CSIPMsgAssembler ()
	{
	delete iMsgBuf;
	delete iMessage;	
	}
  
// -----------------------------------------------------------------------------
// CSIPMsgAssembler::CSIPMsgAssembler
// -----------------------------------------------------------------------------
//  
CSIPMsgAssembler::CSIPMsgAssembler(MSIPMsgAssemblerOwner& aMsgOwner,
		                           MSigCompController&    aSigComp)
 : iMsgOwner  ( aMsgOwner ),
   iSigComp   ( aSigComp ),
   iStates    ( EMsgMaxStates ),
   iStateValue( MMsgAssemblerContext::EMsgInit ),
   iIsSigComp ( EFalse ),
   iUnConsumedBytes( 0 )
	{
	}

// -----------------------------------------------------------------------------
// CSIPMsgAssembler::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPMsgAssembler::ConstructL ()
	{
	iMsgBuf = CBufFlat::NewL(100);
		
	iStates.AppendL( TTCPMsgInit( *this ),
					 sizeof( TTCPMsgInit ) );
	iStates.AppendL( TTCPMsgHeaderStart( *this ),
					 sizeof( TTCPMsgHeaderStart ) );
	iStates.AppendL( TTCPMsgHeaderEnd( *this ),
					 sizeof( TTCPMsgHeaderEnd ) );
	iStates.AppendL( TTCPMsgContentStart( *this ),
					 sizeof( TTCPMsgContentStart ) );
	iStates.AppendL( TTCPMsgContentEnd( *this ),
					 sizeof( TTCPMsgContentEnd ) );
	iStates.AppendL( TTCPCompMsgStart( *this ),
					 sizeof( TTCPCompMsgStart ) );
	iStates.AppendL( TTCPCompMsgEnd( *this ),
					 sizeof( TTCPCompMsgEnd ) );
	iStates.AppendL( TTCPMsgComplete( *this ),
					 sizeof( TTCPMsgComplete ) );
	}

// -----------------------------------------------------------------------------
// CSIPMsgAssembler::InputL
// -----------------------------------------------------------------------------
//
void CSIPMsgAssembler::InputL ( HBufC8* aData, TUint& aNextLength )
	{
	__ASSERT_DEBUG(aData != 0,
		User::Panic(_L("CSIPMsgAssembler::InputL, aData == 0"),
		KErrArgument));

	__ASSERT_DEBUG(aData->Length() != 0,
		User::Panic(_L("CSIPMsgAssembler::InputL, aData->Length() == 0"),
		KErrArgument));

	
	aNextLength = CSIPMsgAssembler::EMsgBufferSize;
	
	TCleanupItem cleanupItem( ResetState, this );
	CleanupStack::PushL( cleanupItem );

    TPtr8 dataPtr = aData->Des();
	CurrentState().DataReceivedL( dataPtr, aNextLength );
	
	CleanupStack::Pop(); // cleanupItem
	
	delete aData;
	}

// -----------------------------------------------------------------------------
// CSIPMsgAssembler::MsgOwner
// -----------------------------------------------------------------------------
//
MSIPMsgAssemblerOwner& CSIPMsgAssembler::MsgOwner()
	{
	return iMsgOwner;
	}
	
// -----------------------------------------------------------------------------
// CSIPMsgAssembler::SigComp
// -----------------------------------------------------------------------------
//	
MSigCompController& CSIPMsgAssembler::SigComp()
	{
	return iSigComp;
	}

// -----------------------------------------------------------------------------
// CSIPMsgAssembler::MsgBuffer
// -----------------------------------------------------------------------------
//		
CBufFlat& CSIPMsgAssembler::MsgBuffer()
	{
	return *iMsgBuf;
	}

// -----------------------------------------------------------------------------
// CSIPMsgAssembler::Message
// -----------------------------------------------------------------------------
//	
CSIPMessage* CSIPMsgAssembler::Message() 
	{
	return iMessage;
	}

// -----------------------------------------------------------------------------
// CSIPMsgAssembler::SetMessage
// -----------------------------------------------------------------------------
//	
void CSIPMsgAssembler::SetMessage( CSIPMessage* aMessage )
	{
	delete iMessage;
	iMessage = aMessage;
	}
  
// -----------------------------------------------------------------------------
// CSIPMsgAssembler::DetachMsg
// -----------------------------------------------------------------------------
//      
void CSIPMsgAssembler::DetachMsg()
	{
	iMessage = NULL;
	}

// -----------------------------------------------------------------------------
// CSIPMsgAssembler::ChangeState
// -----------------------------------------------------------------------------
//   	
void CSIPMsgAssembler::ChangeState( 
	MMsgAssemblerContext::TTCPMsgStateValue aStateValue )
	{
	iStateValue = aStateValue;
	}
     
// -----------------------------------------------------------------------------
// CSIPMsgAssembler::CurrentState
// -----------------------------------------------------------------------------
//    
TTCPMsgState& CSIPMsgAssembler::CurrentState()
	{
	return const_cast< TTCPMsgState& >( iStates.At( iStateValue ) );
	}

// -----------------------------------------------------------------------------
// CSIPMsgAssembler::SetIsSigComp
// -----------------------------------------------------------------------------
// 	
void CSIPMsgAssembler::SetIsSigComp( TBool aIsSigComp )
    {
    iIsSigComp = aIsSigComp;
    }

// -----------------------------------------------------------------------------
// CSIPMsgAssembler::IsSigComp
// -----------------------------------------------------------------------------
//     
TBool CSIPMsgAssembler::IsSigComp() const
    {
    return iIsSigComp;
    }

// -----------------------------------------------------------------------------
// CSIPMsgAssembler::SetUnConsumedBytes
// -----------------------------------------------------------------------------
//     
void CSIPMsgAssembler::SetUnConsumedBytes( TInt aUnConsumedBytes )
    {
    iUnConsumedBytes = aUnConsumedBytes;
    }

// -----------------------------------------------------------------------------
// CSIPMsgAssembler::UnConsumedBytes
// -----------------------------------------------------------------------------
//     
TInt CSIPMsgAssembler::UnConsumedBytes() const
    {
    return iUnConsumedBytes;
    }
	
// -----------------------------------------------------------------------------
// CSIPMsgAssembler::ResetState
// -----------------------------------------------------------------------------
//
void CSIPMsgAssembler::ResetState ( TAny* aMsgAssembler )
	{
	__SIP_ASSERT_RETURN( aMsgAssembler, KErrGeneral );

	CSIPMsgAssembler* msgAssembler = 
		reinterpret_cast<CSIPMsgAssembler*>( aMsgAssembler );

	msgAssembler->ChangeState( MMsgAssemblerContext::EMsgInit );	
	}
