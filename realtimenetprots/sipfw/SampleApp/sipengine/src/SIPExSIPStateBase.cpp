
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
// INCLUDE FILES
//



#include "SIPExSIPStateBase.h"
#include "SIPExSIPEngine.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPExSIPStateBase::CSIPExSIPStateBase
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CSIPExSIPStateBase::CSIPExSIPStateBase()
	{
	}

// -----------------------------------------------------------------------------
// CSIPExSIPStateBase::CSIPExSIPStateBase
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CSIPExSIPStateBase::~CSIPExSIPStateBase()
	{
	}

// -----------------------------------------------------------------------------
// CSIPExSIPStateBase::SendInviteL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPStateBase::SendInviteL( 
    CSIPExSIPEngine& /* aEngine */,
	const TDesC8& /* aSipUri */ )
	{
	//User::Leave( KErrTotalLossOfPrecision );
	}

// -----------------------------------------------------------------------------
// CSIPExSIPStateBase::CancelInviteL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPStateBase::CancelInviteL(
	CSIPExSIPEngine& /* aEngine */ )
	{
	//User::Leave( KErrTotalLossOfPrecision );
	}


// -----------------------------------------------------------------------------
// CSIPExSIPStateBase::AcceptInviteL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPStateBase::AcceptInviteL(
	CSIPExSIPEngine& /* aEngine */ )
	{
	//User::Leave( KErrTotalLossOfPrecision );
	}


// -----------------------------------------------------------------------------
// CSIPExSIPStateBase::DeclineInviteL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPStateBase::DeclineInviteL(
	CSIPExSIPEngine& /* aEngine */ )
	{
	//User::Leave( KErrTotalLossOfPrecision );
	}



// -----------------------------------------------------------------------------
// CSIPExSIPStateBase::EndSessionL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPStateBase::EndSessionL( CSIPExSIPEngine& /* aEngine */ )
	{
	//User::Leave( KErrTotalLossOfPrecision );
	}


// -----------------------------------------------------------------------------
// CSIPExSIPStateBase::ResponseReceivedL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPStateBase::ResponseReceivedL(
	CSIPExSIPEngine& /* aEngine */,
	CSIPClientTransaction& /* aTransaction */ )
	{
	//User::Leave( KErrTotalLossOfPrecision );
	}


// -----------------------------------------------------------------------------
// CSIPExSIPStateBase::AckReceivedL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPStateBase::AckReceivedL(
	CSIPExSIPEngine& /* aEngine */,
	CSIPServerTransaction& /* aTransaction */)
	{
	//User::Leave( KErrTotalLossOfPrecision );
	}


// -----------------------------------------------------------------------------
// CSIPExSIPStateBase::ByeReceivedL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPStateBase::ByeReceivedL(
	CSIPExSIPEngine& /* aEngine */,
	CSIPServerTransaction& /* aTransaction */ )
	{
	//User::Leave( KErrTotalLossOfPrecision );
	}


// -----------------------------------------------------------------------------
// CSIPExSIPStateBase::CancelReceivedL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPStateBase::CancelReceivedL(
	CSIPExSIPEngine& /* aEngine */,
	CSIPClientTransaction& /* aTransaction */ )
	{
	//User::Leave( KErrTotalLossOfPrecision );
	}


// -----------------------------------------------------------------------------
// CSIPExSIPStateBase::InviteReceivedL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSIPExSIPStateBase::InviteReceivedL(
	CSIPExSIPEngine& /* aEngine */,
	CSIPServerTransaction* /* aTransaction */ )
	{
	//User::Leave( KErrTotalLossOfPrecision );
	}


// -----------------------------------------------------------------------------
// CSIPExSIPStateBase::ErrorOccurred()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPExSIPStateBase::ErrorOccured(
	CSIPExSIPEngine& /*aContext*/,
	TInt /*aError*/)
	{
	}

