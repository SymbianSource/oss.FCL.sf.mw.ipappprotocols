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
// Name        : TTCPCompMsgEnd.cpp
// Part of     : ConnectionMgr
// Implementation
// Version     : SIP/4.0
//



#include "TTCPCompMsgEnd.h"
#include "MMsgAssemblerContext.h"
#include "MSigCompController.h"
#include "sipmessage.h"
#include "SipAssert.h"

// ----------------------------------------------------------------------------
// TTCPCompMsgEnd::DataReceivedL
// ----------------------------------------------------------------------------
//
void TTCPCompMsgEnd::DataReceivedL( TPtr8 aData, TUint&  aNextLength )
	{
	// panic if sigcomp is not supported in debug mode.leaves in release mode.
	__SIP_ASSERT_LEAVE(	iMsgAssembler.SigComp().IsSupported(), KErrGeneral );
	// panic if received data is not compressed in debug mode.
	// leaves in release mode.
	__SIP_ASSERT_LEAVE(	iMsgAssembler.SigComp().IsSigCompMsg( aData ), 
	                    KErrGeneral );
	// panic if received data is not completed compressed msg in debug mode.
	// leaves in release mode.
	__SIP_ASSERT_LEAVE(	
			iMsgAssembler.SigComp().IsCompleteSigCompMessageL( aData ), 
	        KErrGeneral );
	        
	TUint bytesConsumed( 0 );
	CBufBase* decompressedData = iMsgAssembler.SigComp().DecompressL( 
											aData, bytesConsumed, ETrue);

    TUint dataLen( static_cast<TUint>( aData.Length() ) );
    
    // Whole data was not decompressed and non-decompressed data might
	// be part of next sigcomp message, remember amount of non-decompressed
	// data
	iMsgAssembler.SetUnConsumedBytes( dataLen - bytesConsumed );
		
	if ( bytesConsumed < dataLen )
		{	
    	CleanupStack::PushL(decompressedData);
		aData.Delete(0, bytesConsumed);

		HBufC8* newData = 
		   HBufC8::NewL( decompressedData->Size() + aData.Length() );
	    // copy the msg buffer data and the received data to new data buffer
	    TPtr8 newDataPtr = newData->Des();
	    newDataPtr.Append(decompressedData->Ptr(0));
 	    CleanupStack::PopAndDestroy(decompressedData);
	    newDataPtr.Append(aData);
	    // delete all content of received data
	    aData.Delete( 0, aData.Length() );
  	    CleanupStack::PushL(newData);
  	    DecideNextStateL( newDataPtr, aNextLength );
	    CleanupStack::PopAndDestroy(newData);	
		}	
	else if ( bytesConsumed == dataLen )
		{
		CleanupStack::PushL( decompressedData );
		aData.Delete(0, bytesConsumed);	
		TPtr8 decompressedDataPtr = decompressedData->Ptr(0);
	    DecideNextStateL( decompressedDataPtr, aNextLength );
	    CleanupStack::PopAndDestroy( decompressedData );
		}
	else // bytesConsumed > dataLen error happens, reset the state
		{
		delete decompressedData;
		iMsgAssembler.ChangeState( MMsgAssemblerContext::EMsgInit );
		}				
	}

// ----------------------------------------------------------------------------
// TTCPCompMsgEnd::DecideNextStateL
// ----------------------------------------------------------------------------
//	
void TTCPCompMsgEnd::DecideNextStateL( TPtr8 aData, TUint&  aNextLength )
    {
    // if there's already some content, decompressed stuff (aData) is 
    // complementing existing content and header "parsing" states can be
    // skipped
    CSIPMessage* message = iMsgAssembler.Message();
    if ( message && message->Content().Length() > 0 )
        {
        iMsgAssembler.ChangeState( MMsgAssemblerContext::EMsgContentStart );  
        }
    else
        {
        iMsgAssembler.ChangeState( MMsgAssemblerContext::EMsgHeaderStart );
        }
    iMsgAssembler.CurrentState().DataReceivedL( aData, aNextLength );
    }

// End of File
