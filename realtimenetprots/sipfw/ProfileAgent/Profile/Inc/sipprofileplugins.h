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
* Name        : sipprofileplugins.h
* Part of     : SIP / SIP Profile Agent / SIP Concrete Profile
* Implementation
* Version     : %version: 2.1.1 %
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPROFILEPLUGINS_H
#define CSIPPROFILEPLUGINS_H

//  INCLUDES
#include <e32base.h>
#include <s32strm.h>
#include "sipprofiletypeinfo.h"

// CLASS DECLARATION
/**
* Class provides container for profile plug-ins type information.
* It provides serialization and deserialization functions
*/
class CSIPProfilePlugins: public CBase
    {
    public: // Constructors and destructor

        /**
        * Constructs an object
        * @returns new instance
        */
        IMPORT_C static CSIPProfilePlugins* NewL();

        /**
        * Constructs an object
         * @returns new instance
         */
        IMPORT_C static CSIPProfilePlugins* NewLC();

        /**
         * Destructor.
         */
        IMPORT_C ~CSIPProfilePlugins();

    public: // New functions 

        /**
         * Get array of profile plugin types
         * @return array of types
         */
        IMPORT_C const RArray<TSIPProfileTypeInfo>& Plugins() const;

        /**
         * Set array of profile plugin types
         * @param aPlugins array of types
         */
        IMPORT_C void SetPluginsL(
            const RArray<TSIPProfileTypeInfo>& aPlugins);

        /**
         * Deserialize profile plugins from stream, ownership is transferred
         * @param aReadStream stream holding serialized profile plugins
         * @return new instance
         */
        IMPORT_C static CSIPProfilePlugins* InternalizeL(
            RReadStream& aReadStream);

        /**
         * Serialize profile plugins to stream
         * @param aWriteStream stream where profile plugins is to be serialized
         */
        IMPORT_C void ExternalizeL(
            RWriteStream& aWriteStream) const;

        /**
         * Get serialized profile plugins size
         * @return size needed to serialize profile plugins
         */
        IMPORT_C TUint ExternalizedSize() const;

    private: // Constructors

        CSIPProfilePlugins();
        void ConstructL();

    private: // New functions

        void DoInternalizeL(RReadStream& aReadStream);

    private: // Data

        RArray<TSIPProfileTypeInfo> iPlugins;

    };

#endif // CSIPPROFILEPLUGINS_H
