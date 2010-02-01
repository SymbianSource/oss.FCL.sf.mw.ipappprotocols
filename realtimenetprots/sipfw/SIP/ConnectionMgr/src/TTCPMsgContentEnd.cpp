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
// Name        : TTCPMsgContentEnd.cpp
// Part of     : ConnectionMgr
// Implementation
// Version     : SIP/4.0
//



#include "TTCPMsgContentEnd.h"
#include "MMsgAssemblerContext.h"
#include "sipmessage.h"
#include "SipAssert.h"


// ----------------------------------------------------------------------------
// TTCPMsgContentEnd::DataReceivedL
// ----------------------------------------------------------------------------
//
void TTCPMsgContentEnd::DataReceivedL( TPtr8 aData, TUint&  aNextLength )
	{
	CSIPMessage* message = iMsgAssembler.Message();
	
	// panic if message == 0 in debug mode. leaves in release mode.
	__SIP_ASSERT_LEAVE( message, KErrGeneral );
	
	// panic if there is no content length in debug mode. 
	// leaves in release mode.                
	__SIP_ASSERT_LEAVE( message->HasAnnouncedContentLength(), 
	                    KErrGeneral );	     
	                    
	TUint contentLen = message->AnnouncedContentLength();
	// panic if content length <= 0 in debug mode. 
	// leaves in release mode.                
	__SIP_ASSERT_LEAVE( contentLen > 0, KErrGeneral );
	
	// panic if content contentLen > aData.Length() in debug mode. 
	// leaves in release mode.                
	__SIP_ASSERT_LEAVE( contentLen <= static_cast< TUint >( aData.Length() ), 
	                    KErrGeneral );
		 
	HBufC8* content = aData.Left(contentLen).AllocL();
	message->SetContent( content );
	content = NULL;

	// delete the content part from received data after the content
	// has been set to SIP message header
	aData.Delete( 0, contentLen );
	
	// enter EMsgComplete state
	iMsgAssembler.ChangeState( MMsgAssemblerContext::EMsgComplete );
	iMsgAssembler.CurrentState().DataReceivedL(aData, aNextLength );
	}
