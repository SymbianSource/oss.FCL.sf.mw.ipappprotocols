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
// Name        : sipimsconnectioncontext.cpp
// Part of     : sip ims connection context
// implementation
// Version     : 1.0
//




// INCLUDE FILES
#include    <in_sock.h>
#include    "sipimsconnectioncontext.h"
#include	"sipmessageelements.h"
#include	"sipregistrationbinding.h"
#include	"sipconnection.h"
#include	"sipclienttransaction.h"
#include	"siprefresh.h"
#include	"sipimsprofilecontext.h"
#include	"sipprofileagent.h"
#include	"sipconcreteprofile.h"
#include	"SipProfileLog.h"
#include	"CSIPProxyResolver.h"
#include 	"sipgendefs.h"
#include    <siperr.h>
#include    <sipresponseelements.h>
#include    <sip.h>
#include    <siphttpdigest.h>
#include    <sipprofile.h>


const TInt KMaxIPAddressLength = 256;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::NewLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPIMSConnectionContext* CSIPIMSConnectionContext::NewLC()
	{
	CSIPIMSConnectionContext* self =
		new (ELeave) CSIPIMSConnectionContext();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::NewL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPIMSConnectionContext* CSIPIMSConnectionContext::NewL()
	{
	CSIPIMSConnectionContext* self = 
		CSIPIMSConnectionContext::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::CSIPIMSConnectionContext
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIPIMSConnectionContext::CSIPIMSConnectionContext()
 : iPreviousConnectionState(CSIPConnection::EInit)
	{
	}
	
// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSIPIMSConnectionContext::ConstructL()
    {
    iLocalIP = HBufC8::NewL(KMaxIPAddressLength);
    }
    
// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::~CSIPIMSConnectionContext
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPIMSConnectionContext::~CSIPIMSConnectionContext()
	{
	iContexts.ResetAndDestroy();
	delete iProxyResolver;
	delete iConnection;
	delete iLocalIP;
	
	iUsers.Reset();
	iUsers.Close();

    delete iSipHttpDigest;
    delete iSip;
	}	

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::SetConnectionL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSConnectionContext::SetConnectionL(CSIPConnection* aConnection)
	{
	__ASSERT_ALWAYS(aConnection != NULL, User::Leave(KErrArgument));
	if (aConnection->State() == CSIPConnection::EActive)
	    {
	    RefreshLocalIPAddressL(*aConnection);
	    }
	iConnection = aConnection;
	iPreviousConnectionState = iConnection->State();
	}

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::Connection
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPConnection* CSIPIMSConnectionContext::Connection()
	{
	return iConnection;
	}

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::IsIdle
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSIPIMSConnectionContext::IsIdle() const
	{
	return (iContexts.Count()==0);
	}

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::FindContext
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPIMSProfileContext* 
	CSIPIMSConnectionContext::FindContext(TUint32 aProfileId)
	{
	CSIPIMSProfileContext* context = 0;
	TBool found = EFalse;
	for (TInt i=0; i<iContexts.Count() && !found; i++)
		{
		if (iContexts[i]->Profile() && iContexts[i]->Profile()->Id() == aProfileId)
			{
			context = iContexts[i];
			found = ETrue;
			}
		}
	return context;
	}
	
// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::IsIPSecAlreadyInUseL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSIPIMSConnectionContext::IsIPSecAlreadyInUse(TBool& aRegisterInProgress)
	{
	_LIT8(KSIPIpSec3gpp, "ipsec-3gpp");
	
	TBool ipSecInUse = EFalse;
	for (TInt i=0; i<iContexts.Count(); i++)
		{
		if (iContexts[i]->Profile() && 
		    iContexts[i]->Profile()->IsSecurityNegotiationEnabled() )
			{
			if (iContexts[i]->Profile()->NegotiatedSecurityMechanism() == KSIPIpSec3gpp)
				{
				ipSecInUse = ETrue;
				}
			
			if (iContexts[i]->Profile()->Status() == CSIPConcreteProfile::ERegistrationInProgress &&
				!CSIPIMSProfileContext::IsHttpDigestSettingsConfigured(*iContexts[i]->Profile()))
				{
				aRegisterInProgress = ETrue;
				}
			}
		}
	return ipSecInUse;
	}	
	
// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::IsIPSecInUseL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint32 CSIPIMSConnectionContext::IsIPSecInUse()
	{
	for (TInt i=0; i<iContexts.Count(); i++)
		{
		if ( iContexts[i]->ConfiguredType() == 
									CSIPIMSProfileContext::EIMSReleaseType )
			{
			return iContexts[i]->Profile()->Id();
			}
		}
	return 0;
	}	
	
// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::AddProfileContextL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void 
CSIPIMSConnectionContext::AddProfileContextL(CSIPIMSProfileContext* aContext)
	{
	__ASSERT_DEBUG(aContext->Connection().IapId()==iConnection->IapId(),
															 User::Invariant());
	User::LeaveIfError(iContexts.Append(aContext));
	}

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::CleanIdleContexts
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSConnectionContext::CleanIdleContexts()
	{
	CSIPIMSProfileContext* context = 0;
	for (TInt i= iContexts.Count()-1; i >= 0;i--)
		{
		if (iContexts[i]->CurrentState() == MSIPProfileContext::EInit &&
		    iContexts[i]->IsIdle())
			{
			PROFILE_DEBUG1(
			    "CSIPIMSConnectionContext::CleanIdleContexts, context removed")
			context = iContexts[i];
			iContexts.Remove(i);
			delete context;
			context = NULL;	
			}
		}
	iContexts.Compress();
	}

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::SetCredentials
// -----------------------------------------------------------------------------
//
TBool CSIPIMSConnectionContext::SetCredentials(
    const CSIPClientTransaction& aTransaction,
    CSIPHttpDigest& aDigest)
    {
	TBool found = EFalse;
	for (TInt i=0; i < iContexts.Count() && !found; i++)
		{
		if (iSipHttpDigest)
		    {
		    found = iContexts[i]->SetCredentials(aTransaction,*iSipHttpDigest);
		    }
		else
		    {
		    found = iContexts[i]->SetCredentials(aTransaction,aDigest);
		    }
		}
    return found;   
    }

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::SetCredentials
// -----------------------------------------------------------------------------
//
TBool CSIPIMSConnectionContext::SetCredentials(
    const CSIPRefresh& aRefresh,
    CSIPHttpDigest& aDigest)
    {
	TBool found = EFalse;
	for (TInt i=0; i < iContexts.Count() && !found; i++)
		{
		if (iSipHttpDigest)
		    {
		    found = iContexts[i]->SetCredentials(aRefresh,*iSipHttpDigest);
		    }
		else
		    {
		    found = iContexts[i]->SetCredentials(aRefresh,aDigest);
		    }
		}
    return found;    
    }

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::ConnectionUser
// -----------------------------------------------------------------------------
//
TBool CSIPIMSConnectionContext::ConnectionUser( CSIPConcreteProfile& aProfile )
    {
    TBool connectionUser( EFalse );
    if ( aProfile.IapId() == iConnection->IapId() )
        {
        connectionUser = ( iUsers.Find( &aProfile ) != KErrNotFound );
        }
    return connectionUser;
    }

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::SetConnectionUserL
// -----------------------------------------------------------------------------
//    
void CSIPIMSConnectionContext::SetConnectionUserL( 
    CSIPConcreteProfile& aProfile )
    {
    __ASSERT_ALWAYS( iConnection, User::Leave( KErrNotReady ) );
    
    iUsers.AppendL( &aProfile );

    SetConnectionParameters();
    }

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::CreateSipL
// -----------------------------------------------------------------------------
//
CSIP& CSIPIMSConnectionContext::CreateSipL( 
    MSIPObserver& aSipObserver,
    MSIPHttpDigestChallengeObserver2& aSipHttpDigestObserver )
    {
    __ASSERT_ALWAYS( !iSip, User::Leave( KErrAlreadyExists ) );
    __ASSERT_ALWAYS( !iSipHttpDigest, User::Leave( KErrAlreadyExists ) );
    
	iSip = CSIP::NewL( TUid::Null(), aSipObserver );
	iSipHttpDigest = CSIPHttpDigest::NewL( *iSip, aSipHttpDigestObserver );
    return *iSip;
    }

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::ResolveL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSConnectionContext::ResolveL()
	{
	CleanProxyResolving();
	ResolveProxyL();
	}

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::ResolveProxyL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSConnectionContext::ResolveProxyL()
	{
	PROFILE_DEBUG1("CSIPIMSConnectionContext::ResolveProxyL")
	if (iProxyResolveRequestId == 0)
		{
		if (iProxyResolver == 0)
			{
			iProxyResolver = CSIPProxyResolver::NewL();
			}

		if (iConnection->State() == CSIPConnection::EActive)
			{
			PROFILE_DEBUG1("CSIPIMSConnectionContext::ResolveProxyL, resolving")
			iProxyResolver->ResolveProxyL(
				iProxyResolveRequestId, iConnection->IapId(), *this);
			}
		}
	PROFILE_DEBUG1("CSIPIMSConnectionContext::ResolveProxyL, exit")
	}

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::CleanProxyResolving
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSConnectionContext::CleanProxyResolving()
	{
	PROFILE_DEBUG1("CSIPIMSConnectionContext::CleanProxyResolving")
	if(iProxyResolveRequestId)
		{
		iProxyResolver->Cancel(iProxyResolveRequestId);
		iProxyResolveRequestId = 0;
		}
	}
	
// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::PreviousSIPConnectionState
// -----------------------------------------------------------------------------
//	
CSIPConnection::TState 
CSIPIMSConnectionContext::PreviousSIPConnectionState() const
    {
    return iPreviousConnectionState;
    }

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::Contexts
// -----------------------------------------------------------------------------
//    
RPointerArray<CSIPIMSProfileContext>& CSIPIMSConnectionContext::Contexts()
    {
    return iContexts;
    }

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::ConnectionStateChanged
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSConnectionContext::ConnectionStateChanged(
	CSIPConnection::TState aState)
	{
	PROFILE_DEBUG1("CSIPIMSConnectionContext::ConnectionStateChanged")
	if (aState == CSIPConnection::EActive)
		{
		PROFILE_DEBUG1("ConnectionStateChanged, Active")
		TRAP_IGNORE(RefreshLocalIPAddressL(*iConnection))
		}
	else if (aState == CSIPConnection::EInactive
			|| aState == CSIPConnection::EUnavailable)
		{
		PROFILE_DEBUG3("ConnectionStateChanged, state: ", aState)
		CleanProxyResolving();
		}
	else
		{
		PROFILE_DEBUG3("ConnectionStateChanged, state: ", aState)
		//makes pclint happy
		}

	CleanIdleContexts();

	for (TInt i=0; i<iContexts.Count(); i++)
		{
		iContexts[i]->ConnectionStateChanged(aState);
		}
	iPreviousConnectionState = aState;
	} 

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::IncomingResponse
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSConnectionContext::IncomingResponse (
	CSIPClientTransaction& aTransaction,
	CSIPRegistrationBinding& aRegistration)
	{
	PROFILE_DEBUG1("CSIPIMSConnectionContext::IncomingResponse")
	TBool handled = EFalse;

	CleanIdleContexts();

	for (TInt i=0; i<iContexts.Count() && !handled; i++)
		{
		const CSIPResponseElements* response = aTransaction.ResponseElements();
		if (response)
		    {
    		TBool isErrorResponse = (response->StatusCode() >= 300);
    		TInt contextCountBefore = iContexts.Count();
    		CSIPIMSProfileContext* context = iContexts[i];
    		
    		context->IncomingResponse(aTransaction, aRegistration, handled);
    		
    		TBool contextRemoved = (iContexts.Count() != contextCountBefore);	
    		if (handled && !contextRemoved && isErrorResponse)
    			{
    			if (context->RetryTimerInUse())
    				{
    				context->RetryDeltaTimer(iContexts[i]->RetryAfterTime(), KErrNone);
    				}
    			else
    				{
    				context->RetryRegistration();
    				}
    			}
		    }
		}
	}

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::ErrorOccured
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSConnectionContext::ErrorOccured (
	TInt aError,
	CSIPClientTransaction& aTransaction,
	CSIPRegistrationBinding& aRegistration)
	{
	PROFILE_DEBUG3("CSIPIMSConnectionContext::ErrorOccured with transaction", aError)
	TBool handled = EFalse;
	
	CleanIdleContexts();

	for (TInt i=0; i<iContexts.Count() && !handled; i++)
		{
		iContexts[i]->CSIPProfileContextBase::ErrorOccured(aError,
		                                                   aTransaction,
		                                                   aRegistration,
		                                                   handled);
		if (handled && (iProxyResolveRequestId == 0))
			{
			if ( iContexts[i]->RetryTimerInUse() )
				{
				iContexts[i]->RetryDeltaTimer( iContexts[i]->RetryAfterTime(), aError );
				}
			else
				{
				iContexts[i]->RetryRegistration();
				}
			}
		}
	}

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::ErrorOccured
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSConnectionContext::ErrorOccured (
	TInt aError,
	CSIPRegistrationBinding& aRegistration)
	{
	PROFILE_DEBUG3("CSIPIMSConnectionContext::ErrorOccured", aError)
	TBool handled = EFalse;

	CleanIdleContexts();

	for (TInt i=0; i<iContexts.Count() && !handled; i++)
		{
		if ( aError == KErrSIPTerminatedWithResponse )
			{
			const CSIPClientTransaction* transaction = 
                iContexts[i]->Registration()->SIPRefresh()->SIPTransaction();
    		if (transaction)
            	{
            	aError = transaction->ResponseElements()->StatusCode();
            	iContexts[i]->CSIPProfileContextBase::ErrorOccured(aError,
		                           const_cast<CSIPClientTransaction&> (*transaction),
		                           aRegistration,
		                           handled);
            	}
            else
            	{
            	iContexts[i]->CSIPProfileContextBase::ErrorOccured(aError,
		                                                   aRegistration,
		                                                   handled);
            	}
			}
		else
			{
			iContexts[i]->CSIPProfileContextBase::ErrorOccured(aError,
		                                                   aRegistration,
		                                                   handled);
			}
		
		if (handled && (iProxyResolveRequestId == 0))
			{
			if ( iContexts[i]->RetryTimerInUse() )
				{
				iContexts[i]->RetryDeltaTimer( iContexts[i]->RetryAfterTime(), aError );
				}
			else
				{
				iContexts[i]->RetryRegistration();
				}
			}
		}
	}

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::ProxyResolvingRequestComplete
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSConnectionContext::ProxyResolvingRequestComplete (
	TUint /*aRequestId*/, MDesC8Array* aProxies)
	{
	PROFILE_DEBUG1("CSIPIMSConnectionContext::ProxyResolvingRequestComplete")
	iProxyResolveRequestId = 0;

	CleanIdleContexts();

	TInt err = KErrNone;
	for (TInt i=0; i<iContexts.Count(); i++)
		{
		TRAP(err, iContexts[i]->ProxyResolvingRequestCompleteL(*aProxies));
		if (err != KErrNone)
			{
			iContexts[i]->HandleProxyResolvingError(err);	
			}
		}
	delete aProxies;
	}

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::ProxyResolvingRequestFailed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSConnectionContext::ProxyResolvingRequestFailed (
	TUint /*aRequestId*/, TInt aError)
	{
	PROFILE_DEBUG3("CSIPIMSConnectionContext::\
		ProxyResolvingRequestFailed", aError)
	CleanIdleContexts();
	iProxyResolveRequestId = 0;

	for (TInt i=0; i<iContexts.Count(); i++)
		{
		iContexts[i]->ProxyResolvingRequestFailed(aError);
		}
	}

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::ErrorOccured
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSConnectionContext::ErrorOccured (
	TInt /*aError*/,
	CSIPTransactionBase& /*aTransaction*/)
	{
	}

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::IncomingRequest
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSConnectionContext::IncomingRequest (
	CSIPServerTransaction* /*aTransaction*/)
	{
	}

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::IncomingRequest
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSConnectionContext::IncomingRequest (
	CSIPServerTransaction* aTransaction,
	CSIPDialog& aDialog)
	{
	PROFILE_DEBUG1("CSIPIMSConnectionContext::IncomingRequest")
	TBool handled = EFalse;

	CleanIdleContexts();

	for (TInt i=0; i<iContexts.Count() && !handled; i++)
		{
		iContexts[i]->IncomingRequest(aTransaction, aDialog, handled);
		}
	}

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::IncomingResponse
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSConnectionContext::IncomingResponse (
	CSIPClientTransaction& aTransaction)
	{
	PROFILE_DEBUG1("CSIPIMSConnectionContext::IncomingResponse")
	TBool handled = EFalse;

	CleanIdleContexts();

	for (TInt i=0; i<iContexts.Count() && !handled; i++)
		{
		iContexts[i]->IncomingResponse(aTransaction, handled);
		}
	}

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::IncomingResponse
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSConnectionContext::IncomingResponse (
	CSIPClientTransaction& aTransaction,
	CSIPDialogAssocBase& aDialogAssoc)
	{
	PROFILE_DEBUG1("CSIPIMSConnectionContext::IncomingResponse")
	TBool handled = EFalse;

	CleanIdleContexts();

	for (TInt i=0; i<iContexts.Count() && !handled; i++)
		{
		iContexts[i]->IncomingResponse(aTransaction, aDialogAssoc, handled);
		}
	}

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::IncomingResponse
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSConnectionContext::IncomingResponse (
	CSIPClientTransaction& /*aTransaction*/,
	CSIPInviteDialogAssoc* /*aDialogAssoc*/)
	{
	}


// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::ErrorOccured
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSConnectionContext::ErrorOccured (
	TInt aError,
	CSIPDialogAssocBase& aDialogAssoc)
	{
	PROFILE_DEBUG3("CSIPIMSConnectionContext::ErrorOccured", aError)
	TBool handled = EFalse;

	CleanIdleContexts();

	for (TInt i=0; i<iContexts.Count() && !handled; i++)
		{
		iContexts[i]->ErrorOccured(aDialogAssoc, aError, handled);
		}
	}
	
// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::ErrorOccured
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSConnectionContext::ErrorOccured (
	TInt aError,
	CSIPTransactionBase& aTransaction,
	CSIPDialogAssocBase& aDialogAssoc)
	{
	PROFILE_DEBUG3("CSIPIMSConnectionContext::ErrorOccured", aError)
	TBool handled = EFalse;

	CleanIdleContexts();

	for (TInt i=0; i<iContexts.Count() && !handled; i++)
		{
		iContexts[i]->ErrorOccured(aTransaction, aDialogAssoc, 
		                                 aError, handled);
		}
	}

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::ErrorOccured
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSConnectionContext::ErrorOccured (
	TInt /*aError*/,
	CSIPRefresh& /*aRefresh*/)
	{
	}

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::InviteCompleted
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSConnectionContext::InviteCompleted (
	CSIPClientTransaction& /*aTransaction*/)
	{
	}
	
// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::InviteCanceled
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIMSConnectionContext::InviteCanceled(
    CSIPServerTransaction& /*aTransaction*/)
	{
	}
	
// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::LocalIPAddress
// -----------------------------------------------------------------------------
//
const TDesC8& CSIPIMSConnectionContext::LocalIPAddress() const
	{
	return *iLocalIP;
	}	

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::AddIntoQueue
// -----------------------------------------------------------------------------
//
TBool CSIPIMSConnectionContext::AddIntoQueue( const TDesC8& aValue )
	{
	PROFILE_DEBUG1("CSIPIMSConnectionContext::AddIntoQue")
    TBool result = EFalse;
	for (TInt i=0; i<iContexts.Count() && !result; i++)
		{
		result = iContexts[i]->AddIntoQueue( aValue );
		}
	return result;
	}

// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::RefreshLocalIPAddressL
// -----------------------------------------------------------------------------
//	
void CSIPIMSConnectionContext::RefreshLocalIPAddressL(
    const CSIPConnection& aConnection)
    {
    TInetAddr addr;
    aConnection.GetLocalAddrL(addr);
	HBufC* tmp = HBufC::NewLC(KMaxIPAddressLength);
	TPtr tmpPtr(tmp->Des());
	addr.Output(tmpPtr);
	iLocalIP->Des().Copy(tmpPtr);
	CleanupStack::PopAndDestroy(tmp);
    }


// -----------------------------------------------------------------------------
// CSIPIMSConnectionContext::SetConnectionParameters
// -----------------------------------------------------------------------------
//
void CSIPIMSConnectionContext::SetConnectionParameters()
    {
    PROFILE_DEBUG1("CSIPIMSConnectionContext::SetConnectionParameters")
    
    // Params are taken from first user
    if ( iUsers.Count() > 0 && 
         !iConnectionParamsSet && 
         iConnection )
        {
        TUint32 signallingQosValue( 0 );
        if ( iUsers[ 0 ]->ExtensionParameter( 
                KSIPSoIpTOS, signallingQosValue ) != KErrNotFound )
            {
            PROFILE_DEBUG3(
                "CSIPIMSConnectionContext::SetConnectionParametersL, val:", 
                signallingQosValue)
            
            // Errors can be ignored as they are not preventing registration    
            TRAP_IGNORE( iConnection->SetOptL( KSoIpTOS, 
                                               KProtocolInetIp, 
                                               signallingQosValue ) )
            iConnectionParamsSet = ETrue;
            }
        }
        
    PROFILE_DEBUG1("CSIPIMSConnectionContext::SetConnectionParameters, exit")
    }

