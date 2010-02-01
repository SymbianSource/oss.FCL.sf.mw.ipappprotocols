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
// Name        : COwnerSettings.cpp
// Part of     : ConnectionMgr
// See class definition below.
// Version     : SIP/4.0
//



#include <in_sock.h>

#include "COwnerSettings.h"
#include "CSocketOpt.h"
#include "CommonConsts.h"
   
const TInt KSocketOptsGranularity = 2;
const TInt KPortsGranularity = 2;

// -----------------------------------------------------------------------------
// COwnerSettings::NewLC
// -----------------------------------------------------------------------------
//
COwnerSettings* COwnerSettings::NewLC( const TSIPTransportParams& aParams )
    {
    COwnerSettings* self = new ( ELeave ) COwnerSettings( aParams );
    CleanupStack::PushL( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// COwnerSettings::~COwnerSettings
// -----------------------------------------------------------------------------
//
COwnerSettings::~COwnerSettings()
    {
    delete iRemoteAddr;
    iSocketOpts.ResetAndDestroy();
    iOriginalOpts.ResetAndDestroy();
    iPorts.Close();
    }

// -----------------------------------------------------------------------------
// COwnerSettings::COwnerSettings
// -----------------------------------------------------------------------------
//    
COwnerSettings::COwnerSettings( const TSIPTransportParams& aParams ) :
    iSocketOpts( KSocketOptsGranularity ),
    iPorts( KPortsGranularity ),
	iParams( aParams ),
	iUseTLS( EFalse )
    {
    }
    
// -----------------------------------------------------------------------------
// COwnerSettings::AddOptL
// -----------------------------------------------------------------------------
//
void COwnerSettings::AddOptL(
    TUint aOptionName,
	TUint aOptionLevel,
	TInt aOption )
    {
    CSocketOpt* sockopt = 
                    CSocketOpt::NewLC( aOptionName, aOptionLevel, aOption );
    AddOptL( sockopt );
    CleanupStack::Pop( sockopt );
    }   
    
// -----------------------------------------------------------------------------
// COwnerSettings::AddOptL
// -----------------------------------------------------------------------------
//
void COwnerSettings::AddOptL(
    TUint aOptionName,
	TUint aOptionLevel,
	const TDesC8& aOption )
    {
    CSocketOpt* sockopt = 
                    CSocketOpt::NewLC( aOptionName, aOptionLevel, aOption );
    AddOptL( sockopt );
    CleanupStack::Pop( sockopt );
    }   

// -----------------------------------------------------------------------------
// COwnerSettings::TransportParams
// -----------------------------------------------------------------------------
//      
const TSIPTransportParams& COwnerSettings::TransportParams() const
    {
    return iParams;
    }

// -----------------------------------------------------------------------------
// COwnerSettings::SetOpts
// -----------------------------------------------------------------------------
//     
void COwnerSettings::SetOpts( RSocket& aSocket )
    {
    // Clear original opts, those will be re-created while setting new options
    iOriginalOpts.ResetAndDestroy();
    
    TRAP_IGNORE( SetOptsL( aSocket ) )
    }

// -----------------------------------------------------------------------------
// COwnerSettings::ClearOpts
// -----------------------------------------------------------------------------
//     
void COwnerSettings::ClearOpts( RSocket& aSocket )
    {
    TRAP_IGNORE( ClearOptsL( aSocket ) )
    }
    
// -----------------------------------------------------------------------------
// COwnerSettings::RememberPortsL
// -----------------------------------------------------------------------------
//    
void COwnerSettings::SaveTransportInfoL( 
    TUint aLocalSendPort,
    TUint aLocalReceivePort,
    const TInetAddr& aRemoteAddr,
    TUint aRemoteSendPort )
    {
    __ASSERT_ALWAYS( !iLocalSendPort && 
                     !iLocalReceivePort && 
                     !iRemoteAddr &&
                     !iRemoteSendPort, 
                     User::Leave( KErrAlreadyExists ) );
     
    iRemoteAddr = new ( ELeave ) TInetAddr( aRemoteAddr );                
    iLocalSendPort = aLocalSendPort;
    iLocalReceivePort = aLocalReceivePort;
    iRemoteSendPort = aRemoteSendPort;
    }
    
// -----------------------------------------------------------------------------
// COwnerSettings::AddReservedPortL
// -----------------------------------------------------------------------------
//       
void COwnerSettings::AddReservedPortL( TUint aPort )
    {
    // Don't allow duplicates in the array, however in such situation no need
    // to leave
    TInt err = iPorts.InsertInOrder( aPort );
    if ( err && err != KErrAlreadyExists )
        {
        User::Leave( err );
        }
    }

// -----------------------------------------------------------------------------
// COwnerSettings::IsUnreservedPort
// -----------------------------------------------------------------------------
//    
TBool COwnerSettings::IsUnreservedPort( TUint aPort ) const
    {
    TInt count = iPorts.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iPorts[ i ] == aPort )
            {
            return EFalse;
            }
        }
    return ETrue;
    }

// -----------------------------------------------------------------------------
// COwnerSettings::IsOpts
// -----------------------------------------------------------------------------
//      
TBool COwnerSettings::IsOpts() const
    {
    return ( iSocketOpts.Count() > 0 );
    }
 
// -----------------------------------------------------------------------------
// COwnerSettings::LocalSendPort
// -----------------------------------------------------------------------------
//         
TUint COwnerSettings::LocalSendPort() const
    {
    return iLocalSendPort;
    }
    
// -----------------------------------------------------------------------------
// COwnerSettings::LocalReceivePort
// -----------------------------------------------------------------------------
//         
TUint COwnerSettings::LocalReceivePort() const
    {
    return iLocalReceivePort;
    }
    
// -----------------------------------------------------------------------------
// COwnerSettings::RemoteAddr
// -----------------------------------------------------------------------------
//         
const TInetAddr* COwnerSettings::RemoteAddr() const
    {
    return iRemoteAddr;
    }
    
// -----------------------------------------------------------------------------
// COwnerSettings::IsMatchingWithSavedInfo
// -----------------------------------------------------------------------------
//         
TBool COwnerSettings::IsMatchingWithSavedInfo( 
    const TSIPTransportParams& aParams, 
    const TInetAddr& aRemoteAddr,
    TUint aProtocol,
    TUint aLocalPort ) const
    {
    TUint32 transportId( aParams.TransportId() );
    TBool isReservedTransport( transportId > 0 );
    
    TBool addressMatch( EFalse );
    
    if ( aProtocol == KProtocolInetUdp && isReservedTransport )
        {
        // In case of protected UDP, check only remote send and local receive
        // ports
        addressMatch = ( IsMatchingWithRemoteSendAddr( aRemoteAddr, ETrue ) &&
                         iLocalReceivePort == aLocalPort );
        }
    else
        {
        // Otherwise check both, remote and local, send and receive ports
        addressMatch = ( IsMatchingWithRemoteSendAddr( aRemoteAddr, EFalse ) &&
                         ( iLocalReceivePort == aLocalPort ||
                           iLocalSendPort == aLocalPort ) );
        }
    return ( addressMatch &&
             ( !isReservedTransport || iParams.TransportId() == transportId ) );
    }

// -----------------------------------------------------------------------------
// COwnerSettings::IsMatchingWithRemoteSendAddr
// -----------------------------------------------------------------------------
//    
TBool COwnerSettings::IsMatchingWithRemoteSendAddr( 
    const TInetAddr& aRemoteSendAddr,
    TBool aCheckOnlySendPort ) const
    {
    if ( !iRemoteAddr )
        {
        return EFalse;
        }
    
    TBool portMatch = ( iRemoteSendPort == aRemoteSendAddr.Port() );
    if ( !aCheckOnlySendPort && !portMatch )
        {
        portMatch = ( iRemoteAddr->CmpPort( aRemoteSendAddr ) );
        }

    return ( iRemoteAddr->Match( aRemoteSendAddr ) && portMatch );
    }

// -----------------------------------------------------------------------------
// COwnerSettings::SetUseTLS
// -----------------------------------------------------------------------------
//   
void COwnerSettings::SetUseTLS( TBool aUseTLS )
    {
    iUseTLS = aUseTLS;
    }

// -----------------------------------------------------------------------------
// COwnerSettings::UseTLS
// -----------------------------------------------------------------------------
//     
TBool COwnerSettings::UseTLS() const
    {
    return iUseTLS;
    }

// -----------------------------------------------------------------------------
// COwnerSettings::SaveTLSRemoteL
// -----------------------------------------------------------------------------
// 
void COwnerSettings::SaveTLSRemoteL( const TInetAddr& aRemoteAddr )   
    {
    __ASSERT_ALWAYS( iUseTLS, User::Leave( KErrNotReady ) );
    
    TInetAddr* remoteAddr = new ( ELeave ) TInetAddr( aRemoteAddr );  
    delete iRemoteAddr;
    iRemoteAddr = remoteAddr;
    
    iRemoteAddr->SetPort( KDefaultSipsPort );
    }
    
// -----------------------------------------------------------------------------
// COwnerSettings::AddOptL
// -----------------------------------------------------------------------------
//
void COwnerSettings::AddOptL( CSocketOpt* aSocketOpt )
    {
    __ASSERT_ALWAYS( aSocketOpt, User::Leave( KErrArgument ) );
    // Don't allow multiple socket opts with same option name
    for ( TInt i = 0; i < iSocketOpts.Count(); i++ )
        {
        if ( iSocketOpts[ i ]->OptionName() == aSocketOpt->OptionName() )
            {
            delete iSocketOpts[ i ];
            iSocketOpts.Remove( i );
            iSocketOpts.InsertL( aSocketOpt, i );
            return;
            }
        }
    iSocketOpts.AppendL( aSocketOpt );
    }

// -----------------------------------------------------------------------------
// COwnerSettings::SetOptsL
// -----------------------------------------------------------------------------
//
void COwnerSettings::SetOptsL( RSocket& aSocket )
    {
    TInt count = iSocketOpts.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        // Store previous opt as it will be used when ClearOpts is called
        CSocketOpt* previousOpt = iSocketOpts[ i ]->SetOptL( aSocket );
        if ( previousOpt )
            {
            CleanupStack::PushL( previousOpt );
            iOriginalOpts.AppendL( previousOpt );
            CleanupStack::Pop( previousOpt );
            }
        }
    }

// -----------------------------------------------------------------------------
// COwnerSettings::ClearOptsL
// -----------------------------------------------------------------------------
//     
void COwnerSettings::ClearOptsL( RSocket& aSocket )
    {
    TInt count = iOriginalOpts.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        CSocketOpt* previousOpt = iOriginalOpts[ i ]->SetOptL( aSocket );
        // Not interested about previous value
        delete previousOpt;
        }
    }
       
// end of file
