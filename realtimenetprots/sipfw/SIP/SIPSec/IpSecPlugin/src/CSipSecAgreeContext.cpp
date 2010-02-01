// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CSipSecAgreeContext.cpp
// Part of       : SIPSec
// Version       : SIP/5.0
//



#include "sipstrings.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "sipcseqheader.h"
#include "MSIPTransportMgr.h"
#include "MIpSecMechanismParams.h"
#include "MSIPSecSecurityMechanismObserver.h"
#include "CSipSecAgreeRecord.h"
#include "CSipSecAgreeContext.h"
#include "SipSecUtils.h"

#ifdef CPPUNIT_TEST
#include "CActiveObjController1.h"
#endif


// ----------------------------------------------------------------------------
// CSipSecAgreeContext::NewL
// ----------------------------------------------------------------------------
//
CSipSecAgreeContext* CSipSecAgreeContext::NewL(
	MIpSecMechanismParams& aParams,
	TSIPTransportParams& aTransportParams,
	CSIPResponse& aResponse,
	CSIPRequest& aRequest,
	TInetAddr& aRemoteAddress,
	MSIPSecUser* aUser,
	MSIPSecSecurityMechanismObserver& aObserver,
	RPointerArray<CState>& aStates )	
	{
	CSipSecAgreeContext* 
		self = CSipSecAgreeContext::NewLC( aParams,
		                                   aTransportParams,
		                                   aResponse,
		                                   aRequest,
										   aRemoteAddress,
										   aUser,
										   aObserver,
										   aStates );
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeContext::NewLC
// ----------------------------------------------------------------------------
//
CSipSecAgreeContext* CSipSecAgreeContext::NewLC(
	MIpSecMechanismParams& aParams,
	TSIPTransportParams& aTransportParams,
	CSIPResponse& aResponse,
	CSIPRequest& aRequest,
	TInetAddr& aRemoteAddress,
	MSIPSecUser* aUser,
	MSIPSecSecurityMechanismObserver& aObserver,
	RPointerArray<CState>& aStates )
	{
	CSipSecAgreeContext* self = 
	    new ( ELeave ) CSipSecAgreeContext( aParams, aObserver, aStates );
	CleanupStack::PushL(self);
	self->ConstructL( aTransportParams,
	                  aResponse,
	                  aRequest,
					  aRemoteAddress,
					  aUser );
	return self;
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeContext::CSipSecAgreeContext
// ----------------------------------------------------------------------------
//
CSipSecAgreeContext::CSipSecAgreeContext( 
	MIpSecMechanismParams& aParams,
	MSIPSecSecurityMechanismObserver& aObserver,
	RPointerArray<CState>& aStates ) :
	iParams( aParams ),
	iObserver( &aObserver ),
	iStates( aStates )
#ifdef CPPUNIT_TEST
    , iSecAgreeRecords( 1 )
#endif
	{
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeContext::ConstructL
// ----------------------------------------------------------------------------
//
void CSipSecAgreeContext::ConstructL( TSIPTransportParams& aTransportParams,
								      CSIPResponse& aResponse,
								      CSIPRequest& aRequest,
									  TInetAddr& aRemoteAddress,
									  MSIPSecUser* aUser )
	{
	User::LeaveIfError( iParams.TransportMan().GetLocalAddress(
                                                aTransportParams.IapId(),
                                                iLocalAddress ) );
	iRemoteAddress = aRemoteAddress;
	iSecUser = aUser;

	CSipSecAgreeRecord* r =
	    CSipSecAgreeRecord::NewLC( iStates, *this, aTransportParams );

#ifdef CPPUNIT_TEST
	iObjCtr = new ( ELeave ) CActiveObjController1();
	iObjCtr->iContext = this;
	iObjCtr->iRcrdCtrl1 = r->iObjCtr;
	r->iObjCtr->iFeedBack = iObjCtr;
#endif

	r->Resp4xxL( aResponse, aRequest );
	iSecAgreeRecords.AppendL( r );
	CleanupStack::Pop( r );
	}
	
// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CSipSecAgreeContext::~CSipSecAgreeContext()
	{
	iSecAgreeRecords.ResetAndDestroy();	
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeContext::MechParams
// ----------------------------------------------------------------------------
//
MIpSecMechanismParams& CSipSecAgreeContext::MechParams()
	{
	return iParams;
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeContext::LocalAddress
// ----------------------------------------------------------------------------
//
TInetAddr CSipSecAgreeContext::LocalAddress()
	{
	return iLocalAddress;
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeContext::PolicyServer
// ----------------------------------------------------------------------------
//	
RIpsecPolicyServ& CSipSecAgreeContext::PolicyServer()
    {
    return iParams.PolicyServer();
    }

// ----------------------------------------------------------------------------
// CSipSecAgreeContext::RemoteAddress
// ----------------------------------------------------------------------------
//  
TInetAddr CSipSecAgreeContext::RemoteAddress()
	{
	return iRemoteAddress;
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeContext::HasOnlyOneRecord
// ----------------------------------------------------------------------------
//
TBool CSipSecAgreeContext::HasOnlyOneRecord() const
	{
	TInt pos( 0 );
	return ( NextRecord( pos ) != NULL && !NextRecord( pos ) );
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeContext::HasRecordInState
// ----------------------------------------------------------------------------
//
TBool
CSipSecAgreeContext::HasRecordInState( CState::TSecAgreeRecordState aState )
	{	
	TInt pos( 0 );
	TBool found( EFalse );
	CSipSecAgreeRecord* record = NextRecord( pos );
	for ( ; !found && record; record = NextRecord( pos ) )
		{
		found = ( record->State() == aState );
		}
	return found;
	}
	
// ----------------------------------------------------------------------------
// CSipSecAgreeContext::SaDeleted
// ----------------------------------------------------------------------------
//
void CSipSecAgreeContext::SaDeleted( CSipSecAgreeRecord* aRecord )
	{
	DeleteRecord( aRecord );
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeContext::SaCleared
// ----------------------------------------------------------------------------
//
void CSipSecAgreeContext::SaCleared( CSipSecAgreeRecord* aRecord )
	{
	DeleteRecord( aRecord );
	if ( iSecAgreeRecords.Count() == 0 )
		{
		iParams.ContextCleared( this );
		}
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeContext::SAReady
// ----------------------------------------------------------------------------
//	
void CSipSecAgreeContext::SAReady( TBool aSuccess )
    {
    if ( iObserver )
        {
        iObserver->SAReady( aSuccess );
        }
    }

// ----------------------------------------------------------------------------
// CSipSecAgreeContext::HasLongerLifetimeSA
// ----------------------------------------------------------------------------
//
TBool
CSipSecAgreeContext::HasLongerLifetimeSA( TUint aTimeToCompareInMillisecs,
										  TUint& aLongerLifetimeInMillisecs )
    {
    // Compare only with "inuse" records
    TBool found( EFalse );
    TInt pos( 0 );
	for ( CSipSecAgreeRecord* record = NextRecord( pos );
		  !found && record;
		  record = NextRecord( pos ) )	
		{
		TUint expiresAfter = record->ExpiresAfter();

		if ( record->State() == CState::EInUse &&
		     ( expiresAfter > aTimeToCompareInMillisecs ) )
			{
			aLongerLifetimeInMillisecs = expiresAfter;
			found = ETrue;
			}		
		}
	return found;
    }
	
// ----------------------------------------------------------------------------
// CSipSecAgreeContext::HasSecUser
// ----------------------------------------------------------------------------
//
TBool CSipSecAgreeContext::HasSecUser( const MSIPSecUser* aSecUser ) const
	{
	return aSecUser == iSecUser;
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeContext::HasNextHop
// Ignore context for which clearing has started.
// ----------------------------------------------------------------------------
//
TBool CSipSecAgreeContext::HasNextHop( const TInetAddr& aNextHop ) const
    {
    return ( !aNextHop.IsWildAddr() &&
    		 iRemoteAddress.Match( aNextHop ) &&
    		 !iClearOrdered );
    }

// ----------------------------------------------------------------------------
// CSipSecAgreeContext::ApplyRulesL
// ----------------------------------------------------------------------------
//
void CSipSecAgreeContext::ApplyRulesL( TSIPTransportParams& aTransportParams, 
									   CSIPRequest& aRequest,
									   const TDesC8& aOutboundProxy,
									   MSIPSecUser* aUser )
	{
	TInt pos( 0 );
	CSipSecAgreeRecord* record = NextRecord( pos );

	if ( SipSecUtils::Match( SipStrConsts::ERegister, aRequest.Method() ) )
		{
		// Other MSIPSecUsers cannot use existing context (SA)
		// for registration to the same outbound proxy.
		__ASSERT_ALWAYS( HasSecUser( aUser ), User::Leave( KErrInUse ) );

		for ( ; record; record = NextRecord( pos ) )
			{
			record->RegisterL( aTransportParams, aRequest, aOutboundProxy );
			}
		}
	else
		{
		for ( ; record; record = NextRecord( pos ) )
			{
			record->ReguestL( aTransportParams, aRequest, aOutboundProxy );
			}
		}
	}
	
// ----------------------------------------------------------------------------
// CSipSecAgreeContext::ApplyRulesL
// IPSec is only interested on 401 and 200 responses
// ----------------------------------------------------------------------------
//	
void CSipSecAgreeContext::ApplyRulesL( 
	CSIPResponse& aResponse,
	CSIPRequest& aRequest,
	MSIPSecSecurityMechanismObserver& aObserver )
	{
	if ( aResponse.ResponseCode() == K401Response )
		{
		// First check if a new record must be created
		CreateNewSecAgreeRecordIfNeededL();

		TInt pos( 0 );		
		for ( CSipSecAgreeRecord* record = NextRecord( pos );
			  record;
			  record = NextRecord( pos ) )
			{
			record->Resp4xxL( aResponse, aRequest );
			}
		iObserver = &aObserver;	
		}
	if ( aResponse.ResponseCode() == K200Response )
		{
		CSIPCSeqHeader* h = static_cast<CSIPCSeqHeader*>(
			SipSecUtils::SingleHeader( aResponse, SipStrConsts::ECSeqHeader ) );
		if ( h && h->Method() == _STRINGF( SipStrConsts::ERegister ) )
			{
			TInt pos( 0 );
			for ( CSipSecAgreeRecord* record = NextRecord( pos );
				  record;
				  record = NextRecord( pos ) )
				{
				record->Resp2xxL( aResponse );
				}
			iObserver = &aObserver;
			}
		}
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeContext::CreateNewSecAgreeRecordIfNeededL
// Create new record if an existing record is in InUse, Deleting or Clearing
// state. Deleting and Clearing states are included because in some very rare
// scenario 4xx might be received before SA deletion is completed, and such
// situation must not cause malfunction.
//
//
// If two records exist already. Drop one of them by setting it obsolete.
//
// Selecting the record to be obsoleted:
//   SA-1's state	  SA-2's state				  drop
//   ------------     ------------                ----
//   InUse      	  Tentative..PendingPolicy	  SA-2
//   Old..Deleting	  InUse						  SA-1
//
// When 2xx is received, SA-1 enters "Old" and SA-2 enters "InUse",
// so two records can't be in "InUse".
//
//
// New record uses initially same transport params, as it must share protected
// server port with old record. When new record is reserving transport,
// transport params will be updated. New record derives Security-Client and
// Security-Verify headers from response and corresponding request.
// ----------------------------------------------------------------------------
//
void CSipSecAgreeContext::CreateNewSecAgreeRecordIfNeededL()
	{
	if ( HasRecordInState( CState::EInUse ) ||
		 HasRecordInState( CState::EDeleting ) ||
		 HasRecordInState( CState::EClearing ) )
		{
		if ( !HasOnlyOneRecord() )	
			{			
			TInt pos( 0 );
			CSipSecAgreeRecord* rec = NextRecord( pos );
			if(rec)
			{
			if ( rec->State() == CState::EInUse )
				{
				// Record is "in use", drop the other record
				rec = NextRecord( pos );
				if(rec)
				{
					__ASSERT_ALWAYS( rec->State() != CState::EInUse,
							 	 User::Leave( KErrGeneral ) );
				}
				}
			}
			if(rec)
			{
				rec->SetObsoleteL();
			}
			}
		TInt pos = 0;
		CSipSecAgreeRecord* rec = NextRecord( pos );
		__ASSERT_ALWAYS( rec, User::Leave( KErrNotFound ) );

		CSipSecAgreeRecord*	newRecord =
		    CSipSecAgreeRecord::NewLC( iStates, *this, rec->TransportParams() );

#ifdef CPPUNIT_TEST
	    if ( !rec->iObjCtr->iFeedBack->iRcrdCtrl2 )
	    	{
	    	rec->iObjCtr->iFeedBack->iRcrdCtrl2 = newRecord->iObjCtr;
	    	}
	    else if ( !rec->iObjCtr->iFeedBack->iRcrdCtrl3 )
	    	{
	    	rec->iObjCtr->iFeedBack->iRcrdCtrl3 = newRecord->iObjCtr;
	    	}
	    else if ( !rec->iObjCtr->iFeedBack->iRcrdCtrl4 )
	    	{
	    	rec->iObjCtr->iFeedBack->iRcrdCtrl4 = newRecord->iObjCtr;
	    	}
#endif
		iSecAgreeRecords.AppendL( newRecord );
		CleanupStack::Pop( newRecord );
		}
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeContext::ApplyRulesL
// ----------------------------------------------------------------------------
//	
void CSipSecAgreeContext::ApplyRulesL( 
	RPointerArray<CSIPSecurityVerifyHeader>& aSecurityVerify )
	{
	TInt pos( 0 );
	CSipSecAgreeRecord* record( NULL );
	while ( ( record = NextRecord( pos ) ) != NULL )
		{
		record->ProcessSecVerifyL( aSecurityVerify );
		}
	}
	
// ----------------------------------------------------------------------------
// CSipSecAgreeContext::SetAuthKeyL
// ----------------------------------------------------------------------------
//
TBool CSipSecAgreeContext::SetAuthKeyL( const TDesC8& aAuthKey )
	{
	TBool ret( EFalse );
	TInt pos( 0 );
	CSipSecAgreeRecord* record( NULL );
	while ( ( record = NextRecord( pos ) ) != NULL )
		{
		ret |= record->AuthKeyL( aAuthKey );
		}
	return ret;
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeContext::AuthKeyFailedL
// ----------------------------------------------------------------------------
//	
void CSipSecAgreeContext::AuthKeyFailedL()
    {
    // Reverse order since call might lead to deletion of record
    TInt lastIndex( iSecAgreeRecords.Count() - 1 );
    for ( TInt i = lastIndex; i >= 0; --i )
		{
		if ( !iSecAgreeRecords[ i ]->IsObsolete() )
			{
			iSecAgreeRecords[ i ]->AuthKeyFailedL();
			}
		}
    }

// ----------------------------------------------------------------------------
// CSipSecAgreeContext::ClearL
// Affects also obsolete records.
// ----------------------------------------------------------------------------
//	
void CSipSecAgreeContext::ClearL()
	{	
	iClearOrdered = ETrue;

	// Reverse order since call might lead to deletion of record, also whole
	// context (this) will be removed if clearing leads to immediate deletion
	// of all records.
    TInt lastIndex( iSecAgreeRecords.Count() - 1 );
	for ( TInt i = lastIndex; i >= 0; i--  )
		{
		iSecAgreeRecords[ i ]->ClearSaL();
		}
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeContext::CancelPendingOps
// Affects also obsolete records.
// ----------------------------------------------------------------------------
//   
void CSipSecAgreeContext::CancelPendingOps(
    const MSIPSecSecurityMechanismObserver* aObserver )
    {
    if ( iObserver && iObserver == aObserver )
        {
        // Observer cannot be used anymore
        iObserver = NULL;
        for ( TInt i = 0; i < iSecAgreeRecords.Count(); ++i )
		    {
		    iSecAgreeRecords[ i ]->CancelPendingOps();
		    }
        }
    }

// ----------------------------------------------------------------------------
// CSipSecAgreeContext::SADBAddMsgReceived
// ----------------------------------------------------------------------------
//
void CSipSecAgreeContext::SADBAddMsgReceived( TUint aMsgSeq, TInt aError )
    {    
    TInt pos( 0 );
	CSipSecAgreeRecord* record( NULL );
	while ( ( record = NextRecord( pos ) ) != NULL )	    
        {
        record->SADBAddMsgReceived( aMsgSeq, aError );
        }
    }

// ----------------------------------------------------------------------------
// CSipSecAgreeContext::RemovalCompleted
// Affects also obsolete records.
// ----------------------------------------------------------------------------
//
void CSipSecAgreeContext::RemovalCompleted( TUint32 aTransportId )
    {    
    for ( TInt i = 0; i < iSecAgreeRecords.Count(); ++i  )
        {
        iSecAgreeRecords[ i ]->RemovalCompleted( aTransportId );
        }
    }

// ----------------------------------------------------------------------------
// CSipSecAgreeContext::DeleteRecord
// Affects also obsolete records.
// ----------------------------------------------------------------------------
//
void CSipSecAgreeContext::DeleteRecord( CSipSecAgreeRecord* aRecord )
	{
	TInt i = iSecAgreeRecords.Find( aRecord );
	iSecAgreeRecords.Remove( i );
	iSecAgreeRecords.Compress();
	delete aRecord;	
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeContext::NextRecord
// ----------------------------------------------------------------------------
//
CSipSecAgreeRecord* CSipSecAgreeContext::NextRecord( TInt& aPos ) const
	{
	for ( TInt i = aPos; i < iSecAgreeRecords.Count(); ++i )
		{
		if ( !iSecAgreeRecords[ i ]->IsObsolete() )
			{
			// Set aPos point to next record
			aPos = i + 1;
			return iSecAgreeRecords[ i ];
			}
		}
	return NULL;
	}

// End of File
