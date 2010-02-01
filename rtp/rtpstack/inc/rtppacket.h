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




#ifndef __RTPPACKET_H
#define __RTPPACKET_H

//  INCLUDES
#include <e32def.h>
#include "rtppacketparam.h"
#include <rtpheader.h>
#include "rtputil.h"

// CLASS DECLARATION

/**
*  The class for RTP packet building and processing. 
*
*  @lib RtpService.dll
*/
class CRtpPacket : public CBase
    {
    #if defined( _DEBUG ) && defined( RTP_UNIT_TEST )
	    //Friend class used for testing
	    friend class CRtpUtRtpPacket; 
	    friend class CRtpUtRtpStream; 
    #endif 
    
    public:
        static CRtpPacket* NewL( TUint aPacketSize, const TUint32* aRtpTimeRates );

        ~CRtpPacket();

        void RtpPacketReset();  
        void RtpPacketResetPtr();

        TRtpSSRC RtpPacketGetSSRC();   
        TRtpPayloadType RtpPacketGetPayloadType();  

        TInt RtpPacketBuild( TRtpPacketStreamParam* aStreamParam, 
                             TRtpPacketIOParam* aInitParam );
        TRtpRtcpEnum RtpPacketProcessL( TRtpPacketStreamParam* aStreamParam, 
                                       TRtpPacketIOParam* aExtractParam );
                                       
//        TInt RtpPacketBuildNonRtp( const TDesC8& aData, TUint aMaxPacketLength);

        inline void SetSize( TInt aSize );
        inline void SetType( TRtpPacketType aType );

        inline TInt Size() const;
        inline TUint8* Data();
        inline HBufC8* GetHBuf();
        inline TRtpPacketType Type() const;
        inline const TPtrC8 Des() const;

    private:
        TInt RtpPacketBuildRtp( TRtpPacketStreamParam* aStreamParam, 
                                TRtpPacketIOParam* aInitParam );

        TInt RtpPacketBuildRtcp( TRtpPacketStreamParam* aStreamParam, 
                                 TRtpPacketIOParam* aInitParam );

        TInt RtpPacketBuildApp( TRtpPacketStreamParam* aStreamParam, 
                                TRtpPacketIOParam* aInitParam );

        TInt RtpPacketBuildBye( TRtpPacketStreamParam* aStreamParam, 
                                TRtpPacketIOParam* aInitParam );

        TInt RtpPacketBuildRr( TRtpPacketStreamParam* aStreamParam, 
                               TRtpPacketIOParam* aInitParam );

        TInt RtpPacketBuildSr( TRtpPacketStreamParam* aStreamParam, 
                               TRtpPacketIOParam* aInitParam );

        TInt RtpPacketBuildSdes( TRtpPacketStreamParam* aStreamParam, 
                                 TRtpPacketIOParam* aInitParam );

        TRtpRtcpEnum RtpPacketProcessRtpL( TRtpPacketStreamParam* aStreamParam,
                                          TRtpPacketIOParam* aExtractParam );
        TRtpRtcpEnum RtpPacketProcessRtcp( TRtpPacketStreamParam* aStreamParam,
                                          TRtpPacketIOParam* aExtractParam );
        TRtpRtcpEnum RtpPacketProcessAppL( TRtpPacketStreamParam* aStreamParam,
                                          TRtpPacketIOParam* aExtractParam );
        TRtpRtcpEnum RtpPacketProcessByeL( TRtpPacketStreamParam* aStreamParam,
                                          TRtpPacketIOParam* aExtractParam );
        TRtpRtcpEnum RtpPacketProcessRr( TRtpPacketStreamParam* aStreamParam,
                                         TRtpPacketIOParam* aExtractParam );
        TRtpRtcpEnum RtpPacketProcessSr( TRtpPacketStreamParam* aStreamParam,
                                         TRtpPacketIOParam* aExtractParam );
        TRtpRtcpEnum RtpPacketProcessSdesL( TRtpPacketStreamParam* aStreamParam,
                                          TRtpPacketIOParam* aExtractParam );

        CRtpPacket( const TUint32* aRtpTimeRates );
        void ConstructL( TUint aPacketSize );

        void Write16( TUint8* const aPointer, TUint16 aValue );
        TUint16 Read16( const TUint8* const aPointer );
        void Write24( TUint8* const aPointer, TUint32 aValue );
        TUint32 Read24( const TUint8* const aPointer );
        void Write32( TUint8 *const aPointer, TUint32 aValue );
        TUint32 Read32( const TUint8* const aPointer );

	public:
		TBool iExdataAlloc;
		TBool iCsrcAlloc;
		TRtpPacketIOParam* iExtractParam;
    private:
        HBufC8* iBuf;
        TInt iSize;   
        TUint8* iData;          // pointer to the beginning of the iBuf
        TUint8* iDataPtr;       // pointer to the beginning of date unhandled
        TRtpPacketType iType;   // an enum for packet type
        const TUint32* iProfileRTPTimeRates; //pointer to 
    private:    
    #ifdef EUNIT_TESTING
	  friend class UT_CRtpPacket;
	  friend class UT_CRtpStream;
	  friend class UT_CRtpSession;
	  friend class UT_CRtpTranStream;
	#endif     	    
    };

#include "rtppacket.inl"

#endif  // __RTPPACKET_H

// End of File
