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

#ifndef __MMMSRPREADER_H
#define __MMMSRPREADER_H

// INCLUDES
//#include "MMSRPReaderObserver.h"
#include "MSRPBuffer.h"

// CLASS DECLARATIONS

/**
 * The socket reader request interface.
 */
class MMSRPReader
	{
	public:
	    
	    inline virtual ~MMSRPReader(){}

        /**
         * issue socket recv and set reader active  
         */
        virtual void StartReceivingL(RMsrpBuf aBuf) = 0;

        /**
         * Cancel receive
         */
        //virtual void CancelReceiving() = 0;
              
	};

#endif // __MMMSRPREADER_H

// End of file
