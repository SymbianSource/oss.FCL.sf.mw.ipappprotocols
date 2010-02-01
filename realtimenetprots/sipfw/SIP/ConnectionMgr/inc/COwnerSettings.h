/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Name        : COwnerSettings.h
* Part of     : ConnectionMgr
* See class definition below.
* Version     : SIP/4.0
*
*/



#ifndef __COWNERSETTINGS_H__

/**
* @internalComponent
*/
#define __COWNERSETTINGS_H__

// INCLUDES
#include <e32base.h>
#include <es_sock.h> 

#include "TSIPTransportParams.h"

// FORWARD DECLARATIONS
class CSocketOpt;
class TInetAddr;

// CLASS DECLARATION
class COwnerSettings : public CBase
    {
public: // Constructor and destructor
    
    static COwnerSettings* NewLC( const TSIPTransportParams& aParams );
 
    ~COwnerSettings();
    
private: // Private contruction
    
    COwnerSettings( const TSIPTransportParams& aParams );
    
public: // Public methods
   
    void AddOptL( TUint aOptionName,
                  TUint aOptionLevel,
                  TInt aOption );
                  
    void AddOptL( TUint aOptionName,
                  TUint aOptionLevel,
                  const TDesC8& aOption );
                  
    const TSIPTransportParams& TransportParams() const; 
      
    void SetOpts( RSocket& aSocket );
    
    void ClearOpts( RSocket& aSocket );
    
    void SaveTransportInfoL( TUint aLocalSendPort,
                             TUint aLocalReceivePort,
                             const TInetAddr& aRemoteAddr,
                             TUint aRemoteSendPort );
    
    void AddReservedPortL( TUint aPort );
    
    TBool IsUnreservedPort( TUint aPort ) const;
    
    TBool IsOpts() const;
    
    TUint LocalSendPort() const;
    
    TUint LocalReceivePort() const;
    
    const TInetAddr* RemoteAddr() const;
    
    TBool IsMatchingWithSavedInfo( const TSIPTransportParams& aParams, 
                                   const TInetAddr& aRemoteAddr,
                                   TUint aProtocol,
                                   TUint aLocalPort ) const;
                                   
    TBool IsMatchingWithRemoteSendAddr( const TInetAddr& aRemoteSendAddr,
                                        TBool aCheckOnlySendPort ) const;
    
    void SetUseTLS( TBool aUseTLS );
    
    TBool UseTLS() const;
    
    void SaveTLSRemoteL( const TInetAddr& aRemoteAddr );

private: // Private methods

    void AddOptL( CSocketOpt* aSocketOpt );
    
    void SetOptsL( RSocket& aSocket );
    
    void ClearOptsL( RSocket& aSocket );
    
private: // Data

    RPointerArray<CSocketOpt> iSocketOpts;
    
    RPointerArray<CSocketOpt> iOriginalOpts;
    
    RArray<TUint> iPorts;
    
    TUint iLocalSendPort;
    
    TUint iLocalReceivePort;
    
    TInetAddr* iRemoteAddr;
    
    TUint iRemoteSendPort;
    
    TSIPTransportParams iParams;
    
    TBool iUseTLS;
    
private: // For unit testing
#ifdef CPPUNIT_TEST
    friend class COwnerSettingsListTest;
#endif
    };
    
#endif // __COWNERSETTINGS_H__
