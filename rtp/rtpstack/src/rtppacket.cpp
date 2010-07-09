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




// INCLUDE FILES 
#include <e32base.h>
#include <e32std.h>

#include "rtputil.h"
#include "rtpsdes.h"
#include "rtppacket.h"
#include "rtpmanager.h"

// CONSTANTS
const TInt KRtpVersion = 2;    // Current protocol version          
const TUint8 KNullTerm( '\0' );
const TUint KMinRtpHeaderLen( 12 );
const TUint KMinRtcpAppLen( 3 );

const TInt KCSRCListMax = 15;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CRtpPacket::CRtpPacket( const TUint32* aRtpTimeRates ) :
    iSize( 0 ),
    iProfileRTPTimeRates( aRtpTimeRates )
    {
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CRtpPacket::ConstructL( TUint aPacketSize )
    {
    iBuf = HBufC8::NewL( aPacketSize );
    iData = const_cast<TUint8*>( iBuf->Des().Ptr() );
    iDataPtr = iData;
    iExdataAlloc =EFalse;
	iCsrcAlloc= EFalse;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CRtpPacket* CRtpPacket::NewL( TUint aPacketSize, const TUint32* aRtpTimeRates )
    {
    CRtpPacket* self = new ( ELeave ) CRtpPacket( aRtpTimeRates );
    CleanupStack::PushL( self );
    self->ConstructL( aPacketSize );
    CleanupStack::Pop(); //self
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CRtpPacket::~CRtpPacket()
    {
    delete iBuf; iBuf = NULL;
    }

// ---------------------------------------------------------------------------
// 
// 
// ---------------------------------------------------------------------------
//
void CRtpPacket::RtpPacketReset()
    {
    iSize = 0;
    iDataPtr = iData;
    }

// ---------------------------------------------------------------------------
// CRtpPacket::RtpPacketResetPtr()
// 
// ---------------------------------------------------------------------------
//
void CRtpPacket::RtpPacketResetPtr()
    {
    iDataPtr = iData;
    }

// ---------------------------------------------------------------------------
// CRtpPacket::RtpPacketBuildRtp()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpPacket::RtpPacketBuildRtp( TRtpPacketStreamParam* aStreamParam, 
                                    TRtpPacketIOParam* aInitParam )
    {
    //Get a pointer to the beginning of iBuf
    TUint8* dataP = iData;
    
    //Fill dataP with binary zeroes (i.e. 0x00), replacing any existing content
    Mem::FillZ( dataP, KMinRtpHeaderLen );

    /****************************
     * dataP[0] = 8 bit         *
     *                          *
     *    2 bit = version       *
     *    1 bit = padding       *
     *    1 bit = extension     *
     *    4 bit = CRSRC Count   *
     ****************************/
    
    //version (2 bit)
    // 0111001
    // 1100000
    // -------
    // 1111001
    dataP[0] |= ( KRtpVersion << 6 );

    // padding (1 bit)
    dataP[0] |= static_cast<TUint8>( ( aInitParam->TRTP.padding << 5 ) );

    // header extension (1 bit)
    if ( aInitParam->TRTP.fHeaderExtension)
        {
        dataP[0] |= ( 1 << 4 );
        }

    // CC 4 bit)
    if( aInitParam->TRTP.numCSRC)
        {
        dataP[0] |= aInitParam->TRTP.numCSRC;
        }
    
    /****************************
     * dataP[1] = 8 bit         *
     *                          *
     *    1 bit = marker        *
     *    7 bits = payload type  *
     ****************************/
    
    // marker (1 bit)
    dataP[1] |= static_cast<TUint8>( aInitParam->TRTP.marker << 7 );

    // payload type (7 bit)
    // confirm that payload can fit the 7 bit (0 - 127)
    if(aStreamParam->TRTP.payload > KPayloadTypeMax )
    	{
    	return KErrTooBig;
    	}
        
    
    dataP[1] |= aStreamParam->TRTP.payload;
    
    //set the pointer to point at the beginning of the sequence number.
    dataP += 2;
    // sequence number (16 bit)
    Write16( dataP, aStreamParam->TRTP.seqNum );
    
    //set the pointer to point at the beginning of the time stamp.
    dataP += 2; 
    // RTP timestamp (32 bit)
    Write32( dataP, aStreamParam->TRTP.timeStamp );
    
    //set the pointer to point at the beginning of SSRC.
    dataP += 4;
    // SSRC (32 bit)
    Write32( dataP, aStreamParam->TRTP.SSRC );

    //set the pointer to point to the first bit after SSRC
    dataP += 4;
    
    
    //Set CSRC if Present
    for(TInt count = 0; count<aInitParam->TRTP.numCSRC; count++)
        {
        Write32( dataP, (*aInitParam->TRTP.iCsrcList)[count]);
        dataP += 4;
        }
    

    if ( aInitParam->TRTP.fHeaderExtension )
        {
	    
	    //calculate 8 bit values to 32 bit words
	    TInt16 length32 =
            static_cast<TInt16>( aInitParam->TRTP.extension.length / 4 );
        
        //if length mod 4 != 0, 
        if ( aInitParam->TRTP.extension.length % 4 )
	        {
    		length32++;
    		}

	    // header extension type defined by profile
        // aInitParam->TRTP.extension.type is 32-bit and part of the API
        // (i.e. unchangeable), so we have to extract the 16-LSB
	    Write16( dataP, aInitParam->TRTP.extension.type & 0xFFFF );
	    dataP += 2;

	    // header extension length in number of 32-bit words
	    Write16( dataP, length32 );
	    dataP += 2;

	    // header extension data
	    if (length32 && aInitParam->TRTP.extension.data )
		    {
		    Mem::FillZ( dataP, length32 * 4 );
		    for ( TInt i = 0; i < aInitParam->TRTP.extension.length; i++ )
		        {
	    		dataP[i] = aInitParam->TRTP.extension.data[i];
	    		}
		    dataP += ( length32 * 4 );	
		    }
	    }
    
    //Copy(Target, Source, Length)
    Mem::Copy( dataP, aInitParam->TRTP.payloadData,
               aInitParam->TRTP.payloadDataLen );
    iSize = aInitParam->TRTP.payloadDataLen + ( dataP - iData );
    return KErrNone;

    }

// ---------------------------------------------------------------------------
// TInt CRtpPacket::RtpPacketBuildRtcp()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpPacket::RtpPacketBuildRtcp( TRtpPacketStreamParam* aStreamParam, 
                                     TRtpPacketIOParam* aInitParam )
    {
    Mem::FillZ( iDataPtr, 4 );

    // version
    iDataPtr[0] |= ( KRtpVersion << 6 );

    // padding = 0
    iDataPtr[0] |= ( 0 << 5 );

    // source count
    iDataPtr[0] |= static_cast<TUint8>( aInitParam->TRTCP_HEADER.sourceCount );

    // packet type
    iDataPtr[1] = static_cast<TUint8>( aInitParam->TRTCP_HEADER.pt );
    iDataPtr += 2;

    // length of RTCP packet
    // aInitParam->TRTCP_HEADER.length is 32-bit and part of the API
    // (i.e. unchangeable), so we have to extract the 16-LSB
    Write16( iDataPtr, aInitParam->TRTCP_HEADER.length & 0xFFFF );
    iDataPtr += 2;

    // SSRC of sender in case of RR
    if ( aInitParam->TRTCP_HEADER.pt == ERTCP_RR )
        {
	    Write32( iDataPtr, aStreamParam->TRTCP_HEADER.SSRC );
	    iDataPtr += 4;
	    }

    iSize += ( aInitParam->TRTCP_HEADER.length + 1 ) * 4;

    return KErrNone;
    }
        
// ---------------------------------------------------------------------------
// TInt CRtpPacket::RtpPacketBuildApp()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpPacket::RtpPacketBuildApp( TRtpPacketStreamParam* aStreamParam, 
                                    TRtpPacketIOParam* aInitParam )
    {
    // SSRC
    Write32( iDataPtr, aStreamParam->TRTCP_APP.SSRC );
    iDataPtr += 4;

    // name
    Mem::Copy( iDataPtr, aInitParam->TRTCP_APP.name, 4 );
    iDataPtr += 4;

    // application data
    Mem::Copy( iDataPtr, aInitParam->TRTCP_APP.appData,
               aInitParam->TRTCP_APP.appDataLen );

    iDataPtr += aInitParam->TRTCP_APP.appDataLen;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TInt CRtpPacket::RtpPacketBuildBye()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpPacket::RtpPacketBuildBye( TRtpPacketStreamParam* aStreamParam, 
                                    TRtpPacketIOParam* aInitParam )
    {
    TUint8 theIndex( 0 );
    // SSRC
    Write32( iDataPtr, aStreamParam->TRTCP_BYE.SSRC );
    iDataPtr += 4;

    // reason
    iDataPtr[0] = static_cast<TUint8>( aInitParam->TRTCP_BYE.reasonSize );
    iDataPtr += 1;

    Mem::Copy( iDataPtr, aInitParam->TRTCP_BYE.reason,
               aInitParam->TRTCP_BYE.reasonSize );

    iDataPtr += aInitParam->TRTCP_BYE.reasonSize;

    // padding
    if ( aInitParam->TRTCP_BYE.paddingSize != 0 )
        {
	    for ( theIndex = 0; theIndex < aInitParam->TRTCP_BYE.paddingSize;
              theIndex++ )
	        {
    		iDataPtr[theIndex] = KNullTerm;
    		}

	    iDataPtr += aInitParam->TRTCP_BYE.paddingSize;
	    }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TInt CRtpPacket::RtpPacketBuildRr()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpPacket::RtpPacketBuildRr( TRtpPacketStreamParam* aStreamParam, 
                                   TRtpPacketIOParam* aInitParam )
    {
    // SSRC
    Write32( iDataPtr, aStreamParam->TRTCP_RR.SSRC );
    iDataPtr += 4;

    // fraction lost
    iDataPtr[0] = aStreamParam->TRTCP_RR.fractionLost;
    iDataPtr++;

    // cumulative number of packets lost
    Write24( iDataPtr, aStreamParam->TRTCP_RR.cumNumPacketsLost );
    iDataPtr += 3;

    // extended highest sequence number received
    Write32( iDataPtr, aStreamParam->TRTCP_RR.seqNumReceived );
    iDataPtr += 4;

    // interarrival jitter
    Write32( iDataPtr, aStreamParam->TRTCP_RR.arrivalJitter );
    iDataPtr += 4;

    // last SR timestamp
    Write32( iDataPtr, aInitParam->TRTCP_RR.lastSRTimeStamp );
    iDataPtr += 4;

    // delay since last SR timestamp
    Write32( iDataPtr, aInitParam->TRTCP_RR.delaySinceLSR );
    iDataPtr += 4;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TInt CRtpPacket::RtpPacketBuildSr()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpPacket::RtpPacketBuildSr( TRtpPacketStreamParam* aStreamParam, 
                                   TRtpPacketIOParam* aInitParam )
    {
    // SSRC
    Write32( iDataPtr, aStreamParam->TRTCP_SR.SSRC );
    iDataPtr += 4;

    // NTP timestamp (seconds)
    Write32( iDataPtr, aInitParam->TRTCP_SR.NTPTimeStampSec );
    iDataPtr += 4;

    // NTP timestamp (fraction)
    Write32( iDataPtr, aInitParam->TRTCP_SR.NTPTimeStampFrac );
    iDataPtr += 4;

    // RTP timestamp
    Write32( iDataPtr, aInitParam->TRTCP_SR.timeStamp );
    iDataPtr += 4;

    // sender's packet count
    Write32( iDataPtr, aStreamParam->TRTCP_SR.numPacketsSent );
    iDataPtr += 4;

    // sender's octet count
    Write32( iDataPtr, aStreamParam->TRTCP_SR.cumNumOctetsSent );
    iDataPtr += 4;

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TInt CRtpPacket::RtpPacketBuildSdes()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpPacket::RtpPacketBuildSdes( TRtpPacketStreamParam* aStreamParam, 
                                     TRtpPacketIOParam* aInitParam )
    {
    TUint8 theIndex( 0 );

    // SSRC
    Write32( iDataPtr, aStreamParam->TRTCP_SDES.SSRC );
    iDataPtr += 4;

    // SDES items
    for ( theIndex = 0; theIndex < ERTCP_NUM_OF_SDES_ITEMS; theIndex++ )
        {
        if ( aInitParam->TRTCP_SDES.sdesItemsSize[theIndex] != 0 )
            {

            iDataPtr[0] = static_cast<TUint8>( theIndex + 1 );
            iDataPtr[1] = static_cast<TUint8>(
                              aInitParam->TRTCP_SDES.sdesItemsSize[theIndex] );
            iDataPtr += 2;

            Mem::Copy( iDataPtr,
            		 aInitParam->TRTCP_SDES.sdesItems[theIndex],
            		 aInitParam->TRTCP_SDES.sdesItemsSize[theIndex] );

            iDataPtr += aInitParam->TRTCP_SDES.sdesItemsSize[theIndex];
            }
        }

    // include end of list null octet
    iDataPtr[0] = KNullTerm;
    iDataPtr++;

    // padding
    if ( aInitParam->TRTCP_SDES.paddingSize != 0 )
        {
	    for ( theIndex = 0; theIndex < aInitParam->TRTCP_SDES.paddingSize;
              theIndex++ )
	        {
    		iDataPtr[theIndex] = KNullTerm;
    		}

	    iDataPtr += aInitParam->TRTCP_SDES.paddingSize;
	    }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// TInt CRtpPacket::RtpPacketBuild()
// 
// ---------------------------------------------------------------------------
//
TInt CRtpPacket::RtpPacketBuild( TRtpPacketStreamParam* aStreamParam, 
                                 TRtpPacketIOParam* aInitParam )
    {
    TInt result( KErrNone );
    if ( !aStreamParam || !aInitParam )
        {
        return KErrUnderflow;
        }

    // build the packet content
    switch ( iType )
        {
        case ERTP:
            result = RtpPacketBuildRtp( aStreamParam, aInitParam );
            break;

        case ERTCP_HEADER:
            result = RtpPacketBuildRtcp( aStreamParam, aInitParam );
            break;

        case ERTCP_SR:
            result = RtpPacketBuildSr( aStreamParam, aInitParam );
            break;

        case ERTCP_RR:
            result = RtpPacketBuildRr( aStreamParam, aInitParam );
            break;

        case ERTCP_SDES:
            result = RtpPacketBuildSdes( aStreamParam, aInitParam );
            break;

        case ERTCP_BYE:
            result = RtpPacketBuildBye( aStreamParam, aInitParam );
            break;

        case ERTCP_APP:
            result = RtpPacketBuildApp( aStreamParam, aInitParam );
            break;
            
        default:
            result = ERTCP_PACKET_ERROR; //unknown packet type
            break; 
        }

    return result;
    }

// ---------------------------------------------------------------------------
// TRtpSSRC CRtpPacket::RtpPacketGetSSRC()
// assumed to be called for an RTCP packet only after 
// RTCP_HEADER has determined the packet type
// ---------------------------------------------------------------------------
TRtpSSRC CRtpPacket::RtpPacketGetSSRC()
    {
    TUint8* dataP;

    // parse the packet content
    switch ( iType )
        {
        case ERTP:
            {
            dataP = iData + 8;
            return ( Read32( dataP ) );
            }

        case ERTCP_HEADER:
            {
            dataP = iData + 4;
            return ( Read32( dataP ) );
            }

        case ERTCP_SR:
        case ERTCP_RR:
        case ERTCP_SDES:
        case ERTCP_BYE:
        case ERTCP_APP:
            {
            dataP = iDataPtr;
            return ( Read32( dataP ) );
            }

        default:
            break;
        }

    return 0;
    }
// ----------------------------------------------------------------------------
// CRtpPacket::RtpPacketGetPayloadType()
// ----------------------------------------------------------------------------
//
TRtpPayloadType CRtpPacket::RtpPacketGetPayloadType()
	{
    TUint8* dataP = iData;
    // payload type
    return static_cast<TUint8>( dataP[1] & 0x7F );
	
	}

// ----------------------------------------------------------------------------
// CRtpPacket::RtpPacketProcessRtp()
// ----------------------------------------------------------------------------
//
TRtpRtcpEnum CRtpPacket::RtpPacketProcessRtpL( 
                             TRtpPacketStreamParam* aStreamParam,
                             TRtpPacketIOParam* aExtractParam )
    {
    RTP_DEBUG_PACKET( "CRtpPacket::RtpPacketProcessRtpL Entry" );
    TInt k;
    TInt err( KErrNone );
    TUint8* dataP = iData;

    // version          
    //   0xC0 = 1100 0000 
    // & 0xC0 removes all ones except the version number from the bit stream
    // 
    //   11011011
    //   11000000
    //   --------
    //   11000000 >> 6 = 00000011
    if ( ( ( dataP[0] & 0xC0 ) >> 6 ) != KRtpVersion )
        {
        RTP_DEBUG_DETAIL( "RtpPacketProcess: invalid version" );
        
        return ERTCP_PACKET_ERROR;
        }
    
    // padding         
    //    0x20 = 0010 0000 
    //
    // Checks if padding bit is set
    if ( dataP[0] & 0x20 )
        {
        
        // The last octet of the packet must contain a valid octet count
        // ( octet count < the total packet lenght minus the header size)
        if ( ( iSize - iData[ iSize - 1] - KMinRtpHeaderSize ) > 0 )
            {
            aExtractParam->TRTP.padding = 1;

            // The last octet of the padding contains a count of how many 
            // padding octets should be ignored, including itself.
            iSize -= iData[iSize - 1];
            }
        else
            {
            // Invalid padding value
            RTP_DEBUG_DETAIL( "RtpPacketProcess: invalid padding size" );
          
            return ERTCP_PACKET_ERROR;
            }
        }
    else //padding bit == 0
        {
        aExtractParam->TRTP.padding = 0;
        }

    // header extension
    aExtractParam->TRTP.fHeaderExtension =
        static_cast<TUint8>( ( dataP[0] & 0x10 ) >> 4 );

    // CC
    aExtractParam->TRTP.numCSRC = static_cast<TUint8>( dataP[0] & 0x0F );

    // marker
    aExtractParam->TRTP.marker =
        static_cast<TUint8>( ( dataP[1] & 0x80 ) >> 7 );

    // payload type
    aStreamParam->TRTP.payload = static_cast<TUint8>( dataP[1] & 0x7F );
    
    
   /**
    * The payload type must be known, and in particular it must not 
    * be equal to SR or RR[RFC3550]
    *
    * 7 bit (= 127) is only reserved for payload, so no need to check 
    * that payload differ from SR (SR = 200) and RR (RR = 201)
    * Since it always &0x7F so there is no possiblility to be more than
    * 127 
    **/
   
    dataP += 2;

    // sequence number
    aStreamParam->TRTP.seqNum = static_cast<TUint16>( Read16( dataP ) );
    dataP += 2;

    // RTP timestamp
    aStreamParam->TRTP.timeStamp = Read32( dataP );
    dataP += 4;

    // SSRC
    aStreamParam->TRTP.SSRC = Read32( dataP );
    dataP += 4;

    // CSRCs
    if ( aExtractParam->TRTP.numCSRC >0 )
        {
        // Calculate the minimum required size of the remainder of this
        // packet, in number of 8-bit words
        TInt minRequiredSizeLeft = aExtractParam->TRTP.numCSRC * 4;

        // Make sure there is are CSRCs to read
        if ( ( iSize - ( dataP - iData ) ) < minRequiredSizeLeft )
            {
            RTP_DEBUG_DETAIL( "RtpPacketProcess: invalid CSRC size" );

            return ERTCP_PACKET_ERROR;
            }

        // Originally Allocate memory for all CSRC:s
        //it will have memeory resouce problem so only allocate when
        // CSRC<2
        if ( aExtractParam->TRTP.numCSRC <= KCSRCListMax )
        	{
        	if ( !aExtractParam->TRTP.CSRCarray )
	        	{
	             TRAP( err,
	                   aExtractParam->TRTP.CSRCarray =
	                       static_cast<TUint32*>(
	                       User::AllocL( sizeof(TUint32) * aExtractParam->TRTP.numCSRC ) )
	                 );
	 
	             if (err==0)
	             	{
	             	iCsrcAlloc = ETrue;
	             	}
	            }
	        if ( err || !aExtractParam->TRTP.CSRCarray )
	            {
	            RTP_DEBUG_DETAIL( "RtpPacketProcess: CSRC missing" );

	            return ERTCP_PACKET_ERROR;
	            }
	        int count=0;    	
	        for ( k = 0; k < aExtractParam->TRTP.numCSRC; k++ )
	            {
	            // CSRCs ignored
	            TUint32 temp =Read32( dataP );
	            if (!temp)
	            	{
	            	return ERTCP_PACKET_ERROR;
	            	}
	            aExtractParam->TRTP.CSRCarray[k] = Read32( dataP );
	            dataP += 4;
	            count ++;
	            }
	        if (count!= aExtractParam->TRTP.numCSRC)
	        	{
	        	RTP_DEBUG_DETAIL( "RtpPacketProcess: numCSRC is not matched" );
	        	return ERTCP_PACKET_ERROR;
	        	}
        	}
    	else
    		{
    		for ( k = 0; k < aExtractParam->TRTP.numCSRC; k++ )
	    		{
	            // CSRCs ignored
	        	dataP += 4;
	       	 	}
    		}
        }
    if ( aExtractParam->TRTP.fHeaderExtension )
        {
        // Make sure there is a header extension to read
        if ( ( iSize - ( dataP - iData ) ) < 4 )
            {
            RTP_DEBUG_DETAIL( "RtpPacketProcess: invalid extension size" );
			return ERTCP_PACKET_ERROR;
            }

        // header extension type defined by profile
        aExtractParam->TRTP.extension.type = Read16( dataP );
        dataP += 2;

        // header extension length in number of 32-bit words
        aExtractParam->TRTP.extension.length = Read16( dataP );
        dataP += 2;

		// Calculate the extension length in number of 8-bit words
        TInt extensionLen8 = aExtractParam->TRTP.extension.length * 4;

        // Make sure the extension length is valid
        if ( ( iSize - ( dataP - iData ) ) < extensionLen8 )
            {
            RTP_DEBUG_DETAIL( "RtpPacketProcess: invalid extension length" );

            return ERTCP_PACKET_ERROR;
            }

        if ( !aExtractParam->TRTP.extension.data )
            {
             TRAP( err,
                   aExtractParam->TRTP.extension.data = static_cast<TUint8*>(
                   User::AllocL( aExtractParam->TRTP.extension.length * 4 ) )
                 );
             if(err==KErrNone)
             	{
             	 iExdataAlloc= ETrue; 
             	}     
            }
        if ( err || !aExtractParam->TRTP.extension.data )
            {
            RTP_DEBUG_DETAIL( "RtpPacketProcess: extension missing" );

            return ERTCP_PACKET_ERROR;
            }

        // header extension data
        for ( k = 0; k < aExtractParam->TRTP.extension.length*4; k++ )
            {
            aExtractParam->TRTP.extension.data[k] = dataP[k];
            }
        dataP += ( aExtractParam->TRTP.extension.length * 4 );
        }

    // Make sure there is a payload
    if ( ( iSize - ( dataP - iData ) ) > 0 )
        {
        aExtractParam->TRTP.payloadDataLen = iSize - ( dataP - iData );
        aExtractParam->TRTP.payloadData = static_cast<TText8*>( dataP );
        }
    else
        {
        aExtractParam->TRTP.payloadDataLen = 0;
        aExtractParam->TRTP.payloadData = NULL;
        
        RTP_DEBUG_DETAIL( "RtpPacketProcess: packet length is 0" );
        }
	RTP_DEBUG_PACKET( "CRtpPacket::RtpPacketProcessRtpL Exit" );
    return ERTCP_NO_ERROR;
    }

// ----------------------------------------------------------------------------
// CRtpPacket::RtpPacketProcessRtcp()
// ----------------------------------------------------------------------------
//
TRtpRtcpEnum CRtpPacket::RtpPacketProcessRtcp( 
                             TRtpPacketStreamParam* aStreamParam,
                             TRtpPacketIOParam* aExtractParam )
    {
    // version
    if ( ( ( iDataPtr[0] & 0xC0 ) >> 6 ) != KRtpVersion )
        {
        RTP_DEBUG_DETAIL( "ERROR: Wrong Rtp version in header" );
        return ERTCP_PACKET_ERROR;
        }

    // padding
    if ( iDataPtr[0] & 0x20 )
        {
    	iSize -= iData[iSize - 1];
    	}

    // determine RTCP packet type
    aExtractParam->TRTCP_HEADER.pt =
        static_cast<TRtpPacketType>( iDataPtr[1] );

    if ( ( aExtractParam->TRTCP_HEADER.pt != ERTCP_SR ) &&
	     ( aExtractParam->TRTCP_HEADER.pt != ERTCP_RR ) &&
	     ( aExtractParam->TRTCP_HEADER.pt != ERTCP_SDES ) &&
	     ( aExtractParam->TRTCP_HEADER.pt != ERTCP_BYE ) &&
	     ( aExtractParam->TRTCP_HEADER.pt != ERTCP_APP ) )
        {
        
        RTP_DEBUG_DETAIL( "ERROR: Incorrect packet type in header" );
        
	    return ERTCP_PACKET_ERROR;
        }

    // source/report Count
    aExtractParam->TRTCP_HEADER.sourceCount = ( iDataPtr[0] & 0x1F );

    iDataPtr += 2;
    
    aExtractParam->TRTCP_HEADER.length =
        static_cast<TInt>( Read16( iDataPtr ) );

    if ( aExtractParam->TRTCP_HEADER.length * 4 > iSize )
        {
        RTP_DEBUG_DETAIL( "ERROR: Incorrect packet size in header" );
        return ERTCP_PACKET_ERROR;
        }
    iDataPtr += 2;

    // SSRC of sender in case of RR
    if ( aExtractParam->TRTCP_HEADER.pt == ERTCP_RR )
        {
	    aStreamParam->TRTCP_HEADER.SSRC = Read32( iDataPtr );
	    iDataPtr += 4;
        }
                        
    return ERTCP_NO_ERROR;
    }
        
// ----------------------------------------------------------------------------
// CRtpPacket::RtpPacketProcessApp()
// ----------------------------------------------------------------------------
//
TRtpRtcpEnum CRtpPacket::RtpPacketProcessAppL( 
                             TRtpPacketStreamParam* aStreamParam,
                             TRtpPacketIOParam* aExtractParam )
    {
    TInt err( KErrNone );

    // TRTCP_APP.totalPacketLen is the 32-bit packet length MINUS ONE.
    TUint totalLen( aStreamParam->TRTCP_APP.totalPacketLen );

    // Calculate the length of the data in the packet in number of 8-bit words
    TUint appDataLen8 = ( totalLen + 1 - KMinRtcpAppLen ) * 4;

    // The packet is faulty if the total length of the packet is results in a
    // negative memory allocation . Also, it cannot be too big due to framework
    // restrictions on memory allocation.
    if ( ( totalLen + 1 < KMinRtcpAppLen ) ||
         ( sizeof( TUint8 ) * appDataLen8 > ( KMaxTInt / 2 ) ) )
        {
        return ERTCP_PACKET_ERROR;
        }

    // SSRC
    aStreamParam->TRTCP_APP.SSRC = Read32( iDataPtr );
    iDataPtr += 4;

    // name
    Mem::Copy( aExtractParam->TRTCP_APP.name, iDataPtr, 4 );

    iDataPtr += 4;

    // Allocate memory for application data
    TRAP ( err,
           aExtractParam->TRTCP_APP.appData =
           static_cast<TUint8*>
               (
		       User::AllocL( sizeof( TUint8 ) * appDataLen8 ) )
               );

    if ( err )
        {
    	return ERTCP_PACKET_ERROR; 
    	}

    Mem::Copy( aExtractParam->TRTCP_APP.appData, iDataPtr, appDataLen8 );
    iDataPtr += appDataLen8;
    aExtractParam->TRTCP_APP.appDataLen = appDataLen8;

    return ERTCP_NO_ERROR;
    }
        
// ----------------------------------------------------------------------------
// CRtpPacket::RtpPacketProcessBye()
// ----------------------------------------------------------------------------
//
TRtpRtcpEnum CRtpPacket::RtpPacketProcessByeL( 
                             TRtpPacketStreamParam* aStreamParam,
                             TRtpPacketIOParam* aExtractParam )
    {
    TInt err;
    TUint8 theIndex( 0 );

    // SSRC
    aStreamParam->TRTCP_BYE.SSRC = Read32( iDataPtr );
    iDataPtr += 4;

    // reason
    aExtractParam->TRTCP_BYE.reasonSize = iDataPtr[0];

    iDataPtr += 1;

    TRAP ( err, aExtractParam->TRTCP_BYE.reason = static_cast<TText8*>(
		   User::AllocL( sizeof( TText8 ) *
           ( aExtractParam->TRTCP_BYE.reasonSize + 1 /* trailing NULL */) ) )
         );

    if ( err )
        {
    	return ERTCP_PACKET_ERROR;
    	}

    Mem::Copy( aExtractParam->TRTCP_BYE.reason, iDataPtr,
               aExtractParam->TRTCP_BYE.reasonSize );

    *( aExtractParam->TRTCP_BYE.reason + aExtractParam->TRTCP_BYE.reasonSize )
        = KNullTerm;

    iDataPtr += aExtractParam->TRTCP_BYE.reasonSize;

    // calculate size of padding
    if ( ( 1 + aExtractParam->TRTCP_BYE.reasonSize ) % 4 != 0 )
        {
	    aExtractParam->TRTCP_BYE.paddingSize =
            4 - ( 1 + aExtractParam->TRTCP_BYE.reasonSize ) % 4;
        }
    else
        {
	    aExtractParam->TRTCP_BYE.paddingSize = 0;
        }
                        
    for ( theIndex = 0; theIndex < aExtractParam->TRTCP_BYE.paddingSize;
          theIndex++ )
        {
	    iDataPtr++;
        }
    return ERTCP_NO_ERROR;
    }
        
// ----------------------------------------------------------------------------
// CRtpPacket::RtpPacketProcessRr()
// ----------------------------------------------------------------------------
//
TRtpRtcpEnum CRtpPacket::RtpPacketProcessRr( 
                             TRtpPacketStreamParam* aStreamParam,
                             TRtpPacketIOParam* aExtractParam )
    {
    // SSRC
    aStreamParam->TRTCP_RR.SSRC = Read32( iDataPtr );
    iDataPtr += 4;

    // fraction lost
    aStreamParam->TRTCP_RR.fractionLost = iDataPtr[0];
    iDataPtr++;

    // cumulative number of packets lost
    aStreamParam->TRTCP_RR.cumNumPacketsLost = Read24( iDataPtr );
    iDataPtr += 3;

    // extended highest sequence number received
    aStreamParam->TRTCP_RR.seqNumReceived = Read32( iDataPtr );
    iDataPtr += 4;

    // interarrival jitter
    aStreamParam->TRTCP_RR.arrivalJitter = Read32( iDataPtr );
    iDataPtr += 4;

    // last SR timestamp
    aExtractParam->TRTCP_RR.lastSRTimeStamp = Read32( iDataPtr );
    iDataPtr += 4;

    // delay since last SR timestamp
    aExtractParam->TRTCP_RR.delaySinceLSR = Read32( iDataPtr );
    iDataPtr += 4;

    return ERTCP_NO_ERROR;
    }
        
// ----------------------------------------------------------------------------
// CRtpPacket::RtpPacketProcessSr()
// ----------------------------------------------------------------------------
//
TRtpRtcpEnum CRtpPacket::RtpPacketProcessSr( 
                             TRtpPacketStreamParam* aStreamParam,
                             TRtpPacketIOParam* aExtractParam )
    {
    // SSRC
    aStreamParam->TRTCP_SR.SSRC = Read32( iDataPtr );
    iDataPtr += 4;

    // NTP timestamp (seconds)
    aExtractParam->TRTCP_SR.NTPTimeStampSec = Read32( iDataPtr );
    iDataPtr += 4;

    // NTP timestamp (fraction)
    aExtractParam->TRTCP_SR.NTPTimeStampFrac = Read32( iDataPtr );
    iDataPtr += 4;

    // RTP timestamp
    aExtractParam->TRTCP_SR.timeStamp = Read32( iDataPtr );
    iDataPtr += 4;

    // sender's packet count
    aStreamParam->TRTCP_SR.numPacketsSent = Read32( iDataPtr );
    iDataPtr += 4;

    // sender's octet count
    aStreamParam->TRTCP_SR.cumNumOctetsSent = Read32( iDataPtr );
    iDataPtr += 4;

    return ERTCP_NO_ERROR;
    }
        
// ----------------------------------------------------------------------------
// CRtpPacket::RtpPacketProcessSdes()
// ----------------------------------------------------------------------------
//
TRtpRtcpEnum CRtpPacket::RtpPacketProcessSdesL( 
                             TRtpPacketStreamParam* aStreamParam,
                             TRtpPacketIOParam* aExtractParam )
    {
    TUint8 theIndex( 0 );
    TUint8 tempValue( 0 );
    TInt err( KErrNone );
    TUint8 sdesItems = 0;
    TUint32 totalSDESSize( 0 );

    // SSRC
    aStreamParam->TRTCP_SDES.SSRC = Read32( iDataPtr );
    iDataPtr += 4;
                        
    for ( theIndex = 0; theIndex < ERTCP_NUM_OF_SDES_ITEMS; theIndex++ )
        {
	    aExtractParam->TRTCP_SDES.sdesItemsSize[theIndex] = 0;
        }

    // Read each SDES items
    for ( theIndex = 0; theIndex < ERTCP_NUM_OF_SDES_ITEMS; theIndex++ )
        {
	    if ( iDataPtr[0] != KNullTerm )
	        {
		    tempValue = iDataPtr[0];

		    if ( tempValue != ( theIndex + 1 ) )
		        {
			    aExtractParam->TRTCP_SDES.sdesItemsSize[theIndex] = 0;
			    TRAP( err, aExtractParam->TRTCP_SDES.sdesItems[theIndex] = 
					static_cast<TUint8*>( User::AllocL( sizeof( TUint8 ) ) ) );
			    if ( err )
			        { 
    				return ERTCP_PACKET_ERROR;
    				}

			    *( aExtractParam->TRTCP_SDES.sdesItems[theIndex] ) = KNullTerm;
		        }
		    else
		        {
			    aExtractParam->TRTCP_SDES.sdesItemsSize[theIndex] = iDataPtr[1];

			    // keep track of number of SDES items
			    sdesItems++;
			    totalSDESSize +=
                    aExtractParam->TRTCP_SDES.sdesItemsSize[theIndex];

			    iDataPtr += 2;

			    TRAP( err, aExtractParam->TRTCP_SDES.sdesItems[theIndex] =
                        static_cast<TUint8*>(
					    User::AllocL( sizeof( TUint8 ) * (
                        aExtractParam->TRTCP_SDES.sdesItemsSize[theIndex] + 1 )
                        ) ) );

			    if ( err )
			        {
				    return ERTCP_PACKET_ERROR;
				    }
                        
			    Mem::Copy( aExtractParam->TRTCP_SDES.sdesItems[theIndex],
                           iDataPtr,
                           aExtractParam->TRTCP_SDES.sdesItemsSize[theIndex] );
        
			    *( aExtractParam->TRTCP_SDES.sdesItems[theIndex] +
                aExtractParam->TRTCP_SDES.sdesItemsSize[theIndex] ) = KNullTerm;
    
			    iDataPtr += aExtractParam->TRTCP_SDES.sdesItemsSize[theIndex];
		        }
	        }
	    }


    iDataPtr++;    // take into account end null octet

    for ( theIndex = tempValue; theIndex < ERTCP_NUM_OF_SDES_ITEMS; theIndex++ )
        {

	    TRAP( err, aExtractParam->TRTCP_SDES.sdesItems[theIndex] =
                   static_cast<TUint8*>( User::AllocL( sizeof( TUint8 ) ) )
            );

	    if ( err )
	        {
    		return ERTCP_PACKET_ERROR;
    		}

	    *( aExtractParam->TRTCP_SDES.sdesItems[theIndex] ) = KNullTerm;
		  }

    // calculate size of padding
    if ( ( sdesItems * 2 + totalSDESSize + 1 ) % 4 != 0 )
        {
    	aExtractParam->TRTCP_SDES.paddingSize =
            4 - ( sdesItems * 2 + totalSDESSize + 1 ) % 4;
    	}
    else
        {
    	aExtractParam->TRTCP_SDES.paddingSize = 0;
    	}

    for ( theIndex = 0; theIndex < aExtractParam->TRTCP_SDES.paddingSize;
          theIndex++ )
        {
    	iDataPtr++;
    	}
    return ERTCP_NO_ERROR;
    }
    
// ---------------------------------------------------------------------------
// Return Value:
//     0  : if RTP packet has been processed OK or
//          if RTCP packet has been processed OK and there are no more RTCP
//          packets in this compound packet to be processed
//     1  : if RTCP packet has been processed OK and there is still RTCP
//          packets in this compound packet to be processed
//    -1  : if packet is invalid or some other error has occured
// ---------------------------------------------------------------------------
TRtpRtcpEnum CRtpPacket::RtpPacketProcessL( TRtpPacketStreamParam* aStreamParam,
                                           TRtpPacketIOParam* aExtractParam )
    {
    TRtpRtcpEnum result( ERTCP_NO_ERROR );

    // parse the packet content
    switch ( iType )
        {
        case ERTP:
            result = RtpPacketProcessRtpL( aStreamParam, aExtractParam );
            return result;
            // no break necessary

        case ERTCP_HEADER:
            result = RtpPacketProcessRtcp( aStreamParam, aExtractParam );
            break;

        case ERTCP_SR:
            result = RtpPacketProcessSr( aStreamParam, aExtractParam );
            break;

        case ERTCP_RR:
            result = RtpPacketProcessRr( aStreamParam, aExtractParam );
            break;

        case ERTCP_SDES:
            result = RtpPacketProcessSdesL( aStreamParam, aExtractParam );
            break;

        case ERTCP_BYE:
            result = RtpPacketProcessByeL( aStreamParam, aExtractParam );
            break;

        case ERTCP_APP:
            result = RtpPacketProcessAppL( aStreamParam, aExtractParam );
            break;

        default:
            // unknown packet type, return error
            result = ERTCP_PACKET_ERROR;
            break;
        }
       
        // Only RTCP packets get this far
        if ( result != ERTCP_NO_ERROR )
            {
            return result;
            }
 
        // check if RTCP compound is ready
        if ( iDataPtr == ( iData + iSize ) )
            {
            iDataPtr = iData;
            return ERTCP_NO_ERROR;
            }
        else if ( iDataPtr < ( iData + iSize ) )
            {
            return ERTCP_PACKET_MORE;
            }
        else
            {
            return ERTCP_PACKET_ERROR;
            }
    }

// ---------------------------------------------------------------------------
// CRtpPacket::Write16()
// Write a 16-bit value as 2 consecutive bytes in MSB order
// Memory (at least 2 bytes) must have been allocated to pointer
// before the function is called.
// ---------------------------------------------------------------------------
void CRtpPacket::Write16( TUint8* const aPointer, TUint16 aValue )
    {
    // check value range (16 bits)
    aPointer[0] = static_cast<TUint8>( ( aValue & 0xFF00 ) >> 8 );
    aPointer[1] = static_cast<TUint8>( aValue & 0x00FF );
    }

// ---------------------------------------------------------------------------
// TUint16 CRtpPacket::Read16()
// Read a 16-bit value given as 2 consecutive bytes in MSB order
// Memory (at least 2 bytes) must have been allocated to pointer
// before the function is called.
// ---------------------------------------------------------------------------
TUint16 CRtpPacket::Read16( const TUint8* const aPointer )
    {
    return static_cast<TUint16>( aPointer[1] + ( aPointer[0] << 8 ) );
    }

// ---------------------------------------------------------------------------
// CRtpPacket::Write32()
// Write a 32-bit aValue as 4 consecutive bytes in MSB order
// Memory (at least 4 bytes) must have been allocated to aPointer
// before the function is called.
// ---------------------------------------------------------------------------
void CRtpPacket::Write32( TUint8* const aPointer, TUint32 aValue )
    {
    aPointer[0] = static_cast<TUint8>( ( aValue & 0xFF000000 ) >> 24 );
    aPointer[1] = static_cast<TUint8>( ( aValue & 0x00FF0000 ) >> 16 );
    aPointer[2] = static_cast<TUint8>( ( aValue & 0x0000FF00 ) >> 8 );
    aPointer[3] = static_cast<TUint8>( aValue & 0x000000FF );
    }

// ---------------------------------------------------------------------------
// TUint32 CRtpPacket::Read32()
// Read a 32-bit aValue given as 4 consecutive bytes in MSB order
// Memory (at least 4 bytes) must have been allocated to aPointer
// before the function is called.
// ---------------------------------------------------------------------------
TUint32 CRtpPacket::Read32( const TUint8* const aPointer )
    {
    return ( aPointer[3] +
             ( static_cast<TUint32>( aPointer[2] ) << 8 ) +
             ( static_cast<TUint32>( aPointer[1] ) << 16 ) +
             ( static_cast<TUint32>( aPointer[0] ) << 24 ) );
    }

// ---------------------------------------------------------------------------
// CRtpPacket::Write24()
// 
// ---------------------------------------------------------------------------
//
void CRtpPacket::Write24( TUint8* const aPointer, TUint32 aValue )
    {
    aPointer[0] = static_cast<TUint8>( ( aValue & 0xFF0000 ) >> 16 );
    aPointer[1] = static_cast<TUint8>( ( aValue & 0x00FF00 ) >> 8 );
    aPointer[2] = static_cast<TUint8>( aValue & 0x0000FF );
    }

// ---------------------------------------------------------------------------
// TUint32 CRtpPacket::Read24()
// 
// ---------------------------------------------------------------------------
//
TUint32 CRtpPacket::Read24( const TUint8 *const aPointer )
    {
    return ( aPointer[2] + ( static_cast<TUint32>( aPointer[1] ) << 8 ) +
             ( static_cast<TUint32>( aPointer[0] ) << 16 ) );
    }


//  End of File
