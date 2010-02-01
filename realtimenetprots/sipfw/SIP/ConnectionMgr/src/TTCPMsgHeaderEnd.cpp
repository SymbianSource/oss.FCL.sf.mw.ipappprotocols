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
// Name        : TTCPMsgHeaderEnd.cpp
// Part of     : ConnectionMgr
// Implementation
// Version     : SIP/4.0
//



#include "TTCPMsgHeaderEnd.h"
#include "sipmessage.h"
#include "sipmessageparser.h"
#include "MMsgAssemblerContext.h"
#include "MSIPMsgAssemblerOwner.h"
#include "SipAssert.h"

// ----------------------------------------------------------------------------
// TTCPMsgHeaderEnd::DataReceivedL
// ----------------------------------------------------------------------------
//
void TTCPMsgHeaderEnd::DataReceivedL( TPtr8 aData, TUint&  aNextLength )
	{
	// if the header end mark can be found from the received data.
	// parse the header to SIP message.
	TInt endPos = FindHeaderEndPosition( aData );
	
	// panic if endPos < 0 in debug mode. leaves in release mode.
	__SIP_ASSERT_LEAVE(endPos >= 0, KErrGeneral);
	
	// get the msg header part from received data, 
	// and parse it to SIP message
	TPtrC8 headerDescriptor = aData.Left( endPos );
	CSIPMessageParser* messageParser = CSIPMessageParser::NewLC();
	HBufC8* headerBuf = headerDescriptor.AllocLC();
	CSIPMessage* message = iMsgAssembler.Message();
	TInt parseErr = messageParser->FromText( headerBuf, &message );
	CleanupStack::Pop( headerBuf );
	headerBuf = NULL;
	CleanupStack::PopAndDestroy(messageParser);
	messageParser = NULL;
	
#ifdef CPPUNIT_TEST
	if ( parseErr == KErrNoMemory ) 
	    {
	    delete message;
	    iMsgAssembler.DetachMsg();
		User::Leave( parseErr );
	    }
#endif

	// parsing error happens
	if ( parseErr )
		{
		CleanupStack::PushL( message );

		// pass err and message to msg owner
		iMsgAssembler.MsgOwner().MessageCompleteL( message, 
		                                           parseErr, 
		                                           iMsgAssembler.IsSigComp() );
		CleanupStack::Pop( message );
      	message = NULL;
		iMsgAssembler.DetachMsg();

		// reset the state to msg init.
		iMsgAssembler.ChangeState( MMsgAssemblerContext::EMsgInit );
		}
	else
		{	
		iMsgAssembler.SetMessage( message );
		message = NULL;
		
		// delete the header part from the received data,
		// since the header has been parsed to SIP message
		aData.Delete( 0, endPos );
			
		// check if the parsed SIP message header contains 
		// content length header and content length > 0
		if ( iMsgAssembler.Message() != 0 && 
		     iMsgAssembler.Message()->HasAnnouncedContentLength() && 
		     iMsgAssembler.Message()->AnnouncedContentLength() != 0 )
			{
			// enter to EMsgContentStart state			
			iMsgAssembler.ChangeState( 
							MMsgAssemblerContext::EMsgContentStart );
			iMsgAssembler.CurrentState().DataReceivedL( aData, aNextLength );
			}
		else  
			{
			// enter EMsgCompleteState	
			iMsgAssembler.ChangeState(MMsgAssemblerContext::EMsgComplete);
			iMsgAssembler.CurrentState().DataReceivedL( aData, aNextLength );
			}
		}
	}
