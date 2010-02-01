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




#ifndef RtpStpPacket_INL
#define RtpStpPacket_INL

void CRtpStpPacket::RegisterRtpObserver( MRtpObserver& aObserver ) 
    {
    iObserver = &aObserver;
    };

void CRtpStpPacket::UnregisterRtpObserver()
    {
    iObserver = NULL;
    };

void CRtpStpPacket::SetSize( TInt aSize )
    {
    iSize = aSize;
    };
TInt CRtpStpPacket::Size() const
    {
    return iSize;
    };

TUint8* CRtpStpPacket::Data() const
    {
    return iData;
    };

HBufC8* CRtpStpPacket::GetHBuf() const
    {
    return iBuf;
    };

const TPtrC8 CRtpStpPacket::Des() const
    {
    return TPtrC8( iData, iSize );
    };

#endif

// End of file