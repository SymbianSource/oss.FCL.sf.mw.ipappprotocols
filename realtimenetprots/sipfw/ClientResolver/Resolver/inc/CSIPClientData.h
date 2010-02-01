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
* Name          : CSIPClientData.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/
#ifndef CSIPCLIENTDATA_H
#define CSIPCLIENTDATA_H

// INCLUDES
#include <e32base.h>
#include "MSipClient.h"

// FORWARD DECLARATIONS
class CSdpMediaField;

// CLASS DEFINITION
/**
 * CSIPClientData implements a data container for 
 * SIP client information.
 */
class CSIPClientData : public CBase, public MSipClient
	{
	public: // Constructors and destructor

		static CSIPClientData* NewL( const TUid& aImplementationUid,
		                             const TUid& aClientUid,
		                             TBool aRomBased,
		                             TBool aHasDynamicCapabilities );

		static CSIPClientData* NewLC( const TUid& aImplementationUid,
		                              const TUid& aClientUid,
		                              TBool aRomBased,
		                              TBool aHasDynamicCapabilities );
		                              		                              
		~CSIPClientData();


	public: // From MSipClient

		const TUid& ClientUid() const;
		TBool RomBased() const;
		TBool AllowStarting() const;
	    RPointerArray< CSIPHeaderBase >& SIPHeaders();
	    CSIPFeatureSet& SIPFeatureSet();
		CSdpDocument& SdpDocument();


  	public: // New methods

        const TUid& ImplementationUid() const;

		TBool HasDynamicCapabilities() const;

		void SetAllowStarting( TBool aAllowStarting );

		void AddMediaFieldL( CSdpMediaField* aMediaField );

        CSIPClientData* CloneWithoutCapabilitiesLC();

	private: // Constructors

		/// Default constructor.
		CSIPClientData( const TUid& aImplementationUid,
		                const TUid& aClientUid,
		                TBool aRomBased,
		                TBool aHasDynamicCapabilities );

		/// 2nd phase constructor.
		void ConstructL();

	private: // Data

        // Implementation Uid of the client's ECom plugin 
        TUid iImplementationUid;

		// Client Uid.
		TUid iClientUid;
		
		// ETrue if application is ROM base, EFalse otherwise.
		TBool iRomBased;		
		
		// ETrue if the client provides its capabilities dynamically 
		TBool iHasDynamicCapabilities;

		// ETrue if application is allowed to be started, EFalse otherwise.
		TBool iAllowStarting;

		// SIP header collection. Owned.
		RPointerArray< CSIPHeaderBase > iSIPHeaders;
		
		// SIP feature set. Owned
        CSIPFeatureSet* iSIPFeatureSet;

		// Sdp document object. Owned.
		CSdpDocument* iSdpDocument;
	};

#endif // CSIPCLIENTDATA_H
