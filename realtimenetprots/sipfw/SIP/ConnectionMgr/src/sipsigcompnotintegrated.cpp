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
// Name          : sipsigcompnotintegrated.cpp
// Part of       : ConnectionMgr
// Version       : SIP/4.0 
//



#include "sipsigcompnotintegrated.h"
#include "sipmessage.h"
#include "sipresponse.h"
#include "siprequest.h"

// -----------------------------------------------------------------------------
// CSigCompNotIntegrated::NewL
// -----------------------------------------------------------------------------
//
CSigCompNotIntegrated* CSigCompNotIntegrated::NewL()
	{
	CSigCompNotIntegrated* self = NewLC();
	CleanupStack::Pop();
	return self;
	}

// -----------------------------------------------------------------------------
// CSigCompNotIntegrated::NewLC
// -----------------------------------------------------------------------------
//
CSigCompNotIntegrated* CSigCompNotIntegrated::NewLC()
	{
	CSigCompNotIntegrated* self = new (ELeave) CSigCompNotIntegrated();
	CleanupStack::PushL(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSigCompNotIntegrated::IsSupported 
// -----------------------------------------------------------------------------
//
TBool CSigCompNotIntegrated::IsSupported () const
	{
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSigCompNotIntegrated::CreateCompartmentL
// -----------------------------------------------------------------------------
//
TUint32 CSigCompNotIntegrated::CreateCompartmentL (TUint32 /*aIapId*/)
	{
	User::Leave(KErrNotSupported);
	return 0;
	}

// -----------------------------------------------------------------------------
// CSigCompNotIntegrated::RemoveCompartment
// -----------------------------------------------------------------------------
//
void CSigCompNotIntegrated::RemoveCompartment (TUint32 /*aCompartmentId*/)
	{
	}

// -----------------------------------------------------------------------------
// CSigCompNotIntegrated::EncodeL 
// -----------------------------------------------------------------------------
//
CBufBase* CSigCompNotIntegrated::EncodeL (
    const TSIPTransportParams& /*aTransportParams*/,
	CSIPRequest& /*aRequest*/,
    const TInetAddr& /*aAddress*/,
	TBool /*aStreambasedProtocol*/,
	TBool& /*aCompressed*/)
	{
	User::Leave(KErrNotSupported);
	return 0;
	}

// -----------------------------------------------------------------------------
// CSigCompNotIntegrated::EncodeL 
// -----------------------------------------------------------------------------
//
CBufBase* CSigCompNotIntegrated::EncodeL (    
    const TSIPTransportParams& /*aTransportParams*/,
	CSIPResponse& /*aResponse*/,
    const TInetAddr& /*aAddress*/,
	TBool /*aStreambasedProtocol*/,
	TBool& /*aCompressed*/)
	{
	User::Leave(KErrNotSupported);
	return 0;
	}

// -----------------------------------------------------------------------------
// CSigCompNotIntegrated::IsCompleteSigCompMessageL
// -----------------------------------------------------------------------------
//
TBool CSigCompNotIntegrated::IsCompleteSigCompMessageL (
										const TDesC8& /*aMessage*/) const 
	{
	User::Leave(KErrNotSupported);
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSigCompNotIntegrated::IsSigCompMsg
// -----------------------------------------------------------------------------
//
TBool CSigCompNotIntegrated::IsSigCompMsg (const TDesC8& /*aMessage*/) const
	{
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSigCompNotIntegrated::DecompressL 
// -----------------------------------------------------------------------------
//
CBufBase* CSigCompNotIntegrated::DecompressL (const TDesC8& /*aMessage*/,
                                TUint& /*aBytesConsumed*/,
                                TBool /*aIsStreamBased*/)
	{
	User::Leave(KErrNotSupported);
	return (CBufBase*)0;
	}

// -----------------------------------------------------------------------------
// CSigCompNotIntegrated::CompartmentCount
// -----------------------------------------------------------------------------
//
TInt CSigCompNotIntegrated::CompartmentCount ()
	{
	return 0;
	}

// -----------------------------------------------------------------------------
// CSigCompNotIntegrated::AllowL
// -----------------------------------------------------------------------------
//
void CSigCompNotIntegrated::AllowL(
    const TInetAddr& /*aAddr*/, 
    TUint32 /*aIapId*/)
	{
	User::Leave(KErrNotSupported);	
	}

// -----------------------------------------------------------------------------
// CSigCompNotIntegrated::Deny
// -----------------------------------------------------------------------------
//
void CSigCompNotIntegrated::Deny()
	{
	}

// -----------------------------------------------------------------------------
// CSigCompNotIntegrated::SendFailedL
// -----------------------------------------------------------------------------
//
void CSigCompNotIntegrated::SendFailedL (TUint32 /*aCompartmentId*/)
	{
	}
