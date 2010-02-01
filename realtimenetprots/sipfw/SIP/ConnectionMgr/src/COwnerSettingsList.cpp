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
// Name        : COwnerSettingsList.cpp
// Part of     : ConnectionMgr
// See class definition below.
// Version     : SIP/4.0
//



#include "COwnerSettingsList.h"
#include "COwnerSettings.h"
#include "CommonConsts.h"
#include "siperr.h"

const TInt KSettingsGranularity = 2;
const TUint KFirstUnreservedPort = KDefaultSipPort+2;

// -----------------------------------------------------------------------------
// COwnerSettingsList::NewL
// -----------------------------------------------------------------------------
//
COwnerSettingsList* COwnerSettingsList::NewL()
    {
    COwnerSettingsList* self = new ( ELeave ) COwnerSettingsList;
    return self;
    }
    
// -----------------------------------------------------------------------------
// COwnerSettingsList::~COwnerSettingsList
// -----------------------------------------------------------------------------
//
COwnerSettingsList::~COwnerSettingsList()
    {
    iSettings.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// COwnerSettingsList::COwnerSettingsList
// -----------------------------------------------------------------------------
//    
COwnerSettingsList::COwnerSettingsList() :
    iSettings( KSettingsGranularity )
    {
    }

// -----------------------------------------------------------------------------
// COwnerSettingsList::AddSettingsL
// -----------------------------------------------------------------------------
//
void COwnerSettingsList::AddSettingsL(     
    const TSIPTransportParams& aParams,
    TUint aOptionName,
	TUint aOptionLevel,
	TInt aOption )
    {
    COwnerSettings& settings = FindOwnerSettingsL( aParams );
    settings.AddOptL( aOptionName, aOptionLevel, aOption );
    }
    
// -----------------------------------------------------------------------------
// COwnerSettingsList::AddSettingsL
// -----------------------------------------------------------------------------
//
void COwnerSettingsList::AddSettingsL(     
    const TSIPTransportParams& aParams,
    TUint aOptionName,
	TUint aOptionLevel,
	const TDesC8& aOption )
    {
    COwnerSettings& settings = FindOwnerSettingsL( aParams );
    settings.AddOptL( aOptionName, aOptionLevel, aOption );
    }
    
// -----------------------------------------------------------------------------
// COwnerSettingsList::RemoveSettings
// -----------------------------------------------------------------------------
//
void COwnerSettingsList::RemoveSettings( const TSIPTransportParams& aParams )
    {
    // Remove all settings of this owner
    TInt count = iSettings.Count();
    for ( TInt i = ( count-1 ); i >= 0; i-- )
        {
        if ( iSettings[ i ]->TransportParams().OwnerId() == aParams.OwnerId() )
            {
            COwnerSettings* settings = iSettings[ i ];
            iSettings.Remove( i );
            delete settings;
            }
        }
    iSettings.Compress();
    }
    
// -----------------------------------------------------------------------------
// COwnerSettingsList::RemoveSettings
// -----------------------------------------------------------------------------
//
TBool COwnerSettingsList::RemoveSettings( TUint32 aTransportId )
    {
    // Remove all settings containing passed transportId
    TBool removed( EFalse );
    TInt count = iSettings.Count();
    for ( TInt i = ( count-1 ); i >= 0; i-- )
        {
        if ( iSettings[ i ]->TransportParams().TransportId() == aTransportId )
            {
            COwnerSettings* settings = iSettings[ i ];
            iSettings.Remove( i );
            delete settings;
            removed = ETrue;
            }
        }
    iSettings.Compress();
    return removed;
    }

// -----------------------------------------------------------------------------
// COwnerSettingsList::IsRequestedOpts
// -----------------------------------------------------------------------------
//
TBool COwnerSettingsList::IsRequestedOpts( 
    const TSIPTransportParams& aParams ) const
    {
    TInt count = iSettings.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iSettings[ i ]->TransportParams().OwnerId() == aParams.OwnerId() &&
             iSettings[ i ]->IsOpts() )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// COwnerSettingsList::SetOpts
// -----------------------------------------------------------------------------
//    
void COwnerSettingsList::SetOpts( 
    const TSIPTransportParams& aParams, 
    RSocket& aSocket )
    {
    // Apply to socket all opts this owner has set
    TInt count = iSettings.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iSettings[ i ]->TransportParams().OwnerId() == aParams.OwnerId() )
            {
            iSettings[ i ]->SetOpts( aSocket );
            }
        }
    }


// -----------------------------------------------------------------------------
// COwnerSettingsList::ClearOpts
// -----------------------------------------------------------------------------
//    
void COwnerSettingsList::ClearOpts( 
    const TSIPTransportParams& aParams, 
    RSocket& aSocket )
    {
    // Apply to socket all opts this owner has set
    TInt count = iSettings.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iSettings[ i ]->TransportParams().OwnerId() == aParams.OwnerId() )
            {
            iSettings[ i ]->ClearOpts( aSocket );
            }
        }
    }
        
// -----------------------------------------------------------------------------
// COwnerSettingsList::SaveTransportInfoL
// -----------------------------------------------------------------------------
// 
TBool COwnerSettingsList::SaveTransportInfoL( 
    const TSIPTransportParams& aOldParams, 
    const TSIPTransportParams& aParams, 
    TUint aLocalSendPort,
    TUint aLocalReceivePort,
    const TInetAddr& aRemoteAddr,
    TUint aRemoteSendPort )
    { 
    // If there is already settings matching old tp params and old transport id
    // is not default, it is allowed to have duplicate reservations between
    // those two transports since in this case new IPSec record is just changing
    // local sending port but sharing receiving port.
    
    TInt index( KErrNotFound );
    if ( IsReservedTransport( aOldParams.TransportId() ) )
        {
        index = FindOwnerSettingsIndex( aOldParams );
        }

    for ( TInt i = 0; i < iSettings.Count(); i++ )
        {
        if ( !iSettings[ i ]->IsUnreservedPort( aLocalSendPort ) ||
           ( i != index && !iSettings[ i ]->IsUnreservedPort( aLocalReceivePort ) ) )
            {
            User::Leave( KErrSIPTransportFailure );
            }
        }
            
    COwnerSettings& settings = FindOwnerSettingsL( aParams );
    
    settings.SaveTransportInfoL( aLocalSendPort,
                                 aLocalReceivePort,
                                 aRemoteAddr,
                                 aRemoteSendPort );
                            
    settings.AddReservedPortL( aLocalSendPort );
    settings.AddReservedPortL( aLocalReceivePort );
    
    return ( index != KErrNotFound );
    }
    
// -----------------------------------------------------------------------------
// COwnerSettingsList::SaveTLSTransportInfoL
// -----------------------------------------------------------------------------
// 
void COwnerSettingsList::SaveTLSTransportInfoL( 
    const TSIPTransportParams& aParams )
    { 
    COwnerSettings& settings = FindOwnerSettingsL( aParams, EFalse );
    settings.SetUseTLS( ETrue );
    }

// -----------------------------------------------------------------------------
// COwnerSettingsList::ReservePrivatePortL
// -----------------------------------------------------------------------------
// 
void COwnerSettingsList::ReservePrivatePortL( 
    const TSIPTransportParams& aParams, 
    TUint aPort )
    {  
    // Ignore attempt to reserve default sip port. Also ignore protected
    // transports since those use SaveTransportInfoL for reserving ports
    // (two SAs may share same private ports).  
    if ( aPort == KDefaultSipPort || 
         IsReservedTransport( aParams.TransportId() ) )
        {
        return;
        }
    
    // First check that port is not already reserved 
    // (it can be reserved for owner)
    TInt index = FindOwnerSettingsIndex( aParams );

    for ( TInt i = 0; i < iSettings.Count(); i++ )
        {
        if ( i != index && !iSettings[ i ]->IsUnreservedPort( aPort ) )
            {
            User::Leave( KErrSIPTransportFailure );
            }
        }
        
    // Then reserve it
    COwnerSettings& settings = FindOwnerSettingsL( aParams );   
    settings.AddReservedPortL( aPort );
    }

// -----------------------------------------------------------------------------
// COwnerSettingsList::GetPrivatePort
// -----------------------------------------------------------------------------
//        
TUint COwnerSettingsList::GetPrivatePortL( const TSIPTransportParams& aParams )
    {
    // Find first unreserved port
    TBool found( EFalse );
    TUint unreservedport = KFirstUnreservedPort;
    TInt count = iSettings.Count();  
    for ( TInt i = 0; i < count && !found; i++ )
        {
        if ( iSettings[ i ]->IsUnreservedPort( unreservedport ) )
            {
            found = ETrue;
            }
        else
            {
            unreservedport++;
            }
        }
    // Then reserve it
    COwnerSettings& settings = FindOwnerSettingsL( aParams );   
    settings.AddReservedPortL( unreservedport );
    return unreservedport;
    }

// -----------------------------------------------------------------------------
// COwnerSettingsList::HasSavedTransportInfo
// -----------------------------------------------------------------------------
//       
TBool COwnerSettingsList::HasSavedTransportInfo( 
    TUint32 aTransportId ) const
    {
    return ( FindOwnerSettingsIndex( aTransportId ) != KErrNotFound );
    }

// -----------------------------------------------------------------------------
// COwnerSettingsList::HasSavedTLSTransportInfo
// -----------------------------------------------------------------------------
//    
TBool COwnerSettingsList::HasSavedTLSTransportInfo( TUint32 aTransportId ) const
    {
    TInt index( KErrNotFound );
    if ( ( index = FindOwnerSettingsIndex( aTransportId ) ) != KErrNotFound )
        {
        return iSettings[ index ]->UseTLS();
        }
    return EFalse;
    }
 
// -----------------------------------------------------------------------------
// COwnerSettingsList::SavedLocalSendPort
// -----------------------------------------------------------------------------
//      
TUint COwnerSettingsList::SavedLocalSendPort( 
    TUint32 aTransportId ) const
    {
    TInt index( KErrNotFound );
    if ( ( index = FindOwnerSettingsIndex( aTransportId ) ) != KErrNotFound )
        {
        return iSettings[ index ]->LocalSendPort();
        }
    return 0;
    }
    
// -----------------------------------------------------------------------------
// COwnerSettingsList::SavedLocalReceivePort
// -----------------------------------------------------------------------------
//      
TUint COwnerSettingsList::SavedLocalReceivePort( 
    TUint32 aTransportId ) const
    {
    TInt index( KErrNotFound );
    if ( ( index = FindOwnerSettingsIndex( aTransportId ) ) != KErrNotFound )
        {
        return iSettings[ index ]->LocalReceivePort();
        }
    return 0;
    }
    
// -----------------------------------------------------------------------------
// COwnerSettingsList::SavedRemoteAddr
// -----------------------------------------------------------------------------
//      
const TInetAddr* COwnerSettingsList::SavedRemoteAddr( 
    TUint32 aTransportId ) const
    {
    TInt index( KErrNotFound );
    if ( ( index = FindOwnerSettingsIndex( aTransportId ) ) != KErrNotFound )
        {
        return iSettings[ index ]->RemoteAddr();
        }
    return 0;
    }
 
// -----------------------------------------------------------------------------
// COwnerSettingsList::IsReceivingAllowed
// -----------------------------------------------------------------------------
//      
TBool COwnerSettingsList::IsReceivingAllowed( 
    const TSIPTransportParams& aParams, 
    const TInetAddr& aRemoteAddr,
    TUint aProtocol,
    TUint aLocalPort ) const
    {
    TBool reservedTransport = IsReservedTransport( aParams.TransportId() );
    
    // When TLS is used explicitly, there's no need to do remote address
    // specific checking
    if ( reservedTransport && 
         HasSavedTLSTransportInfo( aParams.TransportId() ) )
        {
        return ETrue;
        }
    
    // If transport id is not specified, receiving from protected (reserved)
    // ports is not allowed. If id is specified, receiving from unprotected
    // ports is not allowed
    TBool match( EFalse );
    for ( TInt i = 0; i < iSettings.Count() && !match; i++ )
        {
        if ( iSettings[ i ]->IsMatchingWithSavedInfo( aParams, 
                                                      aRemoteAddr, 
                                                      aProtocol,
                                                      aLocalPort ) )
            {
            match = ETrue;
            }
        }
        
    return ( reservedTransport && match ) || ( !reservedTransport && !match );
    }

// -----------------------------------------------------------------------------
// COwnerSettingsList::FindTransportId
// -----------------------------------------------------------------------------
//     
TUint32 COwnerSettingsList::FindTransportId( const TInetAddr& aRemoteAddr ) const
    {
    for ( TInt i = 0; i < iSettings.Count(); i++ )
        {
        COwnerSettings* settings = iSettings[ i ];
        if ( settings->IsMatchingWithRemoteSendAddr( aRemoteAddr, ETrue ) )
            {
            return settings->TransportParams().TransportId();
            }
        }
    return 0;
    }

// -----------------------------------------------------------------------------
// COwnerSettingsList::SaveTLSRemoteL
// -----------------------------------------------------------------------------
//
void COwnerSettingsList::SaveTLSRemoteL( 
    const TSIPTransportParams& aParams,
    const TInetAddr& aRemoteAddr )
    {
    TInt index( KErrNotFound );
    if ( ( index = FindOwnerSettingsIndex( 
            aParams.TransportId() ) ) != KErrNotFound )
        {
        iSettings[ index ]->SaveTLSRemoteL( aRemoteAddr );
        }
    }
    
// -----------------------------------------------------------------------------
// COwnerSettingsList::FindOwnerSettingsL
// -----------------------------------------------------------------------------
//        
COwnerSettings& COwnerSettingsList::FindOwnerSettingsL( 
    const TSIPTransportParams& aParams, TBool aAllowExisting )
    {
    // Find existing settings for this owner or create new entry to the
    // settings list.
    TInt index( 0 );
    if ( ( index = FindOwnerSettingsIndex( aParams ) ) != KErrNotFound )
        {
        __ASSERT_ALWAYS( aAllowExisting, User::Leave( KErrAlreadyExists ) );
        return *iSettings[ index ];
        }

    COwnerSettings* settings = COwnerSettings::NewLC( aParams );
    iSettings.AppendL( settings );
    CleanupStack::Pop( settings );
    return *settings;
    }
 
// -----------------------------------------------------------------------------
// COwnerSettingsList::FindOwnerSettingsIndex
// -----------------------------------------------------------------------------
//         
TInt COwnerSettingsList::FindOwnerSettingsIndex( 
    const TSIPTransportParams& aParams ) const
    {
    COwnerSettings* settings = 0;
    for ( TInt i = 0; i < iSettings.Count(); i++ )
        {
        settings = iSettings[ i ];
        if ( settings->TransportParams().OwnerId() == aParams.OwnerId() &&
             settings->TransportParams().TransportId() == aParams.TransportId() )
            {
            return i;
            }
        }
    return KErrNotFound;
    }
    
// -----------------------------------------------------------------------------
// COwnerSettingsList::FindOwnerSettingsIndex
// -----------------------------------------------------------------------------
//         
TInt COwnerSettingsList::FindOwnerSettingsIndex( 
    TUint32 aTransportId ) const
    {
    if ( !IsReservedTransport( aTransportId ) )
        {
        return KErrNotFound;
        }
    
    COwnerSettings* settings = 0;
    for ( TInt i = 0; i < iSettings.Count(); i++ )
        {
        settings = iSettings[ i ];
        if ( settings->TransportParams().TransportId() == aTransportId )
            {
            return i;
            }
        }
    return KErrNotFound;
    }
 
// -----------------------------------------------------------------------------
// COwnerSettingsList::IsReservedTransport
// -----------------------------------------------------------------------------
//     
TBool COwnerSettingsList::IsReservedTransport( TUint32 aTransportId ) const
    {
    return ( aTransportId > 0 );
    }
    
// end of file
