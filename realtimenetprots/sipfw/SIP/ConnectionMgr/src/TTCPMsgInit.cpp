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
// Name        : TTCPMsgInit.cpp
// Part of     : ConnectionMgr
// Implementation
// Version     : SIP/4.0
//



#include "TTCPMsgInit.h"
#include "MMsgAssemblerContext.h"
#include "MSigCompController.h"


// ----------------------------------------------------------------------------
// TTCPMsgInit::DataReceivedL
// ----------------------------------------------------------------------------
//
void TTCPMsgInit::DataReceivedL( TPtr8 aData, TUint&  aNextLength )
	{
	iMsgAssembler.SetUnConsumedBytes( 0 );
	
	// check first if the data received is compressed. 
	// if so, enter to state TTCPCompMsgStart state.
	if ( iMsgAssembler.SigComp().IsSupported() &&
	     iMsgAssembler.SigComp().IsSigCompMsg( aData ) )
		{
		iMsgAssembler.SetIsSigComp( ETrue );
		iMsgAssembler.ChangeState( MMsgAssemblerContext::ECompMsgStart );
		iMsgAssembler.CurrentState().DataReceivedL( aData , aNextLength );
		}
	else // received data is not compressed
		{
		iMsgAssembler.SetIsSigComp( EFalse );
		TInt startPos = FindHeaderStartPosition( aData );
		
		// if header not start from position 0
		// delete extra characters so that header start from pos 0.
		if ( startPos > 0 ) 
			{
			aData.Delete(0, startPos );
			}
			
		// if the header start position is found
		if ( startPos > KErrNotFound )
			{
			// enter into next state EMsgHeaderStart. 
			iMsgAssembler.ChangeState( MMsgAssemblerContext::EMsgHeaderStart );
			iMsgAssembler.CurrentState().DataReceivedL( aData , aNextLength );
			}
		}
	}
