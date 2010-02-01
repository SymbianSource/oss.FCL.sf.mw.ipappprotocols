// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : SIPImplementation.cpp
// Part of       : SIP API
// Version       : SIP/6.0 
//



#include "SipImplementation.h"
#include "SipAssert.h"
#include "sipclient.h"
#include "sipobserver.h"
#include "SipConnectionImplementation.h"
#include "siphttpdigest.h"
#include "unregistered.h"
#include "registering.h"
#include "registered.h"
#include "unregistering.h"
#include "sipdialogtrying.h"
#include "sipdialogearly.h"
#include "sipdialogconfirmed.h"
#include "sipdialogterminated.h"
#include "sipinviteclienttransaction.h"
#include "sipclienttransaction.h"
#include "sipservertransaction.h"
#include "PendingTransaction.h"
#include "siprequestelements.h"
#include "sipresponseelements.h"
#include "sipmessageelements.h"
#include "siphttpdigestchallengeobserver2.h"
#include "sipcseqheader.h"
#include "sipauthheaderbase.h"
#include "sipstrings.h"
#include "sipstrconsts.h"


#ifdef CPPUNIT_TEST

#include "TestCleanupStack.h"

#endif


// -----------------------------------------------------------------------------
// CSIPImplementation::NewL
// -----------------------------------------------------------------------------
//
CSIPImplementation*
CSIPImplementation::NewL(const TUid& aUid, MSIPObserver& aObserver)
	{
	CSIPImplementation* self = new (ELeave) CSIPImplementation(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aUid);
    CleanupStack::Pop(self);
    return self;    
	}

// -----------------------------------------------------------------------------
// CSIPImplementation::CSIPImplementation
// -----------------------------------------------------------------------------
//
CSIPImplementation::CSIPImplementation(MSIPObserver& aObserver) :
    iSecurityHandlingEnabled(ETrue),
	iObserver(aObserver)
#ifdef CPPUNIT_TEST
    //For unit tests the granularity of arrays is set to 1 to cause them to
    //allocate memory every time an item is appended to array
    , iConnections(1),
    iPendingServerTransactions(1),
    iDigests(1)
#endif
	{
	}

// -----------------------------------------------------------------------------
// CSIPImplementation::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPImplementation::ConstructL(const TUid& aUid)
	{    
    iSIPClient = CSIPClient::NewL(aUid, *this);

    //Registration state machine
    iUnregistered = CUnregistered::NewL();
    iRegistering = CRegistering::NewL();
    iRegistered = CRegistered::NewL();
    iUnregistering = CUnregistering::NewL();
    
    iUnregistered->SetNeighbourStates(*iRegistering);
    iRegistering->SetNeighbourStates(*iRegistered, *iUnregistered);
    iRegistered->SetNeighbourStates(*iUnregistering, *iUnregistered);
    iUnregistering->SetNeighbourStates(*iRegistered, *iUnregistered);


    //Dialog state machine
    iDialogTrying = CDialogTrying::NewL();
    iDialogEarly = CDialogEarly::NewL();
    iDialogConfirmed = CDialogConfirmed::NewL();
    iDialogTerminated = CDialogTerminated::NewL();

    iDialogTrying->SetNeighbourStates(*iDialogEarly,
                                      *iDialogConfirmed,
                                      *iDialogTerminated);    
    iDialogEarly->SetNeighbourStates(*iDialogConfirmed,
                                     *iDialogTerminated);
    iDialogConfirmed->SetNeighbourStates(*iDialogTerminated);
	}

// -----------------------------------------------------------------------------
// CSIPImplementation::~CSIPImplementation
// -----------------------------------------------------------------------------
//
CSIPImplementation::~CSIPImplementation()
	{
	delete iSIPClient;

	TInt i = 0;
	for (i = 0; i < iPendingServerTransactions.Count(); i++)
        {
        iPendingServerTransactions[i]->Transaction()->Detach(*this);
        }
    iPendingServerTransactions.ResetAndDestroy();

    //Free the array, not the connection objects
    for (i = 0; i < iConnections.Count(); i++)
        {
        iConnections[i]->Implementation().CSIPDeleted();
        }
    iConnections.Reset();

    for (i = 0; i < iDigests.Count(); i++)
        {
        iDigests[i]->CSIPDeleted();
        }
    iDigests.Reset();

    delete iUnregistered;
    delete iRegistering;
    delete iRegistered;
    delete iUnregistering;

    delete iDialogTrying;
    delete iDialogEarly;
    delete iDialogConfirmed;
    delete iDialogTerminated;    
	}

// -----------------------------------------------------------------------------
// CSIPImplementation::IncomingRequestL
// Don't use __TEST_INVARIANT after using observer as application may've deleted
// the CSIP within callback.
// -----------------------------------------------------------------------------
//
void CSIPImplementation::IncomingRequestL(TUint32 aIapId,
                            			  CSIPRequestElements* aElements,
                            			  TUint32 aRequestId)
    {
    __TEST_INVARIANT;
    __SIP_ASSERT_LEAVE(aElements, KErrArgument);

    CSIPConnection* connection = Connection(aIapId);
    if (connection)
        {
        connection->Implementation().IncomingRequestL(aElements, aRequestId);        
        }
    else
        {
        CSIPServerTransaction* ta =
        	CSIPServerTransaction::NewLC(aRequestId, *this, aElements);
    	CleanupStack::PushL(
        	TCleanupItem(CSIPServerTransaction::DetachRequestElements, ta));
        CPendingTransaction* pendingTa = CPendingTransaction::NewLC(ta, aIapId);

        iPendingServerTransactions.AppendL(pendingTa);
        CleanupStack::Pop(3); //pendingTa, TCleanupItem, ta

        iObserver.IncomingRequest(aIapId, ta);
        }
    }

// -----------------------------------------------------------------------------
// CSIPImplementation::TimeOut
// Do everything before calling iObserver.TimedOut() as application may delete
// CSIP withing the callback.
// -----------------------------------------------------------------------------
//
void CSIPImplementation::TimeOut(TUint32 aRequestId)
    {
    __TEST_INVARIANT;

    CPendingTransaction* ta = NULL;
    for (TInt i = 0; i < iPendingServerTransactions.Count(); i++)
        {
        ta = iPendingServerTransactions[i];
        __SIP_ASSERT_RETURN(ta && ta->Transaction(), KErrNotFound);

        if (ta->Transaction()->RequestId() == aRequestId)
            {
			CSIPServerTransaction* timeOutedTa = ta->Transaction();
			timeOutedTa->Detach(*this);
			iPendingServerTransactions.Remove(i);
			delete ta;
			iObserver.TimedOut(*timeOutedTa);
			return;
            }
        }
    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CSIPImplementation::ChallengeReceivedL
// -----------------------------------------------------------------------------
//
void CSIPImplementation::ChallengeReceivedL(
    TUint32 aRequestId,
    CSIPResponseElements* aResponse,
    MSIPHttpDigestChallengeObserver2& aObserver)
    {
    __SIP_ASSERT_LEAVE(aResponse != NULL, KErrArgument);

	CSIPClientTransaction* ta(NULL);
    for (TInt i = 0; i < iConnections.Count(); ++i)
        {
        ta = iConnections[i]->Implementation().FindClientTransaction(aRequestId);
        if (ta)
            {
			// Transaction matches aRequestId, so it hasn't yet got response
            ta->SetResponseElements(aResponse);
            aObserver.ChallengeReceived(*ta);
            return;
            }
        }

	// If dialog's trusted user is a non-refreshed registration and INVITE is
	// challenged, INVITE transaction is not found, as ChallengeReceivedL() is
	// called in registration's SIP client instance (dialog has another SIP
	// client instance). Ignore the challenge, so it gets passed to dialog's
	// SIP client.

	// Only one CSIPHttpDigest instance can exist in iDigests
	if (iDigests.Count() > 0)
		{
		ta = CreateTransactionL(aRequestId, aResponse, NULL);
		IgnoreChallenges(*ta, *iDigests[0]);
		delete ta;
		}
	else
		{
		delete aResponse;
		}
    }

// -----------------------------------------------------------------------------
// CSIPImplementation::ChallengeReceivedInRefreshL
// Profile sets credentials synchronously. If application sets them
// asynchronously, update code to handle many simultaneous challenges.
// E.g. dialog's trusted user is registration, both REGISTER and INVITE are
// challenged at the same time with different realms.
// -----------------------------------------------------------------------------
//
void CSIPImplementation::ChallengeReceivedInRefreshL(
    TUint32 aRefreshId,
    TUint32 aRequestId,
    CSIPResponseElements* aResponse,
    MSIPHttpDigestChallengeObserver2& aObserver)
    {
    __SIP_ASSERT_LEAVE(aResponse != NULL, KErrArgument);
    
    CSIPRefresh* refresh(NULL);
    for (TInt i = 0; i < iConnections.Count(); i++)
        {
        refresh = iConnections[i]->Implementation().FindRefresh(aRequestId,
        														aRefreshId);
        if (refresh)
            {
		    refresh->SetRefreshIdIfEmpty(aRefreshId);
	        refresh->UpdateRefreshState(aResponse->StatusCode());
	        CSIPClientTransaction* ta = refresh->Transaction();
			if (ta)
				{
				ta->SetResponseElements(aResponse);
                aObserver.ChallengeReceived(*refresh);
				}
			else
				{
				ta = CreateTransactionL(aRequestId, aResponse, refresh);
                aObserver.ChallengeReceived(*refresh);

				// INVITE transaction doesn't point to refresh. Manually unlink
				// transaction and refresh, instead of letting destructor do it
                refresh->RemoveTransaction();
                if (ta->Refresh())
                	{
    				ta->RemoveRefresh();
                	}
                delete ta;
				}
            return;
            }
        }
	delete aResponse;
    }

// -----------------------------------------------------------------------------
// CSIPImplementation::SupportedSecurityMechanismsL
// -----------------------------------------------------------------------------
//
CDesC8Array* CSIPImplementation::SupportedSecurityMechanismsL() const
    {
    __TEST_INVARIANT;
    return iSIPClient->SupportedSecurityMechanismsL();
    }

// -----------------------------------------------------------------------------
// CSIPImplementation::IsSigCompSupportedL
// -----------------------------------------------------------------------------
//
TBool CSIPImplementation::IsSigCompSupportedL() const
    {
    __TEST_INVARIANT;
    return iSIPClient->IsSigCompSupportedL();
    }

// -----------------------------------------------------------------------------
// CSIPImplementation::NegotiatedSecurityMechanismL
// -----------------------------------------------------------------------------
//
HBufC8*
CSIPImplementation::NegotiatedSecurityMechanismL(const TDesC8& aHop) const
	{
	__TEST_INVARIANT;
    return iSIPClient->NegotiatedSecurityMechanismL(aHop);
	}

// -----------------------------------------------------------------------------
// CSIPImplementation::Connection
// -----------------------------------------------------------------------------
//
CSIPConnection* CSIPImplementation::Connection(TUint32 aIapId) const
    {
    __TEST_INVARIANT;

    for (TInt i = 0; i < iConnections.Count(); i++)
        {
        if (iConnections[i]->IapId() == aIapId)
            {
            return iConnections[i];
            }
        }
    return NULL;
	}

// -----------------------------------------------------------------------------
// CSIPImplementation::SetSecurityHandlingL
// -----------------------------------------------------------------------------
//
void CSIPImplementation::SetSecurityHandlingL(TBool aEnabled)
    {
    __TEST_INVARIANT;
    iSIPClient->SetSecurityHandlingL(aEnabled);
    iSecurityHandlingEnabled = aEnabled;
    }

// -----------------------------------------------------------------------------
// CSIPImplementation::IsSecurityHandlingEnabled
// -----------------------------------------------------------------------------
//
TBool CSIPImplementation::IsSecurityHandlingEnabled() const
    {
    __TEST_INVARIANT;
    return iSecurityHandlingEnabled;
    }

// -----------------------------------------------------------------------------
// CSIPImplementation::CreateTransactionL
// It doesn't matter what MTransactionAssociation is given to NewL.
// -----------------------------------------------------------------------------
//
CSIPClientTransaction*
CSIPImplementation::CreateTransactionL(TUint32 aRequestId,
									   CSIPResponseElements* aResponse,
									   CSIPRefresh* aRefresh)
	{
	__SIP_ASSERT_LEAVE(aResponse && aResponse->CSeqHeader(), KErrNotFound);

	CSIPClientTransaction* ta(NULL);
	RStringF method = aResponse->CSeqHeader()->Method();
	if (method == SIPStrings::StringF(SipStrConsts::EInvite))
		{
		ta = CSIPInviteClientTransaction::NewL(*this);
		if (aRefresh)
			{
			// Links refresh to INVITE transaction, but not the other way
			aRefresh->AddTransaction(*ta);
			}
		}
	else
		{
		ta = CSIPClientTransaction::NewL(method, *this, aRefresh);
		}
    ta->SetRequestId(aRequestId);
    ta->SetResponseElements(aResponse);
    return ta;
	}

// -----------------------------------------------------------------------------
// CSIPImplementation::IgnoreChallenges
// -----------------------------------------------------------------------------
//
void
CSIPImplementation::IgnoreChallenges( const CSIPClientTransaction& aTransaction,
        					   		  CSIPHttpDigest& aDigest )
	{
	const CSIPResponseElements* resp = aTransaction.ResponseElements();
	if ( resp )
		{
    	const RPointerArray<CSIPHeaderBase>& headers =
    		resp->MessageElements().UserHeaders();

		RStringF wwwAuth =
			SIPStrings::StringF(SipStrConsts::EWWWAuthenticateHeader);
		RStringF proxyAuth =
			SIPStrings::StringF(SipStrConsts::EProxyAuthenticateHeader);
		RStringF realm = SIPStrings::StringF(SipStrConsts::ERealm);

	    for (TInt i = 0; i < headers.Count(); ++i)
	        {
	        const CSIPHeaderBase* header = headers[i];
	        if (header->Name() == wwwAuth || header->Name() == proxyAuth)
	        	{
	            const CSIPAuthHeaderBase* auth =
	                static_cast<const CSIPAuthHeaderBase*>(header);
	            aDigest.IgnoreChallenge(aTransaction,
	            						auth->DesParamValue(realm));
	            }
	        }
		}
	}
        					   
// -----------------------------------------------------------------------------
// CSIPImplementation::SIPClient
// -----------------------------------------------------------------------------
//
CSIPClient& CSIPImplementation::SIPClient()
    {
    __TEST_INVARIANT;
    return *iSIPClient;
    }

// -----------------------------------------------------------------------------
// CSIPImplementation::AddConnectionL
// CSIPClient checks if a connection for the IAP-id already exists. No need to
// do it here.
//
// Associate server transactions with the same IAP-id (aIapId), to aConnection.
// Start search from the end of array, so if an item is removed, it doesn't
// affect the position of items that haven't yet been checked.
// -----------------------------------------------------------------------------
//
void CSIPImplementation::AddConnectionL(
							CSIPConnectionImplementation& aConnImplementation,
							TUint32 aIapId)
	{
	__TEST_INVARIANT;

	CSIPConnection& conn = aConnImplementation.SIPConnectionL();    
    iConnections.AppendL(&conn);

    CPendingTransaction* ta(NULL);
    for (TInt i = iPendingServerTransactions.Count(); i > 0; i--)
        {
        ta = iPendingServerTransactions[i - 1];
        __SIP_ASSERT_LEAVE(ta, KErrNotFound);

        if (ta->IapId() == aIapId)
            {            
            ta->Transaction()->ReAssociateL(aConnImplementation);
            }
        }
    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CSIPImplementation::RemoveConnection
// -----------------------------------------------------------------------------
//
void CSIPImplementation::RemoveConnection(const CSIPConnection& aConnection)
    {
    __TEST_INVARIANT;

    TInt pos = iConnections.Find(&aConnection);
    if (pos != KErrNotFound)
        {
        iConnections.Remove(pos);
        }
    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CSIPImplementation::RemoveConnection
// -----------------------------------------------------------------------------
//
void CSIPImplementation::AddDigestL(CSIPHttpDigest& aDigest)
	{
	__TEST_INVARIANT;

    iDigests.AppendL(&aDigest);
	}

// -----------------------------------------------------------------------------
// CSIPImplementation::RemoveDigest
// -----------------------------------------------------------------------------
//
void CSIPImplementation::RemoveDigest(const CSIPHttpDigest& aDigest)
	{
	__TEST_INVARIANT;

	TInt pos = iDigests.Find(&aDigest);
    if (pos != KErrNotFound)
        {
        iDigests.Remove(pos);
        }
    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CSIPImplementation::ClientConnectionL
// CSIPImplementation doesn't know which CSIPClientConnection to return since
// no IAP-id is passed as parameter.
// -----------------------------------------------------------------------------
//
CSIPClientConnection& CSIPImplementation::ClientConnectionL()
    {
    __TEST_INVARIANT;

    User::Leave(KErrUnknown);    
    return *(CSIPClientConnection*)1;
    }

// -----------------------------------------------------------------------------
// CSIPImplementation::ClientConnection
// CSIPImplementation doesn't know which CSIPClientConnection to return since
// no IAP-id is passed as parameter.
// -----------------------------------------------------------------------------
//
CSIPClientConnection* CSIPImplementation::ClientConnection()
    {
    __TEST_INVARIANT;
    return NULL;
    }

// -----------------------------------------------------------------------------
// CSIPImplementation::SIPConnectionL
// CSIPImplementation doesn't know which CSIPConnection to return since no
// IAP-id is passed as parameter.
// -----------------------------------------------------------------------------
//
CSIPConnection& CSIPImplementation::SIPConnectionL()
    {
    __TEST_INVARIANT;

    User::Leave(KErrUnknown);
    return *(CSIPConnection*)1;
    }

// -----------------------------------------------------------------------------
// CSIPImplementation::AddTransactionL
// -----------------------------------------------------------------------------
//
void CSIPImplementation::AddTransactionL(CSIPTransactionBase& /*aTransaction*/)
    {
    // No action    
    }

// -----------------------------------------------------------------------------
// CSIPImplementation::RemoveTransaction
// -----------------------------------------------------------------------------
//
void
CSIPImplementation::RemoveTransaction(const CSIPTransactionBase& aTransaction)
    {
    __TEST_INVARIANT;

    CPendingTransaction* ta(NULL);
    for (TInt i = 0; i < iPendingServerTransactions.Count(); i++)
        {
        ta = iPendingServerTransactions[i];
        __SIP_ASSERT_RETURN(ta && ta->Transaction(), KErrNotFound);

        if (ta->Transaction() == &aTransaction)
            {
            iPendingServerTransactions.Remove(i);
            delete ta;
            return;
            }
        }
    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// CSIPImplementation::InitialRegisterState
// -----------------------------------------------------------------------------
//
CRegistrationState* CSIPImplementation::InitialRegisterState()
    {
    __TEST_INVARIANT;
    return iUnregistered;
    }

// -----------------------------------------------------------------------------
// CSIPImplementation::InitialDialogState
// -----------------------------------------------------------------------------
//
CDialogState* CSIPImplementation::InitialDialogState()
    {
    __TEST_INVARIANT;
    return iDialogTrying;
    }

// -----------------------------------------------------------------------------
// CSIPImplementation::__DbgTestInvariant
// -----------------------------------------------------------------------------
//


void CSIPImplementation::__DbgTestInvariant() const
	{
	if (!iSIPClient ||
		!iUnregistered || !iRegistering || !iRegistered || !iUnregistering ||
		!iDialogTrying || !iDialogEarly || !iDialogConfirmed ||
		!iDialogTerminated)
		{		
		User::Invariant();
		}
	}
