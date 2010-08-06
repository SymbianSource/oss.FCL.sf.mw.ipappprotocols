/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Settings item for SIP settings, handles also the saving of
*                settings item. 
*
*/


#ifndef CWPSIPITEM_H
#define CWPSIPITEM_H

// INCLUDES
#include <e32base.h>                // CBase
#include <sipprofiletypeinfo.h>

// FORWARD DECLARATIONS
class CWPCharacteristic;
class CWPSIPAdapter;
class CSIPProfileRegistryObserver;
class CSIPManagedProfileRegistry;

// CLASS DECLARATION

/**
 * CWPSIPItem handles saving of SIP settings item.
 * @lib WPSIPAdapter.
 * @since 3.0.
 */ 
class CWPSIPItem : public CBase
    {
    public:

        static CWPSIPItem* NewL();

        virtual ~CWPSIPItem();

        /**
         * Setter for profile type.
         * @since 3.0.
         * @param const TDesC8& aPTYPE.
         */
        void SetProfileTypeL( const TDesC8& aPTYPE );

        /**
         * Setter for APPREF.
         * @since .
         * @param const TDesC8& aAPPREF.
         */
        void SetAppRefL( const TDesC8& aAPPREF );

        /**
         * Setter for AOL/Public User Identity.
         * @since 3.0.
         * @param const TDesC8& aPUID.
         */
        void SetAORL( const TDesC8& aPUID );

        /**
         * Setter for provider.
         * @since 3.0.
         * @param const TDesC8& aPROVIDERID.
         */
        void SetProviderIdL( const TDesC8& aPROVIDERID );

        /**
         * Setter for user viewable name of the setting.
         * @since 3.0.
         * @param const TDesC& aNAME. Value is TCP or UDP.
         */
        void SetNameL( const TDesC& aNAME );

        /**
         * Setter for transport protocol between UE and outbound proxy/P-CSCF.
         * @since 3.0.
         * @param const TDesC8& aAPROTOCOL.
         */
        void SetProtocolForOutboundConnectionL( const TDesC8& aAPROTOCOL );

        /**
         * Setter for NAPDEF name. Name of IAP.
         * @since 3.0.
         * @param const TDesC8& aTONAPID.
         */
        void SetNapDefNameL( const TDesC8& aTONAPID );

        /**
         * Setter for signal compression usage.
         * @since 3.0.
         * @param TBool aEnabled.
         */
        void SetSignalCompressionEnabled( TBool aEnabled );

        /**
         * Setter for security negotiation usage.
         * @since 3.0.
         * @param TBool aEnabled.
         */
        void SetSecurityNegotiationEnabled( TBool aEnabled );

        /**
         * Setter for loose routing usage.
         * @since 3.0.
         * @param TBool aEnabled.
         */
        void SetLooseRoutingForOutboundConnectionEnabled( TBool aEnabled );

        /**
         * Setter for profile lock usage.
         * @since 3.1.
         * @param TBool aEnabled.
         */
        void SetProfileLockEnabled( TBool aEnabled );

        /**
         * Setter for outbound proxy/P-CSCF address.
         * @since 3.0.
         * @param const TDesC8& aOutboundAddr.
         */
        void SetOutboundAddrL( const TDesC8& aOutboundAddr );

        /**
         * Setter for port number to be used when connecting outbound
         * proxy/P-CSCF.
         * @since 3.0.
         * @param const TDesC8& aPORT.
         */
        void SetPortNumberForOutboundConnectionL( const TDesC8& aPORT );

        /**
         * Setter for authentication type when autenticating to outbound
         * proxy.
         * @since 3.0.
         * @param const TDesC8& aAAUTHTYPE.
         */
        void SetAuthenticationTypeForOutboundL( const TDesC8& aAAUTHTYPE );

        /**
         * Setter for username when autenticating to outbound proxy.
         * @since 3.0.
         * @param const TDesC8& aAAUTHNAME.
         */
        void SetUsernameForOutboundL( const TDesC8& aAAUTHNAME );

        /**
         * Setter for password when autenticating to outbound proxy.
         * @since 3.0.
         * @param const TDesC8& aAAUTHSECRET.
         */
        void SetPasswordForOutboundL( const TDesC8& aAAUTHSECRET );

        /**
         * Setter for realm when autenticating to outbound proxy.
         * @since 3.0.
         * @param const TDesC8& aAAUTHDATA.
         */
        void SetRealmForOutboundL( const TDesC8& aAAUTHDATA );

        /**
         * Setter for registrar/S-CSCF address.
         * @since 3.0.
         * @param const TDesC8& aRegistrarAddr.
         */
        void SetRegistrarAddrL( const TDesC8& aRegistrarAddr );

        /**
         * Setter for authentication type when autenticating to registrar
         * proxy/S-CSCF.
         * @since 3.0.
         * @param const TDesC8& aAAUTHTYPE.
         */
        void SetAuthenticationTypeForRegistrarL( const TDesC8& aAAUTHTYPE );

        /**
         * Setter for username when autenticating to registrar proxy/S-CSCF.
         * @since 3.0.
         * @param const TDesC8& aAAUTHNAME.
         */
        void SetUsernameForRegistrarL( const TDesC8& aAAUTHNAME );

        /**
         * Setter for password when autenticating to registrar proxy/S-CSCF.
         * @since 3.0.
         * @param const TDesC8& aAAUTHSECRET.
         */
        void SetPasswordForRegistrarL( const TDesC8& aAAUTHSECRET );

        /**
         * Setter for realm when autenticating to registrar proxy/S-CSCF.
         * @since 3.0.
         * @param const TDesC8& aAAUTHDATA.
         */
        void SetRealmForRegistrarL( const TDesC8& aAAUTHDATA );

        /**
         * Setter for NAPDEF (IAP settings).
         * @since 3.0.
         * @param CWPCharacteristic* aNapDef.
         */
        void SetNapDef( CWPCharacteristic* aNapDef );

        /**
         * Sets the id of IAP to save item.
         * @since 3.0.
         * @param aUid the uid of IAP.
         */
        void SetIAPId( TUint32 aUid );

        /**
         * Sets the SIP profile as default.
         * @since 3.0.
         */
        void SetAsDefaultL();

        /**
         * Sets automatic registration enabled.
         * @since 3.0.
         */
        void SetAutomaticRegistrationEnabled( 
            TBool aAutomaticRegistrationEnabled );

         /**
          * Gets SIP ID.
          * @since 3.0.
          */
        TDesC8& GetAppRef();

        /**
         * Getter for Address-Of-Record/Public User Identity.
         * @since 3.0.
         * @return TDesC8&.
         */
        TDesC8& GetAOR();

        /**
         * Getter for provider.
         * @since 3.0.
         * @return TDesC8&.
         */
        TDesC8& GetProviderId();

        /**
         * Getter for user viewable nema of SIP settings item.
         * @since 3.0.
         * @return TDesC8&.
         */
        TDesC& GetName();

        /**
         * Getter for protocol name for usage between UE and outbound proxy.
         * @since 3.0.
         * @return TDesC8&.
         */
        TDesC8& GetProtocolForOutboundConnection();

        /**
         * Getter for NAPDEF name (IAP settings name).
         * @since 3.0.
         * @return TDesC8&.
         */
        TDesC8& GetNapDefName();

        /**
         * Getter for signal compression usage.
         * @since 3.0.
         * @return TDesC8&.
         */
        TBool GetSignalCompressionEnabled();

        /**
         * Getter for security negotiation usage.
         * @since 3.0.
         * @return TBool.
         */
        TBool GetSecurityNegotiationEnabled();

        /**
         * Getter for loose routing usage.
         * @since 3.0.
         * @return TBool.
         */
        TBool GetLooseRoutingForOutboundConnectionEnabled();

        /**
         * Getter for profile lock.
         * @since 3.1.
         * @return TBool.
         */
        TBool GetProfileLockEnabled();

        /**
         * Getter for outbound proxy/P-CSCF address.
         * @since 3.0.
         * @return TDesC8&.
         */
        TDesC8& GetOutboundAddr();

        /**
         * Getter for port number of outbound proxy.
         * @since 3.0.
         * @return TDesC8&.
         */
        TDesC8& GetPortNumberForOutboundConnection();

        /**
         * Getter for authentication type of outbound proxy.
         * @since 3.0.
         * @return TDesC8&.
         */
        TDesC8& GetAuthenticationTypeForOutbound();

        /**
         * Getter for username of outbound proxy realm.
         * @since 3.0.
         * @return TDesC8&.
         */
        TDesC8& GetUsernameForOutbound();

        /**
         * Getter for password of outbound proxy realm.
         * @since 3.0.
         * @return TDesC8&.
         */
        TDesC8& GetPasswordForOutbound();

        /**
         * Getter for outbound proxy realm.
         * @since 3.0.
         * @return TDesC8&.
         */
        TDesC8& GetRealmForOutbound();

        /**
         * Getter for registrar proxy/S-CSCF address.
         * @since 3.0.
         * @return TDesC8&.
         */
        TDesC8& GetRegistrarAddr();

        /**
         * Getter for authentication type of registrar proxy/S-CSCF.
         * @since 3.0.
         * @return TDesC8&.
         */
        TDesC8& GetAuthenticationTypeForRegistrar();

        /**
         * Getter for username of registrar proxy/S-CSCF realm.
         * @since 3.0.
         * @return TDesC8&.
         */
        TDesC8& GetUsernameForRegistrar();

        /**
         * Getter for password of registrar proxy/S-CSCF realm.
         * @since 3.0.
         * @return TDesC8&.
         */
        TDesC8& GetPasswordForRegistrar();

        /**
         * Getter for registrar proxy/S-CSCF realm.
         * @since 3.0.
         * @return TDesC8&.
         */
        TDesC8& GetRealmForRegistrar();

        /**
         * Getter for automatic registration state
         * @since 3.0.
         * @return TBool.
         */
        TBool GetAutomaticRegistrationEnabled();

        /**
         * Getter for IAPId.
         * @since 3.0.
         * @return TUint32.
         */
        TUint32 GetIAPId();

        /**
         * Getter for NAPDEF (IAP settings).
         * @since 3.0.
         * @return CWPCharacteristic*.
         */
        CWPCharacteristic* GetNapDef();

        /**
         * Getter for SIP profile id.
         * @since 3.0.
         * @return TUint32. Uid of the SIP profile in SIP Profile Registry.
         */
        TUint32 GetProfileId();

        /**
         * Saves SIP settings point by using .
         * @since 3.0.
         * @return Id of saved service access point.
         */
        TUint32 StoreL();

        /**
         * Return profile id.
         * @since 3.0.
         * @return TDesC8&. Id of the SIP item.
         */
        const TDesC8& SaveData();

        /**
         * Check for duplicate providername and rename if same
         * @since Series 60_3.0
         * @param aProfile Profile to check.
         * @param aProfileId ProfileId to be skipped in check.
         */
        void CheckDuplicateNameL( CSIPManagedProfile& aProfile, 
            const TInt& aProfileId );

        /**
         * Set wlan availability switch
         * @since Series 60_3.0
         * @param aScanRate Scan rate to be set.
         */
        void SetAvailabilitySwitchL( const TUint32 aScanRate );

        /**
         * Cleanup method for array.
         *
         * @since S60 3.1
         * @param aParameter Parameter to be cleaned.
         */
        static void CleanupArrayItem( TAny* aArray );

        /**
         * Setter for TO-APPREF value.
         *
         * @since S60 3.2
         * @param aToAppRef TO-APPREF value to be set.
         */
        void SetToAppRefL( const TDesC8& aToAppRef );

        /**
         * Getter for TO-APPREF value.
         *
         * @since S60 3.2
         * @return TO-APPREF value.
         */
        const TDesC8& ToAppRef() const;

        /**
         * Setter for Destination network name.
         *
         * @since S60 3.2.
         * @param aDestination Destination name to be set.
         */
        void SetNetworkL( const TDesC& aNetwork );

        /**
         * Getter for Destination network name.
         *
         * @since S60 3.2.
         * @return Destination network name.
         */
        const TDesC& Network() const;

        /**
         * Setter for SNAP ID.
         *
         * @since S60 3.2.
         * @param aSnapId SNAP ID.
         */
        void SetSnapId( TInt aSnapId );

        /**
         * Setter for Signalling QoS value.
         *
         * @since S60 3.2.
         * @param aSigQos Signalling QoS value.
         */
        void SetSigQos( TUint32 aSigQos );
        
        void SetBearerID(TUint32 aBearerId);
        TUint32 GetBearerID();

    private:

        void ConstructL();
        CWPSIPItem();

        /**
         * Getter for Destination Network ID.
         *
         * @since S60 3.2.
         * @param aName Destination Network name.
         * @return Destination Network ID.
         */
        TInt NetworkIdL( const TDesC& aName );

    private:

        /**
         * SIP Profile reference identity in provisioning document.
         * Own.
         */
        HBufC8* iAppRef;

        /**
         * Address-Of-Record/Public User Identity.
         * Own.
         */
        HBufC8* iAOR;

        /**
         * Protocol used between UE and outbound proxy/P-CSCF.
         * Own.
         */
        HBufC8* iProtocolForOutboundConnection;

        /**
         * Name of SIP settings visible to user when viewing the provisioning 
         * message.
         * Own.
         */
        HBufC* iName;

        /**
         * Provider of settings.
         * Own.
         */
        HBufC8* iProviderId;

        /** 
         * The access point name.
         * Own.
         */
        HBufC8* iNapDefName;        

        /**
         * Signal compression usage.
         */
        TBool iSignalCompressionEnabled;      

        /**
         * Security negotiation usage.
         */
        TBool iSecurityNegotiationEnabled;

        /**
         * Loose routing usage.
         */
        TBool iLooseRoutingForOutboundConnectionEnabled;

        /**
         * Registration mode (always on/when needed).
         */
        TBool iAutomaticRegistrationEnabled;

        /**
         * Profile Lock.
         */
        TBool iProfileLockEnabled;

        /**
         * Outbound proxy/P-CSCF address.
         * Own.
         */
        HBufC8* iOutboundAddr;

        /**
         * Port number of outbound proxy/P-CSCF.
         * Own.
         */
        HBufC8* iPortNumberForOutboundConnection;

        /**
         * Authentication type of outbound proxy.
         * Own.
         */
        HBufC8* iAuthenticationTypeForOutbound;

        /**
         * Username of outbound proxy realm.
         * Own.
         */
        HBufC8* iUsernameForOutbound;

        /**
         * Password of outbound proxy realm.
         * Own.
         */
        HBufC8* iPasswordForOutbound;

        /**
         * Outbound proxy realm.
         * Own.
         */
        HBufC8* iRealmForOutbound;

        /**
         * Registrar proxy/S-CSCF address.
         * Own.
         */
        HBufC8* iRegistrarAddr;

        /**
         * Authentication type of registrar proxy/S-CSCF.
         * Own.
         */
        HBufC8* iAuthenticationTypeForRegistrar;  

        /**
         * Username of registrar proxy/S-CSCF realm.
         * Own.
         */
        HBufC8* iUsernameForRegistrar;

        /**
         * Password of registrar proxy/S-CSCF realm.
         * Own.
         */
        HBufC8* iPasswordForRegistrar;

        /**
         * Registrar proxy/S-CSCF realm.
         * Own.
         */
        HBufC8* iRealmForRegistrar;

        /**
         * NAPDEF. Pointer to IAP settings.
         */
        CWPCharacteristic* iNapDef;

        /**
         * ID value of the referred IAP.
         */
        TUint32 iIAPId;

        /**
         * ID of the SIP profile in profile registry after saving.
         */
        TUint32 iProfileId;

        /**
         * Defaultness of the SIP profile.
         */
        TBool iSetAsDefault;

        /**
         * Type of profile + name of type.
         */
        TSIPProfileTypeInfo iSIPProfileTypeInfo;  

        /**
         * SIP ID as descriptor to be returned by the SaveData method to the
         * framework via adapter SaveDataL.
         * Own.
         */
        HBufC8* iSavedProfileId;

        /**
         * TO-APPREF value for linking SIP profile to a destination network.
         * Own.
         */
        HBufC8* iToAppRef;

        /**
         * Destination network name to be set.
         * Own.
         */
        HBufC* iNetwork;

        /**
         * SNAP ID.
         */
        TInt iSnapId;

        /**
         * Signalling QoS value.
         */
        TUint32 iSigQos;
        
        TUint32 iBearerId;

    private:
        //friend class UT_CWPSIPItem; // For testing purposes.
        //friend class UT_CWPSIPAdapter; // For testing purposes.


    }; // class

#endif // CWPSIPITEM_H
