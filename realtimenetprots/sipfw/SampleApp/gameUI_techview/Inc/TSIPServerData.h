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
* Name        : TSIPServerData.h
* Part of     : SIP Profile Client / Settings User Interface
* Interface   : 
* Data structure for holding the profile data temporarily
* Version     : 1.0
*
*/



#ifndef TSIPSERVERDATA_H
#define TSIPSERVERDATA_H

// CONSTANTS

const TInt KMaxProxyLength = 256;
const TInt KMaxRealmLength = 256;
const TInt KMaxServerUsernameLength = 72;
const TInt KMaxServerPasswordLength = 20;
const TInt KMaxPortNumber = 15;

// CLASS DECLARATION

/**
*  Data structure that contains the server data
*  Part of SIP profile data structure
*
*  @lib SIPSettingsUI.app
*  @since 1.0
*/
class TSIPServerData
    {
    public:     // Data

        // Server's IP address
        TBuf<KMaxProxyLength> iAddress;
        // Realm
        TBuf<KMaxRealmLength> iRealm;
        // Username
        TBuf<KMaxServerUsernameLength> iUsername;
        // Password
        TBuf<KMaxServerPasswordLength> iPassword;
        // Use loose routing
        TInt iLooseRouting;
        // Port number
        TInt iPortNumber;
        // Transport protocol
        TInt iTransportProtocol;
    };    

#endif      // TProfileData
            
// End of File
