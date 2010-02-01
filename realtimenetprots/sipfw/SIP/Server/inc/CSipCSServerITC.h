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
* Name          : CSipCSServerITC.h
* Part of       : SIPServerCore
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPCSSERVERITC_H
#define CSIPCSSERVERITC_H

#include <e32base.h>
#include <badesca.h>
#include <in_sock.h> 
#include "sipclientserver.h"
#include "SipStackServerDefs.h"
#include "sipconnection.h"
#include "sipinternalstates.h"

class CSIPCSSerializer;
class CSIPMessage;
class CSIPRequest;
class CSIPResponse;
class TSIPRemoteTargetAndProxy;
class CSIPHeaderBase;
class CSIPRouteHeader;


class CSipCSServerITC : public CBase
	{
public:

    static CSipCSServerITC* NewL ();
    static CSipCSServerITC* NewLC ();

    ~CSipCSServerITC ();

    CSIPCSSerializer& Serializer ();
    
    void WriteL (const RMessage2& aMessage,
                 CBufFlat& aSipHeaders) const;
    
    void WriteL (const RMessage2& aMessage,
                 const TDesC8& aDes,
                 TSipItcArguments aItcArgIndex) const;

    void WriteL (const RMessage2& aMessage,
                 const TSIPIds& aIds) const;

    void WriteL (const RMessage2& aMessage,
                 const TSIPMessageBufSizes aSizes) const;
	
    void WriteL (const RMessage2& aMessage,
                 const CSIPHeaderBase& aHeader) const;

    void WriteL (const RMessage2& aMessage,
                 TInt aValue,
                 TSipItcArguments aItcArgIndex) const;

    void WriteL (const RMessage2& aMessage,
                 CSIPConnection::TState aState) const;

    void WriteL (const RMessage2& aMessage,
                 MDesC8Array& aSecurityMechanisms) const;
                 
    void WriteL (const RMessage2& aMessage,
                 const TInetAddr& aInetAddr) const;                 

    TUid ReadAppUidL (const RMessage2& aMessage) const;

	void ReadRequestL (const RMessage2& aMessage,
                       CSIPRequest& aRequest,
                       TBool aMethodExpected=EFalse) const;

	void ReadRequestL (const RMessage2& aMessage,
                       CSIPRequest& aRequest,
                       TSIPRemoteTargetAndProxy& aRemoteTargetAndProxy,
                       TBool aMethodExpected=EFalse,
                       TBool aProxyExpected=EFalse) const;
    
    CSIPResponse* ReadResponseLC (const RMessage2& aMessage) const;
    
	CSIPRouteHeader* ReadProxyLC (const RMessage2& aMessage) const;    
    
    TSIPIds ReadSipIdsL (const RMessage2& aMessage) const;

    TSIPSockOpt ReadSIPSockOptL (const RMessage2& aMessage) const;

    void ReadL (const RMessage2& aMessage,
                TInt& aValue,
                TSipItcArguments aItcArgIndex) const;
                   
    HBufC8* ReadLC (const RMessage2&  aMessage,
                    TSipItcArguments aItcArgIndex) const;
                    
    CDesC8Array* ReadCredentialsLC (const RMessage2& aMessage) const;                                   

    void Complete (const RMessage2& aMessage, TInt aCompletionCode) const;

    void PanicClient(const RMessage2& aMessage, TInt aPanic) const;

private:

    void ConstructL ();
    CSipCSServerITC ();

    const TAny* ITCArgPtr (TSipItcArguments aItcArgIndex,
                           const RMessage2&  aMessage) const;

    TInt ITCArgInt (TSipItcArguments aItcArgIndex,
                    const RMessage2&  aMessage) const;

    void WriteL (TSipItcArguments aItcArgIndex,
                 const RMessage2&  aMessage,
                 const TDesC8&    aDes) const;

    CSIPMessage* ReadSipMessageLC (const RMessage2& aMessage) const;

    HBufC8* ReadL (const RMessage2&  aMessage,
                   TSipItcArguments aItcArgIndex) const;

private: // Data

    CSIPCSSerializer* iSerializer;
	};

#endif // CSIPCSSERVERITC_H

// End of File
