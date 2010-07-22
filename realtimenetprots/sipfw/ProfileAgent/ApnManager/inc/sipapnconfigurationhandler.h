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
// Name        : sipapnconfigurationhandler.h
// Part of     : SIP Profile Server
// implementation
// Version     : 1.0
//

#ifndef __CSIPAPNCONFIGURATIONHANDLER_H__
#define __CSIPAPNCONFIGURATIONHANDLER_H__

// INCLUDES
#include <etelpckt.h>
#include <es_enum.h>
#include <es_enum_partner.h>
#include <centralrepository.h>
#include "sipapnmanager.h"

// CONSTANTS
const TInt KDBMaxRetryCount = 100;

// FORWARD DECLARATIONS
class CCommsDatabase;
namespace CommsDat
    {
    class CCDRecordBase;
    }

// CLASS DEFINITION sipapnconfigurationhandler.h
/**
 *
 */
class CSIPApnConfigurationHandler : public CActive
	{
	public:	// Constructors and destructor
	
	    enum TSipApnMonitoringState
	        {
	        EMonitoringIdle,
	        EMonitoringConnection,
	        EMonitoringDatabase
	        };

		/**
		 * Static constructor.
		 *
		 * @return An initialized instance of this class.
		 */
		static CSIPApnConfigurationHandler* NewL( MSIPApnChangeObserver& aObserver,
                                                        TUint32 aIapId);
		
		/**
		 * Static constructor.
		 *
		 * @return An initialized instance of this class.
		 */
		static CSIPApnConfigurationHandler* NewLC( MSIPApnChangeObserver& aObserver, 
                                                        TUint32 aIapId);

		/// Destructor
		~CSIPApnConfigurationHandler();
		
	public:
	    
	    /**
	    * Change APN of specified IAP. If async change is allowed, change
	    * may be done later if it was not possible at the moment. Once apn
	    * is changed, observer is notified about change.
	    */
	    void SetApnL( const TDesC8& aApn,
	                  TBool aUseSecureAuthentication,
	                  TBool aAllowAsync );
	                   
	    TBool IsPrimaryApnUsed();
	    
		TBool HasPendingTasks() const;
		
        TUint32 HandlerIapId() const;
        
        TBool IsFailed() const;
        
        void SetFailed(TBool aIsFailed, TBool aIsFatalFailure );
        
        void UpdateApnL( TBool aIsPrimaryApn, const TDesC8& aApn );

	protected: // From CActive

		void DoCancel();
		void RunL();
		TInt RunError( TInt aError );
		
    private:
    
        TBool IsInUseL( TConnectionInfo& aConnectionInfo );

        void StartMonitoringConnectionL( TConnectionInfo& aConnectionInfo );
        
        void WatchConnectionStatusChange();
        
        void WatchDatabaseStatusChangeL();
        
        TBool ApnChangeNeededL( const TDesC8& aApn );
        
        TBool ChangeApnIfNotInUseL( TBool aAllowAsync = ETrue );
        
        TBool IssueApnChangeL( TUint32 aIapId, 
                               const TDesC8& aApn, 
                               TBool aUseSecureAuthentication,
                               TBool aAllowAsync = ETrue );
        
        void ChangeApnL( const TDesC8& aApn, 
                         TBool aUseSecureAuthentication );
        
        TBool ClearProtectedRecord( CommsDat::CCDRecordBase& aRecord );
        
        void SetMonitoringState( TSipApnMonitoringState aMonitoringState );
        
        TSipApnMonitoringState MonitoringState() const;
        
        void ConnectionMonitoringCompletedL( TInt aError );
        
        void DatabaseMonitoringCompletedL( TInt aError );
        
        void SendApnChangedNotificationL( const TDesC8& aApn, TInt aError = KErrNone );
        
        TDesC8& PrimaryApn();
        
        TDesC8& SecondaryApn();
        
        /**
        * Reads the APN of specified IAP.
        */
        void ReadCurrentApnL();
        

	private: // Constructors

		/// Constructor
		CSIPApnConfigurationHandler( MSIPApnChangeObserver& aObserver,
                                                            TUint32 aIapId);

		/// 2nd phase constructor
		void ConstructL();
		
		void BlockCellularDataUsageL();
		
		void AllowCellularDataUsage();

		
	    /**
	    * Rollsback db in case of failure      
	        * @param aDb 
	    */
	    static void RollBackDBTransaction(TAny* aDb);

	private: // Data
	
		MSIPApnChangeObserver&          iObserver;    
	    TSipApnMonitoringState          iMonitoringState;
		RSocketServ                     iSocketSrv;
		RConnection                     iConnection;
		TPckgBuf<TConnectionInfo>       iConnectionInfo;
        TNifProgressBuf                 iProgress;
        HBufC8*                         iApnProposal;
        TUint32                         iIapId;
        CCommsDatabase*                 iCommsDatabase;
        TInt                            iDBMonitoringRetryCount;
        HBufC8*                         iCurrentApn;
        TBool                           iApnUseSecureAuthProposal;
        TBool                           iIsFailed;
        TBool                           iIsFatalFailure;
        HBufC8*                         iPrimaryApn;
        HBufC8*                         iSecondaryApn;
        CRepository*                    iRepository;
        TInt                            iCurrentUsageStatus;
        TBool                           iCellularDataBlocked;
        
#ifdef CPPUNIT_TEST	
	    friend class CSIPApnManagerTest;
	    friend class CSIPApnManager;
#endif
	
	};

#endif // __CSIPAPNCONFIGURATIONHANDLER_H__

