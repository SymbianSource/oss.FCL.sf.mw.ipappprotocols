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
    
    /**
    * Informs when the whole message has been sent successfully
    * @param aMessageHandler message handler instance, ownership not transferred
     */
    virtual void MessageSendCompleteL( CMSRPMessageHandler* aMessageHandler ) = 0;
    
    /**
    * Informs when response has been sent to the incoming message
    * @param aMessageHandler message handler instance, ownership not transferred
     */
	virtual void MessageResponseSendCompleteL( CMSRPMessageHandler& aMsg ) = 0;

    /**
    * Informs when report has been sent to the incoming message
    * @param aMessageHandler message handler instance, ownership not transferred
     */
    virtual void MessageReportSendCompleteL( CMSRPMessageHandler& aMsg ) = 0;

    /**
    * Current message send progress reported to connected subsessions
    * @param aMessageHandler message handler instance, ownership not transferred
     */
	virtual void MessageSendProgressL( CMSRPMessageHandler* aMessageHandler ) = 0;
	
    /**
    * Current message receive progress reported to connected subsessions
    * @param aMessageHandler message handler instance, ownership not transferred
     */
	virtual void MessageReceiveProgressL( CMSRPMessageHandler* aMessageHandler ) = 0;
	
	virtual void MessageCancelledL( ) = 0;
        
    virtual void WriterError( ) = 0;
    
    };

#endif /* MMSRPMSGOBSERVER_H_ */
