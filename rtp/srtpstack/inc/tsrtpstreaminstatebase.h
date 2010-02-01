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
* Description:    Base class for incoming SRTP stream states
*
*/




#ifndef TSRTPSTREAMINSTATEBASE_H
#define TSRTPSTREAMINSTATEBASE_H

// INCLUDES
#include <e32std.h>
#include "tsrtpstreamstatebase.h"

// FORWARD DECLARATIONS
class MSRTPStreamInContext;
class MSRTPCryptoHandlerContext;
class MSRTPCryptoHandlerContextRTP;
class MSRTPCryptoHandlerContextRTCP;

// CLASS DECLARATION
class TSRTPStreamInStateBase : public TSRTPStreamStateBase
	{
	protected:	// Constructors and destructor

		TSRTPStreamInStateBase(MSRTPStreamInContext& aStreamContext,
		    MSRTPCryptoHandlerContextRTP& aCryptoHandlerRTPContext,
		    MSRTPCryptoHandlerContextRTCP& aCryptoHandlerRTCPContext);

	public: // New methods
        /**
        * virtual function for unprotecting encrypted RTP packet.
        * implemented in substates of TSRTPStreamInStateBase.
        * @param aPacket encrypted packet
        * @leave KErrNone if success, system-wide error code otherwise        
        * @return HBufC8* decrypted packet
        */                              
		virtual HBufC8* DoUnprotectSrtpL(const TDesC8& aPacket)=0;

        /**
        * virtual function for unprotecting encrypted RTCP packet.
        * implemented in substates of TSRTPStreamInStateBase.
        * @param aPacket encrypted packet
        * @leave KErrNone if success, system-wide error code otherwise        
        * @return HBufC8* decrypted packet
        */                              
		virtual HBufC8* DoUnprotectSrtcpL(const TDesC8& aPacket)=0;
		
	private: // Constructors
		/// Default constructor. Not implemented.
		TSRTPStreamInStateBase();

	protected: // Data
	    /* 
	    * interface for a receiver stream for changing state
	    */
		MSRTPStreamInContext& iStreamContext;
		
	};

#endif // TSRTPStreamInStateBase_H
