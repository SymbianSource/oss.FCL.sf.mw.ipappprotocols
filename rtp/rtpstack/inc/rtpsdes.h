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




#ifndef __RTPSDES_H
#define __RTPSDES_H

//  INCLUDES
#include <e32base.h>
#include <rtpheader.h>

// DATA TYPES
enum TRtcpSdesType
    {
    ERTCP_SDES_CNAME        = 0,
    ERTCP_SDES_NAME         = 1,
    ERTCP_SDES_EMAIL        = 2,
    ERTCP_SDES_PHONE        = 3,
    ERTCP_SDES_LOC          = 4,
    ERTCP_SDES_TOOL         = 5,
    ERTCP_SDES_NOTE         = 6,
    ERTCP_SDES_PRIV         = 7,
    ERTCP_NUM_OF_SDES_ITEMS = 8
    } ;

// FORWARD DECLARATIONS
class TRtpSdesParams;

/**
*  RTCP SDES class. 
*
*  @lib RtpService.dll
*/
class CRtpSDES : public CBase
    {
    public:
        static CRtpSDES* NewL();
        static CRtpSDES* NewL( const TRtpSdesParams& aSDES );

        ~CRtpSDES();

        void GetSDES( TRtpSdesParams& aSDES );
        void SetSDES( const TRtpSdesParams& aSDES );
        void AddSsrcToSdesL( const TRtpSSRC aSSRC );
        TBool GetSdesSsrc( TRtpSSRC aSSRC );
        
    private:
        CRtpSDES();
        void ConstructL();
        void ConstructL( const TRtpSdesParams& aSDES );

    public:
        HBufC8* iSDESBuf[ERTCP_NUM_OF_SDES_ITEMS];
        TUint8* iSDESItems[ERTCP_NUM_OF_SDES_ITEMS];
        TInt iSDESItemsSize[ERTCP_NUM_OF_SDES_ITEMS];
        CArrayFixFlat<TRtpSSRC>* iSndSSRCArray; //array of SSRCs comming from this SDES participant
    };

#endif // __RTPSDES_H

// End of File
