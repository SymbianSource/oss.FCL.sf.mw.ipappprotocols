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
* Description:    Base class for outgoing SRTP stream states
*
*/





#ifndef TSRTPSTREAMOUTSTATEBASE_H
#define TSRTPSTREAMOUTSTATEBASE_H

// INCLUDES
#include <e32std.h>
#include "tsrtpstreamstatebase.h"

// FORWARD DECLARATIONS
class MSRTPStreamOutContext;
class MSRTPCryptoHandlerContext;
class MSRTCPCryptoHandlerContext;

// CLASS DECLARATION
class TSRTPStreamOutStateBase : public TSRTPStreamStateBase
	{
	protected:	// Constructors and destructor

		TSRTPStreamOutStateBase(MSRTPStreamOutContext& aStreamContext,
		    MSRTPCryptoHandlerContextRTP& aCryptoHandlerRTPContext,
		    MSRTPCryptoHandlerContextRTCP& aCryptoHandlerRTCPContext);

	public: // New methods
        /**
        * virtual function for protecting plain RTP packet.
        * implemented in substates of TSRTPStreamOutStateBase.
        * @param aPacket plain RTP packet
        * @leave KErrNone if success, system-wide error code otherwise        
        * @return HBufC8* encrypted packet
        */                              	
		virtual HBufC8* DoProtectRtpL(const TDesC8& aPacket)=0;

        /**
        * virtual function for protecting plain RTCP packet.
        * implemented in substates of TSRTPStreamOutStateBase.
        * @param aPacket plain RTCP packet
        * @leave KErrNone if success, system-wide error code otherwise        
        * @return HBufC8* encrypted packet
        */                              	
		virtual HBufC8* DoProtectRtcpL(const TDesC8& aPacket)=0;

	private: // Constructors
		/// Default constructor. Not implemented.
		TSRTPStreamOutStateBase();

	protected: // Data
		/* 
	    * interface for a transmitting stream for changing state
	    */
		MSRTPStreamOutContext& iStreamContext;
	};

#endif // TSRTPStreamOutStateBase_H
