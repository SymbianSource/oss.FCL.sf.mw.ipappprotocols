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
* Name          : CSIPClientResolver.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPCLIENTRESOLVER_H
#define CSIPCLIENTRESOLVER_H

#include <e32base.h>
#include <ecom/ecom.h>
#include "MSipClients.h"
#include "_sipcodecdefs.h"


// FORWARD DECLARATIONS
class CSIPClientData;
class CSIPClientDataParser;
class CSipLaunchingStrategies;
class TSipClient;
class CSIPRequest;
class CSIPResponse;
class CSIPClientResolver2;

// CLASS DEFINITIONS
/**
 *
 */
class CSIPClientResolver : public CActive, public MSipClients
    {
    public: // Constructors and destructor
	
        IMPORT_C static CSIPClientResolver* NewL ();
    
        IMPORT_C ~CSIPClientResolver ();

    public: // New functions

        IMPORT_C CSIPResponse* FindUidL(const CSIPRequest& aRequest,
								RArray< TSipClient >& aUids );
		
    public: // From MSipClients

        MSipClient* GetByUID( const TUid& aUid ) const;

    protected: // From CActive

        void DoCancel();
        void RunL();   

    private: // New methods

		/**
		* Refreshes clients' data for those clients
		* which are returning their SIP capabilities dynamically
		* instead of using fixed values in their plugin's resource file.
		*/
        void RefreshClientDataL();

		/// Fill implentation registry
		void ListImplementationsL();

		/// Delete implementation registry
		void RemoveRegistry();

		/// Start monitoring changes in ECom plugin implementations 
		void NotifyOnChange();

		/// Copies all the current implementations to the given array  
		void CopyAllUidsL( RArray< TUid >& aUids );

        /// Adds ROM- and RAM-based clients to the array
        void AddClientsL( RPointerArray< CSIPClientData >& aRegistry );

        /// Converts and appends ECom implementation information
        /// to and array of CSIPClientDatas
        void ConvertClientDataL( const RImplInfoPtrArray& aImplInfo,
                                 RPointerArray< CSIPClientData >& aRegistry,
                                 TBool aRomClient );

	    /// Adds CSIPClientData to the array based on ECom-plugin's data
		void AddClientDataL( RPointerArray< CSIPClientData >& aRegistry,
			                 CImplementationInformation& aInfo,
			                 TBool aRomClient );
		
		/// Creates a thread to load the client plugin and 
	    /// ask the capabilities from it and parses the results	                 
        void ReloadClientDataL( CSIPClientData& aClientData );

        /// Removes the implementations present in the first array 
        /// from the second array.
        void RemoveDuplicates( const RImplInfoPtrArray& aRomInfo,
                               RImplInfoPtrArray& aAllInfo );

		// Needed for cleanup of a RPointerArray< CSIPClientData >:
		static void ResetAndDestroy( TAny* anArray );
		
	    void CreateWorkerThreadL();
	
	    static TInt WorkerThreadFunction(TAny* aPtr);
	
	    void ReadPluginCapsL();		

    private:  // Constructors

        /// Default constructor
        CSIPClientResolver();
    
        /// 2nd phase constructor.
        void ConstructL();
    
    private: // private data

        /// Handle to ECom session. Owned.
        REComSession* iECom;

        /// XML parser for SIP client data
        CSIPClientDataParser* iClientDataParser;

        /// Information registry for installed SIP clients. Owned.
        RPointerArray< CSIPClientData >* iRegistry;

        /// Launching strategies. Owned.
        CSipLaunchingStrategies* iLaunchingStrategies;
        
        /// Used for parameter passing to worker thread
        TUid iTmpImplementationUid;
        
        /// Used for parameter passing to worker thread
        HBufC8* iTmpPluginCaps;

    private: // For testing purposes

         UNIT_TEST(CSIPClientResolver_test)
    };

#endif // CSIPCLIENTRESOLVER_H

// End of File
