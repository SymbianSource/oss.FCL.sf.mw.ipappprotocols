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
* Name          : CSdpMediaAttributeStrategy.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSDPMEDIAATTRIBUTESTRATEGY_H
#define CSDPMEDIAATTRIBUTESTRATEGY_H

#include "CSdpStrategyBase.h"

class MSipClient;
class CSdpMediaField;
class CSdpAttributeField;


class CSdpMediaAttributeStrategy : public CSdpStrategyBase
    {
    public: // Constructors and destructor

        static CSdpMediaAttributeStrategy* NewLC(
            const MSipClients& aSipClients);
        
        ~CSdpMediaAttributeStrategy();

    protected: // From CSdpStrategyBase

        CSIPResponse* ApplyL(CSdpDocument& aDocument,
                             RArray<TUid>& aUids);

    private: // Constructors

        CSdpMediaAttributeStrategy(const MSipClients& aSipClients);
        void ConstructL();
    
    private: // New functions
    
        TBool Match(CSdpDocument& aDocument,
                    MSipClient& aClient);    
    
        TBool HasMatchingAttribute(CSdpMediaField& aReceivedMedia,
                                   CSdpMediaField& aClientMedia);
   
    private: // For testing purposes

	    UNIT_TEST(CSdpMediaAttributeStrategyTest)
    };

#endif // CSDPMEDIAATTRIBUTESTRATEGY_H

// End of File
