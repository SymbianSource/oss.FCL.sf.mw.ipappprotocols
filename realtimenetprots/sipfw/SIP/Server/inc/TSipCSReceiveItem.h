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
* Name          : TSipCSReceiveItem.h
* Part of       : SIPServerCore
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef TSIPCSRECEIVEITEM_H
#define TSIPCSRECEIVEITEM_H

#include <e32base.h>
#include "sipclientserver.h"


class TSipCSReceiveItem
    {
public:

    TSipCSReceiveItem (const TSIPIds& aIds,
                       CBufFlat* aSipMessageHeaders,
                       HBufC8* aSipMessageContent,
                       TBool aContainsSIPRequest,
                       TInt aError=KErrNone);

    TSipCSReceiveItem (const TSIPIds& aIds, TInt aError);

    CBufFlat* Headers ();
    HBufC8* Content ();
    TSIPMessageBufSizes BufSizes() const;
    const TSIPIds& Ids () const;
    TBool ContainsSIPRequest() const;
	TInt Error() const;

private:

    TSipCSReceiveItem ();

private: // Data
    
    TSIPIds iIds;
    CBufFlat* iHeaders;
    HBufC8* iContent;
    TBool iContainsSIPRequest;
    TInt iError;
    };

#endif // TSIPCSRECEIVEITEM_H

// End of File
