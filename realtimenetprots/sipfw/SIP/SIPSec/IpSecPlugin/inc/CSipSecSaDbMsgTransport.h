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
* Name          : CSipSecSaDbMsgTransport.h
* Part of       : SIPSec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPSECSADBMSGTRANSPORT_H
#define CSIPSECSADBMSGTRANSPORT_H

#include <e32base.h>
#include <networking/pfkeyv2.h>

class RSADB;
class MSipSecSaDbSendReceiveObserver;

/**
 * @brief Base class for SADB message transport mechanism
 * 
 * 
 */

class CSipSecSaDbMsgTransport : public CActive
	{

public:
		
	CSipSecSaDbMsgTransport( MSipSecSaDbSendReceiveObserver& aObserver, 
							 RSADB& aSadb,
							 TInt aMsgType = SADB_DUMP );
		
	virtual ~CSipSecSaDbMsgTransport();
	
	TInt Type();
		
	void ConstructL();
		
	virtual void RunL() = 0;
    	
	virtual void DoCancel() = 0;
	
protected:

	// Does not own
	MSipSecSaDbSendReceiveObserver& iObserver;
	RSADB& iSaDb;
	TInt iMsgType;


#ifdef CPPUNIT_TEST
    friend class CActiveObjController1;
#endif	
	
	};



#endif // CSIPSECSADBMSGTRANSPORT_H


// End of File
