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
* Name          : CSdpMLineMediaStrategy.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSDPMLINEMEDIASTRATEGY_H
#define CSDPMLINEMEDIASTRATEGY_H

#include <e32base.h>
#include "CSdpMLineStrategyBase.h"


class CSdpMLineMediaStrategy : public CSdpMLineStrategyBase
    {
public:

    static CSdpMLineMediaStrategy* NewLC(const MSipClients& aSipClients);
    ~CSdpMLineMediaStrategy ();

protected: // From CSdpMLineStrategyBase

    TBool FieldsMatchL(const CSdpMediaField& aReceivedMedia,
                       const CSdpMediaField& aClientMedia);

private:

    CSdpMLineMediaStrategy(const MSipClients& aSipClients);
    
private: // For testing purposes

	UNIT_TEST(CSdpMLineMediaStrategyTest)
    };

#endif // CSDPMLINEMEDIASTRATEGY_H

// End of File
