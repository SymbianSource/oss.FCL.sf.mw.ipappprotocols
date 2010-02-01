/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    "Uninitialized" state for outgoing SRTP stream.
*
*/




#ifndef TSRTPSTREAMOUTSTATEUNINIT_H
#define TSRTPSTREAMOUTSTATEUNINIT_H

// INCLUDES
#include "tsrtpstreamoutstatebase.h"

// FORWARD DECLARATIONS
class MSRTPCryptoHandlerContext;

// CLASS DECLARATION
class TSRTPStreamOutStateUninit : public TSRTPStreamOutStateBase
	{
	public:	// Constructors

		TSRTPStreamOutStateUninit(MSRTPStreamOutContext& aStreamContext,
		    MSRTPCryptoHandlerContextRTP& aCryptoHandlerRTPContext,
		    MSRTPCryptoHandlerContextRTCP& aCryptoHandlerRTCPContext);

	public: // From TSRTPStreamOutStateBase

        /**
        * Protect plain RTP packet in "Uninitialized" state.
        * @param aPacket plain RTP packet
        * @leave KErrNone if success, system-wide error code otherwise        
        * @return HBufC8* encrypted packet
        */                              
        HBufC8* DoProtectRtpL(const TDesC8& aPacket);
        
        /**
        * Protect plain RTCP packet in "Uninitialized" state.
        * @param aPacket plain RTCP packet
        * @leave KErrNone if success, system-wide error code otherwise        
        * @return HBufC8* encrypted packet
        */                              
        HBufC8* DoProtectRtcpL(const TDesC8& aPacket);

	private: // Constructors

		/// Default constructor. Not implemented.
		TSRTPStreamOutStateUninit();
	};

#endif // TSRTPStreamOutStateUninit_H
