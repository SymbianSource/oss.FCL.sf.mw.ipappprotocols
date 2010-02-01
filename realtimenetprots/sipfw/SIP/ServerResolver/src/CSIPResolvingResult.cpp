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
// Name          : CSIPResolvingResult.cpp
// Part of       : ServerResolver
// Version       : SIP/4.0 
//



#include "sipstrings.h"
#include "sipstrconsts.h"
#include "CSIPResolvingResult.h"

// ----------------------------------------------------------------------------
// CSIPResolvingResult::CSIPResolvingResult
// ----------------------------------------------------------------------------
//
CSIPResolvingResult::CSIPResolvingResult( TInetAddr aAddr,
										  RStringF aProtocol,
										  TBool aForceUDP ) 
 : iProtocol( aProtocol ),
   iAddr( aAddr ),
   iFailed( EFalse ),
   iForceUDP( aForceUDP )
	{
    }

// ----------------------------------------------------------------------------
// CSIPResolvingResult::NewLC
// ----------------------------------------------------------------------------
//
CSIPResolvingResult* CSIPResolvingResult::NewLC( TInetAddr aAddr,
												 RStringF aProtocol,
												 TBool aForceUDP )
	{
	if ( aForceUDP && aProtocol != SIPStrings::StringF( SipStrConsts::EUDP ) )
		{
		User::Leave( KErrArgument );
		}	
	CSIPResolvingResult* self = new ( ELeave ) CSIPResolvingResult( aAddr,
																    aProtocol,
																    aForceUDP );
	CleanupStack::PushL( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPResolvingResult::~CSIPResolvingResult
// ----------------------------------------------------------------------------
//
CSIPResolvingResult::~CSIPResolvingResult()
	{
	iProtocol.Close();
    }

// ----------------------------------------------------------------------------
// CSIPResolvingResult::Address
// ----------------------------------------------------------------------------
//
TInetAddr& CSIPResolvingResult::Address()
	{
	return iAddr;
	}

// ----------------------------------------------------------------------------
// CSIPResolvingResult::Protocol
// ----------------------------------------------------------------------------
//
RStringF CSIPResolvingResult::Protocol()
	{
	return iProtocol;
	}

// ----------------------------------------------------------------------------
// CSIPResolvingResult::SetProtocol
// ----------------------------------------------------------------------------
//
TInt CSIPResolvingResult::SetProtocol( RStringF aProtocol )
	{
	if ( ForceUDP() && aProtocol != SIPStrings::StringF( SipStrConsts::EUDP ) )
		{
		return KErrArgument;
		}

	iProtocol.Close();
	iProtocol = aProtocol.Copy();
	return KErrNone;
	}

// ----------------------------------------------------------------------------
// CSIPResolvingResult::SetFailed
// ----------------------------------------------------------------------------
//
void CSIPResolvingResult::SetFailed( TBool aFailed )
	{
	iFailed = aFailed;
	}

// ----------------------------------------------------------------------------
// CSIPResolvingResult::Failed
// ----------------------------------------------------------------------------
//
TBool CSIPResolvingResult::Failed()
	{
	return iFailed;
	}

// ----------------------------------------------------------------------------
// CSIPResolvingResult::ForceUDP
// ----------------------------------------------------------------------------
//
TBool CSIPResolvingResult::ForceUDP()
	{
	return iForceUDP;
	}
