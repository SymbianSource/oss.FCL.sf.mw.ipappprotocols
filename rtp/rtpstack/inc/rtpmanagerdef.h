/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/




#ifndef __RtpManagerDef_H
#define __RtpManagerDef_H

// CONSTANTS
const TUint KRtpMaxPayloadTypes( 128 ); // number of Payload types available.
const TInt KUseDefaultIap( -1 );

// DATA TYPES
enum TObjectType
    {
    ESession        = 0,
    EStream
    };

// CLASS DECLARATION
class TArrayStore
    {
    public :
        inline TArrayStore( const TRtpId aMagicKey );
        inline TArrayStore( const TRtpId aMagicKey, const TUint aSessionAddress,
                            const TObjectType aObjectType );
        inline TRtpId GetMagicKey() const;
        inline TUint GetSessionAddress() const;
        inline TObjectType GetObjectType() const;

        static const TInt iOffset;
    private:   // Data
        const TRtpId iMagicKey;
        const TUint iSessionAddress;
        const TObjectType iObjectType;
    };

#include "rtpmanagerdef.inl"

#endif

// End of file
