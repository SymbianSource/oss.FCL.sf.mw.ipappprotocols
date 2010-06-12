/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html."
* Initial Contributors:
* Nokia Corporation - initial contribution.
* Contributors:
*
* Description:
* MSRP Implementation
*
*/

#ifndef __MMMSRPREADEROBSERVER_H
#define __MMMSRPREADEROBSERVER_H

// INCLUDES
#include "MSRPBuffer.h"


// CLASS DECLARATIONS

/**
 * Subsession unknown,has to be identified after parsing
 * Hence the observer split as in writer not seen
 * Buf passed to parser thru connection rather than directly 
 * For direct passing to parser, split observers
 */

/**
* The socket reader notifier interface.
* Notifies about events in reader.
* 
*/


class MMSRPReaderObserver
	{
	public:

        /**
        * Called when the message has been read from the socket or on error        
		* @return processing status
        */

        virtual void ReadStatusL(RMsrpBuf& aBuf, TInt aStatus) = 0;
        
	};

#endif // __MMMSRPREADEROBSERVER_H

// End of file
