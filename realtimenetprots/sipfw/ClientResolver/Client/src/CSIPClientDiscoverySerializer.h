/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSIPClientDiscoverySerializer.h
* Part of       : SIP Client Resolver
* Version       : 1.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPCLIENTDISCOVERYSERIALIZER_H
#define CSIPCLIENTDISCOVERYSERIALIZER_H

//  INCLUDES
#include <e32base.h>
#include <s32mem.h>
#include <stringpool.h>
#include "_sipcodecdefs.h"

// FORWARD DECLARATIONS
class CSIPRequest;
class CSIPResponse;


class CSIPClientDiscoverySerializer : public CBase
    {
public:
    static CSIPClientDiscoverySerializer* NewL();
    static CSIPClientDiscoverySerializer* NewLC();
    
    ~CSIPClientDiscoverySerializer ();

    CBufFlat* ExternalizeSIPRequestL(CSIPRequest& aRequest);
    CSIPResponse* InternalizeSIPResponseL(const TDesC8& aResponse);

private:

    void ConstructL();
    CSIPClientDiscoverySerializer();
    void ExternalizeRequestL(CSIPRequest& aRequest, RWriteStream& aWriteStream);

private: // For testing purposes

	UNIT_TEST(CSIPClientDiscoverySerializerTest)
    };

#endif // CSIPCLIENTDISCOVERYSERIALIZER_H

//  End of File  
