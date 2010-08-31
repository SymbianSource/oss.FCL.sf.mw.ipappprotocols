// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Header for class specific to rtp unit tests
// 
//

/**
 @file
 @internalComponent
*/

#if (!defined T_RTPCORESERVER_H__)
#define T_RTPCORESERVER_H__
#include <testexecuteserverbase.h>

#if (!defined __DATA_CAGING__)
IMPORT_C TInt NewServer();
#endif

#include "rtpcontroller.h"
#include "rtcppacket.h"
#include "rtppacket.h"


class CTRtpCoreStep;

class CTRtpCoreServer : public CTestServer
	{
public:
	static CTRtpCoreServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	~CTRtpCoreServer();

	// Member functions used by test steps
	/*
	static void StopScheduler(CTrtpServer* aPtr, const TRtpEvent& aEvent);
	*/
	static TBool CompareRtpHeadersFromBuf(TDesC8& buf1,TDesC8& buf2);
	static TBool CompareRtpHeaderWithBuf(TRtpFixedHdrMemLayout& aHeader,TDesC8& aBuf);
	static TBool CompareRtcpHeadersFromBuf(TDesC8& buf1,TDesC8& buf2);
	static TBool CompareRtcpHeaderWithBuf(TRtcpFixedHdrMemLayout& aHeader,TDesC8& aBuf);
	// Members used by the steps
	
	TBool iOom;
	CTRtpCoreStep* iCurrentStep;
	TBool iStopping;
	TInt iStopCount;
	
	RConnection iConnection;
	RSubConnection iSubConnection;
	
	// generic socket, e..g send
	RSocket iSocket;
	// for connected/bind socket use cases
	RSocket iRtpSocket;
	RSocket iRtcpSocket;
	TBool iUsingConnectedSocket;

	CActiveScheduler* iScheduler;
	RSocketServ iSocketServer;
	CRtpControllerBase* iController;
	CRtpControllerBase* iSendController;
 private:
	CTRtpCoreServer();
	};


class CWaiter : public CBase, public MRtpDataObserver
	{
 public:
	static CWaiter* NewLC(CTRtpCoreServer* aServer);

	~CWaiter();
	
	void DataAvailable(TInt aErrorCode);
	
 private:
	CWaiter(CTRtpCoreServer* aServer) : iServer(aServer) 
			{
			};

	CTRtpCoreServer* iServer;
	
 public:
    TRequestStatus iStatus;
	};

struct TRtpFixedHeader
  	{
  	TUint8 iVersion:2;		// protocol version
  	TUint8 iP:1;			// padding flag
  	TUint8 iX:1;			// header extension flag
  	TUint8 iCC:4;			// CSRC count
  	TUint8 iM:1;			// marker bit
  	TUint8 iPT:7;			// payload type
  	TUint16	iSequenceNo;	// sequence number
  	TUint32	iTimeStamp;		// timestamp
  	TUint32	iSSRC;			// synchronization source
  	//RArray<TUint32> iCSRC; // optional CSRC list
  	TRtpFixedHeader();
  	};

inline TRtpFixedHeader::TRtpFixedHeader()
  	{
  	iVersion = KRtpVersion2;	// protocol version = 2
  	iP = 0;						// padding flag
  	iX = 0;						// header extension flag
  	iCC = 0;					// CSRC count
  	iM = 0;						// marker bit
  	iPT = 0;					// payload type
  	iSequenceNo = 0;			// sequence number
  	iTimeStamp = 0;				// timestamp
  	iSSRC = 0;					// synchronization source
  	}

class CRtpFixedHeader : public CBase
   	{
   public:
   	CRtpFixedHeader(TDes8& packet);
   	CRtpFixedHeader();
   	~CRtpFixedHeader();
   	void SetVersion(TUint aVersion);
   	void SetCSRCCount(TUint aCC);
   	void SetSequenceNumber(TUint16 aNo);
   	void SetSSRC(TUint32 aSSRC);
   	void SetPadding(TBool aPadding);
   	void SetExtension(TBool aExtension);
   	void SetTimeStamp(TUint32 aTime);
   
   	TUint32 SSRC();
   	TUint8 CC();
   	TBool IsMarker();
   	TBool IsPadding();
   	TBool IsExtension();
   	TUint8 PayloadType();
   	TUint8 Version();
   	TUint16 SequenceNumber();
   	TUint32 TimeStamp();
   	TUint32 CSRCs(TInt aIndex);
private:
   	
   	TRtpFixedHeader iHeader;
   	};

// RTCP //
// Reception report block
struct TRtcpRRBlock
	{
	TUint32 iSSRC;	// data source being reported
    TUint iFractionLost:8;	// fraction lost since last SR/RR
	TInt iCumulLost:24;	// cumul. no. pkts lost (signed!)
	TUint32 iExtendedLastSeq;	// extended last seq. no. received
    TUint32 iJitter;	// interarrival jitter
    TUint32 iLastSR;	// last SR packet from this source
    TUint32 iDSLR;	// delay since last SR packet
	};

// SDES item
struct TRtcpSDESItem
	{
	TUint8 iType;		// type of item (rtcp_sdes_type_t)
	TUint8 iLength;		// length of item (in octets)
	HBufC8* iData;		// text, not null-terminated 
   };


//One RTCP packet
struct TRtcpPacketStruct
	{
	struct
		{
		// common part
		// protocol version
		TUint iVersion:2;
		// padding flag
		TUint iPadding:1;
		// varies by packet type
		TUint iCount:5;
		// RTCP packet type
		TUint iPacketType:8;
		// pkt len in words, w/o this word
		TUint16 iLength;
		} common;
	union 
		{
		// sender report (SR)
		struct 
			{
			TUint32 iSSRC;     // sender generating this report
			TUint32 iNtpSeconds;  // NTP timestamp
			TUint32 iNtpFraction;
			TUint32 iRtpTimeStamp;   // RTP timestamp
			TUint32 iPacketsSent;    // packets sent
			TUint32 iOctetsSent;    // octets sent
            TRtcpRRBlock iRR[1];  // variable-length list
			} sr;

		// reception report (RR)
		struct 
			{
			TUint32 iSSRC;		// receiver generating this report
			TRtcpRRBlock iRR[1];	// variable-length list
			} rr;

		// source description (SDES)
		struct
			{
			TUint32 iSRC;			// first SSRC/CSRC
			TRtcpSDESItem iItem[1];	// list of SDES items
			} sdes;

		// BYE
		struct 
			{
			TUint32 iSRC[1];   // list of sources
               /* can't express trailing text for reason */
           } bye;

		// App
		struct
			{
			TUint32 iSRC;
			HBufC8* iAppName;
			HBufC8* iAppData;
			} app;
		
       } specific;
   };

class CRtcpPacketTest : public CBase
   	{
   public:
   	CRtcpPacketTest(TDes8& aPacket);
   	//CRtcpRRPacket();
   	~CRtcpPacketTest();
	void Init(TDes8& aPacket);
	TBool operator==(CRtcpPacketTest& aPacket);
	TBool operator!=(CRtcpPacketTest& aPacket);
   	void SetVersion(TUint aVersion);
	void SetPadding(TBool aPadding);
   	void SetCount(TUint8 aCount);
   	void SetPacketType(TRtcpPacketType aPacketType);
	void SetLength(TUint16 aLength);
   	
	TUint Version();
	TBool IsPadding();
   	TUint8 Count();
	TRtcpPacketType PacketType();
   	TUint16 Length();
   	
protected:
   	
   	TRtcpPacketStruct iPacket;
   	};

class CRtcpRRPacketTest : public CRtcpPacketTest
   	{
   public:
   	CRtcpRRPacketTest(TDes8& aPacket);
   	~CRtcpRRPacketTest();
	TBool operator==(CRtcpRRPacketTest& aPacket);
	TBool operator!=(CRtcpRRPacketTest& aPacket);
	void SetSenderSSRC(TUint32 aSSRC);
	void SetReportSSRC(TUint32 aSSRC);
	void SetFraction(TUint8 aFraction);
	void SetLost(TUint aLost);
	void SetHighestSequence(TUint32 aSeq);
   	void SetJitter(TUint32 aJitter);
	void SetLastSR(TUint32 aLastSR);
   	void SetDSLR(TUint32 aDSLR);
   
   	TUint32 SenderSSRC();
	TUint32 ReportSSRC();
   	TUint8 Fraction();
	TUint Lost();
	TUint32 HighestSequence();
	TUint32 Jitter();
	TUint32 LastSR();
   	TUint32 DSLR();
   	};

class CRtcpSRPacketTest : public CRtcpPacketTest
   	{
   public:
   	CRtcpSRPacketTest(TDes8& aPacket);
   	~CRtcpSRPacketTest();
	TBool operator==(CRtcpSRPacketTest& aPacket);
	TBool operator!=(CRtcpSRPacketTest& aPacket);
	void SetSenderSSRC(TUint32 aSSRC);

	void SetNtpMsb(TUint32 aNtpMsb);
	void SetNtpLsb(TUint32 aNtpLsb);
	void SetRtpTimeStamp(TUint32 aRtpTs);
	void SetPacketCount(TUint32 aPacketCount);
	void SetOctetCount(TUint32 aOctetCount);

	void SetReportSSRC(TUint32 aSSRC);
	void SetFraction(TUint8 aFraction);
	void SetLost(TUint aLost);
	void SetHighestSequence(TUint32 aSeq);
   	void SetJitter(TUint32 aJitter);
	void SetLastSR(TUint32 aLastSR);
   	void SetDSLR(TUint32 aDSLR);
   
   	TUint32 SenderSSRC();

	TUint32 NtpMsb();
	TUint32 NtpLsb();
	TUint32 RtpTimeStamp();
	TUint32 PacketCount();
	TUint32 OctetCount();

	TUint32 ReportSSRC();
   	TUint8 Fraction();
	TUint Lost();
	TUint32 HighestSequence();
	TUint32 Jitter();
	TUint32 LastSR();
   	TUint32 DSLR();
   	};

class CRtcpByePacketTest : public CRtcpPacketTest
   	{
   public:
   	CRtcpByePacketTest(TDes8& aPacket, TBool aIncreaseOffset = ETrue);
   	~CRtcpByePacketTest();
	TBool operator==(CRtcpByePacketTest& aPacket);
	TBool operator!=(CRtcpByePacketTest& aPacket);
	void SetSSRC(TUint32 aSSRC);
	void SetReasonTxt(TDesC8 aReason);

	TInt ReadSROrRRAndSDESAndIncOffset(const TUint8** aPtr);
   
   	TUint32 SSRC();
	TDesC8 ReasonTxt();

	TInt iOffset;  
   	};

class CRtcpAppPacketTest : public CRtcpPacketTest
   	{
   public:

	static CRtcpAppPacketTest* NewL(TDes8& aPacket);
	CRtcpAppPacketTest(TDes8& aPacket);
   	~CRtcpAppPacketTest();
	TBool operator==(CRtcpAppPacketTest& aPacket);
	TBool operator!=(CRtcpAppPacketTest& aPacket);
	void SetSSRC(TUint32 aSSRC);
	void SetAppData(const TDesC8& aAppData);
	void SetAppName(const TDesC8& aAppName);
   
   	TUint32 SSRC();
	const TDesC8& AppData();
	const TDesC8& AppName();

   private :
 void ConstructL(TDes8& aPacket);

   	};



class CItem : public CBase
	{
public:
	static CItem* NewL( const TDesC8& aItem, TInt aType );
	static TInt FlatCompare(const CItem& aItemOne, const CItem& aItemTwo ); //Assume that given CItem are a flat-data structures, compare 
						//them byte by byte, and if all bytes are exactly same
						//return true

private:
	void ConstructL( const TDesC8& aItem );
	CItem( TUint8 aType );
	~CItem();

	TBool DoFlatCompare( const CItem& aItem ) const;

private:
	TUint8 iType;
	HBufC8* iData;
	};


class CRtcpSDESPacketTest : public CRtcpPacketTest
   	{
public:
	static CRtcpSDESPacketTest* NewL(TDes8& aPacket);
	CRtcpSDESPacketTest(TDes8& aPacket); //Should really be private
   
   	~CRtcpSDESPacketTest();
	TBool operator==(CRtcpSDESPacketTest& aPacket);
	TBool operator!=(CRtcpSDESPacketTest& aPacket);
	
	void SetSSRC(TUint32 aSSRC);
	void SetType(TSDESType aType);
	void SetData(const TDesC8& aData);

	TInt SROrRRLength() const;
	

	TUint32 SSRC();
	TSDESType Type();
	const TDesC8& Data();

	CItem* NextL();
	CItem* First();

private:   
   void ConstructL(TDes8& aPacket);

private:
	TInt iOffset;   
	TInt iPktSize; //Assuming that there is no trailing BYE???
	TInt iItemCount; 
	CItem* iFirstItem;

	const TUint8* iStartOfPkt;
	TInt iCurrentOffset;
  	};


class RSdesItemManager
	{
public:
	RSdesItemManager();
	~RSdesItemManager();

	TInt Open();
	void Close();

	TInt StoreL( const TDesC8& aItem, TInt aItemType );
	TInt SegregateItemsL();
	TInt CompareSentAndReceivedValues();

	void SetRecvBuffer( CRtcpSDESPacketTest * aSdesPkt ) { iSdesPkt = aSdesPkt; }

private:
	RPointerArray<CItem> iArray;//Sent items list
	TInt iCurrentItem;

	RPointerArray<CItem> iSegregatedItemsArray;//received items list
	TInt iSegregatedItemsArrayID;

	CRtcpSDESPacketTest * iSdesPkt;
	};

#endif
