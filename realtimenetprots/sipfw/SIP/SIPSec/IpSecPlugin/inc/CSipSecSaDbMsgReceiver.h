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
* Name          : CSipSecSaDbMsgReceiver.h
* Part of       : SIPSec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPSECSADBMSGRECEIVER_H
#define CSIPSECSADBMSGRECEIVER_H

#include "CSipSecSaDbMsgTransport.h"

/**
 * @brief SADB message receiving mechanism
 * 
 * 
 */

class CSipSecSaDbMsgReceiver : public CSipSecSaDbMsgTransport
	{

public:

	static CSipSecSaDbMsgReceiver* NewL( 
	            MSipSecSaDbSendReceiveObserver& aObserver, 
		        RSADB& aSadb );

    static CSipSecSaDbMsgReceiver* NewLC( 
	            MSipSecSaDbSendReceiveObserver& aObserver, 
		        RSADB& aSadb );
	
	~CSipSecSaDbMsgReceiver();

    void Receive( TDes8& aMsg );
    
protected:

	void RunL();

    void DoCancel();
    
private:

    CSipSecSaDbMsgReceiver( 
	            MSipSecSaDbSendReceiveObserver& aObserver, 
	            RSADB& aSadb );
	
	
#ifdef CPPUNIT_TEST
    friend class CActiveObjController1;
#endif
	
	};



#endif // CSIPSECSADBMSGRECEIVER_H


// End of File
