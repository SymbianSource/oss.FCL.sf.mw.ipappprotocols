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
#include <e32base.h>
#include <e32math.h>
#include <rtpheader.h>
#include "rtputil.h"
#include "rtpstream.h"
#include "rtpsdes.h"
#include "rtpcomm.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CRtpStream::CRtpStream( const TRtpId aReceiveStreamId,
                        const TRtpId aSessionId,
                        const TUint32* aProfileRTPTimeRates,
                        MRtcpObserver* aRtcpObserver,
                        const TRtpPayloadType aPayloadType ) 
    : iFlagFirstPkg( ETrue ),
    iStreamId( aReceiveStreamId ),
    iSessionId( aSessionId ),
    iSeqNumCycles( 0 ),
    iPayload( aPayloadType ),
    iProfileRTPTimeRates( aProfileRTPTimeRates ),
    iRtcpObserver( aRtcpObserver )
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CRtpStream::~CRtpStream()
    {
    }

// End of File
