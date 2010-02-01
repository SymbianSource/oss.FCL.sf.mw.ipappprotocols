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




#ifndef RtpManagerDef_INL
#define RtpManagerDef_INL

#include "rtpmanagerdef.h"

// ---------------------------------------------------------------------------
// Constructor
// 
// ---------------------------------------------------------------------------
//
TArrayStore::TArrayStore( const TRtpId aMagicKey )
    : iMagicKey( aMagicKey ),
      iSessionAddress( 0 ),
      iObjectType( ESession )
    {
    }

// ---------------------------------------------------------------------------
// Constructor
// 
// ---------------------------------------------------------------------------
//
TArrayStore::TArrayStore( const TRtpId aMagicKey, const TUint aSessionAddress, 
                          const TObjectType aObjectType ) 
    : iMagicKey( aMagicKey ),
      iSessionAddress( aSessionAddress ),
      iObjectType( aObjectType )
    {
    }

// ---------------------------------------------------------------------------
// const TRtpId TArrayStore::GetMagicKey()
// 
// ---------------------------------------------------------------------------
//
TRtpId TArrayStore::GetMagicKey() const
    {
    return iMagicKey;
    }


// ---------------------------------------------------------------------------
// const TUint TArrayStore::GetSessionAddress()
// 
// ---------------------------------------------------------------------------
//
TUint TArrayStore::GetSessionAddress() const
    {
    return iSessionAddress;
    }

// ---------------------------------------------------------------------------
// const TObjectType TArrayStore::GetObjectType()
// 
// ---------------------------------------------------------------------------
//
TObjectType TArrayStore::GetObjectType() const
    {
    return iObjectType;
    }

#endif

// End of file