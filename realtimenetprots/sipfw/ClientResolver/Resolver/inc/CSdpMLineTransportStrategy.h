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
* Name          : CSdpMLineTransportStrategy.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSDPMLINETRANSPORTSTRATEGY_H
#define CSDPMLINETRANSPORTSTRATEGY_H

#include "CSdpMLineStrategyBase.h"


class CSdpMLineTransportStrategy : public CSdpMLineStrategyBase
    {
public:

    static CSdpMLineTransportStrategy* NewLC(const MSipClients& aSipClients);
    ~CSdpMLineTransportStrategy ();

protected: // From CSdpMLineStrategyBase

    TBool FieldsMatchL(const CSdpMediaField& aReceivedMedia,
                       const CSdpMediaField& aClientMedia);

private:

    CSdpMLineTransportStrategy(const MSipClients& aSipClients);
    
private: // For testing purposes

	UNIT_TEST(CSdpMLineTransportStrategyTest)
    };

#endif // CSDPMLINETRANSPORTSTRATEGY_H

// End of File
