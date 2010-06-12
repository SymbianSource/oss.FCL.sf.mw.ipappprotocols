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

#ifndef __MMMSRPWRITEROBSERVER_H
#define __MMMSRPWRITEROBSERVER_H

// CLASS DECLARATIONS
/**
* Interface for notifying about socket writer events
*/
class MMSRPWriterObserver
	{
	public:
    
    enum TWriteStatus
        {
        EMsrpSocketWrite = 0,//EComplete, EInterrupted, EPending_bufferfull
        EMsrpAvoidSocketWrite, //EPending_buffernotfull 
        };  
	    
	enum TMsgStatus
	    {
        EUndefined = 0,
	    EPending, //Sending incomplete. Same observer is called again immediately
	    EComplete, //No more to send. A new observer is called, this is removed 
	    EInterrupted //There is more to send. Same observer called later
	    };	
	
	    /**
	     * Called by writer to fetch the content to send
	     * @param aData Output param, Points to data to be sent 
	     * @param aInterrupt Signals whether other msgs are queued waiting to be sent
	     * @return KErrNone or system wide eror codes 
	     */	    
	    virtual TWriteStatus GetSendBufferL( TPtrC8& aData, TBool aInterrupt ) = 0;

	   /**           
        * Called when the message has been written to the socket
		* @param aSendstatus status of the write operation, communicates prev send result
        * @return Indicates which observer has to be called the next time                 
        */
	    virtual TMsgStatus WriteDoneL(TInt aStatus ) = 0;
	};


class MMSRPWriterErrorObserver
    {
    public:
        
        /**
         * socket errors detected on write arent msg specific
         * Hence the observer has been separated
         * The observer is expected to be implemented in connection and handled as a connection error
         */
        
        virtual void WriteSocketError(TInt aError) = 0;
    };



#endif // __MMMSRPWRITEROBSERVER_H

// End of file
