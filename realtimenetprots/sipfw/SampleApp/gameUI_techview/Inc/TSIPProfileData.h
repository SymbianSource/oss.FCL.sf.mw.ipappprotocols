/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : TSIPProfileData.h
* Part of     : SIP Profile Client / Settings User Interface
* Interface   : 
* Data structure for holding the profile data temporarily
* Version     : 1.0
*
*/



#ifndef TSIPPROFILEDATA_H
#define TSIPPROFILEDATA_H

// INCLUDES

#include "SIPEx.hrh"
#include "TSIPServerData.h"

#include "sipprofiletypeinfo.h"

// CONSTANTS

const TInt KMaxProviderNameLength = 100;
const TInt KMaxUsernameLength = 256;
const TInt KMaxServiceTypeName = 20;
const TInt KAORIndex = 0;

// Default values for the first profile
// Default profile name is read from .rss - r_qtn_def_profile_name
// Default username is read from .rss - r_qtn_def_username
// Default server realm is read from .rss - r_qtn_def_username
// Default server username is read from .rss - r_qtn_def_username
// TO_DO: Def service profile not yet defined
const TInt KDefaultIAP = -1;
const TInt KDefaultCompression = EComprNo;
const TInt KDefaultRegistration = EAlwaysOn;
const TInt KDefaultSecurity = ESecurityOn;

_LIT( KEmptyString, "" );
_LIT8( KEmptyString8, "" );
const TInt KDefaultLooseRouting = ELooseRoutingOn;
const TInt KDefaultPortNumber = 5060;
const TInt KDefaultTranspProtocol = ETranspProtocolUDP;

const TInt KUnknownProfileIndex = -1;
_LIT8( KLabelPassword, "passwd" );

// SIP Protocol header
_LIT( KSIPProtocolHeader, "sip:" );

// FORWARD DECLARATIONS

class TSIPServerData;

// CLASS DECLARATION

/**
*  Data structure to hold the profile data until placed to actual
*  profile instance. Class does not contain any methods, only data.
*
*  @lib SIPSettingsUI.app
*  @since 1.0
*/
class TSIPProfileData    
    {
    public:     // Data
        
        // Provider name
        TBuf<KMaxProviderNameLength> iProviderName;
        // Service profile
        TSIPProfileTypeInfo iServiceProfile;
        TInt iServiceProfileIndex;

        // AP index
        TInt iAPIndex;
        // Public username
        TBuf<KMaxUsernameLength> iUsername;
        // Use compression
        TInt iCompression;
        // Registration mode
        TInt iRegistrationMode;
        // Security negotiation
        TInt iSecurity;
        // Proxy server settings
        TSIPServerData iProxy;
        // Registrar server settings
        TSIPServerData iRegistrar;

        // Profile ID, -1 if a new profile and not yet saved to permanent
        // storage
        TInt iID;
    };  
   
#endif      // TSIPProfileData
            
// End of File
