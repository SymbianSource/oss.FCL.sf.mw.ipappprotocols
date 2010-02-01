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




#ifndef RtpCommDef_H
#define RtpCommDef_H

#include <e32def.h>
#include "rtputil.h"
//#include "RtpSessionDef.h"


// CONSTANTS
const TUint KDefaultLocalPort = 5000;
const TUint KMaxLocalPort     = 0x0ffff;
const TInt  KMaxPorts         = 2;
const TUint KMaximumRtpPacketSize = 8192; // 65536 bits = 8192 bytes

/**
*  class for rtp&rtcp received call back. 
*
*  @lib RtpService.dll
*/
class MReceivedNotify
    {
    public:
        virtual void OnRtpReceivedL() = 0;
        virtual void OnRtcpReceivedL() = 0;
        virtual void OnReceptionError( TPortType aPort, TInt aError ) = 0;
        virtual void OnPacketRejected(TPortType aPort) = 0;   
    };

/**
* Holds an a packet and request status for buffering if socket is busy.
*
*  @lib RtpService.dll
*/
/*class TSendArrayItem
    {
    public:

        // Copy constructor
        TSendArrayItem( TSendArrayItem& aItem ) :
            iPacket(aItem.iPacket),
            iStatus(aItem.iStatus)
            {
            }

        // Constructor
        TSendArrayItem( const TDesC8& aPacket, TRequestStatus& aStatus ) :
            iPacket(aPacket),        
            iStatus( aStatus )
            {
            }


    public: // Data
        const TDesC8& iPacket;
        TRequestStatus& iStatus;
    };
*/
#endif	// RtpCommDef_H





