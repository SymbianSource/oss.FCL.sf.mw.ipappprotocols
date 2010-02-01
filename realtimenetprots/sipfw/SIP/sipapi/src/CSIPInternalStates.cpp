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
// Name          : CSIPInternalStates.cpp
// Part of       : SIPClient
// Version       : SIP/4.0
//



#include "sipinternalstates.h"
#include "sipclientserver.h"
#include "RSIP.h"
#include "siptransactionbase.h"
#include "SipDialogImplementation.h"


#ifdef CPPUNIT_TEST
#undef EXPORT_C
#define EXPORT_C
#endif

// -----------------------------------------------------------------------------
// CSIPInternalStates::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPInternalStates* CSIPInternalStates::NewL ()
	{
    CSIPInternalStates* self = CSIPInternalStates::NewLC();
    CleanupStack::Pop (self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPInternalStates::NewLC
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPInternalStates* CSIPInternalStates::NewLC ()
	{
	CSIPInternalStates* self = new(ELeave)CSIPInternalStates;
    CleanupStack::PushL (self);
    self->ConstructL ();
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPInternalStates::CSIPInternalStates
// -----------------------------------------------------------------------------
//
CSIPInternalStates::CSIPInternalStates ()
	{
	}

// -----------------------------------------------------------------------------
// CSIPInternalStates::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPInternalStates::ConstructL ()
	{
	iSip = new(ELeave)RSIP;
	User::LeaveIfError(iSip->Connect());
	}

// -----------------------------------------------------------------------------
// CSIPInternalStates::~CSIPInternalStates
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPInternalStates::~CSIPInternalStates()
	{
	if (iSip)
        {
        iSip->Close();
        }
	delete iSip;
	}

// -----------------------------------------------------------------------------
// CSIPInternalStates::GetTransactionState
// -----------------------------------------------------------------------------
//
EXPORT_C TInt 
CSIPInternalStates::GetTransactionState(const CSIPTransactionBase& aTransaction,
                                        TState& aState)
    {
    TSIPIds ids;
    ids.iRequestId = aTransaction.RequestId();
    return GetState (*iSip,ids,aState,ESipItcGetTransactionState);
    }

// -----------------------------------------------------------------------------
// CSIPInternalStates::GetDialogState
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPInternalStates::GetDialogState (const CSIPDialog& aDialog,
                                                  TState& aState)
    {
    TSIPIds ids;    
	ids.iDialogId =
		const_cast<CSIPDialog&>(aDialog).Implementation().DialogId();
    return GetState (*iSip,ids,aState,ESipItcGetDialogState);
    }

// -----------------------------------------------------------------------------
// CSIPInternalStates::SigCompCompartmentCount
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPInternalStates::SigCompCompartmentCount ()
    {    
    TPckgBuf<TInt> compartmentCountPckg;
    TIpcArgs args;
    args.Set (ESipItcArgInternalState, &compartmentCountPckg);
    TInt err = iSip->Send(ESipItcGetCompartmentCount,args);
    if (err == KErrNone)
    	{    	
    	return compartmentCountPckg();
    	}
    return err;    
    }

// -----------------------------------------------------------------------------
// CSIPInternalStates::GetState
// -----------------------------------------------------------------------------
//
TInt CSIPInternalStates::GetState (RSIP& aSip,
								   TSIPIds& aIds,
                                   TState& aState,
                                   TInt aFunction)
    {
    TPckgBuf<TSIPIds> sipIdsPckg(aIds);
  
    TIpcArgs args;
    args.Set (ESipItcArgIds, &sipIdsPckg);
    
    TPckgBuf<TInt> statePckg;
    args.Set (ESipItcArgInternalState, &statePckg);

    TInt err = aSip.Send(static_cast<TSipItcFunctions>(aFunction),args);
    if (err == KErrNone)
        {
        aState = static_cast<TState>(statePckg());
        }
    return err;
    }
