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
* Name          : CSender.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __CSENDER_H__
#define __CSENDER_H__

#include <e32base.h>
#include "COutgoingData.h"
#include "MContext.h"
#include "TSIPTransportParams.h"

// FORWARD DECLARATIONS
//
class COwnerSettingsList;

// CLASS DEFINITION
//
class CSender : public CActive
	{
public:

	virtual ~CSender ();

	static TUint IpAddrType( const TInetAddr& aAddr );
	virtual TBool SendL (const TSIPTransportParams& aTransportParams,
	                     CSIPMessage& aMessage,
		                 const TInetAddr& aAddr,
						 TUint aOrigTransport,
						 TRequestStatus& aStat,
						 TBool aStore,
						 TBool aForceUDP) = 0;

	virtual void SendNextL () = 0;

	virtual void SendL (const TSIPTransportParams& aTransportParams,
	                    CSIPMessage& aMessage,
		                TRequestStatus& aStat);

	virtual void SendL (COutgoingData* aData);

	virtual COutgoingData* StoredData ();
	
	virtual COutgoingData* Data ();

	TBool FindAndCancel (TRequestStatus& aStat);

	void CancelAllRequests (TInt aReason);

    static void CleanCurrentDataOnLeave( TAny* aSelf );
	
protected:

	CSender (MContext& aContext,COwnerSettingsList& aSettingsList);

    COutgoingData* CurrentData();

	void WriteToLog (const TDesC8& aMessage);  
    
protected:	//Data

    MContext& iContext;
    COwnerSettingsList& iSettingsList;
	TPtrC8 iOutgoingMessage;
	TSglQue<COutgoingData> iList;
	TSglQueIter<COutgoingData> iListIter;
	TBool iContinueSending;

private:

	void RunL ();
	void DoCancel ();
	TInt RunError (TInt aError);
	void RemoveCurrentData(TInt aReason);
	void CompleteRequest (TRequestStatus* aStatus, TInt aReason);

private: //For testing purposes
#ifdef CPPUNIT_TEST
	friend class CSenderTest;
	friend class CServerTest;
#endif
	};

#endif // end of __CSENDER_H__

// End of File
