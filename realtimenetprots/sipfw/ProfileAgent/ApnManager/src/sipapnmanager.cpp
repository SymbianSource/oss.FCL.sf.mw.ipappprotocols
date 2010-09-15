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
// Name        : sipapnmanager.cpp
// Part of     : SIP Profile Server
// implementation
// Version     : 1.0
//

#include <commsdattypesv1_1.h>
#include <commsdat.h>
#include <commdb.h>
#include <f32file.h>
#include "sipapnmanager.h"
#include "SipProfileLog.h"
#include "sipapnconfigurationhandler.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CSIPApnManager::NewL
// ----------------------------------------------------------------------------
//
CSIPApnManager* CSIPApnManager::NewL( 
    MSIPApnChangeObserver& aObserver )
    {
    CSIPApnManager* self = new (ELeave) CSIPApnManager( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CSIPApnManager::~CSIPApnManager
// ----------------------------------------------------------------------------
//
CSIPApnManager::~CSIPApnManager()
    {
    iHandlers.ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
// CSIPApnManager::UpdateApnL
// ----------------------------------------------------------------------------
//	
void CSIPApnManager::UpdateApnL( TUint32 aIapId, TBool aIsPrimaryApn, const TDesC8& aApn )
	{
    CSIPApnConfigurationHandler* handler = FindIapIdHandler( aIapId );
    if (!handler)
        {
        CreateHandlerL(aIapId);
        handler = FindIapIdHandler(aIapId);
        }
    
        PROFILE_DEBUG1("CSIPApnManager::UpdateApnL(), handler exists")
        handler->UpdateApnL( aIsPrimaryApn, aApn);
	}

// ----------------------------------------------------------------------------
// CSIPApnManager::IsFailed
// ----------------------------------------------------------------------------
//    
TBool CSIPApnManager::IsFailed( TUint32 aIapId )
    {
    CSIPApnConfigurationHandler* handler = FindIapIdHandler( aIapId );
    return (handler && handler->IsFailed());
    }

// ----------------------------------------------------------------------------
// CSIPApnManager::SetFailed
// Setting back to "not-failed" state is allowed only if fatal failure has not
// occured.
// ----------------------------------------------------------------------------
// 
void CSIPApnManager::SetFailed( TUint32 aIapId ,TBool aIsFailed, TBool aIsFatalFailure )
    {
    CSIPApnConfigurationHandler* handler = FindIapIdHandler( aIapId );
    if(handler)
        {
        handler->SetFailed( aIsFailed, aIsFatalFailure );
        }
    }

// ----------------------------------------------------------------------------
// CSIPApnManager::IsPrimaryApnInUse
// ----------------------------------------------------------------------------
//
TBool CSIPApnManager::IsPrimaryApnInUse( TUint32 aIapId )
    {
    CSIPApnConfigurationHandler* handler = FindIapIdHandler( aIapId );
    return ( handler && handler->IsPrimaryApnUsed() );
    }

// ----------------------------------------------------------------------------
// CSIPApnManager::WriteApnL
// ----------------------------------------------------------------------------
//
void CSIPApnManager::WriteApnL( TUint32 aIapId, TBool /*aIsPrimaryApn*/, const TDesC8* aApn )
	{
	CSIPApnConfigurationHandler* handler = FindIapIdHandler( aIapId );
	    if (!handler)
	        {
	        CreateHandlerL(aIapId);
	        handler = FindIapIdHandler(aIapId);
	        }
	    
	    handler->SetApnL( *aApn, EFalse, ETrue);
	}

// ----------------------------------------------------------------------------
// CSIPApnManager::HasPendingTasks
// ----------------------------------------------------------------------------
//
TBool CSIPApnManager::HasPendingTasks() const
    {    
    TInt count = iHandlers.Count();
    PROFILE_DEBUG3( 
            "CSIPApnManager::HasPendingTasks handler count", count )

    for ( TInt i = 0; i < count; ++i )
        {
        if ( iHandlers[ i ]->HasPendingTasks() )
            {
            PROFILE_DEBUG1( 
                    "CSIPApnManager::HasPendingTasks return ETrue" )
            return ETrue;
            }
        }
    PROFILE_DEBUG1( 
            "CSIPApnManager::HasPendingTasks return EFalse" )
    return EFalse;
    }
// ----------------------------------------------------------------------------
// CSIPApnManager::CSIPApnManager
// ----------------------------------------------------------------------------
//
CSIPApnManager::CSIPApnManager( MSIPApnChangeObserver& aObserver ) :
    iObserver( aObserver )
    {
    }
    
// ----------------------------------------------------------------------------
// CSIPApnManager::ConstructL
// Read APNs from settings file but don't write to CommsDat yet. If IMS profile
// has APNs, use them instead. APN is written to CommsDat when the operation
// (register, deregister, update..) that caused IMS agent to be loaded, starts
// (CSIPIMSProfileAgent::SelectInitialApnL).
// ----------------------------------------------------------------------------
//    
void CSIPApnManager::ConstructL()
    {
    PROFILE_DEBUG1("CSIPApnManager::ConstructL()" )  
    }

// ----------------------------------------------------------------------------
// CSIPApnManager::FindIapIdHandler
// ----------------------------------------------------------------------------
//
CSIPApnConfigurationHandler* CSIPApnManager::FindIapIdHandler( TUint32 aIapId )
    {
    CSIPApnConfigurationHandler* handler = NULL;
    TBool found(EFalse);
    
    for(TInt i =0; i<iHandlers.Count() && !found ; i++)
        {
        if (iHandlers[i]->HandlerIapId()== aIapId)
            {
            PROFILE_DEBUG3("CSIPApnManager::FindIapIdHandler, found handler for aIapId", aIapId )
            found = ETrue;
            handler = iHandlers[ i ];
            }
        }
    
    return handler;
    }

// ----------------------------------------------------------------------------
// CSIPApnManager::CreateHandlerL
// ----------------------------------------------------------------------------
//
void CSIPApnManager::CreateHandlerL( TUint32 aIapId )
	{
    PROFILE_DEBUG1( 
            "CSIPApnManager::CreateHandlerL , enter" )
    PROFILE_DEBUG3( 
        "CSIPApnManager::CreateHandlerL for IapId", aIapId )

    CSIPApnConfigurationHandler* Handler = 
        CSIPApnConfigurationHandler::NewL( iObserver, aIapId);
    CleanupStack::PushL( Handler );
    iHandlers.AppendL( Handler );
    CleanupStack::Pop( Handler );
    PROFILE_DEBUG1( 
              "CSIPApnManager::CreateHandlerL, exit" )
	}

// ----------------------------------------------------------------------------
// CSIPApnManager::IsIapGPRSL
// ----------------------------------------------------------------------------
//
TBool CSIPApnManager::IsIapGPRSL( TUint32 aIapId )
    {
    TBool isIapGPRS(EFalse);          
    using namespace CommsDat;        
    CMDBSession* db = CMDBSession::NewL( KCDVersion1_1 );
    CleanupStack::PushL( db );
    // Set any attributes if any
    db->SetAttributeMask( ECDHidden );    
    // Create an iap record
    CCDIAPRecord* iapRecord = 
            static_cast<CCDIAPRecord*>(
                    CCDRecordBase::RecordFactoryL( KCDTIdIAPRecord ) );
    CleanupStack::PushL( iapRecord );        
    iapRecord->SetRecordId( aIapId );
    iapRecord->LoadL( *db );  
    TPtrC bearerType(iapRecord->iBearerType);
    if(bearerType.CompareF(TPtrC(KCDTypeNameModemBearer)) == 0)
        {
        isIapGPRS = ETrue;
        }
    
    db->ClearAttributeMask( ECDHidden );
    
    CleanupStack::PopAndDestroy( iapRecord );
    CleanupStack::PopAndDestroy( db );
    PROFILE_DEBUG3(
                    "CSIPApnConfigurationHandler::IsIapGPRSL(),isIapGPRS", isIapGPRS )
    return isIapGPRS;
    }
// End of File
