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




#ifndef __RtpSessionDef_H
#define __RtpSessionDef_H

// CONSTANTS
const TUint KMaxRtpPacketSize = 8192; // 65536 bits = 8192 bytes
const TUint KMaxRtcpPacketSize = 1024;

// Default bandwidth in bps
const TUint KRtpDefaultBandWidth = 64000;

// Minimum bandwidth in bps
const TUint KRtpMinimumBandWidth = 8000;

// RTCP minimum interval (ms) (default is 5 seconds)
const TUint KRtpDefaultRtcpInterval = 5000;

// RTCP bandwidth fraction (default is 5%)
const TReal KRtpDefaultRtcpFrac = 0.05;

// Fraction of RTCP bandwidth to be shared among active senders
const TReal KRtpDefaultSenderBWFrac = 0.25;

// Fraction of RTCP bandwidth to be shared among receivers
const TReal KRtpDefaultObserverBWFrac = 1 - KRtpDefaultSenderBWFrac;

// Default RTCP average packet size
const TUint KAverageRtcpPacketLength = 128;

// Default RTCP minimum interval (ms)
const TUint KMinRtcpTimeInterval = 3000;

// Compensation for RTCP interval (ms)
const TReal KCompensationRtcp = 2.71828 - 1.5;

// Used to calculate the average size of an RTCP packet, this represents
// the fraction by which the most recent RTCP packet influences the average value
const TUint KRtcpMaximumParticipants = 256;

// Dictates how often the SDES information is sent
const TInt KSDES_SEND_PACKET = 5;

// use temporarily in building RTCP report
const TUint KMaxNumActiveRcvStreams = 128;  

// CLASS DECLARATION
class TStream
    {
    public :
        inline TStream( const TRtpId aMagicKey );

        inline TStream( const TRtpId aMagicKey, const TUint aStreamAddress );

        inline const TRtpId GetMagicKey() const;

        inline const TUint GetStreamAddress() const;

        static const TInt iOffset;

    private:
        const TRtpId iMagicKey;
        const TUint iStreamAddress;
    };

#include "rtpsessiondef.inl"

#endif

// End of file
