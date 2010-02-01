/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSIPCRITCUtility.h
* Part of       : SIP Client Resolver
* Version       : 1.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPCRITCUTILITY_H
#define CSIPCRITCUTILITY_H

//  INCLUDES
#include <e32base.h>
#include "sipCRclientserver.h"

// FORWARD DECLARATIONS
class SIPCRSerializer;
class CSIPRequest;


class CSIPCRITCUtility : public CBase
    {
public:
	static CSIPCRITCUtility* NewL ();
	
	static CSIPCRITCUtility* NewLC ();
	
	~CSIPCRITCUtility ();
	
	void ReadSIPRequestL(const RMessage2& aMessage,
	                     CSIPRequest& aRequest);

    TUid ReadResolverUidL(const RMessage2& aMessage) const;

    void WriteChannelUidL(const RMessage2& aMessage,
                          const TUid& aUid) const;

	void WriteSIPResponseL(const RMessage2& aMessage,
                           CBufFlat& aResponse) const;
                           
	void WriteSIPResponseContentL(const RMessage2& aMessage,
                                  const TDesC8& aContent) const;                           
                             
	TUid ReadUIDL (const RMessage2& aMessage) const;

	void WriteUIDL(const RMessage2& aMessage, TUid aUID) const;
	
	void WriteRequestIdL (const RMessage2& aMessage,TUint32 aId) const;
	
	void WriteClientResolvedL (const RMessage2& aMessage, 
	                           TSIPCRIpcResponses aResponse) const;
	
	void WriteResponseSizeL (const RMessage2& aMessage,
	                         TSIPCRMessageBufSizes aSizes) const;

	TUint32 ReadRequestIdL (const RMessage2& aMessage) const;
	
	void Complete (const RMessage2& aMessage,
                   TInt aCompletionCode) const;           

    static void WaitForMutexLC(RMutex& aMutex);	

    static void SignalMutex(TAny* aMutex);
                                       
private:

    void ConstructL ();
    
    void WriteL (TSIPCRIpcArguments aItcArgIndex,
                 const RMessage2&  aMessage,
                 const TDesC8&    aDes) const;

    HBufC8* ReadL (TSIPCRIpcArguments aItcArgIndex,
				   const RMessage2&  aMessage) const;
								
    HBufC8* ReadLC (TSIPCRIpcArguments aItcArgIndex,
					const RMessage2&  aMessage) const;
    
private: // Data
    
    SIPCRSerializer* iSerializer;
    };

#endif //CSIPCRITCUTILITY_H

//  End of File  
