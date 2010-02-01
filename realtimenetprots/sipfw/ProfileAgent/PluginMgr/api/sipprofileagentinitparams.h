/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : sipprofileagentinitparams.h
* Part of     : SIP / SIP Profile Agent
* Version     : %version: 2.1.1 %
*
*/



#ifndef TSIPPROFILEAGENTINITPARAMS_H
#define TSIPPROFILEAGENTINITPARAMS_H

//  INCLUDES
#include <e32base.h>

//FORWARD DECLARATIONS
class MSIPProfileAgentObserver;

// CLASS DECLARATION
/**
* @publishedPartner
* @released
*
* Contains construction parameters needed for SIP Profile Agent plug-ins.
* @lib N/A
*/
class TSIPProfileAgentInitParams
    {
    public:  // Constructors and destructor

        /**
        * Constructor
        * @param aDeltaTimer a handle to CDeltaTimer 
        * @param aSIPProfileObserver a sip profile observer
        */
        TSIPProfileAgentInitParams(
            CDeltaTimer& aDeltaTimer,
            MSIPProfileAgentObserver& aSIPProfileAgentObserver);

    public: // Data

        /**
        * A handle to CDeltaTimer common to the whole SIP Profile Agent
        */
        CDeltaTimer& iDeltaTimer;

        /**
        * SIP profile observer
        */
        MSIPProfileAgentObserver& iSIPProfileAgentObserver;
    };

#include "sipprofileagentinitparams.inl"

#endif // TSIPPROFILEAGENTINITPARAMS_H
