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
// Name        : TTCPMsgComplete.cpp
// Part of     : ConnectionMgr
// Implementation
// Version     : SIP/4.0
//



#include "TTCPMsgComplete.h"
#include "MMsgAssemblerContext.h"
#include "MSIPMsgAssemblerOwner.h"
#include "SipAssert.h"

// ----------------------------------------------------------------------------
// TTCPMsgComplete::DataReceivedL
// ----------------------------------------------------------------------------
//
void TTCPMsgComplete::DataReceivedL( TPtr8 aData, TUint&  aNextLength )
	{
	// panic if message == 0 in debug mode. leaves in release mode.
	__SIP_ASSERT_LEAVE(iMsgAssembler.Message() != 0, KErrGeneral);
	
	iMsgAssembler.MsgOwner().MessageCompleteL ( iMsgAssembler.Message(), 
											    KErrNone, 
											    iMsgAssembler.IsSigComp() );
	// ask msgAssembler to detach the message
	iMsgAssembler.DetachMsg();
	
	// reset the state to msg init
	iMsgAssembler.ChangeState(MMsgAssemblerContext::EMsgInit);
	
	if ( aData.Length() > 0)
		{
		// Enter to EMsgInit state, try to find next message header
		iMsgAssembler.CurrentState().DataReceivedL(aData, aNextLength );
		}
	}
