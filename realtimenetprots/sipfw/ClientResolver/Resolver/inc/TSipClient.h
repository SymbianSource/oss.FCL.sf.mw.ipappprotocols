/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : TSipClient.h
* Part of       : SIP / SIP Client Resolver / Resolver
* Version       : SIP 6.0
*
*/




/**
 @internalComponent
*/
#ifndef TSIPCLIENT_H
#define TSIPCLIENT_H

// INCLUDES
#include <e32std.h>

// CLASS DEFINITION
/**
* A container for SIP client information passed 
* from SIP Client Resolver to SIP Client Resolver Server.
*/
class TSipClient
    {
    public:

        /** SIP Client Resolver plug-in types */
        enum TPluginType 
            {
            /** Plug-in implements CSIPResolvedClient */
			EFirstGeneration=1,
			/** Plug-in implements CSIPResolvedClient2 */
			ESecondGeneration
            };    

    public: // Constructors

        inline TSipClient(
            const TUid& aUid,
            TBool aAllowStarting,
            TBool aRomBased);
        
    public: //  new functions    
        
        inline const TUid& Uid() const;
        inline void SetUid( TUid aUid );
        inline TBool AllowStarting() const;
        inline void SetAllowStarting( TBool aAllowStarting );
        inline TBool RomBased() const;
        inline void SetRomBased( TBool aRomBased );
		inline TUid ImplementationUid() const;
		inline void SetImplementationUid( TUid aUid );
        inline void SetPluginType( TPluginType aPluginType );
        inline TPluginType PluginType() const;

    private: // Data

        TUid iUid;
        TBool iAllowStarting;
        TBool iRomBased;
		TUid iImplementationUid;
        TPluginType iPluginType;
    };

#include "TSipClient.inl"

#endif // TSIPCLIENT_H

// End of File
