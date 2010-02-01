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
* Description:    "Normal" state for incoming SRTP stream.
*
*/




#ifndef TSRTPSTREAMINSTATENORMAL_H
#define TSRTPSTREAMINSTATENORMAL_H

// INCLUDES
#include "tsrtpstreaminstatebase.h"

// FORWARD DECLARATIONS
class MSRTPCryptoHandlerContext;
class MSRTPCryptoHandlerContextRTP;

// CLASS DECLARATION
class TSRTPStreamInStateNormal : public TSRTPStreamInStateBase
	{
	public:	// Constructors

		TSRTPStreamInStateNormal(MSRTPStreamInContext& aStreamContext,
		    MSRTPCryptoHandlerContextRTP& aCryptoHandlerRTPContext,
		    MSRTPCryptoHandlerContextRTCP& aCryptoHandlerRTCPContext);		

	public: // From TSRTPStreamInStateBase
		
        /**
        * Unprotect encrypted RTP packet in "normal" state.
        * @param aPacket encrypted packet
        * @leave KErrNone if success, system-wide error code otherwise        
        * @return HBufC8* decrypted packet
        */                              
        HBufC8* DoUnprotectSrtpL(const TDesC8& aPacket);

        /**
        * Unprotect encrypted RTCP packet in "normal" state.
        * @param aPacket encrypted packet
        * @leave KErrNone if success, system-wide error code otherwise        
        * @return HBufC8* decrypted packet
        */                              
        HBufC8* DoUnprotectSrtcpL(const TDesC8& aPacket);

	private: // Constructors

		/// Default constructor. Not implemented.
		TSRTPStreamInStateNormal();
	};

#endif // TSRTPStreamInStateNormal_H
