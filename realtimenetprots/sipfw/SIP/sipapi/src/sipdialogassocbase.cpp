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
// Name          : SIPDialogAssocBase.cpp
// Part of       : SIPAPI
// Version       : SIP/4.0 
//



#include "sipdialogassocbase.h"
#include "SipDialogAssocImplementation.h"
#include "sipclienttransaction.h"
#include "sipservertransaction.h"
#include "sipmessageelements.h"
#include "sipstrings.h"
#include "sipstrconsts.h"

#ifdef CPPUNIT_TEST
#undef EXPORT_C
#define EXPORT_C
#endif


// -----------------------------------------------------------------------------
// CSIPDialogAssocBase::CSIPDialogAssocBase
// -----------------------------------------------------------------------------
//	
CSIPDialogAssocBase::CSIPDialogAssocBase()
    {
    }

// -----------------------------------------------------------------------------
// CSIPDialogAssocBase::ConstructL
// -----------------------------------------------------------------------------
//	
void CSIPDialogAssocBase::ConstructL(RStringF aType, CSIPDialog& aDialog)
    {
    iImplementation = CSIPDialogAssocImplementation::NewL(aType,
    													  *this,
    													  aDialog);
    }
	
// -----------------------------------------------------------------------------
// CSIPDialogAssocBase::ConstructL
// A request (aTransaction) has been received from network and application has
// created a dialog assoc for it.
// aTransaction is currently associated with CSIPConnection, and should
// now be associated with this dialog assoc.
// -----------------------------------------------------------------------------
//	
void CSIPDialogAssocBase::ConstructL(RStringF aType,
									 CSIPDialog& aDialog,
									 CSIPServerTransaction& aTransaction)
    {
    iImplementation = CSIPDialogAssocImplementation::NewL(aType,
    													  *this,
    													  aDialog,
    													  aTransaction);    
    }

// -----------------------------------------------------------------------------
// CSIPDialogAssocBase::~CSIPDialogAssocBase
// -----------------------------------------------------------------------------
//	
CSIPDialogAssocBase::~CSIPDialogAssocBase()
    {    
    delete iImplementation;
    }

// -----------------------------------------------------------------------------
// CSIPDialogAssocBase::Dialog
// -----------------------------------------------------------------------------
//	
EXPORT_C const CSIPDialog& CSIPDialogAssocBase::Dialog() const
    {
    __TEST_INVARIANT;
    return iImplementation->Dialog();
    }

// -----------------------------------------------------------------------------
// CSIPDialogAssocBase::Dialog
// -----------------------------------------------------------------------------
//	
EXPORT_C CSIPDialog& CSIPDialogAssocBase::Dialog()
    {
    __TEST_INVARIANT;
    return iImplementation->Dialog();
    }

// -----------------------------------------------------------------------------
// CSIPDialogAssocBase::SendNonTargetRefreshRequestL
// -----------------------------------------------------------------------------
//	
EXPORT_C CSIPClientTransaction*
CSIPDialogAssocBase::SendNonTargetRefreshRequestL(RStringF aMethod,
                                        	CSIPMessageElements* aElements)
    {
    __TEST_INVARIANT;
    __ASSERT_ALWAYS(aMethod != SIPStrings::StringF(SipStrConsts::ERegister) &&
    				aMethod != SIPStrings::StringF(SipStrConsts::EInvite) &&
    				aMethod != SIPStrings::StringF(SipStrConsts::EBye) &&
    				aMethod != SIPStrings::StringF(SipStrConsts::ECancel) &&
    				aMethod != SIPStrings::StringF(SipStrConsts::ESubscribe) &&
    				aMethod != SIPStrings::StringF(SipStrConsts::ENotify) &&
    				aMethod != SIPStrings::StringF(SipStrConsts::EUpdate) &&
    				aMethod != SIPStrings::StringF(SipStrConsts::EPrack) &&
    				aMethod != SIPStrings::StringF(SipStrConsts::ERefer),
                    User::Leave(KErrArgument));
    return iImplementation->SendNonTargetRefreshRequestL(aMethod, aElements);
    }

// -----------------------------------------------------------------------------
// CSIPDialogAssocBase::Type
// -----------------------------------------------------------------------------
//	
EXPORT_C RStringF CSIPDialogAssocBase::Type() const
    {
    __TEST_INVARIANT;
    return iImplementation->Type();
    }

// -----------------------------------------------------------------------------
// CSIPDialogAssocBase::IsNonTargetRefreshRequest
// -----------------------------------------------------------------------------
//
EXPORT_C TBool
CSIPDialogAssocBase::IsNonTargetRefreshRequest(RStringF aMethod) const
	{
	__TEST_INVARIANT;
	return !CSIPTransactionBase::IsTargetRefresh(aMethod);
	}

// -----------------------------------------------------------------------------
// CSIPDialogAssocBase::Implementation
// -----------------------------------------------------------------------------
//
CSIPDialogAssocImplementation& CSIPDialogAssocBase::Implementation()
	{
	__TEST_INVARIANT;
	return *iImplementation;
	}

// -----------------------------------------------------------------------------
// CSIPDialogAssocBase::FindRefresh
// Those dialog associations that can have refresh, overload this function
// -----------------------------------------------------------------------------
//
CSIPRefresh* CSIPDialogAssocBase::FindRefresh(TUint32 /*aRefreshId*/)
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CSIPDialogAssocBase::DeletingRefresh
// Those dialog associations that can have refresh, overload this function
// -----------------------------------------------------------------------------
//
void CSIPDialogAssocBase::DeletingRefresh(CSIPRefresh& /*aRefresh*/,
										  TUint32 /*aRefreshId*/)
	{	
	}

// -----------------------------------------------------------------------------
// CSIPDialogAssocBase::__DbgTestInvariant
// -----------------------------------------------------------------------------
//

void CSIPDialogAssocBase::__DbgTestInvariant() const
	{
	if (!iImplementation)
		{		
		User::Invariant();
		}
	}
