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

#ifndef MMSRPMESSAGEOBSERVER_H_
#define MMSRPMESSAGEOBSERVER_H_

class CMSRPMessageHandler;

class MMSRPMessageObserver
    {
protected:
    ~MMSRPMessageObserver(){};
			
    // Interface via which the Message informs events to its observer (a server sub session).

public:
    
    virtual void MessageSendCompleteL( ) = 0;
    
	virtual void MessageResponseSendCompleteL( CMSRPMessageHandler& aMsg ) = 0;
	
	virtual void MessageSendProgressL(TInt aBytesSent, TInt aTotalBytes) = 0;
	
	virtual void MessageReceiveProgressL(TInt aBytesSent, TInt aTotalBytes) = 0;
        
    virtual void WriterError( ) = 0;
    
    };

#endif /* MMSRPMSGOBSERVER_H_ */
