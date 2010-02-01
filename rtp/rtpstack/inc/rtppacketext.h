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




#ifndef __RTPPACKETEXT_H
#define __RTPPACKETEXT_H

// INCLUDES
#include <e32def.h>
#include <rtpheader.h>


// CLASS DECLARATION

/**
*  Pure virtual interface for Extended RTP Packet formats.
*
*  @lib RtpService.dll
*/
class MRtpPacketExt
    {
    public:
        virtual void ConstructL( TUint aPacketSize, TRtpId aSessionId ) = 0;
        virtual void Close() = 0;
        virtual void RtpPacketReset() = 0;
        virtual void RtpPacketResetPtr() = 0;  

        virtual TInt RtpPacketBuild( const TRtpSendHeader& aHeader, const TDesC8& aPayload ) = 0;
        virtual TInt RtpPacketProcess( TUint8* aBuf, TInt* aLength ) = 0;

        virtual void RegisterRtpObserver( MRtpObserver& aObserver ) = 0;
        virtual void UnregisterRtpObserver() = 0;
        virtual void SetSize( TUint32 aSize ) = 0;

        virtual TUint32 Size() const = 0;
        virtual TUint8* Data() const = 0;
        virtual HBufC8* GetHBuf() const = 0;
        virtual const TPtrC8 Des() const = 0;


    };

#endif  // __RTPPACKETEXT__ 

// End of File
