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
// Name        : TTCPCompMsgStart.cpp
// Part of     : ConnectionMgr
// Implementation
// Version     : SIP/4.0
//



#include "TTCPCompMsgStart.h"
#include "MMsgAssemblerContext.h"
#include "MSigCompController.h"
#include "SipAssert.h"


// ----------------------------------------------------------------------------
// TTCPCompMsgStart::DataReceivedL
// ----------------------------------------------------------------------------
//
void TTCPCompMsgStart::DataReceivedL( TPtr8 aData, TUint&  aNextLength )
	{
	// panic if sigcomp is not supported in debug mode.leaves in release mode.
	__SIP_ASSERT_LEAVE(	iMsgAssembler.SigComp().IsSupported(), KErrGeneral );
	     
	if ( iMsgAssembler.MsgBuffer().Size() > 0 )
		{
		// insert the received data to msg buffer.
		iMsgAssembler.MsgBuffer().InsertL(
									iMsgAssembler.MsgBuffer().Size(), aData );
		iMsgAssembler.MsgBuffer().Compress();
		// clean the content of received data.
		aData.Delete( 0, aData.Length() );
		TPtr8 bufPtr = iMsgAssembler.MsgBuffer().Ptr( 0 );
		
		// check if the msg buffer a complete sig comp message, if so
		// copy the msg buffer to a new data buf, clean the msg buffer
		// and enter to comp msg end state.
		if ( iMsgAssembler.SigComp().IsCompleteSigCompMessageL( bufPtr ) )
			{
			HBufC8* newData = HBufC8::NewLC( bufPtr.Length() );
			// copy the msg buffer data and the received data to new buffer
			TPtr8 newDataPtr = newData->Des();
			newDataPtr.Append(bufPtr);
			// clean the msg buffer
			iMsgAssembler.MsgBuffer().Reset();
			iMsgAssembler.MsgBuffer().Compress();
			
			iMsgAssembler.ChangeState( MMsgAssemblerContext::ECompMsgEnd );
			iMsgAssembler.CurrentState().DataReceivedL( newDataPtr, 
			                                            aNextLength );
	    	CleanupStack::PopAndDestroy(newData);
			}
		}
	else if (iMsgAssembler.SigComp().IsCompleteSigCompMessageL( aData ) )
		{
		iMsgAssembler.ChangeState( MMsgAssemblerContext::ECompMsgEnd );
		iMsgAssembler.CurrentState().DataReceivedL( aData, aNextLength );
		}
	else
		{
		iMsgAssembler.MsgBuffer().InsertL(
									iMsgAssembler.MsgBuffer().Size(), aData );
		iMsgAssembler.MsgBuffer().Compress();
		}
	}
