/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : TSIPTransportParams.h
* Part of       : SIP Common
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/


#ifndef TSIPTRANSPORTPARAMS_H
#define TSIPTRANSPORTPARAMS_H

#include <e32std.h>

class MSIPNATBindingObserver;

/**
* A container for transport parameters used in SIP ConnectioMgr.
*/
class TSIPTransportParams
    {
    public: // Contructors

        inline TSIPTransportParams(const TUint32& aOwnerId,
                                   const TUint32& aIapId=0);
        inline TSIPTransportParams(const TSIPTransportParams& aParams);

    public: // New functions

        inline void SetOwnerId(TUint32 aOwnerId);
        inline TUint32 OwnerId() const;

        inline void SetIapId(const TUint32& aIapId);
        inline TUint32 IapId() const;

        inline void SetTransportId(const TUint32& aTransportId);
        inline TUint32 TransportId() const;
        
        inline void SetCompartmentId(const TUint32& aCompartmentId);
        inline TUint32 CompartmentId() const;
        
        inline void SetIgnoreCompartmentId(TBool aIgnoreCompartmentId);
        inline TBool IgnoreCompartmentId() const;

        inline void SetNATBindingObserver(MSIPNATBindingObserver* aObserver);
        inline MSIPNATBindingObserver* NATBindingObserver();

        inline TSIPTransportParams& 
            operator=(const TSIPTransportParams& aParams);
            
        inline void SetSkipNaptrAndSrv(TBool aSkipNaptrAndSrv);
        inline TBool SkipNaptrAndSrv() const;

    private: // Data

        TUint32 iOwnerId;
        TUint32 iIapId;
        TUint32 iTransportId;
        TUint32 iCompartmentId;
        TBool iIgnoreCompartmentId;
        MSIPNATBindingObserver* iNATBindingObserver;
        TBool iSkipNaptrAndSrv;
    };

#include "TSIPTransportParams.inl"

#endif // TSIPTRANSPORTPARAMS_H

// End of File
