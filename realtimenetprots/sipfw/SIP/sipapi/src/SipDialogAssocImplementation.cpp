// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : SipDialogAssocImplementation.cpp
// Part of       : SIPAPI
// Version       : SIP/4.0
//



#include "siperr.h"
#include "sipstrings.h"
#include "sipdialogassocbase.h"
#include "SipDialogAssocImplementation.h"
#include "sipclienttransaction.h"
#include "sipservertransaction.h"
#include "SipDialogImplementation.h"
#include "SipConnectionImplementation.h"
#include "csipdialogresponsesender.h"
#include "sipmessageelements.h"
#include "sipheaderbase.h"
#include "MessageHeaderCleanup.h"
#include "SipAssert.h"

#ifdef CPPUNIT_TEST
#include "TestCleanupStack.h"
#endif


// -----------------------------------------------------------------------------
// CSIPDialogAssocImplementation::NewL
// -----------------------------------------------------------------------------
//
CSIPDialogAssocImplementation*
CSIPDialogAssocImplementation::NewL(RStringF aType,
									CSIPDialogAssocBase& aAssoc,
									CSIPDialog& aDialog)
	{
	CSIPDialogAssocImplementation* self =
    	new (ELeave) CSIPDialogAssocImplementation(aAssoc, aDialog);
    CleanupStack::PushL(self);
    self->ConstructL(aType);
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPDialogAssocImplementation::NewL
// -----------------------------------------------------------------------------
//
CSIPDialogAssocImplementation*
CSIPDialogAssocImplementation::NewL(RStringF aType,
									CSIPDialogAssocBase& aAssoc,
									CSIPDialog& aDialog,
									CSIPServerTransaction& aTransaction)
	{
	CSIPDialogAssocImplementation* self =
    	new (ELeave) CSIPDialogAssocImplementation(aAssoc, aDialog);
    CleanupStack::PushL(self);
    self->ConstructL(aType, aTransaction);
    CleanupStack::Pop(self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPDialogAssocImplementation::CSIPDialogAssocImplementation
// -----------------------------------------------------------------------------
//				 
CSIPDialogAssocImplementation::CSIPDialogAssocImplementation(
												CSIPDialogAssocBase& aAssoc,
												CSIPDialog& aDialog) :    
    iAssoc(aAssoc),
    iDialog(aDialog)
#ifdef CPPUNIT_TEST
    , iTransactions(1)
#endif
    , iStringPoolOpened(EFalse)
    {
    }
	
// -----------------------------------------------------------------------------
// CSIPDialogAssocImplementation::ConstructL
// -----------------------------------------------------------------------------
//	
void CSIPDialogAssocImplementation::ConstructL(RStringF aType)
    {
    SIPStrings::OpenL();
    iStringPoolOpened = ETrue;
    iType = aType.Copy();
    iDialog.Implementation().AddAssocL(iAssoc, iType);
    }

// -----------------------------------------------------------------------------
// CSIPDialogAssocImplementation::ConstructL
// A request (aTransaction) has been received from network and application has
// created a dialog assoc for it.
// aTransaction is currently associated with CSIPConnection, and should
// now be associated with this dialog assoc.
// -----------------------------------------------------------------------------
//
void
CSIPDialogAssocImplementation::ConstructL(RStringF aType,
										  CSIPServerTransaction& aTransaction)
    {
    CSIPTransactionBase::TState state = aTransaction.StateL();
    if (state != CSIPTransactionBase::ETrying &&
    	state != CSIPTransactionBase::EProceeding)
        {
        User::Leave(KErrSIPInvalidTransactionState);
        }

    //Set dialog related sender instance for the aTransaction to use
    aTransaction.SetResponseSender(
    	CSIPDialogResponseSender::NewL(iDialog.Implementation()));

    aTransaction.ReAssociateL(*this);

    //Attach CSIPDialogAssocImplementation to dialog after everything else
    ConstructL(aType);
    }

// -----------------------------------------------------------------------------
// CSIPDialogAssocImplementation::~CSIPDialogAssocImplementation
// -----------------------------------------------------------------------------
//	
CSIPDialogAssocImplementation::~CSIPDialogAssocImplementation()
    {    
    iDialog.Implementation().RemoveAssoc(iAssoc);

    //Transactions are not deleted, since application owns them.
    for (TInt i = 0; i < iTransactions.Count(); i++)
        {
        iTransactions[i]->Detach(*this);
        }
    iTransactions.Reset();

    iType.Close();
    if (iStringPoolOpened)
		{
    	SIPStrings::Close();
		}
    }

// -----------------------------------------------------------------------------
// CSIPDialogAssocImplementation::Dialog
// -----------------------------------------------------------------------------
//	
const CSIPDialog& CSIPDialogAssocImplementation::Dialog() const
    {
    return iDialog;
    }

// -----------------------------------------------------------------------------
// CSIPDialogAssocImplementation::Dialog
// -----------------------------------------------------------------------------
//	
CSIPDialog& CSIPDialogAssocImplementation::Dialog()
    {
    return iDialog;
    }

// -----------------------------------------------------------------------------
// CSIPDialogAssocImplementation::SendNonTargetRefreshRequestL
// -----------------------------------------------------------------------------
//	
CSIPClientTransaction*
CSIPDialogAssocImplementation::SendNonTargetRefreshRequestL(RStringF aMethod,
                                        CSIPMessageElements* aElements)
    {
    return iDialog.Implementation().SendNonTargetRefreshRequestL(*this,
    															 aMethod,
    															 aElements);
    }

// -----------------------------------------------------------------------------
// CSIPDialogAssocImplementation::Type
// -----------------------------------------------------------------------------
//	
RStringF CSIPDialogAssocImplementation::Type() const
    {
    return iType;
    }

// -----------------------------------------------------------------------------
// CSIPDialogAssocImplementation::ClientConnectionL
// -----------------------------------------------------------------------------
//
CSIPClientConnection& CSIPDialogAssocImplementation::ClientConnectionL()
    {
    CSIPClientConnection* clientConn = ClientConnection();
    if (!clientConn)
        {
        User::Leave(KErrSIPResourceNotAvailable);
        }
    return *clientConn;    
    }

// -----------------------------------------------------------------------------
// CSIPDialogAssocImplementation::ClientConnection
// -----------------------------------------------------------------------------
//
CSIPClientConnection* CSIPDialogAssocImplementation::ClientConnection()
	{
	CSIPConnection* conn = iDialog.Connection();

	if (conn)
		{
		return conn->Implementation().ClientConnection();
		}
	return NULL;
	}

// -----------------------------------------------------------------------------
// CSIPDialogAssocImplementation::SIPConnectionL
// -----------------------------------------------------------------------------
//	
CSIPConnection& CSIPDialogAssocImplementation::SIPConnectionL()
    {    
    CSIPConnection* conn = iDialog.Connection();
    if (!conn)
        {
        User::Leave(KErrSIPResourceNotAvailable);
        }

    return *conn;
    }

// -----------------------------------------------------------------------------
// CSIPDialogAssocImplementation::AddTransactionL
// -----------------------------------------------------------------------------
//	
void CSIPDialogAssocImplementation::AddTransactionL(
										CSIPTransactionBase& aTransaction)
    {    
    iTransactions.AppendL(&aTransaction);
    }

// -----------------------------------------------------------------------------
// CSIPDialogAssocImplementation::RemoveTransaction
// -----------------------------------------------------------------------------
//	
void CSIPDialogAssocImplementation::RemoveTransaction(
									const CSIPTransactionBase& aTransaction)
    {
    TInt pos = iTransactions.Find(&aTransaction);

    if (pos != KErrNotFound)
        {
        iTransactions.Remove(pos);
        }
    }

// -----------------------------------------------------------------------------
// CSIPDialogAssocImplementation::UpdateRefreshL
// Application is not allowed to update or terminate a dialog association
// related refresh.
// -----------------------------------------------------------------------------
//
CSIPClientTransaction* CSIPDialogAssocImplementation::UpdateRefreshL(
											CSIPRefresh& /*aRefresh*/,
                                         	CSIPMessageElements* /*aElements*/,
                                         	TBool /*aTerminate*/)
    {
    User::Leave(KErrSIPInvalidDialogState);
    return NULL;
    }

// -----------------------------------------------------------------------------
// CSIPDialogAssocImplementation::DeletingRefresh
// -----------------------------------------------------------------------------
//
void CSIPDialogAssocImplementation::DeletingRefresh(CSIPRefresh& aRefresh,
                                               	    TUint32 aRefreshId)
    {
    iAssoc.DeletingRefresh(aRefresh, aRefreshId);    
    }

// -----------------------------------------------------------------------------
// CSIPDialogAssocImplementation::TransactionAssociation
// -----------------------------------------------------------------------------
//
MTransactionAssociation& CSIPDialogAssocImplementation::TransactionAssociation()
	{
	return *this;
	}

// -----------------------------------------------------------------------------
// CSIPDialogAssocImplementation::CheckIfStandAlone
// -----------------------------------------------------------------------------
//
TInt CSIPDialogAssocImplementation::CheckIfStandAlone()
	{	
	return KErrSIPInvalidDialogState;
	}

// -----------------------------------------------------------------------------
// CSIPDialogAssocImplementation::FindTransaction
// -----------------------------------------------------------------------------
//	
CSIPTransactionBase*
CSIPDialogAssocImplementation::FindTransaction(TUint32 aRequestId)
    {    
    for (TInt i = 0; i < iTransactions.Count(); i++)
        {
        if (iTransactions[i]->RequestId() == aRequestId)
            {            
            return iTransactions[i];
            }   
        }

    return NULL;
    }

// -----------------------------------------------------------------------------
// CSIPDialogAssocImplementation::HasTransaction
// -----------------------------------------------------------------------------
//	
TBool
CSIPDialogAssocImplementation::HasTransaction(RStringF aType,
                                    TBool aIsClientTransaction)
    {
    for (TInt i = 0; i < iTransactions.Count(); i++)
        {
        if (iTransactions[i]->Type() == aType &&
            iTransactions[i]->IsSIPClientTransaction() == aIsClientTransaction)
            {
            return ETrue;
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSIPDialogAssocImplementation::ConnectionLost
// -----------------------------------------------------------------------------
//
void CSIPDialogAssocImplementation::ConnectionLost()
    {
    TInt i = 0;
    for (i = 0; i < iTransactions.Count(); i++)
        {        
        iTransactions[i]->ChangeState(CSIPTransactionBase::ETerminated);
        }
    }

// -----------------------------------------------------------------------------
// CSIPDialogAssocImplementation::CopyHeaderL
// -----------------------------------------------------------------------------
//
void
CSIPDialogAssocImplementation::CopyHeaderL(const CSIPHeaderBase& aHeader,
								 CSIPMessageElements& aElements,
								 TMessageHeaderCleanup& aHeaderCleanup)
	{
	CSIPHeaderBase* newHeader = aHeader.CloneL();
	CleanupStack::PushL(newHeader);
	aElements.AddHeaderL(newHeader);
	CleanupStack::Pop(newHeader);
	aHeaderCleanup.AddHeader(*newHeader);
	}
