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
* Description:  Data structure for holding the profile data temporarily
*
*/


#ifndef T_SIP_SETTINGS_DATA_H
#define T_SIP_SETTINGS_DATA_H

// INCLUDES

#include "gssipsettingsplugin.hrh"
#include "tgssipserverdata.h"
#include <sipprofiletypeinfo.h>

// CONSTANTS

const TInt KMaxSIPProviderNameLength = 100; 
const TInt KMaxUsernameLength = 256;
const TInt KMaxSIPAddressLength = 256;
const TInt KMaxServiceTypeName = 20;
const TInt KAORIndex = 0;

const TInt KDefaultIAP = 0;
const TInt KDefaultSNAP = 0;
const TInt KDefaultCompression = EGSComprNo;
const TInt KDefaultRegistration = EGSWhenNeeded;
const TInt KDefaultSecurity = EGSSecurityOff;

const TInt KDefaultLooseRouting = EGSLooseRoutingOn;
const TInt KDefaultPortNumber = -1;
const TInt KDefaultTranspProtocol = EGSTranspProtocolAuto;

const TInt KUnknownProfileIndex = -1;

// For SIP URI parsing.
_LIT( KSegmentSeparator, ";" );
_LIT( KSIPColonSeparator, ":" );
_LIT( KSIPProtocolHeader, "sip" );
_LIT( KSIPSProtocolHeader, "sips" );
_LIT( KLooseRoutingParam, "lr" );
_LIT( KTransportParamPrefix, "transport=" );
_LIT( KTCP, "TCP" );
_LIT( KUDP, "UDP");

const TInt KSIPProtocolHeaderLength = 3;
const TInt KSIPSProtocolHeaderLength = 4;
const TInt KMaxLengthForPort = 10;
const TInt KMaxSegmentLength = 64;
const TInt KMaxLengthForPath = 256;

// FORWARD DECLARATIONS

class TGSSIPServerData;

// CLASS DECLARATION

/**
*  TSIPSettingsData class
*  @since 3.1
*  Data structure to hold the profile data until placed to actual
*  profile instance. Class does not contain any methods, only data.
*/
class TSIPSettingsData    
    {
    public:     // Data
        
        // Provider name
        TBuf<KMaxSIPProviderNameLength> iProviderName;
        // Service profile
        TSIPProfileTypeInfo iServiceProfile;
        TInt iServiceProfileIndex;

        // AP index
        TInt iAPIndex;
        // SNAP index
        TInt iSNAPIndex;
        // Public username
        TBuf<KMaxUsernameLength> iUsername;
        // Use compression
        TInt iCompression;
        // Registration mode
        TInt iRegistrationMode;
        // Security negotiation
        TInt iSecurity;
        // Proxy server settings
        TGSSIPServerData iProxy;
        // Registrar server settings
        TGSSIPServerData iRegistrar;

        // Profile ID, -1 if a new profile and not yet saved to permanent
        // storage
        TInt iID;
    };  
   
#endif      // T_SIP_SETTINGS_DATA_H
            
// End of File
