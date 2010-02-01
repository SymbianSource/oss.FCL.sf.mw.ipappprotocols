/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  implementation
*
*/


//  INCLUDE FILES

#include <sip.h>



EXPORT_C CSIP* CSIP::NewL(const TUid& /*aUid*/,
                           MSIPObserver& /*aObserver*/)
    {
    return new ( ELeave ) CSIP;
    }


EXPORT_C CSIP* CSIP::NewLC(const TUid& /*aUid*/,
                            MSIPObserver& /*aObserver*/)
    {
    CSIP* sip = new ( ELeave ) CSIP;
    CleanupStack::PushL( sip );
    return sip;
    }


EXPORT_C CSIP::~CSIP()
    {
    }



EXPORT_C CDesC8Array* CSIP::SupportedSecurityMechanismsL() const
    {
    return NULL;
    }


EXPORT_C TBool CSIP::IsSigCompSupportedL() const
    {
    return EFalse;
    }


EXPORT_C HBufC8* CSIP::NegotiatedSecurityMechanismL(const TDesC8& /*aHop*/)
    {
    return NULL;
    }


EXPORT_C CSIPConnection* CSIP::Connection(TUint32 /*aIapId*/) const
    {
    return NULL;
    }


CSIPImplementation& CSIP::Implementation()
    {
    return *iImplementation;
    }


CSIP::CSIP()
    {
    }

void CSIP::ConstructL(const TUid& /*aUid*/, MSIPObserver& /*aObserver*/)
    {
    }