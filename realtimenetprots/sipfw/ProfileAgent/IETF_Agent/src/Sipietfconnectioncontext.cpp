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
// Name        : sipietfconnectioncontext.cpp
// Part of     : sip ietf connection context
// implementation
// Version     : 1.0
// INCLUDE FILES
//



#include    "sipietfconnectioncontext.h"
#include	"sipmessageelements.h"
#include	"sipregistrationbinding.h"
#include	"sipconnection.h"
#include	"sipclienttransaction.h"
#include	"siprefresh.h"
#include	"sipietfprofilecontext.h"
#include	"sipprofileagent.h"
#include	"sipconcreteprofile.h"
#include	"SipProfileLog.h"
#include	"CSIPProxyResolver.h"
#include    "siperr.h"
#include    "sipresponseelements.h"
#include    <sip.h>
#include    <siphttpdigest.h>
#include    <sipprofile.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::CSIPIetfConnectionContext
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIPIetfConnectionContext::CSIPIetfConnectionContext()
	{
	}

// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::~CSIPIetfConnectionContext
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPIetfConnectionContext::~CSIPIetfConnectionContext()
	{
	iContexts.ResetAndDestroy();

	delete iProxyResolver;
	delete iConnection;
	
	iUsers.Reset();
	iUsers.Close();

    delete iSipHttpDigest;
    delete iSip;
	}

// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::NewLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPIetfConnectionContext* CSIPIetfConnectionContext::NewLC()
	{
	CSIPIetfConnectionContext* self =
		new (ELeave) CSIPIetfConnectionContext();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::NewL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPIetfConnectionContext* CSIPIetfConnectionContext::NewL()
	{
	CSIPIetfConnectionContext* self = 
		CSIPIetfConnectionContext::NewLC();
	CleanupStack::Pop();
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::SetConnection
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfConnectionContext::SetConnection(CSIPConnection* aConnection)
	{
	delete iConnection;
	iConnection = aConnection;
	}

// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::Connection
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPConnection* CSIPIetfConnectionContext::Connection()
	{
	return iConnection;
	}

// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::IsIdle
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSIPIetfConnectionContext::IsIdle() const
	{
	return (iContexts.Count()==0);
	}

// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::FindContext
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSIPIetfProfileContext* 
	CSIPIetfConnectionContext::FindContext(TUint32 aProfileId)
	{
	CSIPIetfProfileContext* context = 0;
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
// CSIPIetfConnectionContext::AddProfileContextL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void 
CSIPIetfConnectionContext::AddProfileContextL(CSIPIetfProfileContext* aContext)
	{
	__ASSERT_DEBUG(aContext->Connection().IapId()==iConnection->IapId(),
															 User::Invariant());
	User::LeaveIfError(iContexts.Append(aContext));
	}

// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::CleanIdleContexts
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfConnectionContext::CleanIdleContexts()
	{
	PROFILE_DEBUG1("CSIPIetfConnectionContext::CleanIdleContexts")
	CSIPIetfProfileContext* context = 0;
	for (TInt i= iContexts.Count()-1; i >= 0;i--)
		{
		PROFILE_DEBUG3("CSIPIetfConnectionContext::CleanIdleContexts iContexts.Count() > 0 i=",i)
		if (iContexts[i]->CurrentState() == MSIPProfileContext::EInit &&
			iContexts[i]->IsIdle())
			{
			PROFILE_DEBUG1("CSIPIetfConnectionContext::CleanIdleContexts, ProfileContext removed")
			context = iContexts[i];
			iContexts.Remove(i);
			delete context;
			context = NULL;	
			}
		}
	iContexts.Compress();
	}

// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::SetCredentials
// -----------------------------------------------------------------------------
//
TBool CSIPIetfConnectionContext::SetCredentials(
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
// CSIPIetfConnectionContext::SetCredentials
// -----------------------------------------------------------------------------
//
TBool CSIPIetfConnectionContext::SetCredentials(
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
// CSIPIetfConnectionContext::ConnectionUser
// -----------------------------------------------------------------------------
//
TBool CSIPIetfConnectionContext::ConnectionUser( CSIPConcreteProfile& aProfile )
    {
    TBool connectionUser( EFalse );
    if ( aProfile.IapId() == iConnection->IapId() )
        {
        connectionUser = ( iUsers.Find( &aProfile ) != KErrNotFound );
        }
    return connectionUser;
    }

// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::SetConnectionUserL
// -----------------------------------------------------------------------------
//    
void CSIPIetfConnectionContext::SetConnectionUserL( 
    CSIPConcreteProfile& aProfile )
    {
    __ASSERT_ALWAYS( iConnection, User::Leave( KErrNotReady ) );
    
    iUsers.AppendL( &aProfile );

    SetConnectionParameters();
    }

// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::CreateSipL
// -----------------------------------------------------------------------------
//
CSIP& CSIPIetfConnectionContext::CreateSipL( 
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
// CSIPIetfConnectionContext::ResolveL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfConnectionContext::ResolveL()
	{
	CleanProxyResolving();
	ResolveProxyL();
	}

// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::ResolveProxyL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfConnectionContext::ResolveProxyL()
	{
	PROFILE_DEBUG1("CSIPIetfConnectionContext::ResolveProxyL")
	if (iProxyResolveRequestId == 0)
		{
		if (iProxyResolver == 0)
			{
			iProxyResolver = CSIPProxyResolver::NewL();
			}

		if (iConnection->State() == CSIPConnection::EActive)
			{
			PROFILE_DEBUG1("CSIPIetfConnectionContext::ResolveProxyL, resolving")
			iProxyResolver->ResolveProxyL(
				iProxyResolveRequestId, iConnection->IapId(), *this);
			}
		}
	PROFILE_DEBUG1("CSIPIetfConnectionContext::ResolveProxyL, exit")
	}

// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::CleanProxyResolving
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfConnectionContext::CleanProxyResolving()
	{
	PROFILE_DEBUG1("CSIPIetfConnectionContext::CleanProxyResolving")
	if(iProxyResolveRequestId)
		{
		iProxyResolver->Cancel(iProxyResolveRequestId);
		iProxyResolveRequestId = 0;
		}
	}

// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::ConnectionStateChanged
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfConnectionContext::ConnectionStateChanged(
	CSIPConnection::TState aState)
	{
	PROFILE_DEBUG1("CSIPIetfConnectionContext::ConnectionStateChanged")
	if (aState == CSIPConnection::EActive)
		{
		PROFILE_DEBUG1("ConnectionStateChanged, Active")
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
	} 

// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::IncomingResponse
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfConnectionContext::IncomingResponse(
    CSIPClientTransaction& aTransaction,
    CSIPRegistrationBinding& aRegistration)
    {
    PROFILE_DEBUG1("CSIPIetfConnectionContext::IncomingResponse")
    TBool handled = EFalse;

    CleanIdleContexts();

    for (TInt i=0; i<iContexts.Count() && !handled; i++)
        {
        const CSIPResponseElements* response = aTransaction.ResponseElements();
        if (response)
            {
            TInt contextCountBefore = iContexts.Count();
            TBool isErrorResponse = (response->StatusCode() >= 300);
            TInt statusCode = response->StatusCode();
            iContexts[i]->IncomingResponse(aTransaction, aRegistration, handled);
            TBool contextRemoved = (iContexts.Count() != contextCountBefore);
            if (handled && !contextRemoved && isErrorResponse)
                {
                if ( iContexts[i]->RetryTimerInUse() )
                    {
                    iContexts[i]->RetryDeltaTimer(iContexts[i]->DelayTime(), statusCode );
                    }
                else
                    {
                    iContexts[i]->RetryRegistration();
                    }
               }    
            }
        }
    }

// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::ErrorOccured
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfConnectionContext::ErrorOccured(
	TInt aError,
	CSIPClientTransaction& aTransaction,
	CSIPRegistrationBinding& aRegistration)
	{
	PROFILE_DEBUG3("CSIPIetfConnectionContext::ErrorOccured", aError)
	TBool handled = EFalse;

	CleanIdleContexts();

	for (TInt i=0; i<iContexts.Count() && !handled; i++)
		{
		iContexts[i]->ErrorOccured(aError, aTransaction, aRegistration, handled);
		if (handled && (iProxyResolveRequestId == 0))
			{
			if ( iContexts[i]->RetryTimerInUse() )
				{
				iContexts[i]->RetryDeltaTimer( iContexts[i]->DelayTime(), aError );
				}
			else
				{
				iContexts[i]->RetryRegistration();
				}
			}
		}
	}

// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::ErrorOccured
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfConnectionContext::ErrorOccured(
	TInt aError,
	CSIPRegistrationBinding& aRegistration)
	{
	PROFILE_DEBUG3("CSIPIetfConnectionContext::ErrorOccured", aError)
	TBool handled = EFalse;

	CleanIdleContexts();

	for (TInt i=0; i<iContexts.Count() && !handled; i++)
		{
		if (aError == KErrSIPTerminatedWithResponse)
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
            	iContexts[i]->ErrorOccured(aError, aRegistration, handled);
            	}
			}
		else
			{
			iContexts[i]->ErrorOccured(aError, aRegistration, handled);
			}
			
		if (handled && (iProxyResolveRequestId == 0))
			{
			if ( iContexts[i]->RetryTimerInUse() )
				{
				iContexts[i]->RetryDeltaTimer( iContexts[i]->DelayTime(), aError );
				}
			else
				{
				iContexts[i]->RetryRegistration();
				}
			}
		}
	}

// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::ProxyResolvingRequestComplete
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfConnectionContext::ProxyResolvingRequestComplete(
	TUint /*aRequestId*/, MDesC8Array* aProxies)
	{
	PROFILE_DEBUG1("CSIPIetfConnectionContext::ProxyResolvingRequestComplete")
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
// CSIPIetfConnectionContext::ProxyResolvingRequestFailed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfConnectionContext::ProxyResolvingRequestFailed(
	TUint /*aRequestId*/, TInt aError)
	{
	PROFILE_DEBUG3("CSIPIetfConnectionContext::\
		ProxyResolvingRequestFailed", aError)
	CleanIdleContexts();
	iProxyResolveRequestId = 0;

	for (TInt i=0; i<iContexts.Count(); i++)
		{
		iContexts[i]->ProxyResolvingRequestFailed(aError);
		}
	}

// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSIPIetfConnectionContext::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::ErrorOccured
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfConnectionContext::ErrorOccured(
	TInt /*aError*/,
	CSIPTransactionBase& /*aTransaction*/)
	{
	}

// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::IncomingRequest
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfConnectionContext::IncomingRequest(
	CSIPServerTransaction* /*aTransaction*/)
	{
	}

// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::IncomingRequest
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfConnectionContext::IncomingRequest(
	CSIPServerTransaction* /*aTransaction*/,
	CSIPDialog& /*aDialog*/)
	{
	}

// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::IncomingResponse
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfConnectionContext::IncomingResponse(
	CSIPClientTransaction& /*aTransaction*/)
	{
	}

// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::IncomingResponse
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfConnectionContext::IncomingResponse(
	CSIPClientTransaction& /*aTransaction*/,
	CSIPDialogAssocBase& /*aDialogAssoc*/)
	{
	}

// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::IncomingResponse
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfConnectionContext::IncomingResponse(
	CSIPClientTransaction& /*aTransaction*/,
	CSIPInviteDialogAssoc* /*aDialogAssoc*/)
	{
	}


// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::ErrorOccured
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfConnectionContext::ErrorOccured(
	TInt /*aError*/,
	CSIPDialogAssocBase& /*aDialogAssoc*/)
	{
	}
// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::ErrorOccured
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfConnectionContext::ErrorOccured(
	TInt /*aError*/,
	CSIPTransactionBase& /*aTransaction*/,
	CSIPDialogAssocBase& /*aDialogAssoc*/)
	{
	}

// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::ErrorOccured
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfConnectionContext::ErrorOccured(
	TInt /*aError*/,
	CSIPRefresh& /*aRefresh*/)
	{
	}

// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::InviteCompleted
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfConnectionContext::InviteCompleted(
	CSIPClientTransaction& /*aTransaction*/)
	{
	}
	
// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::InviteCanceled
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSIPIetfConnectionContext::InviteCanceled(
    CSIPServerTransaction& /*aTransaction*/)
	{
	}

// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::AddIntoQueue
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CSIPIetfConnectionContext::AddIntoQueue( const TDesC8& aValue )
	{
	PROFILE_DEBUG1("CSIPIetfConnectionContext::AddIntoQue")
    TBool result = EFalse;
	for (TInt i=0; i<iContexts.Count() && !result; i++)
		{
		result = iContexts[i]->AddIntoQueue( aValue );
		}
	return result;
    }

// -----------------------------------------------------------------------------
// CSIPIetfConnectionContext::SetConnectionParameters
// -----------------------------------------------------------------------------
//
void CSIPIetfConnectionContext::SetConnectionParameters()
    {
    PROFILE_DEBUG1("CSIPIetfConnectionContext::SetConnectionParameters")
    
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
                "CSIPIetfConnectionContext::SetConnectionParametersL, val:", 
                signallingQosValue)
            
            // Errors can be ignored as they are not preventing registration    
            TRAP_IGNORE( iConnection->SetOptL( KSoIpTOS, 
                                               KProtocolInetIp, 
                                               signallingQosValue ) )
            iConnectionParamsSet = ETrue;
            }
        }
        
    PROFILE_DEBUG1("CSIPIetfConnectionContext::SetConnectionParameters, exit")
    }
