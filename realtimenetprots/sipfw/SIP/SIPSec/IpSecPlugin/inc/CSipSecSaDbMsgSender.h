/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* Name          : CSipSecSaDbMsgSender.h
* Part of       : SIPSec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPSECSADBMSGSENDER_H
#define CSIPSECSADBMSGSENDER_H

#include "CSipSecSaDbMsgTransport.h"

class TPfkeySendMsgBase;

/**
 * @brief SADB message sending mechanism
 * 
 * 
 */

class CSipSecSaDbMsgSender : public CSipSecSaDbMsgTransport
	{

public:

	static CSipSecSaDbMsgSender* NewL( 
	                      MSipSecSaDbSendReceiveObserver& aObserver, 
					      RSADB& aSadb );
						 
	static CSipSecSaDbMsgSender* NewLC( 
	                      MSipSecSaDbSendReceiveObserver& aObserver, 
					      RSADB& aSadb );
							  
	~CSipSecSaDbMsgSender();
		
	void SendL( TPfkeySendMsgBase* aMessage, TInt aMsgType );

protected:
		
	void RunL();
	
	TInt RunError( TInt aError );
    	
	void DoCancel();

private:

    CSipSecSaDbMsgSender( MSipSecSaDbSendReceiveObserver& aObserver, 
						  RSADB& aSadb );
	
	void StoreMessageL( TPfkeySendMsgBase* aMessage, TInt aMsgType );
					  
	void RemoveMessage();
	
private:

    TPfkeySendMsgBase* iMsg;
						  
#ifdef CPPUNIT_TEST
    friend class CActiveObjController1;
#endif

	};



#endif // CSIPSECSADBMSGSENDER_H


// End of File
