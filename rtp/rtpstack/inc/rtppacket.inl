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




#ifndef RtpPacket_INL
#define RtpPacket_INL

//#include "RtpPacket.h"

void CRtpPacket::SetSize( TInt aSize )
    {
    iSize = aSize;
    };

void CRtpPacket::SetType( TRtpPacketType aType )
    {
    iType = aType;
    };

TInt CRtpPacket::Size() const
    {
    return iSize;
    };

TUint8* CRtpPacket::Data()
    {
    return iData;
    };

HBufC8* CRtpPacket::GetHBuf()
    {
    return iBuf;
    };

TRtpPacketType CRtpPacket::Type() const
    {
    return iType;
    };

const TPtrC8 CRtpPacket::Des() const
    {
    return TPtrC8( iData, iSize );
    };

#endif

// End of file