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

#ifndef __MMMSRPPARSEROBSERVER_H
#define __MMMSRPPARSEROBSERVER_H

// INCLUDES
#include <e32base.h>

/**
 * ideally msg received shud directly go to msrp session mngr(subsession mngr)
 * msgs unclaimed by any session shud be handled by thesession mngr
 * xpected dummy subsession to handle or send responses to unclaimed msgs  
 */

class CMSRPMessageHandler;


// CLASS DECLARATIONS
/**
* The parser event interface
*/
class MMSRPParserObserver
	{
	public: // enums
        /** Error codes returned to the client */
        enum TParseStatusResponses
            {
            EParseStatusNotDefined,
            EParseStatusError,
            EParseStatusMessageUnclaimed,
            EParseStatusMessageHandled
            };
	
	public:

	    /**
	     * Message, Report, Response
	     * @param aMsg received message
	     * @param aStatus message status
	     * @return TParseStatusResponses 
	     */
        virtual TInt ParseStatusL (CMSRPMessageHandler* aMsg, TInt aStatus) = 0;
	    
        /**
         * Report currently received bytes 
         * @param aMsg message handler instance 
         */
        virtual void ReportReceiveprogressL( CMSRPMessageHandler* aMsg ) = 0; 
	    
	    /**
	     * Parse error: Handle similar to connection error now
	     * Try to recover from parse errors, using byte-range etc. later 
	     */
	    //virtual void ParseResult (TInt aStatus, CMessage* aMsg) = 0;
        //virtual void ParseError () = 0;
	    
        /**
	     * Part of large chunk
	     * Same as message received, msg internal state incomplete
	     * msg co-owned by parser and subsession, parsing continues after transfer to msrp session
	     * the content ptrs can be completed and closed (Removing them from msg' content_ptrs_list) and thus freeing content buffers for reuse if needed
	     */
	    
	};

#endif // __MMMSRPPARSEROBSERVER_H

// End of file
