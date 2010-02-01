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
// Name        : TTCPMsgHeaderStart.cpp
// Part of     : ConnectionMgr
// Implementation
// Version     : SIP/4.0
//



#include "TTCPMsgHeaderStart.h"
#include "MMsgAssemblerContext.h"

// ----------------------------------------------------------------------------
// TTCPMsgHeaderStart::DataReceivedL
// ----------------------------------------------------------------------------
//
void TTCPMsgHeaderStart::DataReceivedL( TPtr8 aData, TUint&  aNextLength )
	{
	// if there are existing data in msg buffer
	if ( iMsgAssembler.MsgBuffer().Size() > 0 )
		{	
		// insert the received data to msg buffer.
		iMsgAssembler.MsgBuffer().InsertL(
							iMsgAssembler.MsgBuffer().Size(), aData );
		iMsgAssembler.MsgBuffer().Compress();	
		// delete content of received data.
		aData.Delete( 0, aData.Length() );
		TPtr8 bufPtr = iMsgAssembler.MsgBuffer().Ptr( 0 );
		
		// check if header end mark can be found from msg buffer,
		// if so, allocate new data from msg buffer, and clear msg buffer
		// enter to Msg header end state. 
		if (FindHeaderEndPosition( bufPtr ) >= 0)
			{
			HBufC8* newData = HBufC8::NewLC( bufPtr.Length() );
		    // copy the msg buffer data to new data 
	    	TPtr8 newDataPtr = newData->Des();
	    	newDataPtr.Append( bufPtr );
	    	// clean the msg buffer
			iMsgAssembler.MsgBuffer().Reset();
			iMsgAssembler.MsgBuffer().Compress();
	    				
	    	iMsgAssembler.ChangeState( MMsgAssemblerContext::EMsgHeaderEnd );
			iMsgAssembler.CurrentState().DataReceivedL( newDataPtr, 
												        aNextLength );
	    	CleanupStack::PopAndDestroy(newData);
			}	
		}
	else if ( FindHeaderEndPosition( aData ) >= 0 )
		{
		iMsgAssembler.ChangeState( MMsgAssemblerContext::EMsgHeaderEnd );
		iMsgAssembler.CurrentState().DataReceivedL( aData, aNextLength );
		}
	// if the header end mark is not found, it means the received data 
	// contains only part of SIP message header, copy the received data to 
	// a buffer and wait for next part of message header
	else 
		{
		iMsgAssembler.MsgBuffer().InsertL(
									iMsgAssembler.MsgBuffer().Size(), aData );
		iMsgAssembler.MsgBuffer().Compress();
		}
	}
