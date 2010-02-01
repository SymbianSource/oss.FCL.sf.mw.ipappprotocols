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




// INCLUDES
#include <rtpdef.h>

#include "rtputil.h"
#include "rtpsdes.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CRtpSDES::CRtpSDES() 
    {
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CRtpSDES::ConstructL()
    {
    for ( TInt k = 0; k < ERTCP_NUM_OF_SDES_ITEMS; k++ )
        {
        iSDESBuf[k] = HBufC8::NewL( KMaxSdesItemSize );
        iSDESItems[k] = const_cast<TUint8*>( iSDESBuf[k]->Des().Ptr() );
        Mem::FillZ( iSDESItems[k], KMaxSdesItemSize );
        iSDESItemsSize[k] = 0;
        }

    iSndSSRCArray = new ( ELeave ) CArrayFixFlat<TRtpSSRC>( 4 );
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CRtpSDES* CRtpSDES::NewL()
    {
    CRtpSDES* self = new ( ELeave ) CRtpSDES();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CRtpSDES* CRtpSDES::NewL( const TRtpSdesParams& aSDES )
    {
    CRtpSDES* self = new ( ELeave ) CRtpSDES();
    CleanupStack::PushL( self );
    self->ConstructL();
    self->SetSDES( aSDES );
    CleanupStack::Pop(); // self
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CRtpSDES::~CRtpSDES()
    {
    for ( TInt k = 0; k < ERTCP_NUM_OF_SDES_ITEMS; k++ )
        {
        delete iSDESBuf[k];
        iSDESItemsSize[k] = 0;
        }
	if (iSndSSRCArray)
		{
    	iSndSSRCArray->Reset();
    	delete iSndSSRCArray;
    	}
    }

// ---------------------------------------------------------------------------
// CRtpSDES::SetSDES()
// 
// ---------------------------------------------------------------------------
//
void CRtpSDES::SetSDES( const TRtpSdesParams& aSDES )
    {

    iSDESItemsSize[ERTCP_SDES_CNAME] = ( aSDES.iCName ).Length();
    Mem::Copy( iSDESItems[ERTCP_SDES_CNAME], ( aSDES.iCName ).Ptr(),
               iSDESItemsSize[ERTCP_SDES_CNAME] );
    *( iSDESItems[ERTCP_SDES_CNAME] + iSDESItemsSize[ERTCP_SDES_CNAME] ) = '\0';

    iSDESItemsSize[ERTCP_SDES_NAME] = ( aSDES.iUserName ).Length();
    Mem::Copy( iSDESItems[ERTCP_SDES_NAME], ( aSDES.iUserName ).Ptr(),
               iSDESItemsSize[ERTCP_SDES_NAME] );
    *( iSDESItems[ERTCP_SDES_NAME] + iSDESItemsSize[ERTCP_SDES_NAME] ) = '\0';

    iSDESItemsSize[ERTCP_SDES_EMAIL] = ( aSDES.iEmail ).Length();
    Mem::Copy( iSDESItems[ERTCP_SDES_EMAIL], ( aSDES.iEmail ).Ptr(),
               iSDESItemsSize[ERTCP_SDES_EMAIL] );
    *( iSDESItems[ERTCP_SDES_EMAIL] + iSDESItemsSize[ERTCP_SDES_EMAIL] ) = '\0';

    iSDESItemsSize[ERTCP_SDES_PHONE] = ( aSDES.iPhoneNumber ).Length();
    Mem::Copy( iSDESItems[ERTCP_SDES_PHONE], ( aSDES.iPhoneNumber ).Ptr(),
               iSDESItemsSize[ERTCP_SDES_PHONE] );
    *( iSDESItems[ERTCP_SDES_PHONE] + iSDESItemsSize[ERTCP_SDES_PHONE] ) = '\0';

    iSDESItemsSize[ERTCP_SDES_LOC] = ( aSDES.iLocation ).Length();
    Mem::Copy( iSDESItems[ERTCP_SDES_LOC], ( aSDES.iLocation ).Ptr(),
               iSDESItemsSize[ERTCP_SDES_LOC] );
    *( iSDESItems[ERTCP_SDES_LOC] + iSDESItemsSize[ERTCP_SDES_LOC] ) = '\0';

    iSDESItemsSize[ERTCP_SDES_TOOL] = ( aSDES.iSwToolName ).Length();
    Mem::Copy( iSDESItems[ERTCP_SDES_TOOL], ( aSDES.iSwToolName ).Ptr(),
               iSDESItemsSize[ERTCP_SDES_TOOL] );
    *( iSDESItems[ERTCP_SDES_TOOL] + iSDESItemsSize[ERTCP_SDES_TOOL] ) = '\0';

    iSDESItemsSize[ERTCP_SDES_NOTE] = ( aSDES.iNoticeStatus ).Length();
    Mem::Copy( iSDESItems[ERTCP_SDES_NOTE], ( aSDES.iNoticeStatus ).Ptr(),
               iSDESItemsSize[ERTCP_SDES_NOTE] );
    *( iSDESItems[ERTCP_SDES_NOTE] + iSDESItemsSize[ERTCP_SDES_NOTE] ) = '\0';

    iSDESItemsSize[ERTCP_SDES_PRIV] = ( aSDES.iPrivate ).Length();
    Mem::Copy( iSDESItems[ERTCP_SDES_PRIV], ( aSDES.iPrivate ).Ptr(),
               iSDESItemsSize[ERTCP_SDES_PRIV] );
    *( iSDESItems[ERTCP_SDES_PRIV] + iSDESItemsSize[ERTCP_SDES_PRIV] ) = '\0';

    }


// ---------------------------------------------------------------------------
// CRtpSDES::GetSDES()
// 
// ---------------------------------------------------------------------------
//
void CRtpSDES::GetSDES( TRtpSdesParams& aSDES )
    {
    ( aSDES.iCName ).Set( iSDESItems[ERTCP_SDES_CNAME],
                          iSDESItemsSize[ERTCP_SDES_CNAME] );

    ( aSDES.iUserName ).Set( iSDESItems[ERTCP_SDES_NAME],
                             iSDESItemsSize[ERTCP_SDES_NAME] );

    ( aSDES.iEmail ).Set( iSDESItems[ERTCP_SDES_EMAIL],
                          iSDESItemsSize[ERTCP_SDES_EMAIL] );

    ( aSDES.iPhoneNumber).Set( iSDESItems[ERTCP_SDES_PHONE],
                               iSDESItemsSize[ERTCP_SDES_PHONE] );

    ( aSDES.iLocation ).Set( iSDESItems[ERTCP_SDES_LOC],
                             iSDESItemsSize[ERTCP_SDES_LOC] );

    ( aSDES.iSwToolName ).Set( iSDESItems[ERTCP_SDES_TOOL],
                               iSDESItemsSize[ERTCP_SDES_TOOL] );

    ( aSDES.iNoticeStatus ).Set( iSDESItems[ERTCP_SDES_NOTE], iSDESItemsSize[ERTCP_SDES_NOTE] );

    ( aSDES.iPrivate ).Set( iSDESItems[ERTCP_SDES_PRIV], iSDESItemsSize[ERTCP_SDES_PRIV] );
    }

// ---------------------------------------------------------------------------
// CRtpSDES::AddSsrcToSdesL()
// 
// ---------------------------------------------------------------------------
//
void CRtpSDES::AddSsrcToSdesL( const TRtpSSRC aSSRC )
    {
    TInt index = 0;
    TBool fFoundSSRC = EFalse;
    TInt totalSSRC = iSndSSRCArray->Count();

    if ( totalSSRC > 0 )
        {
        for ( index = 0; index < totalSSRC; index++ )
            {
            if ( iSndSSRCArray->At( index ) == aSSRC )
                {
                fFoundSSRC = ETrue;
                }
            }
        if ( !fFoundSSRC )
            {
            iSndSSRCArray->AppendL( aSSRC );
            }
        }
    else  //first SSRC, added to array
        {
        iSndSSRCArray->AppendL( aSSRC );
        }
    }

// ---------------------------------------------------------------------------
// TBool CRtpSDES::GetSdesSsrc()
// 
// ---------------------------------------------------------------------------
//
TBool CRtpSDES::GetSdesSsrc( TRtpSSRC aSSRC )
    {
    TInt totalSSRC = iSndSSRCArray->Count();

    for ( TInt index = 0; index < totalSSRC; index++ )
        {
        if ( iSndSSRCArray->At( index ) == aSSRC )
            {
            return ETrue;
            }
        }
    return EFalse;
    }


//  End of File
