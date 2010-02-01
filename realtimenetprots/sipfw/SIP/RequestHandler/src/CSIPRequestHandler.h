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
* Name          : CSIPRequestHandler.h
* Part of       : SIP Request Handler
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPREQUESTHANDLER_H
#define CSIPREQUESTHANDLER_H

#include "SipRequestHandlerBase.h"
#include "sipclientdiscoveryobserver.h"
#include <stringpool.h>
class CSIPClientDiscovery;


class CSIPRequestHandler : public CSIPRequestHandlerBase,
                           public MSIPClientDiscoveryObserver
    {
	public: // Constructors and destructor
	    
    	static CSIPRequestHandler* NewL(TAny* aInitParams);
    	
    	~CSIPRequestHandler ();
    	
    	
    public: // From CSIPRequestHandlerBase

		void RegisterL(TUid aChannel);

		TInt Deregister(TUid aChannel);

		TUint32 ChannelL(CSIPRequest& aSIPRequest);
		
		void Cancel(TUint32 aRequestId);
		
		
	public: // From MSIPClientDiscoveryObserver
	
		void ChannelResolvedL(TUid aChannel, TUint32 aRequestId);

		void ClientNotFoundL(TUint32 aRequestId,
						     TUint aStatusCode, 
						     RStringF aReasonPhrase, 
						     RPointerArray<CSIPHeaderBase> aHeaders,
						     HBufC8* aContent);

		void ErrorOccurred(TInt aError,
						   TUint32 aRequestId);

	private:
	
		void ConstructL(TUid aUid);
		CSIPRequestHandler(MSIPRequestHandlerObserver& aObserver);
		
	private: // Data
	
		CSIPClientDiscovery* iSIPClientDiscovery;
		RStringF iContentTypeHeaderName;
    };

#endif // CSIPREQUESTHANDLER_H
