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
* Description:  The model of the server setting views
*
*/



// INCLUDE FILES
#include    "sipsettlistsipsrvsetmodel.h"
#include    <siprouteheader.h>
#include    <gssipsettingspluginrsc.rsg> //GUI Resource
#include    <eikenv.h>
#include    <eikappui.h>
#include    "gssippluginlogger.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvSetModel::CSIPSettListSIPSrvSetModel
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIPSettListSIPSrvSetModel::CSIPSettListSIPSrvSetModel( 
    TGSSIPServerData& aData, 
    TSIPProfileTypeInfo::TSIPProfileClass aProfileType,
    TBool aLocked )
    : iData( aData ),
      iProfileType( aProfileType ),
      iLocked( aLocked )
    {
    __GSLOGSTRING("CSIPSettListSIPSrvSetModel::CSIPSettListSIPSrvSetModel" )
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvSetModel::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPSrvSetModel::ConstructL()
    {
    __GSLOGSTRING("CSIPSettListSIPSrvSetModel::ConstructL" )
    }

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvSetModel::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPSettListSIPSrvSetModel* CSIPSettListSIPSrvSetModel::NewL( 
    TGSSIPServerData& aData, 
    TSIPProfileTypeInfo::TSIPProfileClass aProfileType,
    TBool aLocked )
    {
    __GSLOGSTRING("CSIPSettListSIPSrvSetModel::NewL" )
    CSIPSettListSIPSrvSetModel* self = 
        new ( ELeave ) CSIPSettListSIPSrvSetModel( 
            aData, aProfileType, aLocked );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

    
// Destructor
CSIPSettListSIPSrvSetModel::~CSIPSettListSIPSrvSetModel()
    {
    __GSLOGSTRING("CSIPSettListSIPSrvSetModel::~CSIPSettListSIPSrvSetModel" )
    }
   
// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvSetModel::Settings
// Fetches the settings of the modified server
// -----------------------------------------------------------------------------
//
TGSSIPServerData* CSIPSettListSIPSrvSetModel::Settings()
    {
    return &iData;
    }

// -----------------------------------------------------------------------------
// CGSSettListSIPSrvSetModel::ProfileType
// Returns selected service profile type
// -----------------------------------------------------------------------------
//
TSIPProfileTypeInfo::TSIPProfileClass CSIPSettListSIPSrvSetModel::ProfileType()
    {
    return iProfileType;
    }

// -----------------------------------------------------------------------------
// CGSSettListSIPSrvSetModel::ProfileLocked
// Checks is currently shown profile locked
// -----------------------------------------------------------------------------
//
TBool CSIPSettListSIPSrvSetModel::ProfileLocked()
    {
    return iLocked;
    }
        
//  End of File  
