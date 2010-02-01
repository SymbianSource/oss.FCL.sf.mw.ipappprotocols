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
* Name          : CSdpMLineNameStrategy.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSDPMLINENAMESTRATEGY_H
#define CSDPMLINENAMESTRATEGY_H

#include <e32base.h>
#include <stringpool.h>
#include "CSdpMLineStrategyBase.h"


class CSdpMLineNameStrategy : public CSdpMLineStrategyBase
    {
public:

    static CSdpMLineNameStrategy* NewLC(const MSipClients& aSipClients);
    ~CSdpMLineNameStrategy ();

protected: // From CSdpMLineStrategyBase

    TBool IsApplicable(CSdpMediaField& aMediaField);

    TBool FieldsMatchL(const CSdpMediaField& aReceivedMedia,
                       const CSdpMediaField& aClientMedia);

private:

    CSdpMLineNameStrategy(const MSipClients& aSipClients);
    void ConstructL();
    
    RStringF iApplicationStringF;
    
private: // For testing purposes

	UNIT_TEST(CSdpMLineNameStrategyTest)
    };

#endif // CSDPMLINENAMESTRATEGY_H

// End of File
