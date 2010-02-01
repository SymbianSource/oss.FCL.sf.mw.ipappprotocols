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
* Name          : TSIPTransportParams.inl
* Part of       : SIP Common
* Version       : 1.0
*
*/




#ifndef TSIPTRANSPORTPARAMS_INL
#define TSIPTRANSPORTPARAMS_INL

#include <e32std.h>

// Silence PC-Lint: copy constructor and assigment operator copying a pointer
/*lint -e1554 */
/*lint -e1555 */

// ---------------------------------------------------------------------------
// TSIPTransportParams::TSIPTransportParams 
// ---------------------------------------------------------------------------
//
inline TSIPTransportParams::TSIPTransportParams(const TUint32& aOwnerId,
                                                const TUint32& aIapId)
: iOwnerId(aOwnerId),
  iIapId(aIapId),
  iTransportId(0),
  iCompartmentId(0),
  iIgnoreCompartmentId(EFalse),
  iNATBindingObserver(NULL),
  iSkipNaptrAndSrv(EFalse)
    {
    }

// ---------------------------------------------------------------------------
// TSIPTransportParams::TSIPTransportParams 
// ---------------------------------------------------------------------------
//
inline TSIPTransportParams::TSIPTransportParams(
    const TSIPTransportParams& aParams)
: iOwnerId(aParams.iOwnerId),
  iIapId(aParams.iIapId),
  iTransportId(aParams.iTransportId),
  iCompartmentId(aParams.iCompartmentId),
  iIgnoreCompartmentId(aParams.iIgnoreCompartmentId),
  iNATBindingObserver(aParams.iNATBindingObserver),
  iSkipNaptrAndSrv(aParams.iSkipNaptrAndSrv)
    {
    }

// ---------------------------------------------------------------------------
// TSIPTransportParams::SetOwnerId 
// ---------------------------------------------------------------------------
//
inline void TSIPTransportParams::SetOwnerId(TUint32 aOwnerId)
    {
    iOwnerId = aOwnerId;
    }
    
// ---------------------------------------------------------------------------
// TSIPTransportParams::OwnerId 
// ---------------------------------------------------------------------------
//
inline TUint32 TSIPTransportParams::OwnerId() const
    {
    return iOwnerId;
    }

// ---------------------------------------------------------------------------
// TSIPTransportParams::SetIapId 
// ---------------------------------------------------------------------------
//
inline void TSIPTransportParams::SetIapId(const TUint32& aIapId)
    {
    iIapId = aIapId;
    }

// ---------------------------------------------------------------------------
// TSIPTransportParams::IapId 
// ---------------------------------------------------------------------------
//
inline TUint32 TSIPTransportParams::IapId() const
    {
    return iIapId;
    }

// ---------------------------------------------------------------------------
// TSIPTransportParams::SetTransportId 
// ---------------------------------------------------------------------------
//
inline void TSIPTransportParams::SetTransportId(const TUint32& aTransportId)
    {
    iTransportId = aTransportId;
    }

// ---------------------------------------------------------------------------
// TSIPTransportParams::TransportId 
// ---------------------------------------------------------------------------
//
inline TUint32 TSIPTransportParams::TransportId() const
    {
    return iTransportId;
    }
    
// ---------------------------------------------------------------------------
// TSIPTransportParams::SetCompartmentId 
// ---------------------------------------------------------------------------
//
inline void TSIPTransportParams::SetCompartmentId(
    const TUint32& aCompartmentId)
    {
    iCompartmentId = aCompartmentId;
    }

// ---------------------------------------------------------------------------
// TSIPTransportParams::CompartmentId 
// ---------------------------------------------------------------------------
//
inline TUint32 TSIPTransportParams::CompartmentId() const
    {
    return iCompartmentId;
    }

// ---------------------------------------------------------------------------
// TSIPTransportParams::SetIgnoreCompartmentId 
// ---------------------------------------------------------------------------
//    
inline void TSIPTransportParams::SetIgnoreCompartmentId(
    TBool aIgnoreCompartmentId)
    {
    iIgnoreCompartmentId = aIgnoreCompartmentId;
    }

// ---------------------------------------------------------------------------
// TSIPTransportParams::IgnoreCompartmentId 
// ---------------------------------------------------------------------------
//      
inline TBool TSIPTransportParams::IgnoreCompartmentId() const
    {
    return iIgnoreCompartmentId;
    }

// ---------------------------------------------------------------------------
// TSIPTransportParams::SetNATBindingObserver
// ---------------------------------------------------------------------------
//  
inline void TSIPTransportParams::SetNATBindingObserver(
    MSIPNATBindingObserver* aObserver)
    {
    iNATBindingObserver = aObserver;
    }

// ---------------------------------------------------------------------------
// TSIPTransportParams::SetNATBindingObserver
// ---------------------------------------------------------------------------
//
inline MSIPNATBindingObserver* TSIPTransportParams::NATBindingObserver()
    {
    return iNATBindingObserver;
    }

// ---------------------------------------------------------------------------
// TSIPTransportParams::SetSkipNaptrAndSrv 
// ---------------------------------------------------------------------------
//    
inline void TSIPTransportParams::SetSkipNaptrAndSrv(
    TBool aSkipNaptrAndSrv)
    {
    iSkipNaptrAndSrv = aSkipNaptrAndSrv;
    }

// ---------------------------------------------------------------------------
// TSIPTransportParams::SkipNaptrAndSrv
// ---------------------------------------------------------------------------
//      
inline TBool TSIPTransportParams::SkipNaptrAndSrv() const
    {
    return iSkipNaptrAndSrv;
    }
   
// ---------------------------------------------------------------------------
// TSIPTransportParams::operator=
// ---------------------------------------------------------------------------
//    
inline TSIPTransportParams& TSIPTransportParams::operator=(
    const TSIPTransportParams& aParams)
    {
    if (&aParams != this)
        {
        // Copy all the members:
        iOwnerId = aParams.iOwnerId;
        iIapId = aParams.iIapId;
        iTransportId = aParams.iTransportId;
        iCompartmentId = aParams.iCompartmentId;
        iIgnoreCompartmentId = aParams.iIgnoreCompartmentId;
        iNATBindingObserver = aParams.iNATBindingObserver;
        iSkipNaptrAndSrv = aParams.iSkipNaptrAndSrv;
        }
    return *this;
    }

#endif // TSIPTRANSPORTPARAMS_INL

// End of File
