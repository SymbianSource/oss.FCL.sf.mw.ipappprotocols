/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handles SIP settings in provisioning.
*
*/


#ifndef CWPSIPADAPTER_H
#define CWPSIPADAPTER_H

// INCLUDES
#include <CWPAdapter.h>
#include <MWPContextExtension.h>

// FORWARD DECLARATIONS
class CWPCharacteristic;
class CWPSIPItem;
class CSIPProfileRegistryObserver;
class CSIPManagedProfileRegistry;
class CSIPManagedProfile;

// CLASS DECLARATION

/**
 * CWPSIPAdapter handles SIP settings.
 * @lib WPSIPAdapter
 * @since 3.0
 */ 
class CWPSIPAdapter : public CWPAdapter, private MWPContextExtension
    {

    public: // Constructors and destructor

        static CWPSIPAdapter* NewL();
        virtual ~CWPSIPAdapter();
        
        // From MWPContextExtension:
        
        /**
         * Returns a pointer to a context extension.
         * @param aExtension Contains a pointer to MWPContextExtension if supported
         * @return KErrNotSupported if not supported, KErrNone otherwise
         */
        TInt ContextExtension( MWPContextExtension*& aExtension );

        /**
         * Returns the data used for saving.
         * @param aIndex The index of the data
         * @return The data. Ownership is transferred.
         */
        const TDesC8& SaveDataL( TInt aIndex ) const;

        /**
         * Deletes a saved item.
         * @param aSaveData The data used for saving
         */
        void DeleteL( const TDesC8& aSaveData );

        /**
         * Returns the UID of the adapter.
         * @return UID
         */
        TUint32 Uid() const;

        // From CWPAdapter:

        /**
         * Returns the number of the summary lines to be shown in 
         * BIO control.
         * @since 3.0.
         * @return TInt. Quantity of items (SIP profiles).
         */
        TInt ItemCount() const;

        /**
         * Getter for the summary text of the SIP item (SIP profile) 
         * located in the array. Text is shown to the user in 
         * opened configuration message.
         * @since 3.0.
         * @param TInt aIndex. Location of the SIP item.
         * @return TDesC16&. Summary title.
         */
        const TDesC16& SummaryTitle( TInt aIndex ) const;

        /**
         * Getter for the summary text of the SIP item (SIP profile) 
         * located in the array. Text is shown to the user in 
         * opened configuration message.
         * @since 3.0.
         * @param TInt aIndex. Location of the SIP item.
         * @return TDesC16&. Summary text.
         */
        const TDesC16& SummaryText( TInt aIndex ) const;

        /**
         * Saves SIP item (SIP profile) located in array.
         * @since 3.0.
         * @param TInt aIndex. Location of the SIP item to be saved.
         */
        void SaveL( TInt aIndex );

        /**
         * Query if the SIP item on place pointed by parameter can 
         * be set as default.
         * @since 3.0.
         * @param TInt aIndex. Location of the SIP item to be queried.
         * @return TBool.
         */
        TBool CanSetAsDefault( TInt aIndex ) const;
    
        /**
         * Sets the SIP item on place pointed by parameter as default.
         * @since 3.0.
         * @param TInt aIndex. Location of the SIP item to be queried.
         */
        void SetAsDefaultL( TInt aIndex );

        /**
         * Query for the detail information about the SIP profile. 
         * MWPPairVisitor is used for retrieving the details of a single 
         * setting entry. This is not supported feature as in the other 
         * adapters in the framework.
         * @since 3.0.
         * @param MWPPairVisitor aVisitor.
         * @param TInt aItem.
         * @return TInt. 
         */
        TInt  DetailsL( TInt aItem, MWPPairVisitor& aVisitor );

        /**
         * Visit method. Visits the adapter. Adapter then checks from the 
         * aCharacteristic if the data is targetet to it and acts according to 
         * that.
         * @since 3.0.
         * @param CWPCharacteristic& aCharacteristic.
         */
        void  VisitL( CWPCharacteristic& aCharacteristic );		

        /**
         * Visit method. Visits the adapter. Adapter checks from the 
         * aParameter if the data is targetet to it and acts according to 
         * that.
         * @since 3.0.
         * @param CWPParameter& aParameter.
         */
        void  VisitL( CWPParameter& aParameter );		

        /**
         * Visit method. Visits the adapter. Adapter checks from the 
         * aParameter if the link is targetet to it and acts according to 
         * that. IAP bound to the SIP profile is handled with help of this 
         * method.
         * @since 3.0.
         * @param CWPParameter& aParameter.
         */
        void VisitLinkL( CWPCharacteristic& aLink );

        /**
         * Gets the saving information from the adapter that has saved
         * settings.
         * @param TInt aIndex. Index of the previously saved item.
         * @param TUint32& aUidOfAdapter. UID of the adapter component. 
         * @param TAny& aStorageIdValue. Value that identifies the settings in
         *        its storage.
         * @param TBool& aIsLastOfItsType. Adapter can tell to the framework
         *        if the saved item was the last one of its type.
         * @return TDesC8&. REFID of the saved settings.
         */
	    void GetSavingInfoL( TInt aIndex, 
	        RPointerArray<HBufC8>& aSavingInfo );

        /**
         * Lets the adapters know when the saving has been finalized in 
         * viewpoint of the provisioning framework. The adapters can make 
         * finalization of saving the settings after receiving this call.
         * @since Series60 3.0
         */
        void SavingFinalizedL();

        /**
         * From CWPAdapter.
         * Informs the adapters of the saved settings.
         *
         * @since S60 3.2.
         * @param aAppIdOfSavingItem UID of the adapter component. 
         * @param aAppRef APPREF of the saved settings.
         * @param aStorageIdValue Value that identifies the settings in its
         *        storage.
         */
        void SettingsSavedL( const TDesC8& aAppIdOfSavingItem,
            const TDesC8& aAppRef, const TDesC8& aStorageIdValue );

    private:

        CWPSIPAdapter();
        void ConstructL();

        /**
         * Check whether we need to support setting as default
         * When there are no SIP profiles, query in UI is not needed
         * @since Series 60_3.0
         * @return TBool
         */
        TBool CheckSettingAsDefaultL() const;

        /**
         * Gets IAP ID according to WAP ID.
         * @since S60 3.2
         * @param aWapId WAP ID of access point.
         * @return IAP ID of access point
         */
        TUint32 IapIdFromWapIdL( TUint32 aWapId );

    private:

        // Holds the received SIP item data. Owns.
        RPointerArray<CWPSIPItem> iDatas;

	    // Current SIP setting data. Owns.
	    CWPSIPItem *iCurrentSIPItem;

        // The application id of the current characteristic
        TPtrC iAppID;
 
	    // Default name for SIP settings. Owns.
		HBufC* iDefaultName;

        // Default value for provider name. Owns.
        HBufC* iProviderIdDefault;

        // Title for SIP settings. Owns.
        HBufC*  iTitle;

        // Id of SIP item to be set as default
        TUint32 iSIPIdForDefault;

        // Holds the value of current characteristic type (state).
        TUint iCurrentCharacteristic;

		// Descriptor containing uids of the saved items (as TUint32).
		mutable HBufC8* iUids;

		// SIPIds of stored SIPItems that are set as autoregistrable.
		RArray<TUint32> iSIPIdArray;

    private:    // Friend classes
        //friend class UT_CWPSIPItem; // For testing purposes.
        //friend class UT_CWPSIPAdapter; // For testing purposes.
    };

#endif	// CWPSIPADAPTER_H
            
// End of File
