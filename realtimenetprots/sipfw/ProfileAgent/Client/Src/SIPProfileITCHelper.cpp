// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : sipprofileitchelper
// Part of     : SIP Profile Client
// implementation
// Version     : 1.0
// INCLUDE FILES
//



#include "SIPProfileITCHelper.h"
#include "SIPRemoteProfile.h"
#include "sipconcreteprofile.h"
#include "sipconcreteprofileholder.h"
#include "sipprofileslots.h"
#include "sipprofileplugins.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPProfileITCHelper::CSIPProfileITCHelper
// -----------------------------------------------------------------------------
//
CSIPProfileITCHelper::CSIPProfileITCHelper(RSIPProfile& aSipProfile) 
 : iSip (aSipProfile)
	{
	}

// -----------------------------------------------------------------------------
// CSIPProfileITCHelper::SendL
// -----------------------------------------------------------------------------
//
void CSIPProfileITCHelper::SendL(TIpcArgs& aArgs, TSipProfileItcFunctions aFunction)
	{
	User::LeaveIfError(iSip.Send(aArgs, aFunction));
	}

// -----------------------------------------------------------------------------
// CSIPProfileITCHelper::SendL
// -----------------------------------------------------------------------------
//
void CSIPProfileITCHelper::SendL(
	TSIPProfileSlots& aIds, TSipProfileItcFunctions aITCFunction)
	{
	User::LeaveIfError(Send(aIds,aITCFunction));
	}

// -----------------------------------------------------------------------------
// CSIPProfileITCHelper::SendL
// -----------------------------------------------------------------------------
//
void CSIPProfileITCHelper::SendL(TSIPProfileSlots& aIds, 
								  TSipProfileItcFunctions aITCFunction, 
								  const TDesC8& aNarrator)
	{
	HBufC8* narrator = aNarrator.AllocLC();
	TPtr8 narratorPtr = narrator->Des();
	iITCMsgArgs.Set(ESipProfileItcArgNarrator, &narratorPtr);
    
	User::LeaveIfError(Send(aIds,aITCFunction));
	CleanupStack::PopAndDestroy(narrator);
	}

// -----------------------------------------------------------------------------
// CSIPProfileITCHelper::SendL
// -----------------------------------------------------------------------------
//
void CSIPProfileITCHelper::SendL(TSipProfileItcFunctions aITCFunction,
						   const CSIPConcreteProfile& aProfile)
	{
    CBufFlat* buf = ExternalizeLC (aProfile);
    TPtr8 externalized = buf->Ptr(0);
    iITCMsgArgs.Set (ESipProfileItcArgProfile, &externalized);
	User::LeaveIfError(iSip.Send(iITCMsgArgs,aITCFunction));

    CleanupStack::PopAndDestroy(buf);
	}

// -----------------------------------------------------------------------------
// CSIPProfileITCHelper::SendL
// -----------------------------------------------------------------------------
//
void CSIPProfileITCHelper::SendL(TSIPProfileSlots& aIds,
						   TSipProfileItcFunctions aITCFunction,
						   const CSIPConcreteProfile& aProfile)
	{
    CBufFlat* buf = ExternalizeLC (aProfile);
    TPtr8 externalized = buf->Ptr(0);
    iITCMsgArgs.Set(ESipProfileItcArgProfile, &externalized);

    SendL (aIds,aITCFunction);
    CleanupStack::PopAndDestroy(buf);
	}

// -----------------------------------------------------------------------------
// CSIPProfileITCHelper::Send
// -----------------------------------------------------------------------------
//
TInt CSIPProfileITCHelper::Send(
	TSIPProfileSlots& aIds, TSipProfileItcFunctions aITCFunction)
	{
    TPckgBuf<TSIPProfileSlots> sipIdsPckg(aIds);
	iITCMsgArgs.Set(ESipProfileItcArgSlots, &sipIdsPckg);

    TInt err = iSip.Send(iITCMsgArgs,aITCFunction);
	aIds = sipIdsPckg();
	return err;
	}

// -----------------------------------------------------------------------------
// CSIPProfileITCHelper::ExternalizeLC
// -----------------------------------------------------------------------------
//
template<class T> CBufFlat* CSIPProfileITCHelper::ExternalizeLC(
	const T& aElements)
	{
	CBufFlat* buf = CBufFlat::NewL(100);
	CleanupStack::PushL(buf);
	RBufWriteStream writeStream(*buf,0);
	writeStream.PushL();
	aElements.ExternalizeL(writeStream);
	CleanupStack::Pop(1); // writeStream
	writeStream.Close();
    return buf;
	}
