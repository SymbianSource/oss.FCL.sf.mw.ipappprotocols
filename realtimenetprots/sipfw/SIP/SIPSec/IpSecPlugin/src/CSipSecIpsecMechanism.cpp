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
// Name          : CSipSecIpsecMechanism.cpp
// Part of       : SIPSec
// Version       : SIP/6.0 
//



#include <pfkey_send.h> 

#include "sipstrings.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "sipsecurityclientheader.h"
#include "TSIPTransportParams.h"
#include "SipLogs.h"

#include "CSipSecIpsecMechanism.h"
#include "SipSecUtils.h"
#include "CSipSecAgreeContext.h"
#include "CSipSecSaDbMsgSender.h"
#include "CSipSecSaDbMsgReceiver.h"
#include "CStateTentative.h"
#include "CStatePendingKey.h"
#include "CStatePendingSa.h"
#include "CStatePendingPolicy.h"
#include "CStateTemporal.h"
#include "CStateInUse.h"
#include "CStateOld.h"
#include "CStateDying.h"
#include "CStateDeleting.h"
#include "CStateClearing.h"

#ifdef CPPUNIT_TEST
#include "CActiveObjController1.h"
#endif

const TInt KStatesGranularity = 10;

// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::NewL
// ----------------------------------------------------------------------------
//
CSipSecIpsecMechanism* CSipSecIpsecMechanism::NewL( TAny* aInitParam )
	{
	CSipSecIpsecMechanism* self = CSipSecIpsecMechanism::NewLC( aInitParam );
	CleanupStack::Pop( self );
	return self;
	}
	
// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::NewLC
// ----------------------------------------------------------------------------
//
CSipSecIpsecMechanism* CSipSecIpsecMechanism::NewLC( TAny* aInitParam )
	{
	TSIPSecMechanismInitParams* p = 
		static_cast<TSIPSecMechanismInitParams*>( aInitParam );
	CSipSecIpsecMechanism* 
		self = new ( ELeave ) CSipSecIpsecMechanism( p );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
//
CSipSecIpsecMechanism::~CSipSecIpsecMechanism()
	{
	delete iSaDbMsgReceiver;
	delete iSaDbMsgSender;
	iSecAgreeContexts.ResetAndDestroy();
	iSecAgreeContexts.Close();
	iStates.ResetAndDestroy();
	iStates.Close();
	iPolicyServ.Close();
	iSaDb.Close();
	iSockServ.Close();
	}

// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::Name
// ----------------------------------------------------------------------------
//
const TDesC8& CSipSecIpsecMechanism::Name() const
	{
	return KIpSec3gpp;
	}

// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::InitializeSecurityClientL
// ----------------------------------------------------------------------------
//
void CSipSecIpsecMechanism::InitializeSecurityClientL(
    CSIPSecurityClientHeader& /*aSecurityClient*/ )
    {
    }

// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::ProcessSecurityVerifyL
// ----------------------------------------------------------------------------
//
void CSipSecIpsecMechanism::ProcessSecurityVerifyL(
	TSIPTransportParams& /*aTransportParams*/,
    CSIPRequest& aRequest,
    TInetAddr& aNextHop,
    const CUri8& /*aRemoteTarget*/,
    const TDesC8& /*aOutboundProxy*/,
    MSIPSecUser* /*aUser*/,
    TRegistrationId /*aRegistrationId*/,
    RPointerArray<CSIPSecurityServerHeader>& /*aSecurityServer*/,
	RPointerArray<CSIPSecurityVerifyHeader>& aSecurityVerify )
    {
    // No action if verify hdrs exist already in the request
    if ( aRequest.HasHeader( 
            SIPStrings::StringF( SipStrConsts::ESecurityVerifyHeader ) ) )
        {
        return;
        }
            
    // In case when two records exist already but more recent record is not
    // yet in "temporal" state, ipsec still has to use old sec verify hdrs.
    // FW would like to already use new sec verifys instead.
    CSipSecAgreeContext* c = FindContext( aNextHop );
	if ( c )
		{
		c->ApplyRulesL( aSecurityVerify );
		}
    }

// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::AddSecurityParamsL
// ----------------------------------------------------------------------------
//
void CSipSecIpsecMechanism::AddSecurityParamsL(
    TSIPTransportParams& aTransportParams,
	CSIPRequest& aRequest,
	TRegistrationId /*aRegistrationId*/,
	TTransactionId /*aTransactionId*/,
	TInetAddr& aNextHop,
	const CUri8& /*aRemoteTarget*/,
	const TDesC8& aOutboundProxy,
	MSIPSecUser* aUser )
	{
	CSipSecAgreeContext* c = FindContext( aNextHop );
	if ( c )
		{
		c->ApplyRulesL( aTransportParams, aRequest, aOutboundProxy, aUser );
		}
	else
		{
	 	ProcessInitialReqisterL( aTransportParams, aRequest );
		}
	}
	
// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::ResponseReceivedL
// ----------------------------------------------------------------------------
//
TBool CSipSecIpsecMechanism::ResponseReceivedL(
    TSIPTransportParams& aTransportParams,
	CSIPResponse& aResponse,
	CSIPRequest& aRequest,
	TRegistrationId /*aRegistrationId*/,
	TTransactionId /*aTransactionId*/,
	TInetAddr& aNextHop,
	const CUri8& /*aRemoteTarget*/,
	const TDesC8& /*aOutboundProxy*/,
	MSIPSecUser* aUser,
	MSIPSecSecurityMechanismObserver& aObserver )
	{		
	CSipSecAgreeContext* c = FindContext( aUser );
	if ( c )
		{
		c->ApplyRulesL( aResponse, aRequest, aObserver );
		}
	else
	    {
	    // No context was found for the user, create possibly new one
	    CreateNewContextL( aTransportParams, 
	                       aResponse, 
	                       aRequest,
	                       aNextHop, 
	                       aUser, 
	                       aObserver );
	    }
    
	return EFalse;
	}

// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::IsServerInitiatedSecAgreeAllowed
// ----------------------------------------------------------------------------
//	
TBool CSipSecIpsecMechanism::IsServerInitiatedSecAgreeAllowed() const
    {
    return EFalse;
    }
	
// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::ParametersUpdatedL
// ----------------------------------------------------------------------------
//   
TBool CSipSecIpsecMechanism::ParametersUpdatedL( MSIPSecUser* aUser )
    {
    // Digest has possibly updated keying material
    CSipSecAgreeContext* c = FindContext( aUser );
	if ( c )
		{
		const TDesC8* key = 0;
		
	    TRAPD( err, key = &iEngineContext.ValueL( KSIPSecIK ) );
	    if ( err == KErrNone )
	        {
	    	return c->SetAuthKeyL( *key );
	        }
	    c->AuthKeyFailedL();
		}
    return EFalse;
    }
    
// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::CancelPendingOperations
// ----------------------------------------------------------------------------
//					    
void CSipSecIpsecMechanism::CancelPendingOperations(
	MSIPSecSecurityMechanismObserver* aObserver )
	{
	if ( !aObserver )
	    {
	    return;
	    }
	    
	// Must cancel outstanding add SA requests
	for ( TInt i = 0; i < iSecAgreeContexts.Count(); i++ )
		{
		iSecAgreeContexts[ i ]->CancelPendingOps( aObserver );
		}
	}

// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::ClearCache
// ----------------------------------------------------------------------------
//
void CSipSecIpsecMechanism::ClearCache(
	MSIPSecUser* aUser )
	{
	CSipSecAgreeContext* c = FindContext( aUser );
	if ( c )
		{
		TRAPD( err, c->ClearL() );
		if ( err != KErrNone )
			{
			DeleteContext( c );
			}
		}
	}

// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::SetCredentialsL
// ----------------------------------------------------------------------------
//
void CSipSecIpsecMechanism::SetCredentialsL(
	TTransactionId /*aTransactionId*/,
	const TDesC8& /*aRealm*/,
	const TDesC8& /*aOutboundProxy*/, 
	const TDesC8& /*aUserName*/,
	const TDesC8& /*aPassword*/ )
	{
	}

// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::SetCredentialsL
// ----------------------------------------------------------------------------
//
void CSipSecIpsecMechanism::SetCredentialsL(
	const MSIPSecUser& /*aUser*/,
	const TDesC8& /*aRealm*/,
	const TDesC8& /*aOutboundProxy*/, 
	const TDesC8& /*aUserName*/,
	const TDesC8& /*aPassword*/ )
	{
	}

// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::IgnoreChallenge
// ----------------------------------------------------------------------------
//	
TInt CSipSecIpsecMechanism::IgnoreChallenge( 
    TTransactionId /*aTransactionId*/,
    const TDesC8& /*aRealm*/,
    const MSIPSecUser* /*aTrustedUser*/ )
    {
    return KErrNotFound;
    }

// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::SetCredentialsL
// ----------------------------------------------------------------------------
//
TInt CSipSecIpsecMechanism::RemoveCredentials( const TDesC8& /*aRealm*/ )
    {
    return KErrNotFound;
    }
	
// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::MessageSentL
// ----------------------------------------------------------------------------
//
void CSipSecIpsecMechanism::MessageSentL( TInt /*aStatus*/, TInt /*aMsgType*/ )
	{
	}

// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::MessageReceived
// ----------------------------------------------------------------------------
//
void CSipSecIpsecMechanism::MessageReceived( TInt aStatus )
	{
	if ( aStatus == KErrNone && 
	     iReceiveBuffer.Length() > 0 && 
	     iReceiveBuffer.MsgHdr().sadb_msg_pid == PID )
	    {
	    ConsumeSadbMessage();
	    }
	    
	iReceiveBuffer.FillZ();
	iSaDbMsgReceiver->Receive( iReceiveBuffer );
	}

// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::T1
// ----------------------------------------------------------------------------
//
TUint CSipSecIpsecMechanism::T1()
	{
	return iT1;
	}
	
// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::Sadb
// ----------------------------------------------------------------------------
//	
RSADB& CSipSecIpsecMechanism::Sadb()
	{
	return iSaDb;
	}

// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::PolicyServer
// ----------------------------------------------------------------------------
//	
RIpsecPolicyServ& CSipSecIpsecMechanism::PolicyServer()
    {
    return iPolicyServ;
    }

// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::SeqNumber
// ----------------------------------------------------------------------------
//	
TUint CSipSecIpsecMechanism::SeqNumber()
	{
	return ++iSeqNum;
	}

// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::TimerMan
// ----------------------------------------------------------------------------
//
MTimerManager& CSipSecIpsecMechanism::TimerMan()
	{
	return iTimerMgr;
	}
	
// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::TransportMan
// ----------------------------------------------------------------------------
//
MSIPTransportMgr& CSipSecIpsecMechanism::TransportMan()
    {
    return iTransportMgr;
    }

// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::UpdateSecCliL
// ----------------------------------------------------------------------------
//
void CSipSecIpsecMechanism::UpdateSecCliL(
	RPointerArray<CSIPSecurityHeaderBase>& aSecCliHeaders )
	{
	// Update protected client port and both SPIs
	//
	TUint spic = SipSecUtils::RandomNumber( iSeed, KMinSPI, KMaxSPI );	
	TUint spis = SipSecUtils::RandomNumber( iSeed, KMinSPI, KMaxSPI );	
	TUint portc = SipSecUtils::RandomNumber( iSeed, KMinPort, KMaxPort );
	
	for ( TInt i = 0; i < aSecCliHeaders.Count(); i++ )
	    {
	    CSIPSecurityClientHeader* sch = 
	        static_cast<CSIPSecurityClientHeader*>( aSecCliHeaders[ i ] );
	     
	    if ( SipSecUtils::HasParamL( *sch, KSpic ) &&
	         SipSecUtils::HasParamL( *sch, KSpis ) &&
	         SipSecUtils::HasParamL( *sch, KPortc ) )
	        {
	        SipSecUtils::SetParamL( *sch, KSpic, spic );
	        SipSecUtils::SetParamL( *sch, KSpis, spis );
	        SipSecUtils::SetParamL( *sch, KPortc, portc );
	        }
	    }
	}

// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::ContextCleared
// ----------------------------------------------------------------------------
//
void CSipSecIpsecMechanism::ContextCleared(
	CSipSecAgreeContext* aContext )
	{
	DeleteContext(aContext);
	}

// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::TransportRemovalObserver
// ----------------------------------------------------------------------------
//
MSIPTransportRemovalObserver* CSipSecIpsecMechanism::TransportRemovalObserver()
    {
    return this;
    }

// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::ProcessInitialReqisterL
// ----------------------------------------------------------------------------
//
void CSipSecIpsecMechanism::ProcessInitialReqisterL( 
    TSIPTransportParams& aTransportParams,
    CSIPRequest& aRequest )
	{	
	// 1. Check if Security-Client header is in request
	// 2. Generate IPSEC port numbers if not yet in the request
	// 3. Generate SPIs
	// 4. Do not have to reserve channels yet
	// 5. Disable SigComp usage for initial register
	
	if ( !SipSecUtils::Match( SipStrConsts::ERegister, aRequest.Method() ) )
		{
		return;
		}

	// Ownership of headers inside the returned array is not transferred. 
	// Returned array has been pushed into cleanupstack using TCleanupItem
	// which will call only Reset() for RPointerArray and destructs
	// the object if leave occurs.
	RPointerArray< CSIPSecurityHeaderBase >* secClientHdrs =
    	SipSecUtils::HeadersWithMechanismLC( 
		        aRequest,
				SipStrConsts::ESecurityClientHeader,
				KIpSec3gpp );
	
	TInt ipsecHdrsCount( secClientHdrs->Count() );
	
	if ( ipsecHdrsCount > 0 )
	    {
	    __ASSERT_ALWAYS( ipsecHdrsCount <= 2, User::Leave( KErrOverflow ) );
	    
	    CSIPSecurityClientHeader* sch1 = 
	        static_cast< CSIPSecurityClientHeader* >( (*secClientHdrs)[ 0 ] );
	    InitSecCliHeaderL( sch1 );

	   	if ( iCaps.iAlgMd5.Length() && iCaps.iAlgSha.Length() )
	   	    {
	   	    CSIPSecurityClientHeader* sch2 = 
	            static_cast<CSIPSecurityClientHeader*>( sch1->CloneL() );
	            
	        CleanupStack::PushL( sch2 );
		    SipSecUtils::SetParamL( *sch2, KAlgorithm, iCaps.iAlgSha );
		    if ( ipsecHdrsCount == 1 )
		        {
		        aRequest.AddHeaderL( sch2 );
		        }
		    else
		        {
		        aRequest.ReplaceHeaderL( (*secClientHdrs)[ 1 ], sch2 );
		        }
		    CleanupStack::Pop( sch2 );
	   	    }
	    
	    aTransportParams.SetIgnoreCompartmentId( ETrue );
	    }
    
    CleanupStack::PopAndDestroy( secClientHdrs );
	}
	
// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::RemovalCompleted
// ----------------------------------------------------------------------------
//   
void CSipSecIpsecMechanism::RemovalCompleted( TUint32 aTransportId )
    {
    for ( TInt i = 0; i < iSecAgreeContexts.Count(); i++ )
		{
		iSecAgreeContexts[ i ]->RemovalCompleted( aTransportId );
		}
    }
    
// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::Capable
// ----------------------------------------------------------------------------
//
TBool CSipSecIpsecMechanism::Capable()
	{
	return iCaps.iProt.Length() && iCaps.iMode.Length() && 
		   ( iCaps.iAlgMd5.Length() || iCaps.iAlgSha.Length() );
	}
		
// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::ConsumeSadbMessage
// ----------------------------------------------------------------------------
//
void CSipSecIpsecMechanism::ConsumeSadbMessage()
	{
	switch ( iReceiveBuffer.MsgHdr().sadb_msg_type )
		{
		case SADB_REGISTER:
		    {
			SadbRegister();
		    break;
		    }
		case SADB_ADD:
		    {
		    SadbAdd();
		    break;
		    }
		default:
		    {
    		// SADB_GETSPI, SADB_UPDATE
    		// SADB_DELETE, SADB_ACQUIRE, SADB_FLUSH, SADB_GET, SADB_EXPIRE
    		break;
		    }
		}
	}

// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::SadbRegister
// ----------------------------------------------------------------------------
//
void CSipSecIpsecMechanism::SadbRegister()
	{
	TPfkeyAnyExt ext;
	TInt s = iReceiveBuffer.NextExtension( ext );
	__ASSERT_DEBUG( s == KErrNone, User::Panic( KMsgPanic, s ) );
	TBool foundSupportedAuth( EFalse );
	while ( s == KErrNone && !foundSupportedAuth )
		{
		if ( ext.ExtType() == SADB_EXT_SUPPORTED_AUTH )
			{
			foundSupportedAuth = ETrue;
			}
	    else
	        {
		    s = iReceiveBuffer.NextExtension( ext );
	        }
		}

	if ( s != KErrNone )
		{
		return;
		}
		
	iCaps.iMode.Set( KTransMode );
	iCaps.iProt.Set( KEspProtocol );
	TInt len = ext.ExtLen() * KWordLen;
	TInt offset = KSizeOfSADBSupported;
	TPtrC8 alg;
	for ( ; offset<len; offset = offset+KSizeOfSADBAlg )
		{
		alg.Set( ext.Mid( offset, KSizeOfSADBAlg ) );
		const sadb_alg* a = reinterpret_cast<const sadb_alg*>( alg.Ptr() );
		if ( a->sadb_alg_id == SADB_AALG_MD5HMAC &&
			 a->sadb_alg_maxbits >= MD5_KEYLENGTH &&
			 a->sadb_alg_minbits <= MD5_KEYLENGTH )
			{
			iCaps.iAlgMd5.Set( KHmacMd596 );
			}
		if ( a->sadb_alg_id == SADB_AALG_SHA1HMAC &&
			 a->sadb_alg_maxbits >= SHA_KEYLENGTH &&
			 a->sadb_alg_minbits <= SHA_KEYLENGTH )
			{
			iCaps.iAlgSha.Set( KHmacSha196 );
			}		
		}
	}

// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::SadbAdd
// ----------------------------------------------------------------------------
//
void CSipSecIpsecMechanism::SadbAdd()
    {
    // In case of SADB_ADD message, we are interested only about operations
    // success. Convert sadb_msg error to Symbian error code.
    
	sadb_msg& m = iReceiveBuffer.MsgHdr();
	TInt err = -( ( m.sadb_msg_reserved << 8 ) | m.sadb_msg_errno );	
	
	// Propagate error to contexts
	for ( TInt i = 0; i < iSecAgreeContexts.Count(); i++ )
	    {
		iSecAgreeContexts[ i ]->SADBAddMsgReceived( m.sadb_msg_seq, err );
		}
    }
				       		
// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::InitSecCliHeaderL
// ----------------------------------------------------------------------------
//			
void CSipSecIpsecMechanism::InitSecCliHeaderL( 
	CSIPSecurityClientHeader* aCliHeader )
	{	
	__ASSERT_ALWAYS( Capable(),User::Leave( KErrNotReady ) );	
	if ( iCaps.iAlgMd5.Length() )
		{
		SipSecUtils::SetParamL( *aCliHeader, KAlgorithm, iCaps.iAlgMd5 );
		}
	else
		{
		SipSecUtils::SetParamL( *aCliHeader, KAlgorithm, iCaps.iAlgSha );
		}
	SipSecUtils::SetParamL( *aCliHeader, KProtocol, iCaps.iProt );
	
	SipSecUtils::SetParamL( *aCliHeader, KMode, iCaps.iMode );
	
	TUint rn = SipSecUtils::RandomNumber( iSeed, KMinSPI, KMaxSPI );
	SipSecUtils::SetParamL( *aCliHeader, KSpic, rn );
	
	rn = SipSecUtils::RandomNumber( iSeed, KMinSPI, KMaxSPI );
	SipSecUtils::SetParamL( *aCliHeader, KSpis, rn );
	
	rn = SipSecUtils::RandomNumber( iSeed, KMinPort, KMaxPort );
	SipSecUtils::SetParamL( *aCliHeader, KPortc, rn );
	
	rn = SipSecUtils::RandomNumber( iSeed, KMinPort, KMaxPort );
	SipSecUtils::SetParamL( *aCliHeader, KPorts, rn );
	}
	
// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::CreateNewContextL
// ----------------------------------------------------------------------------
//	
void CSipSecIpsecMechanism::CreateNewContextL(
    TSIPTransportParams& aTransportParams,
    CSIPResponse& aResponse,
    CSIPRequest& aRequest,
	TInetAddr& aNextHop, 
	MSIPSecUser* aUser,
	MSIPSecSecurityMechanismObserver& aObserver )
    {
    if ( aResponse.ResponseCode() != K401Response ||
         !SipSecUtils::HeaderWithMechanism( 
                aResponse, 
		 	    SipStrConsts::ESecurityServerHeader,
			    KIpSec3gpp ) )
        {
        return;
        }
        
    CSipSecAgreeContext* ctx = 
            CSipSecAgreeContext::NewL( *this, 
	                                   aTransportParams,
	                                   aResponse, 
	                                   aRequest,
									   aNextHop, 
									   aUser,
									   aObserver,
									   iStates );
	CleanupStack::PushL( ctx );
	iSecAgreeContexts.AppendL( ctx );
	CleanupStack::Pop( ctx );


#ifdef CPPUNIT_TEST
	if ( iObjCtr->iCtxCtrl1 == NULL )
		{
		iObjCtr->iCtxCtrl1 = ctx->iObjCtr;	
		}
	else
		{
		if ( iObjCtr->iCtxCtrl2 == NULL )
			{
			iObjCtr->iCtxCtrl2 = ctx->iObjCtr;	
			}
		else
			{
			// Add new member (e.g. iObjCtr->iCtxCtrl3) to store ctx->iObjCtr
			User::Panic( KNullDesC, KErrNotSupported );
			}
		}
#endif
    }
    
// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::FindContext
// ----------------------------------------------------------------------------
//
CSipSecAgreeContext* CSipSecIpsecMechanism::FindContext(
    const MSIPSecUser* aSipUser )
	{
	for ( TInt i = 0; i < iSecAgreeContexts.Count(); i++ )
		{
		if ( iSecAgreeContexts[ i ]->HasSecUser( aSipUser ) )
			{
			return iSecAgreeContexts[ i ];
			}
		}
	return 0;
	}

// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::FindContext
// ----------------------------------------------------------------------------
//
CSipSecAgreeContext* CSipSecIpsecMechanism::FindContext(
    const TInetAddr& aNextHop )
	{
	for ( TInt i = 0; i < iSecAgreeContexts.Count(); i++ )
		{
		if ( iSecAgreeContexts[ i ]->HasNextHop( aNextHop ) )
			{
			return iSecAgreeContexts[ i ];
			}
		}
	return 0;
	}

// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::DeleteContext
// ----------------------------------------------------------------------------
//
void CSipSecIpsecMechanism::DeleteContext( 
	CSipSecAgreeContext* aContext )
	{
	TInt i = iSecAgreeContexts.Find( aContext );
	iSecAgreeContexts.Remove( i );
	iSecAgreeContexts.Compress();
	delete aContext;	
	}

// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::InitializeStatesL
// ----------------------------------------------------------------------------
//
void CSipSecIpsecMechanism::InitializeStatesL()
	{
	CTentative* te =  new ( ELeave ) CTentative();
	CleanupStack::PushL( te );
	iStates.AppendL( te );
	CleanupStack::Pop( te );
    CPendingKey* pk = new ( ELeave ) CPendingKey();
   	CleanupStack::PushL( pk );
	iStates.AppendL( pk );
	CleanupStack::Pop( pk );
    CPendingSa* ps = new ( ELeave ) CPendingSa();
   	CleanupStack::PushL( ps );
	iStates.AppendL( ps );
	CleanupStack::Pop( ps );
	CPendingPolicy* pp = new ( ELeave ) CPendingPolicy();
   	CleanupStack::PushL( pp );
	iStates.AppendL( pp );
	CleanupStack::Pop( pp );
    CTemporal* tm = new ( ELeave ) CTemporal();
   	CleanupStack::PushL( tm );
	iStates.AppendL( tm );
	CleanupStack::Pop( tm );
    CInUse* us = new ( ELeave ) CInUse();
   	CleanupStack::PushL( us );
	iStates.AppendL( us );
	CleanupStack::Pop( us );
    COld* ol = new ( ELeave ) COld();
    CleanupStack::PushL( ol );
	iStates.AppendL( ol );
	CleanupStack::Pop( ol );
	CDying* dy = new ( ELeave ) CDying();
	CleanupStack::PushL( dy );
	iStates.AppendL( dy );
	CleanupStack::Pop( dy );
	CDeleting* de = new ( ELeave ) CDeleting();
	CleanupStack::PushL( de );
	iStates.AppendL( de );
	CleanupStack::Pop( de );
	CClearing* cl = new ( ELeave ) CClearing();
	CleanupStack::PushL( cl );
	iStates.AppendL( cl );
	CleanupStack::Pop( cl );	
	}

// ----------------------------------------------------------------------------
// CSipSecIpsecMechanism::ConstructL
// ----------------------------------------------------------------------------
//
void CSipSecIpsecMechanism::ConstructL()
	{
	__SIP_LOG( "IPSecPlugin::ConstructL()" )
	
	InitializeStatesL();

	User::LeaveIfError( iSockServ.Connect() );
	User::LeaveIfError( iSaDb.Open( iSockServ ) );
	User::LeaveIfError( iPolicyServ.Connect() );
	
	iSaDbMsgReceiver = CSipSecSaDbMsgReceiver::NewL( *this, iSaDb );

#ifdef CPPUNIT_TEST
    iObjCtr = new ( ELeave ) CActiveObjController1();
    iObjCtr->SetRecvBuffer( this );	
    iObjCtr->AddMechRecvL( iSaDbMsgReceiver );
#endif

	iSaDbMsgReceiver->Receive( iReceiveBuffer );	

	iSaDbMsgSender = CSipSecSaDbMsgSender::NewL( *this, iSaDb );
		
#ifdef CPPUNIT_TEST
    iObjCtr->AddMechSendL( iSaDbMsgSender );
#endif

	TPfkeySendMsg*
		msg = new ( ELeave ) TPfkeySendMsg( SADB_REGISTER, SADB_SATYPE_ESP, 
		   							  	    ++iSeqNum, PID );
	CleanupStack::PushL( msg );
	sadb_msg &m = msg->MsgHdr();
	m.sadb_msg_len = static_cast<TUint16>( msg->Length() / KWordLen );
	iSaDbMsgSender->SendL( msg, SADB_REGISTER );
	CleanupStack::Pop( msg ); // msg ownership was transferred
	}

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
//
CSipSecIpsecMechanism::CSipSecIpsecMechanism( 
	TSIPSecMechanismInitParams* aInitParams ) : 
	iTimerMgr( aInitParams->iTimer ),
	iEngineContext( aInitParams->iEngineContext ),
	iTransportMgr( aInitParams->iTransportMgr ),
	iT1( aInitParams->iT1 ),
	iStates( KStatesGranularity )
	{
	TTime time;
	time.HomeTime();
	iSeed = time.Int64();	
	}

// End of File
