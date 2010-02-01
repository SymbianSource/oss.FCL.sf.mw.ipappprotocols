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
* Name          : CSIPCRRequestItem.h
* Part of       : SIP Client Resolver
* Version       : 1.0 
*
*/




/**
 @internalComponent
*/
#ifndef CSIPCRREQUESTITEM_H
#define CSIPCRREQUESTITEM_H

//  INCLUDES
#include <e32base.h>
#include "TSipClient.h"

class CSIPCRRequestItem : public CBase
    {
public: // Constructors and destructor

	static CSIPCRRequestItem* NewL (TUint32 aRequestId,
	                                const TSipClient& aClient,
	                                const TUid& aChannelUid,
	                                const TUid& aResolverUid);
	
	static CSIPCRRequestItem* NewLC (TUint32 aRequestId,
	                                 const TSipClient& aClient,
	                                 const TUid& aChannelUid,
	                                 const TUid& aResolverUid);

    ~CSIPCRRequestItem ();
	
public: // New functions	
	
	TUint32 RequestId() const;
	const TSipClient& Client() const;
	const TUid& ChannelUid() const;
	const TUid& ResolverUid() const;
	void SetConnectCalled();
	TBool ConnectCalled() const;
	
private: // Second phase constructors

    CSIPCRRequestItem (TUint32 aRequestId,
                       const TSipClient& aClient,
                       const TUid& aChannelUid,
                       const TUid& aResolverUid);	
    
private: // Data    
    
    TUint32 iRequestId;
    TSipClient iClient;
    TUid iChannelUid;
    TUid iResolverUid;
    TBool iConnectCalled;
    };

#endif // CSIPCRREQUESTITEM_H
            
// End of File
