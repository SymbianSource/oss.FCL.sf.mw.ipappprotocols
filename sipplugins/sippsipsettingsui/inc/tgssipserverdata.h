/*
* Copyright (c) 2003 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef T_GS_SIP_SERVER_DATA_H
#define T_GS_SIP_SERVER_DATA_H

// CONSTANTS

const TInt KMaxProxyLength = 256;
const TInt KMaxRealmLength = 256;
const TInt KMaxServerUsernameLength = 72;
const TInt KMaxServerPasswordLength = 24;
const TInt KMaxServerPrivateIdentityLength = 72;
const TInt KMaxServerPortLength = 40;

// CLASS DECLARATION

/**
*  TGSSIPServerData class
*  @since 3.0
*  Data structure that contains the server data
*/
class TGSSIPServerData
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
        // Private user identity
        TBuf<KMaxServerPrivateIdentityLength> iPrivateIdentity;
        // Has password changed
	    TBool iPasswordChanged;    
    };    

#endif      // T_GS_SIP_SERVER_DATA_H
            
// End of File
