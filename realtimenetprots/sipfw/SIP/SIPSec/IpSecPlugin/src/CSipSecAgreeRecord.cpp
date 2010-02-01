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
// Name          : CSipSecAgreeRecord.cpp
// Part of       : SIPSec, IpSec Plugin
// Version       : SIP/5.0 
//



#include <networking/pfkeyv2.h>
#include "CSipSecAgreeRecord.h"
#include "sipsecurityheaderbase.h"
#include "sipsecurityverifyheader.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "siprouteheader.h"
#include "sipcontactheader.h"
#include "sipaddress.h"
#include "uricontainer.h"
#include "sipuri.h"
#include "siphostport.h"
#include "sipstrings.h"
#include "SipLogs.h"
#include "MSIPTransportMgr.h"
#include "CIpSecSadbHandler.h"
#include "MIpSecAgreeContextParams.h"
#include "MIpSecMechanismParams.h"
#include "SipSecUtils.h"
#include "SipAssert.h"

#ifdef CPPUNIT_TEST
#include "CActiveObjController1.h"
#include "CSipSecSaDbMsgSender.h"
#endif

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::NewL
// ----------------------------------------------------------------------------
//
CSipSecAgreeRecord* CSipSecAgreeRecord::NewL( 
	RPointerArray<CState>& aStates,
	MIpSecAgreeContextParams& aParams,
	const TSIPTransportParams& aTransportParams )
	{
	CSipSecAgreeRecord* self = 
	    CSipSecAgreeRecord::NewLC( aStates, aParams, aTransportParams );
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::NewLC
// ----------------------------------------------------------------------------
//
CSipSecAgreeRecord* CSipSecAgreeRecord::NewLC( 
	RPointerArray<CState>& aStates,
	MIpSecAgreeContextParams& aParams,
	const TSIPTransportParams& aTransportParams )
	{
	CSipSecAgreeRecord* self = 
	    new ( ELeave ) CSipSecAgreeRecord( aStates, aParams, aTransportParams );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::CSipSecAgreeRecord
// ----------------------------------------------------------------------------
//
CSipSecAgreeRecord::CSipSecAgreeRecord( 
	RPointerArray<CState>& aStates,
	MIpSecAgreeContextParams& aParams,
	const TSIPTransportParams& aTransportParams ) :
	iParams( aParams ),
	iTransportParams( aTransportParams ),
	iStates( aStates ),
	iSAsDeleted( EFalse ),
	iPolicyUnloaded( EFalse ),
	iNumAsyncRemovals( 0 )
#ifdef CPPUNIT_TEST
    , iSecClientHeaders( 1 ),
    iSecVerifyHeaders( 1 )
#endif

	{
	NextState( CState::ETentative );
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::ConstructL
// ----------------------------------------------------------------------------
//
void CSipSecAgreeRecord::ConstructL()
	{
	iSadbHandler = 
	    CIpSecSadbHandler::NewL( *this, iParams.MechParams().Sadb() );
	iPolicyHandler = CIpSecPolicyHandler::NewL( *this, iParams.PolicyServer() );			

#ifdef CPPUNIT_TEST
	iObjCtr = new ( ELeave ) CActiveObjController1();
	iObjCtr->iRecord = this;
#endif	
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::~CSipSecAgreeRecord
// ----------------------------------------------------------------------------
//
CSipSecAgreeRecord::~CSipSecAgreeRecord()
	{
	iSecClientHeaders.ResetAndDestroy();	
	iSecVerifyHeaders.ResetAndDestroy();	
	delete iPolicyHandler;
	delete iSadbHandler;
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::SAsAddedL
// ----------------------------------------------------------------------------
// 
void CSipSecAgreeRecord::SAsAddedL( TBool aSuccess )
    {
    if ( aSuccess )
        {
        __SIP_LOG( "IPSecPlugin::SAsAddedL():Success" )
        iState->SaAddedL( *this );
        }
    else
        {
        __SIP_LOG( "IPSecPlugin::SAsAddedL():Failed" )
        iParams.SAReady( EFalse );        
        }        
    }

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::SAsDeletedL
// ----------------------------------------------------------------------------
// 
void CSipSecAgreeRecord::SAsDeletedL()
    {
    __SIP_LOG( "IPSecPlugin::SAsDeletedL()" )
    
    // Must remember deletion completion at this level as SA deletion states
    // ("deleting" and "clearing") must be aware of it. They may initiate call
    // leading to deletion of record and/or context but this can be only done
    // after both unloading the policy AND deleting SAs is done.
    iSAsDeleted = ETrue;
    iState->SaDeleted( *this );
    }

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::PolicyActivatedL
// ----------------------------------------------------------------------------
//
void CSipSecAgreeRecord::PolicyActivatedL()
    {
    __SIP_LOG( "IPSecPlugin::PolicyActivatedL()" )
    
    iState->PolicyActivatedL( *this );
    }

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::PolicyUnloaded
// ----------------------------------------------------------------------------
//      
void CSipSecAgreeRecord::PolicyUnloaded()
    {
    __SIP_LOG( "IPSecPlugin::PolicyUnloaded()" )
    
    // Must remember unloading completion at this level as SA deletion states
    // ("deleting" and "clearing") must be aware of it. They may initiate call
    // leading to deletion of record and/or context but this can be only done
    // after both unloading the policy AND deleting SAs is done.
    iPolicyUnloaded = ETrue;
    iState->PolicyUnloaded( *this );
    }

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::PolicyError
// ----------------------------------------------------------------------------
//
void CSipSecAgreeRecord::PolicyError( 
    CIpSecPolicyHandler::TPolicyHandlerState aState,
    TInt /*aError*/ )
    {
     __SIP_LOG( "IPSecPlugin::PolicyError()" )
     
    if ( aState == CIpSecPolicyHandler::ELoading ||
         aState == CIpSecPolicyHandler::EActivating )
        {
        iParams.SAReady( EFalse );
        }
    }

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::TimerExpiredL
// ----------------------------------------------------------------------------
//
void CSipSecAgreeRecord::TimerExpiredL( TTimerId aTimerId, TAny* aTimerParam )
	{
	iState->TimerExpiredL( *this, aTimerId, aTimerParam );
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::Resp4xxL
// ----------------------------------------------------------------------------
//				   
void CSipSecAgreeRecord::Resp4xxL( CSIPResponse& aResponse,
								   CSIPRequest& aRequest )
	{
	iState->Resp4xxL( *this, aResponse, aRequest );
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::AuthKeyL
// ----------------------------------------------------------------------------
//	
TBool CSipSecAgreeRecord::AuthKeyL( const TDesC8& aAuthKey )
	{
	return iState->AuthKeyL( *this, aAuthKey );
	}
	
// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::AuthKeyFailedL
// ----------------------------------------------------------------------------
//	
void CSipSecAgreeRecord::AuthKeyFailedL()
    {
    iState->AuthKeyFailedL( *this );
    }
	
// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::RegisterL
// ----------------------------------------------------------------------------
//    
void CSipSecAgreeRecord::RegisterL( TSIPTransportParams& aTransportParams,
									CSIPRequest& aRequest,
									const TDesC8& aOutboundProxy )
	{
	iState->RegisterL( *this, aRequest, aTransportParams, aOutboundProxy );	
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::ReguestL
// ----------------------------------------------------------------------------
//
void CSipSecAgreeRecord::ReguestL( TSIPTransportParams& aTransportParams,
								   CSIPRequest& aRequest,
								   const TDesC8& aOutboundProxy )
	{
	iState->ReguestL( *this, aRequest, aTransportParams, aOutboundProxy );	
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::Resp2xxL
// ----------------------------------------------------------------------------
//	
void CSipSecAgreeRecord::Resp2xxL( CSIPResponse& aResponse )
	{
	iState->Resp2xxL( *this, aResponse );	
	}
	
// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::ProcessSecVerifyL
// ----------------------------------------------------------------------------
//	
void CSipSecAgreeRecord::ProcessSecVerifyL( 
	RPointerArray<CSIPSecurityVerifyHeader>& aSecurityVerify )
    {
    iState->ProcessSecVerifyL( *this, aSecurityVerify );
    }

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::ClearSaL
// ----------------------------------------------------------------------------
//		
void CSipSecAgreeRecord::ClearSaL()
	{
	iState->ClearSaL( *this );
	}
	
// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::State
// ----------------------------------------------------------------------------
//
CState::TSecAgreeRecordState CSipSecAgreeRecord::State() const
	{
	return iState->State();
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::ReserveTransportAndCreatePolicyL
// ----------------------------------------------------------------------------
//	
void CSipSecAgreeRecord::ReserveTransportAndCreatePolicyL()
    {
    TInetAddr remote = iParams.RemoteAddress();
    remote.SetPort( iSaParams.iPort_ps );
    iParams.MechParams().TransportMan().ReserveTransportL( iTransportParams,
                                                           iSaParams.iPort_uc,
                                                           iSaParams.iPort_us,
                                                           remote,
                                                           iSaParams.iPort_pc );                                                           
    CreatePolicyL();
    }

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::RemoveTransportL
// ----------------------------------------------------------------------------
//	
void CSipSecAgreeRecord::RemoveTransportL()
    {
    // If transport id is more than zero, transport has been reserved and must
    // be freed
    TUint32 tpId( iTransportParams.TransportId() );
    if ( tpId > 0 )
        { 
        iNumAsyncRemovals = 0;

        // Signal to ConnectionMgr "Stop using associated port numbers"
        MIpSecMechanismParams& mechParams = iParams.MechParams();

        iNumAsyncRemovals = mechParams.TransportMan().RemoveTransport(
					                tpId,
					                mechParams.TransportRemovalObserver() );
        // Connected transport protocols may need to exchange some protocol
        // specific messages in order to close connections. In that case, IPsec
        // policy and SAs cannot be removed until those protocol specific
        // operations have been completed.
        if ( !iNumAsyncRemovals )
            {
	        RemovePolicy();
        	DeleteSasL();	
            }
        }
    }

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::CreatePolicyL
// ----------------------------------------------------------------------------
//	
void CSipSecAgreeRecord::CreatePolicyL()
    {
#ifdef CPPUNIT_TEST
	iObjCtr->AddCreatePolicyL( iPolicyHandler );
#endif

    iPolicyHandler->CreatePolicyL( iParams.LocalAddress(), 
                                   iParams.RemoteAddress(), 
                                   iSaParams );
    }
    
// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::RemovePolicy
// ----------------------------------------------------------------------------
//	    
void CSipSecAgreeRecord::RemovePolicy()
    {
    iPolicyUnloaded = EFalse;
    
#ifdef CPPUNIT_TEST
	TRAP_IGNORE( iObjCtr->AddRemovePolicyL( iPolicyHandler ) )
#endif

    iPolicyHandler->RemovePolicy();
    }

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::CancelPendingOps
// ----------------------------------------------------------------------------
//	
void CSipSecAgreeRecord::CancelPendingOps()
    {
    // Cancel all SADB_ADD operations
    iSadbHandler->CancelAddSaOps();
			
    // Cancel also all policy operations
	iPolicyHandler->Cancel();
    }

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::AddSasL
// ----------------------------------------------------------------------------
//	
void CSipSecAgreeRecord::AddSasL()
    {
    iSadbHandler->AddSasL( iParams.MechParams(), 
                           iParams.LocalAddress(), 
                           iParams.RemoteAddress(),
                           iSaParams );
#ifdef CPPUNIT_TEST
    iObjCtr->AddRecrdAddL( iSadbHandler->iSaDbMsgSender );
#endif
    }

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::DeleteSasL
// ----------------------------------------------------------------------------
//		
void CSipSecAgreeRecord::DeleteSasL()
    {
    iSAsDeleted = EFalse;
    
    iSadbHandler->DeleteSasL( iParams.MechParams(), 
                              iParams.LocalAddress(), 
                              iParams.RemoteAddress(),
                              iSaParams );
#ifdef CPPUNIT_TEST
    iObjCtr->AddRecrdDelL( iSadbHandler->iSaDbMsgSender );
#endif
    }

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::SetSaParamsL
// ----------------------------------------------------------------------------
//		
void CSipSecAgreeRecord::SetSaParamsL( CSIPSecurityHeaderBase& aCliHeader, 
									   CSIPSecurityHeaderBase& aSrvHeader,
									   const TDesC8& aAuthKey )
    {
    iSadbHandler->SetSaParamsL( iSaParams, aCliHeader, aSrvHeader, aAuthKey );
    }

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::SADBAddMsgReceived
// ----------------------------------------------------------------------------
//       
void CSipSecAgreeRecord::SADBAddMsgReceived( TUint aMsgSeq, TInt aError )
    {
    iSadbHandler->SADBAddMsgReceived( aMsgSeq, aError );
    }

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::RemovalCompleted
// ----------------------------------------------------------------------------
//     		
void CSipSecAgreeRecord::RemovalCompleted( TUint32 aTransportId )
    {
    if ( aTransportId != iTransportParams.TransportId() || !iNumAsyncRemovals )
        {
        return;
        }

    // Policy and SAs can be removed when all async transport removals have
    // been completed.
    iNumAsyncRemovals--;
    if ( !iNumAsyncRemovals )
        {
        RemovePolicy();

        TRAPD( saError, DeleteSasL() );
        if ( saError )
            {
            // Even if SA removal initiation failed, continue with policy removal
            iSAsDeleted = ETrue;
            }
        }
    }

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::ExpiresAfter
// ----------------------------------------------------------------------------
//
TUint CSipSecAgreeRecord::ExpiresAfter() const
    {
    TUint expiresAfter( 0 );

    if ( iParams.MechParams().TimerMan().IsRunning( iTimerId ) )
		{
		iParams.MechParams().TimerMan().ExpiresAfter( iTimerId, expiresAfter );
		}

    return expiresAfter;
    }

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::TransportParams
// ----------------------------------------------------------------------------
//
const TSIPTransportParams& CSipSecAgreeRecord::TransportParams() const
	{
	return iTransportParams;	
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::IsObsolete
// ----------------------------------------------------------------------------
//
TBool CSipSecAgreeRecord::IsObsolete() const
	{
	return iObsoleteRecord;
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::SetObsoleteL
// If in Deleting/Clearing state, just continue asynchronous stopping.
// ----------------------------------------------------------------------------
//
void CSipSecAgreeRecord::SetObsoleteL()
	{
	__SIP_ASSERT_LEAVE( !iObsoleteRecord, KErrAlreadyExists );
 
	iObsoleteRecord = ETrue;

	if ( ( State() != CState::EDeleting ) && ( State() != CState::EClearing ) )
		{
		const TInt KExpireImmediately = 1;
		StartTimerL( KExpireImmediately );
		NextState( CState::EDying );
		}	
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::SecurityHeaders
// ----------------------------------------------------------------------------
//
RPointerArray<CSIPSecurityHeaderBase>&
CSipSecAgreeRecord::SecurityHeaders( TBool aIsSecClientHeader )
	{
	if ( aIsSecClientHeader )
	    {
		return iSecClientHeaders;
	    }
	return iSecVerifyHeaders;
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::CopyHeadersToMessageL
// ----------------------------------------------------------------------------
//	
void CSipSecAgreeRecord::CopyHeadersToMessageL( CSIPMessage& aMessage,
												TBool aIsSecCliHeaders )
	{
	RPointerArray<CSIPSecurityHeaderBase>&
		headers = SecurityHeaders( aIsSecCliHeaders );

	CopyHeadersToMessageL( headers, aMessage, aIsSecCliHeaders );
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::CopyHeadersFromMessageL
// ----------------------------------------------------------------------------
//	
void CSipSecAgreeRecord::CopyHeadersFromMessageL( CSIPMessage& aMessage,
												  TBool aIsSecCliHeaders )
	{
	RPointerArray<CSIPSecurityHeaderBase>&
		headers = SecurityHeaders( aIsSecCliHeaders );

    CopyHeadersFromMessageL( aMessage, headers, aIsSecCliHeaders );	
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::UpdateNextHopPort
// ----------------------------------------------------------------------------
//	
void CSipSecAgreeRecord::UpdateNextHopPort( CSIPRequest& aRequest, 
    										const TDesC8& aOutboundProxy )
    {
    const RStringF route = SIPStrings::StringF(SipStrConsts::ERouteHeader);
    if ( aRequest.HasHeader( route ) )
		{
        TSglQueIter<CSIPHeaderBase> iter = aRequest.Headers( route );

        iter.SetToFirst();
        CSIPHeaderBase* header = iter;

		// Update only the topmost header
	    if ( header )
	        {
	       	CSIPRouteHeader* routeHeader =
				static_cast<CSIPRouteHeader*>( header );
	        if ( UpdateURIPort( &routeHeader->SIPAddress().URI(),
	                            aOutboundProxy ) )
	            {
	            // Port of top most route header was updated
	            return;
	            }
	        }
		}

	// Didn't found matching address from top route header,
	// nexthop address might be in request uri
    UpdateURIPort( aRequest.RequestURI(), aOutboundProxy );
    }

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::StartTimerL
// ----------------------------------------------------------------------------
//		
void CSipSecAgreeRecord::StartTimerL( TUint aMilliseconds )
	{
	StopTimer();
	iTimerId = iParams.MechParams().TimerMan().StartL( this, aMilliseconds );
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::StopTimer
// ----------------------------------------------------------------------------
//		
void CSipSecAgreeRecord::StopTimer()
	{
	iParams.MechParams().TimerMan().Stop( iTimerId );	
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::NextState
// ----------------------------------------------------------------------------
//		
void CSipSecAgreeRecord::NextState( CState::TSecAgreeRecordState aState )
	{ 
    __SIP_INT_LOG1( "IPSecPlugin::NextState", aState )

	iState = iStates[ aState ];
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::UnloadingAndDeletionDone
// ----------------------------------------------------------------------------
//	
TBool CSipSecAgreeRecord::UnloadingAndDeletionDone() const
    {
    return iPolicyUnloaded && iSAsDeleted;
    }

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::UpdateURIPort
// ----------------------------------------------------------------------------
//
TBool CSipSecAgreeRecord::UpdateURIPort( CURIContainer* aURIContainer, 
    									 const TDesC8& aOutboundProxy )
    {
    if ( aURIContainer && aURIContainer->IsSIPURI() )
	    {
        CSIPHostPort& hostPort = aURIContainer->SIPURI()->HostPort();
        if ( hostPort.Host() == aOutboundProxy )
            {
            hostPort.SetPort( iSaParams.iPort_ps );
            return ETrue;
            }
        }
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::CopyHeadersToMessageL
// ----------------------------------------------------------------------------
//	
void CSipSecAgreeRecord::CopyHeadersToMessageL( 
    RPointerArray<CSIPSecurityHeaderBase>& aHeaders,
	CSIPMessage& aMessage,
	TBool aIsSecCliHeaders )
	{
	// In case of sec client hdrs, remove existing ones
    if ( aIsSecCliHeaders )
        {
        aMessage.DeleteHeaders(
            SIPStrings::StringF( SipStrConsts::ESecurityClientHeader ) );
        }

	for ( TInt i = 0; i < aHeaders.Count(); i++ )
		{
		CSIPHeaderBase* header = aHeaders[ i ]->CloneL();
		CleanupStack::PushL( header );
		aMessage.AddHeaderL( header );
		CleanupStack::Pop( header );
		}
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::CopyHeadersFromMessageL
// ----------------------------------------------------------------------------
//	
void CSipSecAgreeRecord::CopyHeadersFromMessageL( 
	CSIPMessage& aMessage,
	RPointerArray<CSIPSecurityHeaderBase>& aHeaders,
	TBool aIsSecCliHeaders )
	{
	CSIPSecurityHeaderBase* shb = NULL;
	SipStrConsts::TStrings
		str = aIsSecCliHeaders ? SipStrConsts::ESecurityClientHeader :
		                         SipStrConsts::ESecurityServerHeader;
	TBool found = EFalse;
	TSglQueIter<CSIPHeaderBase> sh =
		SipSecUtils::SecHeaders( aMessage, str, found );
	if ( found )
		{
		aHeaders.ResetAndDestroy();
		sh.SetToFirst();

		CSIPSecurityHeaderBase* sch = NULL;
		while (	( sch = static_cast<CSIPSecurityHeaderBase*>( &( *sh++ ) ) ) 
		    != NULL )
			{
			// Do the job
			if ( aIsSecCliHeaders )
				{
				shb = static_cast<CSIPSecurityHeaderBase*>( sch->CloneL() );
				}
			else
				{
				// shb = Create Sec-Verify from Sec-Server header
				shb = static_cast<CSIPSecurityHeaderBase*>
					(CSIPSecurityVerifyHeader::NewL( *sch ));
				}
			CleanupStack::PushL( shb );
			aHeaders.AppendL( shb );
			CleanupStack::Pop( shb );
			}
		}	
	}

// ----------------------------------------------------------------------------
// CSipSecAgreeRecord::ExpiresFromContactL
// ----------------------------------------------------------------------------
//
TUint CSipSecAgreeRecord::ExpiresFromContactL( CSIPMessage& aMessage )
    {
    // Get expires param from Contact header
	CSIPContactHeader*
		ch = static_cast<CSIPContactHeader*>(
			SipSecUtils::SingleHeader( aMessage,
	                            	   SipStrConsts::EContactHeader ) );
	__ASSERT_ALWAYS( ch, User::Leave( KErrNotFound ) );

    return SipSecUtils::ValueL( *ch, _STRINGF( SipStrConsts::EExpires ).DesC() );
    }
	
// End of File
