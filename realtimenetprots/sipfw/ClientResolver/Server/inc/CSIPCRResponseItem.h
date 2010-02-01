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
* Name          : CSIPCRSessionReceiver.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/
#ifndef CSIPCRRESPONSEITEM_H
#define CSIPCRRESPONSEITEM_H

//  INCLUDES
#include <e32base.h>
#include "sipCRclientserver.h"


class CSIPCRResponseItem : public CBase
    {
public:

    CSIPCRResponseItem(TUint32 aRequestId,
                       TUid aChannel);
    
    CSIPCRResponseItem(TUint32 aRequestId,
                       CBufFlat* aResponse,
                       HBufC8* aContent);
    
    CSIPCRResponseItem(TUint32 aRequestId,
                       TInt aCompletionCode);    
    
    ~CSIPCRResponseItem();
	
    TUint32 RequestId() const;

    CBufFlat* SIPResponse();
    
    HBufC8* SIPResponseContent();

    TUid Channel() const;
    
    TInt Error() const;
    
    TSIPCRMessageBufSizes BufSizes() const;
        
private: // Data

    TInt iError;
    TUint32 iRequestId;
    TUid iChannel;
    CBufFlat* iResponse;
    HBufC8* iContent;          
    };

#endif // CSIPCRRESPONSEITEM_H

// End of File
