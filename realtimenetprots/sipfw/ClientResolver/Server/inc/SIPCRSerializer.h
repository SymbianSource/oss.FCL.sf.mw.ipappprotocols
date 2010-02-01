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
* Name          : SIPCRSerializer.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/


#ifndef SIPCRSERIALIZER_H
#define SIPCRSERIALIZER_H

//  INCLUDES
#include <e32base.h>
#include <stringpool.h>
#include "_sipcodecdefs.h"

class CSIPRequest;
class CSIPResponse;

// CLASS DECLARATION

/**
* Externalize descriptor data to a write stream and internalize data
* from a read stream.
* 
* @lib siprsvsrv.exe
*/
class SIPCRSerializer : public CBase
    {
    public:
	
	    static CBufFlat* ExternalizeL(CSIPRequest& aRequest);
	
	    static void InternalizeL(const TDesC8& aDes,
	                             CSIPRequest& aRequest);

	    static CBufFlat* ExternalizeL(CSIPResponse& aResponse);
	
    private:

        static void InternalizeMethodL(CSIPRequest& aRequest,
                                       RReadStream& aReadStream);

        static RStringF ReadFromStreamL(RReadStream& aReadStream);
    
    private: // For testing purposes

	    UNIT_TEST(SIPCRSerializerTest)                
    };

#endif // SIPCRSERIALIZER_H
            
// End of File
