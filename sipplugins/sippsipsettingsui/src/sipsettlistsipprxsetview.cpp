/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The proxy view controller
*
*/


// INCLUDE FILES

#include    "sipsettlistsipprxsetview.h"
#include    "mgssipprofilehandler.h"
#include    "sipsettlistsipsrvsetmodel.h"
#include    "viewid.h"
#include    <aknViewAppUi.h>
#include    <gssipsettingspluginrsc.rsg> //GUI Resource
#include    "gssippluginlogger.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPSettListSIPPrxSetView::CSIPSettListSIPPrxSetView
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIPSettListSIPPrxSetView::CSIPSettListSIPPrxSetView( 
	MGSSIPProfileHandler* aHandler )
    : CSIPSettListSIPSrvSetView( R_QTN_PROXY_TITLE_PANE_LABEL ),
    iHandler( aHandler )
    {
    __GSLOGSTRING("CSIPSettListSIPPrxSetView::CSIPSettListSIPPrxSetView" )
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPPrxSetView::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPPrxSetView::ConstructL()
    {
    __GSLOGSTRING("CSIPSettListSIPPrxSetView::ConstructL" )
    BaseConstructL( R_GS_SIP_SERVER_SETTING_VIEW );    
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPPrxSetView::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPSettListSIPPrxSetView* CSIPSettListSIPPrxSetView::NewLC( 
    MGSSIPProfileHandler* aHandler )
    {
    __GSLOGSTRING("CSIPSettListSIPPrxSetView::NewLC" )
    CSIPSettListSIPPrxSetView* self = 
        new ( ELeave ) CSIPSettListSIPPrxSetView( aHandler );
    
    CleanupStack::PushL( self );
    self->ConstructL();    

    return self;
    }
    
// Destructor
CSIPSettListSIPPrxSetView::~CSIPSettListSIPPrxSetView()
    {
    __GSLOGSTRING("CSIPSettListSIPPrxSetView::~CSIPSettListSIPPrxSetView" )
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPPrxSetView::Id
// Returns the UID of the view
// -----------------------------------------------------------------------------
//
TUid CSIPSettListSIPPrxSetView::Id() const
    {    
    return KSettListSIPProxyServerView;
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPPrxSetView::DataStruct
// Returns the used data structure
// -----------------------------------------------------------------------------
//
TGSSIPServerData* CSIPSettListSIPPrxSetView::DataStruct()
    {        
    return &( iHandler->ProfileData()->iProxy );
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPPrxSetView::DataStruct
// Returns the used data structure
// -----------------------------------------------------------------------------
//
TInt CSIPSettListSIPPrxSetView::ResourceID()
    {
    return R_SIP_PROXY_SETTING_VIEW_ITEM_LIST;
    }

// -----------------------------------------------------------------------------
// CGSSettListSIPPrxSetView::ProfileType
// Returns the used profile type
// -----------------------------------------------------------------------------
//
TSIPProfileTypeInfo::TSIPProfileClass CSIPSettListSIPPrxSetView::ProfileType()
    {    
    const TSIPSettingsData* data = iHandler->ProfileData();
    // Set loose routing to be always off
    return data->iServiceProfile.iSIPProfileClass;
    }
     
// -----------------------------------------------------------------------------
// CSIPSettListSIPPrxSetView::ProfileLocked
// Checks is currently shown profile locked.
// -----------------------------------------------------------------------------
//    
TBool CSIPSettListSIPPrxSetView::ProfileLocked()
    {
    __GSLOGSTRING("CSIPSettListSIPPrxSetView::ProfileLocked" )
    TBool locked( EFalse );
    
    // Find right profile from SIP profile array.
    const TSIPSettingsData* data = iHandler->ProfileData();
    CArrayPtr<CSIPManagedProfile>* list = iHandler->ProfileArray();
    for ( TInt i = 0; i < list->Count(); i++ )
        {
        TUint32 profileId ( KErrNone );
        list->At( i )->GetParameter( KSIPProfileId, profileId );
        if ( profileId == data->iID )
            {
            // Get SIP profile lock information.
            list->At( i )->GetParameter( KSIPProfileLocked, locked );
            }
        }
    
    return locked;
    }
       
//  End of File  
