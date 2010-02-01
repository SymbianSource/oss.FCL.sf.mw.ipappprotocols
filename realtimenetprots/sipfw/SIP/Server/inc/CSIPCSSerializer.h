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
* Name          : CSIPCSSerializer.h
* Part of       : SIPServerCore
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPCSSERIALIZER_H
#define CSIPCSSERIALIZER_H

#include <e32base.h>
#include <s32mem.h>
#include <badesca.h>
#include <stringpool.h>
#include "_sipcodecdefs.h"

class CSIPMessage;
class CSIPRequest;
class CSIPResponse;
class CSIPRouteHeader;
class TSIPRemoteTargetAndProxy;
class CSIPHeaderBase;


class CSIPCSSerializer : public CBase
	{
public:

    static CSIPCSSerializer* NewL ();
    static CSIPCSSerializer* NewLC ();

    ~CSIPCSSerializer ();
    
    CDesC8Array* InternalizeCredentialsL (const TDesC8& aDes) const;
    CBufFlat* ExternalizeLC (const MDesC8Array& aSecurityMechanisms) const;
	CBufFlat* ExternalizeValueLC (const CSIPHeaderBase& aHeader) const;
	CBufFlat* ExternalizeLC (CSIPMessage& aMessage) const;

    void InternalizeL (const TDesC8& aDes,
                       CSIPRequest& aRequest,
                       TBool aMethodExpected=EFalse) const;

    void InternalizeL (const TDesC8& aDes,
                       CSIPRequest& aRequest, 
                       TSIPRemoteTargetAndProxy& aRemoteTargetAndProxy,
                       TBool aMethodExpected=EFalse,
                       TBool aProxyExpected=EFalse) const;

    CSIPResponse* InternalizeL (const TDesC8& aDes) const;

    RPointerArray<CSIPHeaderBase> 
        InternalizeHeadersL (const TDesC8& aDes) const;
        
    CSIPRouteHeader* InternalizeProxyL (const TDesC8& aDes) const;

private:

    void ConstructL ();
    CSIPCSSerializer ();

	void ExternalizeL (CSIPRequest& aRequest,
                       RWriteStream& aWriteStream) const;

	void ExternalizeL (CSIPResponse& aResponse,
                       RWriteStream& aWriteStream) const;

    void InternalizeMethodL (CSIPRequest& aRequest,
                             RReadStream& aReadStream) const;

	RStringF ReadFromStreamL (RReadStream& aReadStream) const;

	void ExternalizeL (RStringF aStr, RWriteStream& aWriteStream) const;

private: // For testing purposes

	UNIT_TEST(CSIPCSSerializerTest)
	};

#endif // CSIPCSSERIALIZER_H

// End of File
