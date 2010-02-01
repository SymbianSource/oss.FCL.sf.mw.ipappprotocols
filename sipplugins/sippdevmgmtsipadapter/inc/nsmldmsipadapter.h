/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Device Management SIP Settings Adapter
*
*/


#ifndef __NSMLDMSIPADAPTER_H__
#define __NSMLDMSIPADAPTER_H__

// INCLUDES
#include <smldmadapter.h>

// CLASS DECLARATION
class CSIPProfileRegistryObserver;
class CSIPManagedProfileRegistry;
class CSIPManagedProfile;

/**
*  CNSmlDmSIPAdapter
*  OMA DM Adapter for Nokia SIP.
*
*  @lib nsmldmsipadapter.lib
*  @since S60 3.0.
*/
class CNSmlDmSIPAdapter : public CSmlDmAdapter, 
                          public MSIPProfileRegistryObserver

    {
    public:

        static CNSmlDmSIPAdapter* NewL( MSmlDmCallback* aDmCallback );
        static CNSmlDmSIPAdapter* NewLC( MSmlDmCallback* aDmCallback );

        virtual ~CNSmlDmSIPAdapter();

    private:  // Functions from base classes

        /**
         * From base class MsmlDmAdapter
         * Returns DDF version of the adapter.
         *
         * @since S60 3.0.
         * @param aVersion for returning DDF version of adapter
         */
        void DDFVersionL( CBufBase& aDDFVersion );

        /**
         * From base class MsmlDmAdapter
         * Builds DDF structure of the adapter.
         *
         * @since S60 3.0.
         * @param aDDF for returning DDF structure of adapter
         */
        void DDFStructureL( MSmlDmDDFObject& aDDF );

        /**
         * From base class MsmlDmAdapter
         * Updates leaf object value.
         *
         * @since S60 3.0.
         * @param aURI       URI of the object.
         * @param aLUID      LUID of the object (if the adapter has earlier
         *                   returned a LUID to the DM Module). For new
         *                   objects, this is the LUID inherited through the
         *                   parent node.
         * @param aObject    Data of the object.
         * @param aType      MIME type of the object.
         * @param aStatusRef Reference to correct command, i.e. this reference
         *                   must be used when calling the SetStatusL of this
         *                   command.
         */
        void UpdateLeafObjectL( 
            const TDesC8& aURI, 
            const TDesC8& aLUID, 
            const TDesC8& aObject,
            const TDesC8& aType, 
            const TInt aStatusRef );

        /**
         * From base class MsmlDmAdapter
         * Updates leaf object value. Not supported.
         *
         * @since S60 3.0.
         * @param aURI       URI of the object.
         * @param aLUID      LUID of the object (if the adapter has earlier
         *                   returned a LUID to the DM Module). For new
         *                   objects, this is the LUID inherited through the
         *                   parent node.
         * @param aStream    Data of the object. Adapter should create write
         *                   stream and return, when data is written to 
         *                   stream by DM agent, StreamCommittedL() is called
         *                   by DM engine.
         * @param aType      MIME type of the object.
         * @param aStatusRef Reference to correct command, i.e. this reference
         *                   must be used when calling the SetStatusL of this
         *                   command.
         */                            
        void UpdateLeafObjectL( 
            const TDesC8& aURI, 
            const TDesC8& aLUID, 
            RWriteStream*& aStream,
            const TDesC8& aType, 
            TInt aStatusRef );

        /**
         * From base class MsmlDmAdapter
         * Deletes an object.
         * 
         * @since S60 3.0.
         * @param aURI       URI of the object.
         * @param aLUID      LUID of the object (if the adapter has earlier
         *                   returned LUID to the DM Module).
         * @param aStatusRef Reference to correct command, i.e. this reference
         *                   must be used when calling the SetStatusL of this 
         *                   command.
         */        
        void DeleteObjectL( 
            const TDesC8& aURI, 
            const TDesC8& aLUID, 
            const TInt aStatusRef );

        /**
         * From base class MsmlDmAdapter
         * Fetches a leaf object.
         *
         * @since S60 3.0.
         * @param aURI        URI of the object.
         * @param aLUID       LUID of the object (if the adapter has earlier
         *                    returned LUID to the DM Module).
         * @param aType       MIME type of the object.
         * @param aResultsRef Reference to correct results, i.e. this
         *                    reference must be used when returning the result
         *                    by calling the SetResultsL.
         * @param aStatusRef  Reference to correct command, i.e. this
         *                    reference must be used when calling the SetStatusL of
         *                    this command.
         */        
        void FetchLeafObjectL( 
            const TDesC8& aURI, 
            const TDesC8& aLUID, 
            const TDesC8& aType, 
            const TInt aResultsRef, 
            const TInt aStatusRef );

        /**
         * From base class MsmlDmAdapter
         * Lists all children of given URI.
         *
         * @since S60 3.0.
         * @param aParentURI  URI of the parent object.
         * @param aParentLUID LUID of the parent object (if the adapter has
         *                    earlier returned LUID to the DM Module).
         * @param aPreviousURISegmentList URI list with mapping LUID 
         *                    information, which is known by DM engine. An
         *                    adapter can use this information when verifying
         *                    if old objects still exists. An adapter also
         *                    knows what objects are new to DM engine and can
         *                    provide LUID mapping for them.
         *                    aPreviousURISegmentList parameter (see above)
         *                    helps to recognise new objects.
         * @param aResultsRef Reference to correct results, i.e. this
         *                    reference must be used when returning the result
         *                    by calling the SetResultsL.
         * @param aStatusRef  Reference to correct command, i.e. this
         *                    reference must be used when calling the
         *                    SetStatusL of this command.
         */
        void ChildURIListL( 
            const TDesC8& aURI, 
            const TDesC8& aLUID, 
            const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, 
            const TInt aResultsRef, 
            const TInt aStatusRef );

        /**
         * From base class MsmlDmAdapter
         * Adds a node.
         *
         * @since S60 3.0.
         * @param aURI        URI of the object.
         * @param aParentLUID LUID of the parent object (if the adapter has
         *                    earlier returned LUID to the DM Module).   
         * @param aStatusRef  Reference to correct command, i.e. this 
         *                    reference must be used when calling the
         *                    SetStatusL of this command.
         */        
        void AddNodeObjectL( 
            const TDesC8& aURI, 
            const TDesC8& aParentLUID, 
            const TInt aStatusRef );

        /**
         * From base class MsmlDmAdapter
         * Executes command. Not supported.
         *
         * @since S60 3.0.
         * @param aURI       URI of the command.
         * @param aLUID      LUID of the object (if the adapter has earlier
         *                   returned LUID to the DM Module).
         * @param aArgument  Argument for the command.
         * @param aType      MIME type of the object.
         * @param aStatusRef Reference to correct command, i.e. this reference
         *                   must be used when calling the SetStatusL of this
         *                   command.
         * 
         */
        void ExecuteCommandL( 
            const TDesC8& aURI, 
            const TDesC8& aLUID, 
            const TDesC8& aArgument, 
            const TDesC8& aType, 
            const TInt aStatusRef );

        /**
         * From base class MsmlDmAdapter
         * Executes command. Not supported.
         *
         * @since S60 3.0.
         * @param aURI       URI of the command.
         * @param aLUID      LUID of the object (if the adapter has earlier
         *                   returned LUID to the DM Module).
         * @param aStream    Argument for the command. Adapter should create
         *                   write stream and return, when data is written to
         *                   stream by DM agent, StreamCommittedL() is called
         *                   by DM engine.
         * @param aType      MIME type of the object.
         * @param aStatusRef Reference to correct command, i.e. this reference
         *                   must be used when calling the SetStatusL of this
         *                   command.
         * 
         */
        void ExecuteCommandL( 
            const TDesC8& aURI, 
            const TDesC8& aLUID, 
            RWriteStream*& aStream, 
            const TDesC8& aType, 
            const TInt aStatusRef );

        /**
         * From base class MsmlDmAdapter
         * Copies command. Not supported.
         *
         * @since S60 3.0.
         * @param aTargetURI  Target URI for the command.
         * @param aSourceLUID LUID of the target object (if one exists, and if
         *                    the adapter has earlier returned a LUID to the
         *                    DM Module).
         * @param aSourceURI  Source URI for the command.
         * @param aSourceLUID LUID of the source object (if the adapter has
         *                    earlier returned a LUID to the DM Module).
         * @param aType       MIME type of the objects.
         * @param aStatusRef  Reference to correct command, i.e. this
         *                    reference must be used when calling the 
         *                    SetStatusL of this command.
         * 
         */
        void CopyCommandL( 
            const TDesC8& aTargetURI, 
            const TDesC8& aTargetLUID, 
            const TDesC8& aSourceURI, 
            const TDesC8& aSourceLUID, 
            const TDesC8& aType, 
            TInt aStatusRef );

        /**
         * From base class MsmlDmAdapter
         * Fetches the size of a leaf object.
         *
         * @since S60 3.0.
         * @param aURI        URI of the object.
         * @param aLUID       LUID of the object (if the adapter has earlier
         *                    returned LUID to the DM Module).
         * @param aType       MIME type of the object.
         * @param aResultsRef Reference to correct results, i.e. this
         *                    reference must be used when returning the result
         *                    by calling the SetResultsL.
         * @param aStatusRef  Reference to correct command, i.e. this
         *                    reference must be used when calling the
         *                    SetStatusL of this command.
         * 
         */
        void FetchLeafObjectSizeL( 
            const TDesC8& aURI, 
            const TDesC8& aLUID, 
            const TDesC8& aType, 
            TInt aResultsRef, 
            TInt aStatusRef );

        /**
         * From base class MsmlDmAdapter
         *
         * @since S60 3.0.
         * 
         */     
        void StartAtomicL();

        /**
         * From base class MsmlDmAdapter
         *
         * @since S60 3.0.
         * 
         */
        void CommitAtomicL();

        /**
         * From base class MsmlDmAdapter
         *
         * @since S60 3.0.
         * 
         */
        void RollbackAtomicL();

        /**
         * From base class MsmlDmAdapter
         * @since S60 3.0.
         * @param aItemSize size limit for stream usage
         * @return TBool ETrue for streaming support
         * 
         */
        TBool StreamingSupport( TInt& aItemSize );

        /**
         * From MsmlDmAdapter
         *
         * @since S60 3.0.
         * 
         */
        void StreamCommittedL();

        /**
         * From MsmlDmAdapter
         *
         * @since S60 3.0.
         * 
         */
        void CompleteOutstandingCmdsL();

    private: 

        CNSmlDmSIPAdapter( TAny* aEcomArguments );
        void ConstructL();

        /**
         * Fetches an object and stores fetch result to aResult.
         *
         * @since S60 3.0.
         * @param aURI URI of the object to be fetched.
         * @param ALUID Local UID of the object to be fetched.
         * @param aResult Handle for storing fetch result.
         * @return DM status.
         */
        CSmlDmAdapter::TError FetchObjectL( const TDesC8& aURI, 
            const TDesC8& aLUID, 
            CBufBase& aResult );
    
        /**
         * From base class MSIPProfileRegistryObserver.
         * An event related to SIP Profile has occurred.
         *
         * @param aProfileId a profile Id
         * @param aEvent an occurred event
         */
        void ProfileRegistryEventOccurred( TUint32 aProfileId, 
            MSIPProfileRegistryObserver::TEvent aEvent);

        /**
         * From base class MSIPProfileRegistryObserver.
         * An asynchronous error has occurred related to SIP profile
         * Event is send to those observers, who have the
         * corresponding profile instantiated.
         *
         * @param aProfileId the id of failed profile 
         * @param aError an occurred error
         */
        void ProfileRegistryErrorOccurred( TUint32 aProfileId, TInt aError );

        /**
         * Loads SIP profile information
         * @since S60 3.0.
         */        
        void LoadProfilesL();

        /**
         * Converts descriptor to int.
         * @since Series 60 2.8
         * @param aDes descriptor to convert.
         * @return TInt. Converted value.
         */
        TInt DesToInt(const TDesC8& aDes );

        /**
         * Removes last uri segment from aURI
         * @since S60 3.0.
         * @param aDes Descriptor to handle
         * @return pointer to uri
         */        
        TPtrC8 RemoveLastURISeg( const TDesC8& aURI );

        /**
         * Gets number of uri segments.
         * @since S60 3.0.
         * @param aURI Descriptor to handle
         * @return TInt. Number of URI segments.
         */
        TInt NumOfURISegs( const TDesC8& aURI );

        /**
         * Find profile location from aURI
         * @since S60 3.0.
         * @param aURI Descriptor to look for
         * @return Profile ID location.
         */
        TInt FindHexProfile( const TDesC8& aURI );

        /**
         * Gets last uri segment.
         * @since S60 3.0.
         * @param aURI Descriptor to handle
         * @return Pointer to URI
         */
        TPtrC8 LastURISeg( const TDesC8& aURI );

        /**
         * Removes ./ from uri
         * @since S60 3.0.
         * @param aURI Descriptor to handle
         * @return pointer to uri
         */
        TPtrC8 RemoveDotSlash( const TDesC8& aURI );

        /**
         * Set conref value according to aObject
         * @since S60 3.0.
         * @param aObject URI to look from AP adapter
         * @return Access point ID number
         */    
        TInt SetConRefL( const TDesC8& aObject, const TInt& aProfileid );

        /**
         * Get Access Point URI reference from AP adapter
         * @since S60 3.0.
         * @param aObject Object to insert found URI.
         * @param aIapID IapID to look.
         */
        TBool GetConRefL( CBufBase& aObject, const TInt& aProfileId );

        /**
         * Check for duplicate providerName and rename if same
         * @since S60 3.0.
         * @param aProfile Profile to check.
         * @param aProfileId ProfileId to be skipped in check.
         */
        void CheckDuplicateNameL( CSIPManagedProfile& aProfile, 
                                  const TInt& aProfileId );

        /**
         * Get SNAP ID.
         * @since S60 3.2.
         * @param aUri URI from which to find SNAP ID.
         * @return SNAP ID.
         */
        TUint32 SnapIdL( const TDesC8& aUri );

        /**
         * Set SNAP URI.
         * @since S60 3.2.
         * @param aObject Object in which found URI is set.
         * @param aSnapId SNAP ID from which to find URI.
         * @return ETrue if successful, EFalse if not.
         */
        MSmlDmAdapter::TError GetSnapUriL( TDes8& aObject, TUint32 aSnapId );

        /**
         * Update method for OutboundProxy node.
         * @since S60 3.2.
         * @param aProfileId ID of the profile to be updated.
         * @param aUri URI of the object to be updated.
         * @param aObject Value to be updated.
         * @return DM status.
         */
        MSmlDmAdapter::TError UpdateOutboundProxyNodeL( TUint32 aProfileId, 
            const TDesC8& aUri, const TDesC8& aObject );

        /**
         * Update method for RegistrarServer node.
         * @since S60 3.2.
         * @param aProfileId ID of the profile to be updated.
         * @param aUri URI of the object to be updated.
         * @param aObject Value to be updated.
         * @return DM status.
         */
        MSmlDmAdapter::TError UpdateRegistrarServerNodeL( TUint32 aProfileId, 
            const TDesC8& aUri, const TDesC8& aObject );

        /**
         * Fetch method for OutboundProxy node.
         * @since S60 3.2.
         * @param aUri URI of the object to be fetched.
         * @param aLuid LUID of the object to be fetched.
         * @param aSegmentResult Fetch result object.
         * @return DM status.
         */
        MSmlDmAdapter::TError FetchOutboundProxyNodeL( TUint32 aProfileId,
            const TDesC8& aUri, TDes8& aSegmentResult );

        /**
         * Fetch method for RegistrarServer node.
         * @since S60 3.2.
         * @param aUri URI of the object to be fetched.
         * @param aLuid LUID of the object to be fetched.
         * @param aSegmentResult Fetch result object.
         * @return DM status.
         */
        MSmlDmAdapter::TError FetchRegistrarServerNodeL( TUint32 aProfileId,
            const TDesC8& aUri, TDes8& aSegmentResult );

    private:     // Data

        /**
         * Handle for returning data to DM module.
         * Own.
         */
        MSmlDmCallback* iDmCallback;

        /**
         * Object for handling SIP profile settings.
         * Own.
         */
        CSIPManagedProfileRegistry* iSIPManagedProfileRegistry;

        /**
         * Array of pointers to all loaded SIP profiles.
         * Own.
         */
        CArrayPtrFlat<CSIPManagedProfile>* iProfiles;

        /**
         * Status of DM module.
         */
        TInt iStatus;

        /**
         * Array for knowing if there are modifications to profiles in memory.
         */
        RArray<TInt> iModifications;

        /**
         * Handle for returning results to dm-module.
         * Own.
         */
        CBufBase *iResults;

		private:    // Friend classes     

    	#ifdef TEST_EUNIT
    	public:
        	friend class UT_CNSmlDmSIPAdapter;
    	#endif
    };

#endif // __NSMLDMSIPADAPTER_H__
