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

#ifndef __MMMSRPWRITER_H
#define __MMMSRPWRITER_H

#include <e32base.h>
#include <in_sock.h>
#include "MMSRPWriterObserver.h"

// CLASS DECLARATIONS
/**
* writer request Interface 
*/
class MMSRPWriter
	{
	public:
	    inline virtual ~MMSRPWriter(){}
	    /**
	     * register for send service
	     * also triggers writer start, inactive on nothing to send
	     */	    
	    virtual void RequestSendL(MMSRPWriterObserver& aMsg) = 0;

        /**
         * Cancel current send, socket state to be verified
         */
        //virtual void CancelWrite() = 0; //not supported, only cancel message supported, i.e. chunk abort
        
        /**
         * Stops the writer from sending data
         */
        //virtual void Stop() = 0;
        //virtual void Start() = 0;
        
        /**
         * Stops sending and discards all pending data
         */
        //virtual void Flush() = 0;                
        
	};
#endif // __MMMSRPWRITER_H

// End of file
