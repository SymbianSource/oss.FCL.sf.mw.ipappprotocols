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




#ifndef __RTPPACKETPARAM_H
#define __RTPPACKETPARAM_H

//  INCLUDES
//#include <e32def.h>
#include <rtpheader.h>

// DATA TYPES
enum TRtpPacketType
    {
    ERTP                = 1,
    ERTCP_HEADER        = 2,
    ERTCP_SR            = 200,
    ERTCP_RR            = 201,
    ERTCP_SDES          = 202,
    ERTCP_BYE           = 203,
    ERTCP_APP           = 204
    };

/**
*  Stream status parameters as received in latest packet. 
*
*  @lib RtpService.dll
*/
class TRtpPacketStreamParam 
    {
    public://Nested classes
       
       /**
        * RTP packet
        * Header field
        **/
        class TRtpStreamHeaderField
            {
            public: //Constructor
                TRtpStreamHeaderField();
                    
            public: //Data        
                TRtpSSRC SSRC;                 
                TUint8 payload;
                TRtpSequence seqNum;
                TRtpTimeStamp timeStamp;
            };
      
       /**
        * RTCP packet
        * Header field
        **/
        class TRtcpStreamHeaderField
            {
            public://Constructor
                TRtcpStreamHeaderField();
                
            public: //Data
                TRtpSSRC SSRC;
            };
       
       /**
        * RTCP packet
        * SR field
        **/
        class TRtcpStreamHeaderFieldSR
            {
            public://Constructor
                TRtcpStreamHeaderFieldSR();
                    
            public://Data    
                TRtpSSRC SSRC;
                TUint cumNumOctetsSent;
                TUint numPacketsSent;
            };
     
       /**
        * RTCP packet
        * RR field
        **/   
        class TRtcpStreamHeaderFieldRR
            {
            public://Constructor
                TRtcpStreamHeaderFieldRR();
                
            public: //Data    
                TRtpSSRC SSRC;
                TUint8 fractionLost;
                TUint cumNumPacketsLost;
                TUint32 seqNumReceived;
                TUint arrivalJitter;
            };
      
       /**
        * RTCP packet
        * SDES field
        **/    
        class TRtcpStreamHeaderFieldSDES
            {
            public://Constructor
                TRtcpStreamHeaderFieldSDES();
                
            public://Data
                TRtpSSRC SSRC;
            };
     
       /**
        * RTCP packet
        * BYE field
        **/    
        class TRtcpStreamHeaderFieldBYE
            {
            public://Constructor
                TRtcpStreamHeaderFieldBYE();
                
            public://Data
                TRtpSSRC SSRC;
            };
      
       /**
        * RTCP packet 
        * APP field
        **/    
        class TRtcpStreamHeaderFieldAPP
            {
            public: //Constructor
                TRtcpStreamHeaderFieldAPP();
                
            public://Data
                TRtpSSRC SSRC;
                TInt totalPacketLen;
            };
            
    public://Packet data fields
        TRtpStreamHeaderField TRTP;
        TRtcpStreamHeaderField TRTCP_HEADER;
        TRtcpStreamHeaderFieldSR TRTCP_SR;
        TRtcpStreamHeaderFieldRR TRTCP_RR;
        TRtcpStreamHeaderFieldSDES TRTCP_SDES;
        TRtcpStreamHeaderFieldBYE TRTCP_BYE;
        TRtcpStreamHeaderFieldAPP TRTCP_APP;
    };

/**
*  Other passed/parsed parameters of the packet. 
*
*  @lib RtpService.dll
*/
class TRtpPacketIOParam 
    {
    public://Nested classes
        
       /**
        * RTP packet
        * Header field
        **/ 
        class TRtpIOHeaderField
            {
            
            public://Constructor
                TRtpIOHeaderField();
                
             public://Data
                TUint8 padding;
                TUint8 marker;
                TUint8 fHeaderExtension;
                TUint8 numCSRC;
                TUint32* CSRCarray;
                TUint8* payloadData;
                TInt payloadDataLen;
                const TArray<TRtpCSRC> *iCsrcList;
             
             public://Nested class
               /**
                * RTP packet
                * Header extension field
                **/ 
                class TRtpIOHeaderExtension
                    {
                    public://Constructor && Destructor
                       TRtpIOHeaderExtension();
                       
                    public://Data
                        TInt type;
                        TInt length;
                        TUint8* data;
                    };
                    
             public://Packet data field
                TRtpIOHeaderExtension extension;
                
            };
        
       /**
        * RTCP packet
        * Header field
        **/ 
        class TRtcpIOHeaderField
            {
            public://Constructor
                TRtcpIOHeaderField();
                
            public://Data
                TRtpPacketType pt;
                TInt sourceCount;
                TInt length;
            };
        
       /**
        * RTCP packet
        * SR field
        **/ 
        class TRtcpIOHeaderFieldSR
            {
            public://Constructor
                TRtcpIOHeaderFieldSR();
                
            public://Data
                TUint32 NTPTimeStampSec;
                TUint32 NTPTimeStampFrac;
                TRtpTimeStamp timeStamp;
            };
        
       /**
        * RTCP packet
        * RR field
        **/ 
        class TRtcpIOHeaderFieldRR
            {
            public://Constructor
                TRtcpIOHeaderFieldRR();
                
            public://Data
                TUint32 lastSRTimeStamp;
                TUint32 delaySinceLSR;
            };
        
       /**
        * RTCP packet
        * SDES field
        **/ 
        class TRtcpIOHeaderFieldSDES
            {
            public://Constructor
                TRtcpIOHeaderFieldSDES();
                
            public://Data
                TUint8* sdesItems[8];
                TInt sdesItemsSize[8];
                TInt paddingSize;
            };
        
       /**
        * RTCP packet
        * BYE field
        **/ 
        class TRtcpIOHeaderFieldBYE
            {
            public://Constructor
                TRtcpIOHeaderFieldBYE();
                
            public://Data
                TUint8* reason;
                TInt reasonSize;   
                TInt paddingSize;
            };
        
       /**
        * RTCP packet
        * APP field
        **/ 
        class TRtcpIOHeaderFieldAPP
            {
            public://Constructor
                TRtcpIOHeaderFieldAPP();
                
             public://Data
                TRtcpAppName name;
                TUint8* appData;
                TInt appDataLen;
            };
  
    public:// Packet data fields
        TRtpIOHeaderField TRTP;
        TRtcpIOHeaderField TRTCP_HEADER;
        TRtcpIOHeaderFieldSR TRTCP_SR;
        TRtcpIOHeaderFieldRR TRTCP_RR;
        TRtcpIOHeaderFieldSDES TRTCP_SDES;
        TRtcpIOHeaderFieldBYE TRTCP_BYE;
        TRtcpIOHeaderFieldAPP TRTCP_APP;
    };

#endif  // __RTPPACKETPARAMS_H

// End of File
