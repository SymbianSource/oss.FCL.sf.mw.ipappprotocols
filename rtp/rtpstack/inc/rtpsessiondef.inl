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




#ifndef RtpSessionDef_INL
#define RtpSessionDef_INL

#include "rtpsessiondef.h"

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
TStream::TStream( const TRtpId aMagicKey ) 
    : iMagicKey( aMagicKey ), iStreamAddress( 0 )
    {
    };

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
TStream::TStream( const TRtpId aMagicKey, const TUint aStreamAddress )
    : iMagicKey( aMagicKey ), iStreamAddress( aStreamAddress )
    {
    };

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
const TRtpId TStream::GetMagicKey() const
    {
    return iMagicKey;
    };

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
const TUint TStream::GetStreamAddress() const
    {
    return iStreamAddress;
    };

#endif

// End of file