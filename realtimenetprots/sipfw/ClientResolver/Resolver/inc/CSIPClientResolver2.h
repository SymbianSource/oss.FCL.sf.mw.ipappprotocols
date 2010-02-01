/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSIPClientResolver2.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPCLIENTRESOLVER2_H
#define CSIPCLIENTRESOLVER2_H

#include <e32base.h>
#include <ecom/ecom.h>
#include "sipresolvedclient2.h"
#include "_sipcodecdefs.h"

// FORWARD DECLARATIONS
// CLASS DEFINITIONS
class CSipClientResolverConfig;
class CSIPRequest;
class TSipClient;
class CSipHeaderStrategyBase;
/**
 *
 */
NONSHARABLE_CLASS(CSIPClientResolver2) 
: public CBase
    {
    public: // Constructors and destructor
	
        static CSIPClientResolver2* NewL( CSIPRequest& aRequest );
        
        static CSIPClientResolver2* NewLC( CSIPRequest& aRequest );

    
        ~CSIPClientResolver2 ();

	public: //New methods		
		RArray< TSipClient >& ClientsData();
		RPointerArray< CSIPResolvedClient2 >& Clients();
		TBool GetSipClientDataL( RArray< TSipClient >& aUids );
		void FetchMatchingUidsL( CSIPRequest& aRequest, 
								 CSipHeaderStrategyBase& aStrategy );
								 
	    static void PushLC(RPointerArray<CSIPResolvedClient2>* aArray);
	    static void ResetAndDestroyArray(TAny* anArray);

    private: // New methods
		RArray <TUid> UIDsForUserNameL( CSIPRequest& aRequest );
		void ListImplementationsL();
		void AddClientDataL( CImplementationInformation& aInfo );
		void LoadImplementationsL();
		
    private:  // Constructors

        /// Default constructor
        CSIPClientResolver2();
    
        /// 2nd phase constructor.
        void ConstructL( CSIPRequest& aRequest );
    
    private: // private data

		RArray< TUid > 						 iUids;
		RArray< TSipClient > 				 iClientsData;
		RPointerArray< CSIPResolvedClient2 > iResolvedClients;
		CSipClientResolverConfig* 			 iClientResolverConfig;
		
    private: // For testing purposes

         UNIT_TEST(CSIPClientResolver2_test)
    };

#endif // CSIPCLIENTRESOLVER2_H

// End of File
