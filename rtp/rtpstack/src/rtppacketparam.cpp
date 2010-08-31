/*
* Copyright (c) 2002-2003 Nokia Corporation and/or its subsidiary(-ies).
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




#include "rtppacketparam.h"
#include "rtputil.h"

////////////////////////////TRtpPacketStreamParam////////////////////////////////
/**
*  Stream status parameters as received in latest packet. 
*
*  @lib RtpService.dll
*/
TRtpPacketStreamParam::TRtpStreamHeaderField::TRtpStreamHeaderField():
    SSRC( 0 ),
    payload( KPayloadTypeMax ), //last value in payload array which is 0
    seqNum( 0 ),
    timeStamp( 0 )
    {
    }
        
/**
* RTCP packet
* Header field
**/
TRtpPacketStreamParam::TRtcpStreamHeaderField::TRtcpStreamHeaderField():
    SSRC( 0 )
    {
    }
            
/**
* RTCP packet
* SR field
**/
TRtpPacketStreamParam::TRtcpStreamHeaderFieldSR::TRtcpStreamHeaderFieldSR():
    SSRC( 0 ),
    cumNumOctetsSent( 0 ),
    numPacketsSent( 0 )
    {
    }
            
/**
* RTCP packet
* RR field
**/   
TRtpPacketStreamParam::TRtcpStreamHeaderFieldRR::TRtcpStreamHeaderFieldRR():
    SSRC( 0 ),
    fractionLost( 0 ),
    cumNumPacketsLost( 0 ),
    seqNumReceived( 0 ),
    arrivalJitter( 0 )
    {
    }
           
/**
* RTCP packet
* SDES field
**/    
TRtpPacketStreamParam::TRtcpStreamHeaderFieldSDES::TRtcpStreamHeaderFieldSDES():
    SSRC( 0 )
    {
    }
           
/**
* RTCP packet
* BYE field
**/    
TRtpPacketStreamParam::TRtcpStreamHeaderFieldBYE::TRtcpStreamHeaderFieldBYE():
    SSRC( 0 )
    {
    }
            
/**
* RTCP packet 
* APP field
**/    
TRtpPacketStreamParam::TRtcpStreamHeaderFieldAPP::TRtcpStreamHeaderFieldAPP():
    SSRC( 0 ),
    totalPacketLen( 0 )
    {
    }

///////////////////////////////////TRtpPacketIOParam/////////////////////////////
/**
*  Other passed/parsed parameters of the packet. 
*
*  @lib RtpService.dll
*/

/**
* RTP packet
* Header field
**/ 
TRtpPacketIOParam::TRtpIOHeaderField::TRtpIOHeaderField():
    padding( 0 ),
    marker( 0 ),
    fHeaderExtension( 0 ),
    numCSRC( 0 ),
    CSRCarray( NULL ),
    payloadData( NULL ),
    payloadDataLen( 0 ),
	iCsrcList(0),
    extension()
    {
    }
            
/**
* RTP packet
* Header extension field
**/ 
TRtpPacketIOParam::TRtpIOHeaderField::TRtpIOHeaderExtension::
    TRtpIOHeaderExtension():
    type( 0 ),
    length( 0 ),
    data( NULL )
    { 
    }
                     
/**
* RTCP packet
* Header field
**/ 
TRtpPacketIOParam::TRtcpIOHeaderField::TRtcpIOHeaderField():
    pt( ERTCP_HEADER ),
    sourceCount( 0 ),
    length( 0 )
    {
    }

/**
* RTCP packet
* SR field
**/ 
TRtpPacketIOParam::TRtcpIOHeaderFieldSR::TRtcpIOHeaderFieldSR():
    NTPTimeStampSec( 0 ),
    NTPTimeStampFrac( 0 ),
    timeStamp( 0 )
    {
    }
        
/**
* RTCP packet
* RR field
**/ 
TRtpPacketIOParam::TRtcpIOHeaderFieldRR::TRtcpIOHeaderFieldRR():
    lastSRTimeStamp( 0 ),
    delaySinceLSR( 0 )
    {
    }
        
/**
* RTCP packet
* SDES field
**/ 
TRtpPacketIOParam::TRtcpIOHeaderFieldSDES::TRtcpIOHeaderFieldSDES(): 
    paddingSize(0)
    {
    for( int i = 0; i < 8; i++ )
        {
        sdesItems[i]     = NULL;
        sdesItemsSize[i] = 0;
        }
    }
           
/**
* RTCP packet
* BYE field
**/ 
TRtpPacketIOParam::TRtcpIOHeaderFieldBYE::TRtcpIOHeaderFieldBYE() :
    reason( NULL ),
    reasonSize( 0 ),
    paddingSize( 0 )
    {
    }

/**
* RTCP packet
* APP field
**/ 
TRtpPacketIOParam::TRtcpIOHeaderFieldAPP::TRtcpIOHeaderFieldAPP() :
    appData( NULL ),
    appDataLen( 0 )
    {
    for ( TInt i( 0 ); i < 4; i++ )
        {
        name[i] = 0;
        }
    }
    
//  End of File
