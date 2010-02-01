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
* Name        : COwnerSettingsList.h
* Part of     : ConnectionMgr
* See class definition below.
* Version     : SIP/4.0
*
*/




/**
 @internalComponent
*/
#ifndef COWNERSETTINGSLIST_H
#define COWNERSETTINGSLIST_H

// INCLUDES
#include <e32base.h>
#include <es_sock.h> 

// FORWARD DECLARATIONS
class COwnerSettings;
class TSIPTransportParams;
class TInetAddr;

// CLASS DECLARATION
class COwnerSettingsList : public CBase
    {
public: // Constructor and destructor
    
    static COwnerSettingsList* NewL();
    
    ~COwnerSettingsList();
    
private: // Private contruction
    
    COwnerSettingsList();
    
public: // Public methods

    void AddSettingsL( const TSIPTransportParams& aParams,
                       TUint aOptionName,
                       TUint aOptionLevel,
                       TInt aOption );
                       
    void AddSettingsL( const TSIPTransportParams& aParams,
                       TUint aOptionName,
                       TUint aOptionLevel,
                       const TDesC8& aOption );
    
    void RemoveSettings( const TSIPTransportParams& aParams );
    
    TBool RemoveSettings( TUint32 aTransportId );
    
    TBool IsRequestedOpts( const TSIPTransportParams& aParams ) const;
    
    void SetOpts( const TSIPTransportParams& aParams, 
                  RSocket& aSocket );   
    
    void ClearOpts( const TSIPTransportParams& aParams, 
                    RSocket& aSocket );   
                    
    /**
     * @return ETrue if settings matching aOldParams existed
     */            
    TBool SaveTransportInfoL( const TSIPTransportParams& aOldParams, 
                              const TSIPTransportParams& aParams, 
                              TUint aLocalSendPort,
                              TUint aLocalReceivePort,
                              const TInetAddr& aRemoteAddr,
                              TUint aRemoteSendPort );
    
    void SaveTLSTransportInfoL( const TSIPTransportParams& aParams );
     
    void ReservePrivatePortL( const TSIPTransportParams& aParams, 
                              TUint aPort );
                               
    TUint GetPrivatePortL( const TSIPTransportParams& aParams ); 
    
    TBool HasSavedTransportInfo( TUint32 aTransportId ) const;
    
    TBool HasSavedTLSTransportInfo( TUint32 aTransportId ) const;
    
    TUint SavedLocalSendPort( TUint32 aTransportId ) const;
    
    TUint SavedLocalReceivePort( TUint32 aTransportId ) const;
    
    const TInetAddr* SavedRemoteAddr( TUint32 aTransportId ) const;
    
    TBool IsReceivingAllowed( const TSIPTransportParams& aParams, 
                              const TInetAddr& aRemoteAddr,
                              TUint aProtocol,
                              TUint aLocalPort ) const;
                              
    TUint32 FindTransportId( const TInetAddr& aRemoteAddr ) const;
    
    void SaveTLSRemoteL( const TSIPTransportParams& aParams,
                         const TInetAddr& aRemoteAddr );
    
private: // Private methods

    COwnerSettings& FindOwnerSettingsL( const TSIPTransportParams& aParams,
                                        TBool aAllowExisting = ETrue ); 
    
    TInt FindOwnerSettingsIndex( const TSIPTransportParams& aParams ) const;
    
    TInt FindOwnerSettingsIndex( TUint32 aTransportId ) const;
  
    TBool IsReservedTransport( TUint32 aTransportId ) const;
    
private: // Data

    RPointerArray<COwnerSettings> iSettings;
    
private: // For unit testing
#ifdef CPPUNIT_TEST
    friend class COwnerSettingsListTest; 
    friend class CSipConnectionMgrTest;
#endif
    };
    
#endif // COWNERSETTINGSLIST_H

// end of file
