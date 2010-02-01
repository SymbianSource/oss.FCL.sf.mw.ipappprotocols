/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : COutgoingData.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __COUTGOINGDATA_H__
#define __COUTGOINGDATA_H__

#include <e32base.h>
#include <in_sock.h>

class CSIPMessage;
class CSIPViaHeader;
class CSIPURI;
class MSigCompController;
class TSIPTransportParams;

class COutgoingData : public CBase
	{
public:
	
	static COutgoingData* NewL (const TSIPTransportParams& aTransportParams,
	                            CSIPMessage& aMessage,
		                        const TInetAddr& aAddr,
								TUint aOrigTransport,
								TRequestStatus& aStatus,
								MSigCompController& aSigCompHandler,
								TBool aForceUDP = EFalse);

	static COutgoingData* NewLC (const TSIPTransportParams& aTransportParams,
	                             CSIPMessage& aMessage,
		                         const TInetAddr& aAddr, 
		                         TUint aOrigTransport,
								 TRequestStatus& aStatus,
								 MSigCompController& aSigCompHandler,
								 TBool aForceUDP = EFalse);

	static COutgoingData* NewL (const TSIPTransportParams& aTransportParams,
	                            CSIPMessage& aMessage,
		                        TRequestStatus& aStatus,
								MSigCompController& aSigCompHandler);

	static COutgoingData* NewLC (const TSIPTransportParams& aTransportParams,
	                             CSIPMessage& aMessage,
		                         TRequestStatus& aStatus,
								 MSigCompController& aSigCompHandler);


	~COutgoingData();

	TRequestStatus* Status() {return iStatus;}
	TInetAddr& Address() {return iAddress;}
	TUint OrigTransport() {return iOrigTransport;}
	TBool Compressed() {return iCompressed;}
	void SetAddress(const TInetAddr& aAddress);
	TPtr8 EncodeL(TBool aStreambasedProtocol);
	
	TPtr8 HeaderPartL();
	CSIPMessage& Message();
	void Sent();
	const TSIPTransportParams& TransportParams() const;
	TBool ForceUDP() const;

	static const TInt iOffset;
	TSglQueLink iLink;

private:

	COutgoingData (const TSIPTransportParams& aTransportParams,
	               CSIPMessage& aMessage,
				   MSigCompController& aSigCompHandler,
				   TRequestStatus& aStatus);

	COutgoingData (const TSIPTransportParams& aTransportParams,
	               CSIPMessage& aMessage,
		           const TInetAddr& aAddr,
		           TUint aOrigTransport,
				   MSigCompController& aSigCompHandler,
				   TRequestStatus& aStatus,
				   TBool aForceUDP);

	void ConstructL ();

private: // Data

	const TSIPTransportParams& iTransportParams;
	CSIPMessage& iMessage;	
	TRequestStatus* iStatus;
	TInetAddr iAddress;	
	CBufBase* iData;
	MSigCompController& iSigCompHandler;
	TUint iOrigTransport;
	TBool iForceUDP;
	TBool iCompressed;

private: //For testing purposes
#ifdef CPPUNIT_TEST
	friend class COutgoingDataTest;
#endif
	};
#endif // end of __COUTGOINGDATA_H__

// End of File
