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
// Name        : CNetworkManager.cpp
// Part of     : NetworkMonitor
// Implementation
// Version     : SIP/4.0
//



#include <commsdat.h>
#include <metadatabase.h>
#include <es_sock.h>
#include "sipbearermonitor.h"
#include "CNetworkManager.h"
#include "sipnetworkinfoobserver.h"
#include "SipLogs.h"


// Observer array granularity.
#if defined (CPPUNIT_TEST)
const TInt KObserverGranularity( 1 );
#else
const TInt KObserverGranularity( 4 );
#endif

// ECom plugin interface UID
const TUid KSIPBearerInterfaceUid = { 0x102010D8 };


// -----------------------------------------------------------------------------
// CNetworkManager::NewL
// -----------------------------------------------------------------------------
//
CNetworkManager* CNetworkManager::NewL()
	{
	CNetworkManager* self = new( ELeave ) CNetworkManager;
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CNetworkManager::CNetworkManager
// -----------------------------------------------------------------------------
//
CNetworkManager::CNetworkManager() :
	iObservers( KObserverGranularity ),
	iNetworkInfoObserver( NULL )
	{
	}

// -----------------------------------------------------------------------------
// CNetworkManager::ConstructL
// -----------------------------------------------------------------------------
//
void CNetworkManager::ConstructL()
	{
	iContainerIndex = CObjectConIx::NewL();
	iContainer = iContainerIndex->CreateL();
	}

// -----------------------------------------------------------------------------
// CNetworkManager::~CNetworkManager
// -----------------------------------------------------------------------------
//
CNetworkManager::~CNetworkManager()
	{
	// Deregister all observers, delete CConnectionMonitors
	// and free Bearer handles
	TInt count = iObservers.Count();
	for( TInt i = 0; i < count; i++ )
		{
		MSIPNetworkObserver* observer = iObservers[ i ].iObserver;
		if ( observer )
		    {
		    Deregister( *observer );
		    }
		}

	// Destroy reference counted object framework objects
	delete iContainerIndex;
	}

// -----------------------------------------------------------------------------
// CNetworkManager::RegisterObserverL
// -----------------------------------------------------------------------------
//
void CNetworkManager::RegisterObserverL( MSIPNetworkObserver& aObserver,
										 TInt aIapId,
										 TInt aError )
	{
	__SIP_LOG("CNetworkManager::RegisterObserverL, Start")
	// A bearer monitor plugin is not needed on emulator, except when doing
	// unit testing
#if ((defined (__WINS__) || defined(__WINSCW__)) && !defined(CPPUNIT_TEST) )

	aError++; aIapId++; aObserver; // Avoid compiler warnings
	return;

#else

	// Try to find an existing observer entry from the registry
	TInt index = FindRegistryEntry( aObserver );
	if( index != KErrNotFound )
		{
		// Already registered, just refresh.
		const TObserverEntry& entry  = iObservers[ index ];
		__SIP_LOG("CNetworkManager::RegisterObserverL, Already registered, just refresh")
		entry.iBearer->RefreshL( aError );
		return;
		}
    
	// Find a bearer for the specified observer (and its RConnection)
	CSIPBearerMonitor* bearer = FindBearerL( aIapId, aError );
	
	// Associate the observer with a bearer only when the bearer is not NULL
	if (bearer != NULL)
		{
		__SIP_LOG("CNetworkManager::RegisterObserverL, Associate the observer with a bearer")
    	TObserverEntry entry;
    	entry.iObserver = &aObserver;
    	entry.iBearer = bearer;
    	entry.iReportedState = MSIPNetworkObserver::ENetworkUnknown;
    	CleanupClosePushL( *bearer );
   		iObservers.AppendL( entry );
    	CleanupStack::Pop( bearer );	
		}
#endif
	}

// -----------------------------------------------------------------------------
// CNetworkManager::Deregister
// -----------------------------------------------------------------------------
//
void CNetworkManager::Deregister( MSIPNetworkObserver& aObserver )
	{
	// Find a matching observer entry from the registry
	TInt index = FindRegistryEntry( aObserver );

	// Remove from the registry entry if a match was found, otherwise
	// do nothing (fail silently)
	if( index != KErrNotFound )
		{
		TObserverEntry entry = iObservers[ index ];
		iObservers.Delete( index );
		// Notify bearer that a client has gone (decrement reference count)
		if( entry.iBearer )
			{
			entry.iBearer->Close();
			}
		}
	}
	
// -----------------------------------------------------------------------------
// CNetworkManager::ContinueMonitoring
// -----------------------------------------------------------------------------
//
TBool CNetworkManager::ContinueMonitoring( MSIPNetworkObserver& aObserver, 
                                           TInt aError )
    {
	__SIP_LOG("CNetworkManager::ContinueMonitoring, Start")
    // Find a matching observer entry from the registry
	TInt index = FindRegistryEntry( aObserver );

	// Ask from corresponding monitor plugin whether monitoring should be
	// continued after aError has occured while re-establishment of the 
	// connection
	if( index != KErrNotFound )
		{
		__SIP_LOG("CNetworkManager::ContinueMonitoring, returning found")
		TObserverEntry entry = iObservers[ index ];
		if( entry.iBearer )
			{
			return entry.iBearer->ContinueMonitoring( aError );
			}
		}
	__SIP_LOG("CNetworkManager::ContinueMonitoring, returning default")
	return EFalse;
    }

// -----------------------------------------------------------------------------
// CNetworkManager::State
// -----------------------------------------------------------------------------
//
MSIPNetworkObserver::TNetworkState CNetworkManager::State(
											 MSIPNetworkObserver& aObserver ) const
	{
	// Find a matching observer entry from the registry
	TInt index = FindRegistryEntry( aObserver );

	// Return bearer state if successful, otherwise return unknown state.
	if( index != KErrNotFound )
		{
		const TObserverEntry& entry = iObservers[ index ];
 		return entry.iBearer->State();
		}
	else
		{
		return MSIPNetworkObserver::ENetworkUnknown;
		}
	}

// -----------------------------------------------------------------------------
// CNetworkManager::BearerStateChanged
// -----------------------------------------------------------------------------
//
void CNetworkManager::BearerStateChanged( CSIPBearerMonitor* aBearer,
								 		  TInt aError )
	{
	 __SIP_INT_LOG1( "CNetworkManager::BearerStateChanged", aError )	
	// A bearer reported a state change.
	// Broadcast that to all registered observers for that bearer
	TInt count = iObservers.Count();
 	for( TInt i = 0; i < count; i++ )
 		{
 		TObserverEntry& entry = iObservers[ i ];
 		if( entry.iBearer == aBearer )
 			{
 			MSIPNetworkObserver::TNetworkState newState = aBearer->State();
 			if( newState != entry.iReportedState )
 				{
 				entry.iReportedState = newState;
 				}
			__SIP_INT_LOG2( "CNetworkManager::BearerStateChanged, State ,Error",newState, aError )	
 			entry.iObserver->NetworkStateChanged( newState, aError );
 			}
 		}
	}

// -----------------------------------------------------------------------------
// CNetworkManager::InfoChanged
// -----------------------------------------------------------------------------
//
void CNetworkManager::InfoChanged( const TSIPAccessNetworkInfo& aInfo )
	{
	iAccessNetworkInfo = aInfo;

	if ( iNetworkInfoObserver )
		{
		iNetworkInfoObserver->InfoChanged( iAccessNetworkInfo );
		}
	}

// -----------------------------------------------------------------------------
// CNetworkManager::FindRegistryEntry
// -----------------------------------------------------------------------------
//
TInt CNetworkManager::FindRegistryEntry( MSIPNetworkObserver& aObserver ) const
	{
	TInt index;
	TObserverEntry ref;
	ref.iObserver = &aObserver;
	const TInt keyOffset = _FOFF( CNetworkManager::TObserverEntry, iObserver );
	TKeyArrayFix findKey( keyOffset, ECmpTUint32 );

	// Try to find a matching observer entry from the registry
	TInt found = iObservers.Find( ref, findKey, index );

	// Return entry index if successful, otherwise return an error.
	if( found == 0 )
		{
		return index;
		}
	else
		{
		return KErrNotFound;
		}
	}

// -----------------------------------------------------------------------------
// CNetworkManager::FindBearerL
// -----------------------------------------------------------------------------
//
CSIPBearerMonitor* CNetworkManager::FindBearerL( TInt aIapId, TInt aError )
	{	
	__SIP_LOG("CNetworkManager::FindBearerL, Start")
	TBuf< KCDMaxFieldNameLength > serviceType;
	// Reserve space for possibly appended iapId
	const TInt KMaxTIntAsDesLen = 11;
	const TInt objectNameLen( KCDMaxFieldNameLength + KMaxTIntAsDesLen );
	TBuf< objectNameLen > objectName;
	
	// Fetch service type and objectName for this connection from CommDb.
	// Use serviceType as resolving parameter, objectName is used for
	// identifying loaded plugins stored in iContainer
	TInt realIapId =
	    GetServiceTypeAndObjectNameL( aIapId, serviceType, objectName );
	    
	if (realIapId == 0)
		{
		// No bearer associated with IAP ID:0. Hence return NULL. 
		// This results in not starting the bearer monitor 
	    return NULL; 		
		}   
    
	// Try to find a matching object from the object container
	TName tmp;
	TInt objectHandle( 0 );
	TInt found = iContainer->FindByName( objectHandle, objectName, tmp );

	CSIPBearerMonitor* bearer;
	if( found == KErrNotFound )
		{
		__SIP_LOG("CNetworkManager::FindBearerL, Object Was not found and calling CreateImpl")
		// Object was not found, create a new one using ECom
		HBufC8* objectName8 = HBufC8::NewLC( serviceType.Length() * 2 );
		TPtr8 ptrObjectName8( objectName8->Des() );
		ptrObjectName8.Copy( serviceType );
		
		// Find implementation by name
	    TEComResolverParams resolverParams;
	    resolverParams.SetDataType( ptrObjectName8 );
	    
	    TSIPBearerParams bearerParams( *this, realIapId, aError, *this );
	    
    	bearer = reinterpret_cast< CSIPBearerMonitor* >(
    	    REComSession::CreateImplementationL(
    	        KSIPBearerInterfaceUid,
    	        _FOFF( CSIPBearerMonitor, iInstanceKey ),
    	        &bearerParams,
    	        resolverParams,
    	        KRomOnlyResolverUid ) );
		// Register it to the object container
		CleanupClosePushL( *bearer );
		bearer->SetNameL( &objectName );
		iContainer->AddL( bearer );
		CleanupStack::Pop(); // bearer
		CleanupStack::PopAndDestroy( objectName8 );
		}
	else
		{
		__SIP_LOG("CNetworkManager::FindBearerL, Object Was Found")
		// Object was found, go fetch it and cast to appropriate type
		bearer = static_cast< CSIPBearerMonitor* >(
										 iContainer->At( objectHandle ) );
		// Increase reference count
		User::LeaveIfError( bearer->Open() );
		
		// Refresh
		bearer->RefreshL( aError );
		}
		__SIP_LOG("CNetworkManager::FindBearerL, End")
	return bearer;
	}

// -----------------------------------------------------------------------------
// CNetworkManager::GetServiceTypeAndObjectNameL
// -----------------------------------------------------------------------------
//
TInt CNetworkManager::GetServiceTypeAndObjectNameL( 
    TInt aIapId, TDes& aServiceType, TDes& aObjectName )
	{
	const TInt KMaxTIntAsDesLen = 11;
	
	__ASSERT_ALWAYS( 
	    aServiceType.MaxLength() >= KCDMaxFieldNameLength &&
	    aObjectName.MaxLength() >= ( KCDMaxFieldNameLength + KMaxTIntAsDesLen ), 
	    User::Leave( KErrArgument ) );
	
    TInt realIapId( aIapId );

    CMDBSession* db = CMDBSession::NewLC( KCDVersion1_1 ); 
    
    db->SetAttributeMask( ECDHidden );
    
    // Create an iap record
    CCDIAPRecord* iapRecord = 
        static_cast<CCDIAPRecord*>( 
            CCDRecordBase::RecordFactoryL( KCDTIdIAPRecord ) );
    CleanupStack::PushL( iapRecord );
    
    iapRecord->SetRecordId( aIapId );

    iapRecord->LoadL( *db );

    GetServiceTypeL( *iapRecord, aServiceType );
    
    // If VPN IAP is used, "really" used IAP have to be fetched
    if ( aServiceType.Compare( TPtrC( KCDTypeNameVPNService ) ) == 0 )
        {
        realIapId = HandleVPNServiceL( *db, *iapRecord, aServiceType );
        }
    
    CleanupStack::PopAndDestroy( iapRecord );
    CleanupStack::PopAndDestroy( db );
    
    aObjectName.Copy( aServiceType );
    
    // Find out if the IAP is LANService access point.
    // If so, aIapId will be appended to aObjectName descriptor which is
    // used for identifying bearer monitor plugins. By this we achieve
    // that own monitor plugin is loaded for each LANService IAP
    
#ifndef CPPUNIT_TEST
    if ( aServiceType.Compare( TPtrC( KCDTypeNameLANService ) ) == 0 )
        {
        aObjectName.AppendNum( static_cast<TInt64>( realIapId ) );
        }        
#endif
	__SIP_INT_LOG1( "CNetworkManager::GetServiceTypeAndObjectNameL end realIapID =", realIapId )	
	
    return realIapId;
	}

// -----------------------------------------------------------------------------
// CNetworkManager::HandleVPNServiceL
// -----------------------------------------------------------------------------
//	
TInt CNetworkManager::HandleVPNServiceL( 
    CMDBSession& aCommsDat, CCDIAPRecord& aIapRecord, TDes& aServiceType )
    {   
    aIapRecord.iService.LoadL( aCommsDat );
    
    if ( !aIapRecord.iService.iLinkedRecord )
        {
        // Ownership of created record is transferred
        aIapRecord.iService.iLinkedRecord = 
            static_cast<CCDVPNServiceRecord*>(
                CCDRecordBase::RecordFactoryL( KCDTIdVPNServiceRecord ) );
        aIapRecord.iService.iLinkedRecord->SetRecordId( aIapRecord.iService );
        
        aIapRecord.iService.iLinkedRecord->LoadL( aCommsDat );
        }  
    
    CCDVPNServiceRecord* serviceRecord = 
        static_cast<CCDVPNServiceRecord*>( aIapRecord.iService.iLinkedRecord );

	TRAPD(err,serviceRecord->iServiceIAP.LoadL( aCommsDat ));
	if(KErrNone != err )
		{
		//Its valid to not have iServiceIAP record, due to introduction of ServiceSNAP for the VPN with Snap
		__SIP_INT_LOG1( "CNetworkManager::HandleVPNServiceL CCDVPNServiceRecord->iServiceIAP LoadL failed with error=", err)
		aServiceType.Copy(_L8("")); 
		return 0;
		
		}
	else
		{
	    	if ( !serviceRecord->iServiceIAP.iLinkedRecord )
	        {
			// Ownership of created record is transferred
	        serviceRecord->iServiceIAP.iLinkedRecord = 
	            static_cast<CCDIAPRecord*>(
	                CCDRecordBase::RecordFactoryL( KCDTIdIAPRecord ) );
	        serviceRecord->iServiceIAP.iLinkedRecord->SetRecordId( 
	            serviceRecord->iServiceIAP );
	            
	        serviceRecord->iServiceIAP.iLinkedRecord->LoadL( aCommsDat );
			}
		

	    	CCDIAPRecord* iapRecord = 
	        	static_cast<CCDIAPRecord*>( serviceRecord->iServiceIAP.iLinkedRecord );
	    
	    	// We need service type info of the "real" iap
	    	GetServiceTypeL( *iapRecord, aServiceType );
	       	return iapRecord->RecordId();
		}
	}

// -----------------------------------------------------------------------------
// CNetworkManager::GetServiceTypeL
// -----------------------------------------------------------------------------
//    
void CNetworkManager::GetServiceTypeL( 
    CCDIAPRecord& aIapRecord, TDes& aServiceType )
    {
    CMDBField<TDesC>* serviceType = 
            ( CMDBField<TDesC>* )aIapRecord.GetFieldByIdL( 
                                                KCDTIdIAPServiceType );
            
    __ASSERT_ALWAYS( serviceType && !serviceType->IsNull(), 
                     User::Leave( KErrNotFound ) );
    
    aServiceType.Copy( *serviceType );
    }

// -----------------------------------------------------------------------------
// CNetworkManager::RegisterAccessNetworkObserver
// -----------------------------------------------------------------------------
//
const TSIPAccessNetworkInfo& CNetworkManager::RegisterAccessNetworkObserver(
    MSIPNetworkInfoObserver& aObserver )
	{
	// A bearer monitor plugin is not needed on emulator, except when doing
	// unit testing
#if ((defined (__WINS__) || defined(__WINSCW__)) && !defined(CPPUNIT_TEST) )
	aObserver; // Avoid compiler warnings
	
	//Since the real plugin is not started in emulator, pass hardcoded values
	//to ConnectionMgr so that the P-Access-Network-Info header can be tested.
	iAccessNetworkInfo = DummyAccessNetworkValues();
	
#else
	iNetworkInfoObserver = &aObserver;
#endif

	return iAccessNetworkInfo;
	}
	
// -----------------------------------------------------------------------------
// CNetworkManager::DummyAccessNetworkValues
// -----------------------------------------------------------------------------
//
TSIPAccessNetworkInfo CNetworkManager::DummyAccessNetworkValues() const
	{
	_LIT( KDummyCountryCode, "12" );
    _LIT( KDummyNetworkCode, "345" );
    const TUint KDummyLocationAreaCode = 0x6789;
    const TUint KDummyCellId = 0xABCD;
    
    TSIPAccessNetworkInfo accessNetworkInfo;
	
    accessNetworkInfo.iNetworkMode = RMobilePhone::ENetworkModeGsm; 
    accessNetworkInfo.iMobileCountryCode.Copy( KDummyCountryCode );
    accessNetworkInfo.iMobileNetworkCode.Copy( KDummyNetworkCode );
    accessNetworkInfo.iAreaKnown = ETrue;
    accessNetworkInfo.iLocationAreaCode = KDummyLocationAreaCode;
    accessNetworkInfo.iCellId = KDummyCellId;
	
    return accessNetworkInfo;
	}

// -----------------------------------------------------------------------------
// CNetworkManager::DeregisterAccessNetworkObserver
// -----------------------------------------------------------------------------
//
void CNetworkManager::DeregisterAccessNetworkObserver()
	{
	iNetworkInfoObserver = NULL;
	}

// -----------------------------------------------------------------------------
// CNetworkManager::RefreshConnection
// -----------------------------------------------------------------------------
//
void CNetworkManager::RefreshConnectionL(TUint32 aIapId)
	{
        CSIPBearerMonitor* bearer= FindBearerL(aIapId,KErrNone);
	if(bearer!=NULL)
	  {	
	  CleanupClosePushL( *bearer );
          bearer->RefreshL(KErrNone);
          CleanupStack::PopAndDestroy();
	  }
	}
