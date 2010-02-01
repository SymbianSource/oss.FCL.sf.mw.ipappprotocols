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
* Name          : CSdpRtpmapStrategy.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSDPRTPMAPSTRATEGY_H
#define CSDPRTPMAPSTRATEGY_H

#include <e32base.h>
#include <stringpool.h>
#include "CSdpMLineStrategyBase.h"
#include "sdpfmtattributefield.h"


class CSdpRtpmapStrategy : public CSdpMLineStrategyBase
    {
public:

    static CSdpRtpmapStrategy* NewLC(const MSipClients& aSipClients);
    ~CSdpRtpmapStrategy ();

protected: // From CSdpMLineStrategyBase

    TBool IsApplicable(CSdpMediaField& aMediaField);

    TBool FieldsMatchL(const CSdpMediaField& aReceivedMedia,
                       const CSdpMediaField& aClientMedia);

private:

    CSdpRtpmapStrategy(const MSipClients& aSipClients);
    void ConstructL();
    
    TBool HasMatchingRtpmapAttritbute(
        const CSdpMediaField& aMediaField,
        const CSdpFmtAttributeField& aAttribute);

    TBool HasMatchingFormatsL(
        const CSdpMediaField& aReceivedMedia,
        const CSdpMediaField& aClientMedia) const;

    RArray<TUint> ExtractFormatsL(const TDesC8& aFormats) const;
    
private: // Data
    
    RStringF iApplicationStringF;
    RStringF iRtpAvpStringF;
    RStringF iRtpmapStringF;
    
private: // For testing purposes

	UNIT_TEST(CSdpRtpmapStrategyTest)
    };

#endif // CSDPMLINENAMESTRATEGY_H

// End of File
