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
* Description:  The registrar view controller
*
*/


// INCLUDE FILES

#include    "sipsettlistsipregsetview.h"
#include    "viewid.h"
#include    "mgssipprofilehandler.h"
#include    "sipsettlistsipsrvsetmodel.h"
#include    <aknViewAppUi.h>
#include    <gssipsettingspluginrsc.rsg> //GUI Resource
#include    "gssippluginlogger.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPSettListSIPRegSetView::CSIPSettListSIPRegSetView
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIPSettListSIPRegSetView::CSIPSettListSIPRegSetView(
    MGSSIPProfileHandler* aHandler )
    : CSIPSettListSIPSrvSetView( R_QTN_REGISTRAR_TITLE_PANE_LABEL ),
    iHandler( aHandler )
    {
    __GSLOGSTRING("CSIPSettListSIPRegSetView::CSIPSettListSIPRegSetView" )
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPRegSetView::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPRegSetView::ConstructL()
    {
    __GSLOGSTRING("CSIPSettListSIPRegSetView::ConstructL" )
    BaseConstructL( R_GS_SIP_SERVER_SETTING_VIEW );    
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPRegSetView::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPSettListSIPRegSetView* CSIPSettListSIPRegSetView::NewLC(
    MGSSIPProfileHandler* aHandler )
    {
    __GSLOGSTRING("CSIPSettListSIPRegSetView::NewLC" )
    CSIPSettListSIPRegSetView* self = 
        new ( ELeave ) CSIPSettListSIPRegSetView( aHandler );
    
    CleanupStack::PushL( self );
    self->ConstructL();    

    return self;
    }
    
// Destructor
CSIPSettListSIPRegSetView::~CSIPSettListSIPRegSetView()
    {
    __GSLOGSTRING("CSIPSettListSIPRegSetView::~CSIPSettListSIPRegSetView" )
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPRegSetView::Id
// Returns the UID of the view
// -----------------------------------------------------------------------------
//
TUid CSIPSettListSIPRegSetView::Id() const
    {    
    return KSettListSIPRegistrarServerView;
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPRegSetView::DataStruct
// Returns the used data structure
// -----------------------------------------------------------------------------
//
TGSSIPServerData* CSIPSettListSIPRegSetView::DataStruct()
    {  
    __GSLOGSTRING("CSIPSettListSIPRegSetView::DataStruct" )
    TGSSIPServerData* data = &( iHandler->ProfileData()->iRegistrar );
    // Set loose routing to be always off
    data->iLooseRouting = EGSLooseRoutingOff;

    return data;
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPRegSetView::DataStruct
// Returns the used data structure
// -----------------------------------------------------------------------------
//
TInt CSIPSettListSIPRegSetView::ResourceID()
    {
    return R_SIP_REGISTRAR_SETTING_VIEW_ITEM_LIST;
    }

// -----------------------------------------------------------------------------
// CGSSettListSIPRegSetView::ProfileType
// Returns the used profile type
// -----------------------------------------------------------------------------
//
TSIPProfileTypeInfo::TSIPProfileClass CSIPSettListSIPRegSetView::ProfileType()
    { 
    __GSLOGSTRING("CSIPSettListSIPRegSetView::ProfileType" )
    const TSIPSettingsData* data = iHandler->ProfileData();
    // Set loose routing to be always off
    return data->iServiceProfile.iSIPProfileClass;
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPRegSetView::ProfileLocked
// Checks is currently shown profile locked.
// -----------------------------------------------------------------------------
//    
TBool CSIPSettListSIPRegSetView::ProfileLocked()
    {
    __GSLOGSTRING("CSIPSettListSIPRegSetView::ProfileLocked" )
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
