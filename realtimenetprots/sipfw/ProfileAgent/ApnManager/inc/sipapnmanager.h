// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : sipapnmanager.h
// Part of     : SIP Profile Server
// implementation
// Version     : 1.0
//


#ifndef CSIPAPNMANAGER_H
#define CSIPAPNMANAGER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CSIPApnConfigurationHandler;

// OBSERVER
class MSIPApnChangeObserver
    {
public:
    /**
    * Called when apn change completed successfully or failed so fatally
    * that there's no change expected any further on that IAPId.
    */
    virtual void ApnChanged( const TDesC8& aApn, TUint32 aIapId, TInt aError ) = 0;
    };

// CLASS DEFINITION
/**
 * 
 */
class CSIPApnManager : public CBase
	{
	public: // Constructors and destructor

		static CSIPApnManager* NewL( MSIPApnChangeObserver& aObserver );

		~CSIPApnManager();

    public: // New methods

        // Update the cached primary or secondary APN
        void UpdateApnL( TUint32 aIapId, TBool aIsPrimaryApn, const TDesC8& aApn );
        
        TBool IsFailed( TUint32 aIapId );
        void SetFailed( TUint32 aIapId ,TBool aIsFailed, TBool aIsFatalFailure );

        TBool IsPrimaryApnInUse( TUint32 aIapId );
        
		void WriteApnL( TUint32 aIapId, TBool aIsPrimaryApn, const TDesC8* aApn );
		
		TBool HasPendingTasks() const;
		
		/*
		 * Checks whether the IAPId is ModemBearer
		 */
		TBool IsIapGPRSL( TUint32 aIapId );

	private: // Constructors

		CSIPApnManager( MSIPApnChangeObserver& aObserver );		
    
        void ConstructL();
    
    private:
        
        CSIPApnConfigurationHandler* FindIapIdHandler( TUint32 aIapId );
 
        void SetApnL( const TDesC8& aApn, 
                      TBool aUseSecureAuthentication, 
                      TBool aAllowAsync,
                      TUint32 aIapId);
        
        void CreateHandlerL( TUint32 aIapId );

    private: // Data
	
	    MSIPApnChangeObserver&                     iObserver;
	    RPointerArray<CSIPApnConfigurationHandler> iHandlers;
	    
#ifdef CPPUNIT_TEST	
	    friend class CSIPApnManagerTest;
#endif

	};

#endif // CSIPAPNMANAGER_H
