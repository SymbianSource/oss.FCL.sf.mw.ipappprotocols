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
* Description:    Base class for SRTP stream states
*
*/




#ifndef TSRTPSTREAMSTATEBASE_H
#define TSRTPSTREAMSTATEBASE_H

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class MSRTPCryptoHandlerContext;
class MSRTPCryptoHandlerContextRTP;
class MSRTPCryptoHandlerContextRTCP;

// CLASS DECLARATION
class TSRTPStreamStateBase
	{
	protected:	// Constructors and destructor

		TSRTPStreamStateBase(MSRTPCryptoHandlerContextRTP& aCryptoHandlerRTPContext,
		                MSRTPCryptoHandlerContextRTCP& aCryptoHandlerRTCPContext);
		
	public: // Constructors
		/// Default constructor. Not implemented.
		TSRTPStreamStateBase();

	protected: // Data
	    /*
	    * interface for this incoming or outgoing stream to 
	    * RTP-specific crypto handling operations
	    */
		MSRTPCryptoHandlerContextRTP& iCryptoHandlerRTPContext;
	    /*
	    * interface for this incoming or outgoing stream to 
	    * RTCP-specific crypto handling operations
	    */
		MSRTPCryptoHandlerContextRTCP& iCryptoHandlerRTCPContext;		
	};

#endif // TSRTPStreamStateBase_H
