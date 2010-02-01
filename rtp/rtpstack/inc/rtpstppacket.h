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




#ifndef __RTPSTPPACKET_H
#define __RTPSTPPACKET_H

//  INCLUDES
#include <rtpstpheader.h>

// FUNCTION PROTOTYPES
void write16( TUint8* const aPointer, TUint32 aValue );
TUint32 read16( const TUint8* const aPointer );
void write32( TUint8* const aPointer, TUint32 aValue );
TUint32 read32( const TUint8* const aPointer );

// CLASS DECLARATION

/**
*  STP packet interface class 
*
*  @lib RtpStpPacket.dll
*/
class MRtpPacketExt
    {
    public:
        virtual void ConstructL( TUint aPacketSize, TRtpId aSessionId ) = 0;
        virtual void Close() = 0;
        virtual void RtpPacketReset() = 0;
        virtual void RtpPacketResetPtr() = 0;  

        virtual TInt RtpPacketBuild( const TRtpSendHeader& aHeader, 
                                     const TDesC8& aPayload ) = 0;

        virtual TInt RtpPacketProcess( TUint8* aBuf, TInt* aLength ) = 0;

        virtual void RegisterRtpObserver( MRtpObserver& aObserver ) = 0;
        virtual void UnregisterRtpObserver() = 0;
        virtual void SetSize( TInt aSize ) = 0;

        virtual TInt Size() const = 0;
        virtual TUint8* Data() const = 0;
        virtual HBufC8* GetHBuf() const = 0;
        virtual const TPtrC8 Des() const = 0;

    };

/**
*  STP packet concrete class 
*
*  @lib RtpStpPacket.dll
*/
class CRtpStpPacket : public CBase, public MRtpPacketExt
    {
    public:
        CRtpStpPacket();
        ~CRtpStpPacket();
        virtual void ConstructL( TUint aPacketSize, TRtpId aSessionId );
        virtual void Close();

        virtual void RtpPacketReset();
        virtual void RtpPacketResetPtr();  

        virtual TInt RtpPacketBuild( const TRtpSendHeader& aHeader, const TDesC8& aPayload );
        virtual TInt RtpPacketProcess( TUint8* aBuf, TInt* aLength );

        inline virtual void RegisterRtpObserver( MRtpObserver& aObserver );
        inline virtual void UnregisterRtpObserver();
        inline virtual void SetSize( TInt aSize );
        inline virtual TInt Size() const;
        inline virtual TUint8* Data() const;
        inline virtual HBufC8* GetHBuf() const;
        inline const TPtrC8 Des() const;
    
    private : 
        TUint32 read16( const TUint8* const aPointer );
        void write16( TUint8* const aPointer, TUint32 aValue );
        
    private:
        MRtpObserver* iObserver;
        HBufC8* iBuf;
        TInt iSize;   
        TUint8* iData;          // pointer to the beginning of the iBuf
        TUint8* iDataPtr;       // pointer to the beginning of date unhandled
        TRtpId iSessionId;      // session ID value
    };

#include "rtpstppacket.inl"

#endif  // __RTPSTPPACKET_H

// End of File
