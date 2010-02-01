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
* Name          : CSIPITCSender.h
* Part of       : SIPClient
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPITCSENDER_H
#define CSIPITCSENDER_H

#include <e32base.h>
#include <s32mem.h>
#include <stringpool.h>
#include <uri8.h>
#include "sipclientserver.h"

class MSIPITC;
class CSIPMessageElements;
class CSIPRequestElements;
class CSIPResponseElements;
class CSIPHeaderBase;
class CSIPToHeader;
class CSIPFromHeader;
class CSIPRouteHeader;
class CSIPCallIDHeader;


class CSIPITCSender : public CBase
	{
public:

    static CSIPITCSender* NewL (MSIPITC& aITC);
    static CSIPITCSender* NewLC (MSIPITC& aITC);
    ~CSIPITCSender ();

    TInt Send (TSIPIds& aIds, TSipItcFunctions aITCFunction);

	void SendL (TSIPIds& aIds, TSipItcFunctions aITCFunction);

	void SendL (TSIPIds& aIds,
				TSipItcFunctions aITCFunction,
				const CSIPRequestElements& aElements);

	void SendL (TSIPIds& aIds,
				TSipItcFunctions aITCFunction,
                RStringF aMethod,
                const CUri8* aRemoteUri,                
                const CSIPFromHeader* aFrom,
				const CSIPToHeader* aTo,                
                const CSIPMessageElements* aElements=0,
                const CSIPRouteHeader* aOutboundProxy=0);
 
	void SendL (TSIPIds& aIds,
				TSipItcFunctions aITCFunction,
				const CSIPMessageElements* aElements);

	void SendL (TSIPIds& aIds,
				TSipItcFunctions aITCFunction,
                RStringF aMethod,
				const CSIPMessageElements* aElements);

	void SendL (TSIPIds& aIds,
				TSipItcFunctions aITCFunction,
				const CSIPResponseElements& aElements);
				
	void SetOutboundProxyL (TSIPIds& aIds,
	                        const CSIPRouteHeader& aOutboundProxy);
	                        
    CSIPHeaderBase* ReadSIPHeaderL(TSIPIds& aIds,
                                   TSipItcFunctions aITCFunction,
                                   RStringF aHeaderName);
                                   
    CSIPCallIDHeader* ReadCallIDHeaderL(TSIPIds& aIds);
    
    RStringF ReadLocalTagL(TSIPIds& aIds);                                  
    
    void SetRefreshIntervalL(TSIPIds& aIds, TInt aInterval);

    void GetRefreshIntervalL(TSIPIds& aIds, TInt& aInterval);

private:

	CSIPITCSender (MSIPITC& aITC);
    void ConstructL ();

    void SetContentToSend (const CSIPMessageElements* aElements);
    void SetContentToSend (const CSIPMessageElements& aElements);
	template<class T> CBufFlat* ExternalizeLC (const T& aElements);
	template<class T> void ExternalizeL (const T* aElement, 
                                         RWriteStream& aWriteStream,
                                         TBool aAddExpectedFlag=ETrue);
    void ExternalizeL (RStringF aStr, RWriteStream& aWriteStream);
    CSIPHeaderBase* InternalizeL (RStringF aName, TDes8& aDes);

	// data
    TIpcArgs iITCMsgArgs;
    TBuf8<1> iEmptyContent;
    MSIPITC& iITC;
    TBool iLookupOpened;
	};

#endif // CSIPITCSENDER_H

// End of File
