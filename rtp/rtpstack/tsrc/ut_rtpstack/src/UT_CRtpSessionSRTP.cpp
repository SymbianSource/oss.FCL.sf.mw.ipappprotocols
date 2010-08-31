/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/




//  CLASS HEADER
#include "UT_CRtpSessionSRTP.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>


//  INTERNAL INCLUDES
#include "rtpsessionsrtp.h"


// CONSTANTS
const TInt KRtpVersion = 2;    // Current protocol version          
const TUint KMinRtpHeaderLen( 12 );
const TUint16 KTestSeqNr17 = 17171;
const TUint32 KSSRCOut= 4321;
const TInt KLength = 50;

// 80 bit key, test 1
_LIT8(KTestPayload160Bits,  "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

// 80 bit key, test 1
//_LIT8(KTestMKI128Bits,      "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB");
_LIT8(KTestMKI128Bits,      "ABCDEF1234567890ABCDEF1234567890");

// 80 bit key, test 1
_LIT8(KTestAuthTag80Bits,   "CCCCCCCCCCCCCCCCCCCC");

_LIT8(KRFC3711_TestMasterKey128bits,    "E1F97A0D3E018BE0D64FA32C06DE4139");
_LIT8(KRFC3711_TestMasterSalt112bits,   "0EC675AD498AFEEBB6960B3AABE6");	
_LIT8(KRFC3711_SessionEncrKey128bits,   "C61E7A93744F39EE10734AFE3FF7A087");  
_LIT8(KRFC3711_SessionSaltKey128bits,   "30CBBC08863D8C85D49DB34A9AE1");  
_LIT8(KRFC3711_SessionAuthKey128bits,   "CEBE321F6FF7716B6FD4AB49AF256A156D38BAA4");  
_LIT8(KDecryptedPayload,                "2EDE3286C6E33E68C6BB93350620A1245EC0004F");  

_LIT8(KTestM_without_ROC,               "123456789ABCDE");  
_LIT8(KRefM_with_ROC,                   "123456789ABCDEFFFFFFFF");  

_LIT8(KTestAuthTag,                     "D07919ABC9F235CB4516");  
_LIT8(KDummyRTCPPacket, "dummyrtcp");

_LIT8(KRTCPPacket, "80C90001000010E19E30159059A23363F0FBDB62DC85AF52875C944495C5B00280000001ABCDEF1234567890ABCDEF1234567890DFC98F01DE16DA30F20D");

#define RTP_EUNIT_ASSERT_EQUALS( val1, val2 ) \
if ( val2 != KErrNoMemory && val1 == KErrNoMemory ){\
	User::Leave( KErrNoMemory );\
	}\
else{\
	EUNIT_ASSERT_EQUALS( val1, val2 );\
	}

//constant from RTP
const TUint KDefaultIap = 6;
const TUint8 KRtpPayloadClockConversions[KTestRtpNumPayloadTypes] =
    {
    125, 125, 125, 125, 125, 125, 63, 125, 125, 63, // 0-9 
    23, 23, 125, 125, 11, 125, 91, 45, 125, 0,      // 10-19
    0, 0, 0, 0, 0, 11, 11, 0, 11, 0,                // 20-29
    0, 11, 11, 11, 11, 0, 0, 0, 0, 0,               // 30-39
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 40-49
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 50-59
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 60-69
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 70-79
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 80-89
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 90-99
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 100-109
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                   // 110-119
    0, 0, 0, 0, 0, 0, 0, 0                          // 120-127
    };

// CONSTRUCTION
UT_CRtpSessionSrtp* UT_CRtpSessionSrtp::NewL()
    {
    UT_CRtpSessionSrtp* self = UT_CRtpSessionSrtp::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_CRtpSessionSrtp* UT_CRtpSessionSrtp::NewLC()
    {
    UT_CRtpSessionSrtp* self = new( ELeave ) UT_CRtpSessionSrtp();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_CRtpSessionSrtp::~UT_CRtpSessionSrtp()
    {
    }

// Default constructor
UT_CRtpSessionSrtp::UT_CRtpSessionSrtp()
    {
    }

UT_CRtpSessionSrtp::TPayloadDetails::TPayloadDetails() :
        seqNr(),
//      ssrc(),
        padding(0),
        numCSRC(0),
        fHeaderExtension(EFalse),
        payloadLen(0),
        payloadData(NULL),
        MKILen(0),
        MKIData(NULL),
        authTagLen(0),
        authTagData(NULL)
            {            
            }
void UT_CRtpSessionSrtp::SRTPMasterKeyStaleEvent(const CSRTPSession&  /*aSession*/ )
    {
    }    

void UT_CRtpSessionSrtp::SRTPMasterKeyStaleEvent(const CSRTPStream&  /*aStream */)
    {
    }           
    
void UT_CRtpSessionSrtp::Write32( TUint8* const aPointer, TUint32 aValue )
    {
    aPointer[0] = static_cast<TUint8>( ( aValue & 0xFF000000 ) >> 24 );
    aPointer[1] = static_cast<TUint8>( ( aValue & 0x00FF0000 ) >> 16 );
    aPointer[2] = static_cast<TUint8>( ( aValue & 0x0000FF00 ) >> 8 );
    aPointer[3] = static_cast<TUint8>( aValue & 0x000000FF );
    }    
    
    
void UT_CRtpSessionSrtp::Hex(HBufC8& aString)
{
    TPtr8 ptr=aString.Des();
    TInt length = aString.Length();
    if (aString.Length()%2)
        {
        ptr.SetLength(0);
        return;
        }
    TInt i;
    for (i=0;i<aString.Length();i+=2)
        {
        TUint8 tmp;
        tmp=(TUint8)(aString[i]-(aString[i]>'9'?('A'-10):'0'));
        tmp*=16;
        tmp|=(TUint8)(aString[i+1]-(aString[i+1]>'9'?('A'-10):'0'));
        ptr[i/2]=tmp;
        }
    ptr.SetLength(aString.Length()/2);

}

// ---------------------------------------------------------------------------
// UT_CSRTPStreamIn::BuildSrtpPacketL()
// 
// ---------------------------------------------------------------------------
//
HBufC8* UT_CRtpSessionSrtp::BuildPacketL(TBool aSrtp, TPayloadDetails& details)
    {
    TUint packetSize = KMinRtpHeaderLen + details.payloadLen;
    TUint payloadStartOffset = KMinRtpHeaderLen;
    TUint8 padCount = 0;
    
    packetSize += details.numCSRC*4;
    payloadStartOffset += details.numCSRC*4;          
    
    if (details.fHeaderExtension) 
        {
        packetSize += 4;        
        payloadStartOffset += 4;
        }

    if (details.padding) 
        {
        padCount = 3;
        }
        
    packetSize += padCount;            
        
    if (aSrtp)
        {
        packetSize += details.MKILen;                    
        packetSize += details.authTagLen;                        
        }
        
    
    HBufC8* packet = HBufC8::NewMaxL(packetSize);
    TUint8* dataP = const_cast<TUint8*>( packet->Des().Ptr() );    
    TUint8* ptr = dataP;
    Mem::FillZ( ptr, packetSize);
    
    // padding (1 bit)
    ptr[0] |= static_cast<TUint8>( details.padding << 5 );
        
    // header extension (1 bit)
    if ( details.fHeaderExtension )
        {
        ptr[0] |= ( 1 << 4 );
        }
    
    if (details.numCSRC)
        {
        // CC
        ptr[0] |= details.numCSRC;
        }
       
    //sequence number
    ptr+=2;
    Write16( ptr, details.seqNr );    
    
    //ssrc   
    
    ptr+=6;
    if (details.ssrc)
        {
        Mem::Copy( ptr, details.ssrc, 4 );        
        }
   
    //payload
    ptr = dataP + payloadStartOffset;
    if (details.payloadData)
        {
        Mem::Copy( ptr, details.payloadData, details.payloadLen );        
        }

    if (aSrtp)
        {
        //MKI
        ptr = dataP + payloadStartOffset + details.payloadLen + padCount;
        if (details.MKILen > 0)
            {
            Mem::Copy( ptr, details.MKIData, details.MKILen );        
            }
    
        //auth tag
        ptr = dataP + payloadStartOffset + details.payloadLen + padCount + details.MKILen;
        if (details.authTagLen > 0)
            {
            Mem::Copy( ptr, details.authTagData, details.authTagLen );        
            }            
        }    
    
    return packet;
    }
        
// Second phase construct
void UT_CRtpSessionSrtp::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS



void UT_CRtpSessionSrtp::SetupL(  )
    {
   iDecryptedPayload = HBufC8::NewL(KDecryptedPayload().Length());
	*iDecryptedPayload = KDecryptedPayload;
	Hex(*iDecryptedPayload);

    iRFC3711_SessionEncrKey128bits = HBufC8::NewL(KRFC3711_SessionEncrKey128bits().Length());
	*iRFC3711_SessionEncrKey128bits = KRFC3711_SessionEncrKey128bits;
	Hex(*iRFC3711_SessionEncrKey128bits);

    iRFC3711_SessionSaltKey128bits = HBufC8::NewL(KRFC3711_SessionSaltKey128bits().Length());
	*iRFC3711_SessionSaltKey128bits = KRFC3711_SessionSaltKey128bits;
	Hex(*iRFC3711_SessionSaltKey128bits);

    iRFC3711_SessionAuthKey128bits = HBufC8::NewL(KRFC3711_SessionAuthKey128bits().Length());
	*iRFC3711_SessionAuthKey128bits = KRFC3711_SessionAuthKey128bits;
	Hex(*iRFC3711_SessionAuthKey128bits);
	
    
	iTestPayload160Bits = HBufC8::NewL(KTestPayload160Bits().Length());
	*iTestPayload160Bits = KTestPayload160Bits;	
	Hex(*iTestPayload160Bits);	
    
	iTestMKI128Bits = HBufC8::NewL(KTestMKI128Bits().Length());
	*iTestMKI128Bits = KTestMKI128Bits;	
	Hex(*iTestMKI128Bits);	

	iTestAuthTag80Bits = HBufC8::NewL(KTestAuthTag80Bits().Length());
	*iTestAuthTag80Bits = KTestAuthTag80Bits;	
	Hex(*iTestAuthTag80Bits);	
		
		
	iRFC3711_TestMasterKey128bits.Append(0xE1);
	iRFC3711_TestMasterKey128bits.Append(0xF9);
	iRFC3711_TestMasterKey128bits.Append(0x7A);
	iRFC3711_TestMasterKey128bits.Append(0x0D);
	iRFC3711_TestMasterKey128bits.Append(0x3E);
	iRFC3711_TestMasterKey128bits.Append(0x01);
	iRFC3711_TestMasterKey128bits.Append(0x8B);
	iRFC3711_TestMasterKey128bits.Append(0xE0);
	iRFC3711_TestMasterKey128bits.Append(0xD6);
	iRFC3711_TestMasterKey128bits.Append(0x4F);
	iRFC3711_TestMasterKey128bits.Append(0xA3);
	iRFC3711_TestMasterKey128bits.Append(0x2C);
	iRFC3711_TestMasterKey128bits.Append(0x06);
	iRFC3711_TestMasterKey128bits.Append(0xDE);
	iRFC3711_TestMasterKey128bits.Append(0x41);
	iRFC3711_TestMasterKey128bits.Append(0x39);

	iRFC3711_TestMasterSalt112bits.Append(0x0E);
	iRFC3711_TestMasterSalt112bits.Append(0xC6);
	iRFC3711_TestMasterSalt112bits.Append(0x75);
	iRFC3711_TestMasterSalt112bits.Append(0xAD);
	iRFC3711_TestMasterSalt112bits.Append(0x49);
	iRFC3711_TestMasterSalt112bits.Append(0x8A);
	iRFC3711_TestMasterSalt112bits.Append(0xFE);
	iRFC3711_TestMasterSalt112bits.Append(0xEB);
	iRFC3711_TestMasterSalt112bits.Append(0xB6);
	iRFC3711_TestMasterSalt112bits.Append(0x96);
	iRFC3711_TestMasterSalt112bits.Append(0x0B);
	iRFC3711_TestMasterSalt112bits.Append(0x3A);
	iRFC3711_TestMasterSalt112bits.Append(0xAB);
	iRFC3711_TestMasterSalt112bits.Append(0xE6);

    // our test MKI is 16 bit long:
    // ABCDEF1234567890ABCDEF1234567890
    for (TInt i=0; i<2; i++)
        {
        iMKI.Append(0xAB);
        iMKI.Append(0xCD);
        iMKI.Append(0xEF);
        iMKI.Append(0x12);
        iMKI.Append(0x34);
        iMKI.Append(0x56);
        iMKI.Append(0x78);
        iMKI.Append(0x90);        
        }
  // our test MKI is 16 bit long:
    // ABCDEF1234567890ABCDEF1234567890
   	HBufC8* masterKey = HBufC8::NewL(KRFC3711_TestMasterKey128bits().Length());
    CleanupStack::PushL( masterKey );
    *masterKey = KRFC3711_TestMasterKey128bits;

    
    
    HBufC8* masterSalt =HBufC8::NewL(KRFC3711_TestMasterSalt112bits().Length());
    CleanupStack::PushL( masterSalt);
	*masterSalt = KRFC3711_TestMasterSalt112bits;

	
	HBufC8* mki =HBufC8::NewL(KTestMKI128Bits().Length());
    CleanupStack::PushL( mki);
	*mki = KTestMKI128Bits;
	Hex(*mki);
	
	iMasterKey = CSRTPMasterKey::NewL( *masterKey, *mki );
    iMasterSalt = CSRTPMasterSalt::NewL( *masterSalt );
    
   TSrtpCryptoParams cryptoparams;
    
    iContext = CSRTPCryptoContext::NewL(iMasterKey, iMasterSalt, cryptoparams );
    CleanupStack::PopAndDestroy( mki );
    CleanupStack::PopAndDestroy( masterSalt );
    CleanupStack::PopAndDestroy( masterKey ); 
    iSRTPSession = CSRTPSession::NewL( iDestination, iContext, *this ); 
    
    

    //iCryptoHandlerSRTP = iStreamIn->iHandlerRTP;
    
    //////RTP data//////////////////////////////////////////
  
    TCreateSessionParams sessionParams;
    TUint port( 5000 );
    TRtcpParams rtcpParams;
    rtcpParams.iRtcpFraction = 0.10;
    rtcpParams.iSessionBWidth = 65000;
    TInt err( KErrNone );

    
    for ( TUint k = 0; k < KTestRtpNumPayloadTypes; k++ )
        {
        iProfileRTPTimeRates[k] = ( TUint32 ) KRtpPayloadClockConversions[k];
        }

    TBuf8<3> cName; 
    TBuf8<3> userName;
    TBuf8<3> email;
    TBuf8<3> phoneNumber;
    TBuf8<3> location;
    TBuf8<3> swToolName;
    TBuf8<3> noticeStatus;
    TBuf8<3> privateStr;

    // Create SDES
    cName.Format( _L8( "ab" ) );
    cName.ZeroTerminate();
    userName.Format( _L8( "bc" ) );
    userName.ZeroTerminate();
    email.Format( _L8( "cd" ) );
    email.ZeroTerminate();
    phoneNumber.Format( _L8( "de" ) );
    phoneNumber.ZeroTerminate();
    location.Format( _L8( "ef" ) );
    location.ZeroTerminate();
    swToolName.Format( _L8( "fg" ) );
    swToolName.ZeroTerminate();
    noticeStatus.Format( _L8( "gh" ) );
    noticeStatus.ZeroTerminate();
    privateStr.Format( _L8( "hi" ) );
    privateStr.ZeroTerminate();

    TRtpSdesParams sdes;
    sdes.iCName.Set( cName.Ptr() );
    sdes.iUserName.Set( userName.Ptr() );
    sdes.iEmail.Set( email.Ptr() );
    sdes.iPhoneNumber.Set( phoneNumber.Ptr() );
    sdes.iLocation.Set( location.Ptr() );
    sdes.iSwToolName.Set( swToolName.Ptr() );
    sdes.iNoticeStatus.Set( noticeStatus.Ptr() );
    sdes.iPrivate.Set( privateStr.Ptr() );
    
    // Set it
    iSdes = CRtpSDES::NewL( sdes );

    

    // Opens a session to the socket server
    // i.e. creates an IPC communication channel to the socket server
    if ( ( err = iSocketServ.Connect() ) != KErrNone )
        {
        //"CRtpUtRtpCommSend::ConstructL()") );
        //"RSocketServ - Open Connection Error
        User::Leave( KErrGeneral );
        }

    
    TCommDbConnPref prefs;
    prefs.SetDirection( ECommDbConnectionDirectionOutgoing );
    prefs.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );    
    prefs.SetIapId ( KDefaultIap );//6

    if ( ( err = iConn.Open( iSocketServ ) ) != KErrNone )
        {
        //"CRtpUtRtpCommSend::ConstructL()") );
        //"RConnection - Start error %d"), err );
        User::Leave( KErrGeneral );
        }
    if ( ( err = iConn.Start( prefs ) ) != KErrNone )
        {
        ////"CRtpUtRtpCommSend::ConstructL()") );
        ////"RConnection - Start error %d"), err );
        User::Leave( KErrGeneral );
        }

    // Create a session without RTCP
    iSession = CRtpSessionSrtp::NewL( sessionParams,
                                  port,
                                  ETrue, // disable RTCP
                                  NULL, // Rtcp params
                                  iSocketServ,
                                  iConn,
                                  1,//aSessionID
                                  iSdes,
                                  iProfileRTPTimeRates,
                                  ETrue,
                                  iLibrary,
                                  *this,
                                  *this,
                                  *iSRTPSession);
    
    iTranstreamId=1 ;
    iRecvstreamId=2;
    TRtpSSRC ssrcOut( 1234 );
    TRtpSSRC ssrcIn( 4321 );
    TRtpPayloadType payloadType( 0 );
    TInt error(KErrNone);
    error =iSession->CreateTransmitStreamL( iTranstreamId, payloadType, ssrcOut );
	EUNIT_ASSERT(error==KErrNone);
	
	error = iSession->CreateReceiveStreamL( iRecvstreamId, payloadType );                    
	EUNIT_ASSERT(error==KErrNone);
	
	TInetAddr remoteAddr( INET_ADDR( 127, 0, 0, 1 ), 5000 );

    error = iSession->SetRemoteAddress( remoteAddr );
    
	iStreamIn = CSRTPStreamIn::NewL(*iSRTPSession, ssrcIn);
	iStreamOut = CSRTPStreamOut::NewL(*iSRTPSession, ssrcOut);
    }

void UT_CRtpSessionSrtp::Teardown(  )
    {
    delete iSession;
    iSession=NULL; 
    delete iSdes;
    iSdes = NULL;

    iConn.Close();
    iSocketServ.Close();
    
    delete iDecryptedPayload;
    delete iTestPayload160Bits;
    delete iTestMKI128Bits;
    delete iTestAuthTag80Bits;

	iDecryptedPayload = NULL;
    iTestPayload160Bits = NULL;
    iTestMKI128Bits = NULL;
    iTestAuthTag80Bits = NULL;
	
    delete iStreamIn;
    delete iStreamOut;
    delete iSRTPSession;    
    
	iStreamIn = NULL;
    iStreamOut = NULL;
    iSRTPSession = NULL;
	
    delete iRFC3711_SessionEncrKey128bits;
    delete iRFC3711_SessionSaltKey128bits;
    delete iRFC3711_SessionAuthKey128bits;    
    
	iRFC3711_SessionEncrKey128bits = NULL;
    iRFC3711_SessionSaltKey128bits = NULL;
    iRFC3711_SessionAuthKey128bits = NULL;   
	
	
	iRFC3711_TestMasterKey128bits.Zero();
    iRFC3711_TestMasterSalt112bits.Zero();
    iMKI.Zero();

    
    }

void UT_CRtpSessionSrtp::UT_CRtpSessionSrtp_NewLL(  )
    {
    TCreateSessionParams sessionParams;
    TUint port( 6000 );
    TRtcpParams rtcpParams;
    
    rtcpParams.iSessionBWidth = 6000; // less than the minimum of 8000
    rtcpParams.iRtcpFraction = 1.5; // more than the maximum of 1.0

    // Create a session with RTCP and bad parameters -> default values in use
    CRtpSessionSrtp* session = CRtpSessionSrtp::NewL( sessionParams,
                                  port,
                                  ETrue, // enable RTCP
                                  &rtcpParams, // Rtcp params
                                  iSocketServ,
                                  iConn,
                                  2,//aSessionID
                                  iSdes,
                                  iProfileRTPTimeRates,
                                  ETrue,
                                  iLibrary,
                                  *this,
                                  *this,
                                  *iSRTPSession);
    TRtpSSRC ssrc = session->SSRC();
    TRtpSSRC ssrc2= iSession->SSRC();
   	EUNIT_ASSERT( session );
    delete session;
                                
    }

void UT_CRtpSessionSrtp::UT_CRtpSessionSrtp_SendRtpPacketL(  )
    {
    TInt error( KErrNone );

    // Initialise lots of values
    TRtpSendHeader header;
    header.iHeaderExtension = NULL;
    header.iMarker = 0;
    header.iPadding = 0;
    header.iPayloadType = 0;
    header.iTimestamp = 50;

 	TBuf8<KMaxRtpPacketSize+1> dataBig;
    dataBig.Format( _L8( "foo" ) );
    dataBig.SetLength( KMaxRtpPacketSize + 1 );
    TBuf8<5> data;
    data.Format( _L8( "foo" ) );

    TRtpSequence seqNum = 0;
    TRequestStatus status;
    TUint packetSize = 2048;
    
    error = iSession->SendRtpPacket( iTranstreamId, header, dataBig, status ); 
    EUNIT_ASSERT( error ==KErrOverflow  );     
    
    error = iSession->SendRtpPacket( iTranstreamId, header, data, status ); 
    RTP_EUNIT_ASSERT_EQUALS(error, KErrNone);
    User::WaitForRequest( iSession->iCommNet->iSender[ERTPPort]->iStatus, status );
    RTP_EUNIT_ASSERT_EQUALS( status.Int(), KRequestPending );
    iSession->iCommNet->iSender[ERTPPort]->RunL();
    iSession->iCommNet->iSender[ERTPPort]->iStatus = TRequestStatus();
    User::WaitForRequest( status );
 	RTP_EUNIT_ASSERT_EQUALS(status, KErrNone);
    }
    
 void UT_CRtpSessionSrtp::UT_CRtpSessionSrtp_SendRtpPacket1L(  )
    {
    //If it did not find any outgoing stream
    TInt error( KErrNone );

    // Initialise lots of values
    TRtpSendHeader header;
    header.iHeaderExtension = NULL;
    header.iMarker = 0;
    header.iPadding = 0;
    header.iPayloadType = 0;
    header.iTimestamp = 50;

 	
    TBuf8<5> data;
    data.Format( _L8( "foo" ) );

    TRtpSequence seqNum = 0;
    TRequestStatus status;
    TUint packetSize = 2048;
    
    error = iSession->SendRtpPacket( 40/*unknowstreamId*/, header, data, status ); 
    EUNIT_ASSERT(error==KErrNotFound);
    
    //Then create stream with different ssrc
    
    TRtpSSRC ssrcOut( 2224 );
    
    TRtpPayloadType payloadType( 0 );
    
    error =iSession->CreateTransmitStreamL( 4, payloadType, ssrcOut );
	RTP_EUNIT_ASSERT_EQUALS(error, KErrNone);
	//try to send packet with this stream and it will not match the stream in the srtp
	error = iSession->SendRtpPacket( 4, header, data, status ); 
    RTP_EUNIT_ASSERT_EQUALS(error, KErrNotFound)
    
    error = iSession->SendRtpPacket( iTranstreamId, header, data, status ); 
    RTP_EUNIT_ASSERT_EQUALS(error, KErrNone);
    User::WaitForRequest( iSession->iCommNet->iSender[ERTPPort]->iStatus, status );
    RTP_EUNIT_ASSERT_EQUALS( status.Int(), KRequestPending );
    iSession->iCommNet->iSender[ERTPPort]->RunL();
    iSession->iCommNet->iSender[ERTPPort]->iStatus = TRequestStatus();
    User::WaitForRequest( status );
 	RTP_EUNIT_ASSERT_EQUALS(status, KErrNone)
    }

void UT_CRtpSessionSrtp::UT_CRtpSessionSrtp_SendRtpPacket_OneL(  )
    {
    //Test more branch
       TInt error( KErrNone );

    // Initialise lots of values
    TRtpSendHeader header;
    header.iHeaderExtension = NULL;
    header.iMarker = 0;
    header.iPadding = 0;
    header.iPayloadType = 0;
    header.iTimestamp = 50;

 	TBuf8<KMaxRtpPacketSize+1> dataBig;
    dataBig.Format( _L8( "foo" ) );
    dataBig.SetLength( KMaxRtpPacketSize + 1 );
    TBuf8<5> data;
    data.Format( _L8( "foo" ) );

    TRtpSequence seqNum = 0;
    TRequestStatus status;
    TUint packetSize = 2048;
    
    error = iSession->SendRtpPacket( iTranstreamId, 15 /* seqnum */, header, dataBig, status ); 
    RTP_EUNIT_ASSERT_EQUALS( error, KErrOverflow  );     

    error = iSession->SendRtpPacket( iTranstreamId, 15 /* seqnum */, header, data, status ); 
    RTP_EUNIT_ASSERT_EQUALS(error, KErrNone);
    User::WaitForRequest( iSession->iCommNet->iSender[ERTPPort]->iStatus, status );
    RTP_EUNIT_ASSERT_EQUALS( status.Int(), KRequestPending );
    iSession->iCommNet->iSender[ERTPPort]->RunL();
    iSession->iCommNet->iSender[ERTPPort]->iStatus = TRequestStatus();
    User::WaitForRequest( status );
 	RTP_EUNIT_ASSERT_EQUALS(status, KErrNone)
    }
 
void UT_CRtpSessionSrtp::UT_CRtpSessionSrtp_SendRtpPacket_OneAL(  )
    {
       TInt error( KErrNone );

    // Initialise lots of values
    TRtpSendHeader header;
    header.iHeaderExtension = NULL;
    header.iMarker = 0;
    header.iPadding = 0;
    header.iPayloadType = 0;
    header.iTimestamp = 50;

 
    TBuf8<5> data;
    data.Format( _L8( "foo" ) );

    TRtpSequence seqNum = 0;
    TRequestStatus status;
    TUint packetSize = 2048;
    
    error = iSession->SendRtpPacket( 40/*unknowstreamId*/, 15 /* seqnum */, header, data, status ); 
    EUNIT_ASSERT(error==KErrNotFound);
    
    //Then create stream with different ssrc
    
    TRtpSSRC ssrcOut( 2224 );
    
    TRtpPayloadType payloadType( 0 );
    
    error =iSession->CreateTransmitStreamL( 4, payloadType, ssrcOut );
	RTP_EUNIT_ASSERT_EQUALS(error, KErrNone);
	//try to send packet with this stream and it will not match the stream in the srtp
	error = iSession->SendRtpPacket( 4, 15 /* seqnum */, header, data, status ); 
    RTP_EUNIT_ASSERT_EQUALS(error, KErrNotFound);
    
    error = iSession->SendRtpPacket( iTranstreamId, 15 /* seqnum */, header, data, status ); 
    RTP_EUNIT_ASSERT_EQUALS(error, KErrNone);
    User::WaitForRequest( iSession->iCommNet->iSender[ERTPPort]->iStatus, status );
    RTP_EUNIT_ASSERT_EQUALS( status.Int(), KRequestPending );
    iSession->iCommNet->iSender[ERTPPort]->RunL();
    iSession->iCommNet->iSender[ERTPPort]->iStatus = TRequestStatus();
    User::WaitForRequest( status );
 	RTP_EUNIT_ASSERT_EQUALS(status, KErrNone)
    }

void UT_CRtpSessionSrtp::UT_CRtpSessionSrtp_SendRtpPacket_2L(  )
    {
       TInt error( KErrNone );

    // Initialise lots of values
    TRtpSendHeader header;
    header.iHeaderExtension = NULL;
    header.iMarker = 0;
    header.iPadding = 0;
    header.iPayloadType = 0;
    header.iTimestamp = 50;

  	TBuf8<KMaxRtpPacketSize+1> dataBig;
    dataBig.Format( _L8( "foo" ) );
    dataBig.SetLength( KMaxRtpPacketSize + 1 );
    TBuf8<5> data;
    data.Format( _L8( "foo" ) );

    TRtpSequence seqNum = 0;
    
    
    error = iSession->SendRtpPacket( iTranstreamId, header, dataBig); 
    RTP_EUNIT_ASSERT_EQUALS( error, KErrOverflow  );   
    
    error = iSession->SendRtpPacket( iTranstreamId, header, data); 
    RTP_EUNIT_ASSERT_EQUALS(error, KErrNone);
 	
    }
  
  void UT_CRtpSessionSrtp::UT_CRtpSessionSrtp_SendRtpPacket_TwoAL(  )
    {
       TInt error( KErrNone );

    // Initialise lots of values
    TRtpSendHeader header;
    header.iHeaderExtension = NULL;
    header.iMarker = 0;
    header.iPadding = 0;
    header.iPayloadType = 0;
    header.iTimestamp = 50;

  	TBuf8<5> data;
    data.Format( _L8( "foo" ) );

    TRtpSequence seqNum = 0;
    
     error = iSession->SendRtpPacket( 40, header, data); 
    EUNIT_ASSERT(error==KErrNotFound);
    
    //Then create stream with different ssrc
    
    TRtpSSRC ssrcOut( 2224 );
    
    TRtpPayloadType payloadType( 0 );
    
    error =iSession->CreateTransmitStreamL( 6, payloadType, ssrcOut );
	RTP_EUNIT_ASSERT_EQUALS(error, KErrNone);
	//try to send packet with this stream and it will not match the stream in the srtp
	error = iSession->SendRtpPacket( 6,  header, data ); 
    RTP_EUNIT_ASSERT_EQUALS(error, KErrNotFound);
    
    error = iSession->SendRtpPacket( iTranstreamId, header, data); 
    RTP_EUNIT_ASSERT_EQUALS(error, KErrNone);
    
    }  

void UT_CRtpSessionSrtp::UT_CRtpSessionSrtp_OnRtpReceivedL(  )
    {
    
    
    TRtpSSRC ssrcOut( 5678 );
	
	CSRTPStreamOut* streamOut = CSRTPStreamOut::NewL(*iSRTPSession, ssrcOut);
	CleanupStack::PushL( streamOut );
 
    CRtpPacket* packet = CRtpPacket::NewL( 2048, iProfileRTPTimeRates );
    CleanupStack::PushL( packet );
    TRtpPacketStreamParam aStreamParam;
    TRtpPacketIOParam aIoParam;
    TBuf8<10> data;
    data.Format( _L8( "dummydata" ) );

    // Set the parameters
    aStreamParam.TRTP.SSRC = 432;
    aStreamParam.TRTP.payload= 15;
    aStreamParam.TRTP.seqNum = 30;
    aStreamParam.TRTP.timeStamp = 40;
    aIoParam.TRTP.padding = 0;
    aIoParam.TRTP.marker = 0;
    aIoParam.TRTP.fHeaderExtension = 0;
    aIoParam.TRTP.numCSRC = 0;
    aIoParam.TRTP.payloadData = const_cast<TUint8*>( data.Ptr() );
    aIoParam.TRTP.payloadDataLen = data.Length();
    aIoParam.TRTP.extension.type = 0;
    aIoParam.TRTP.extension.length = 0;
    aIoParam.TRTP.extension.data = 0;
    
    // Build the packet
    packet->SetType( ERTP );
    packet->RtpPacketBuild( &aStreamParam, &aIoParam );
    
    HBufC8* temp =iSRTPSession->ProtectRTPL(5678, packet->Des());
    CleanupStack::PushL( temp );
    
    // Set the session's receive buffer to the packet data
    iSession->iRtpRecvBuf=temp->Des();
	/*testing
	iSession->iRtpRecvBuf.Set( packet->Data(), packet->Size(), packet->Size() );
	packet->RtpPacketResetPtr();   
	EUNIT_ASSERT_SPECIFIC_LEAVE(iSession->OnRtpReceived(),KErrCorrupt );
    */
    //it leaves because the packet build here has different iMKI key.
    iSession->CloseStream(iRecvstreamId);
    iSession->OnRtpReceivedL();
    CleanupStack::PopAndDestroy( temp );
    packet->RtpPacketResetPtr();
    
     // Test when ssrc=0, now the recv stream has been assigne to ssrc 4321
     //and here change ssrc = 22 then no stream will be find and ssrc=0
    aStreamParam.TRTP.SSRC = 22;
    aStreamParam.TRTP.payload= 15;
    aStreamParam.TRTP.seqNum = 30;
    aStreamParam.TRTP.timeStamp = 40;
    aIoParam.TRTP.padding = 0;
    aIoParam.TRTP.marker = 0;
    aIoParam.TRTP.fHeaderExtension = 0;
    aIoParam.TRTP.numCSRC = 0;
    aIoParam.TRTP.payloadData = const_cast<TUint8*>( data.Ptr() );
    aIoParam.TRTP.payloadDataLen = data.Length();
    aIoParam.TRTP.extension.type = 0;
    aIoParam.TRTP.extension.length = 0;
    aIoParam.TRTP.extension.data = 0;
    
    
    packet->SetType( ERTP );
    packet->RtpPacketBuild( &aStreamParam, &aIoParam ); 
    iSession->iRtpRecvBuf.Set( packet->Data(), packet->Size(), packet->Size() );
    iSession->OnRtpReceivedL();
    CleanupStack::PopAndDestroy( packet );
    CleanupStack::PopAndDestroy( streamOut );
    
    }
    
void UT_CRtpSessionSrtp::UT_CRtpSessionSrtp_OnRtpReceived_OneL(  )
    {
    
    //cover more branch
    TRtpSSRC ssrcOut( 5678 );
	
	CSRTPStreamOut* streamOut = CSRTPStreamOut::NewL(*iSRTPSession, ssrcOut);
	CleanupStack::PushL( streamOut );
 
    CRtpPacket* packet = CRtpPacket::NewL( 2048, iProfileRTPTimeRates );
    CleanupStack::PushL( packet );
    TRtpPacketStreamParam aStreamParam;
    TRtpPacketIOParam aIoParam;
    TBuf8<10> data;
    data.Format( _L8( "dummydata" ) );

    // Set the parameters
    aStreamParam.TRTP.SSRC = 5678;
    aStreamParam.TRTP.payload= 15;
    aStreamParam.TRTP.seqNum = 30;
    aStreamParam.TRTP.timeStamp = 40;
    aIoParam.TRTP.padding = 0;
    aIoParam.TRTP.marker = 0;
    aIoParam.TRTP.fHeaderExtension = 0;
    aIoParam.TRTP.numCSRC = 0;
    aIoParam.TRTP.payloadData = const_cast<TUint8*>( data.Ptr() );
    aIoParam.TRTP.payloadDataLen = data.Length();
    aIoParam.TRTP.extension.type = 0;
    aIoParam.TRTP.extension.length = 0;
    aIoParam.TRTP.extension.data = 0;
    
    // Build the packet
    packet->SetType( ERTP );
    packet->RtpPacketBuild( &aStreamParam, &aIoParam );
    
    HBufC8* temp =iSRTPSession->ProtectRTPL(5678, packet->Des());
    CleanupStack::PushL( temp );
    
    // Set the session's receive buffer to the packet data
    iSession->iRtpRecvBuf=temp->Des();

    //it leaves because the packet build here has different iMKI key.
   iSession->OnRtpReceivedL();
    CleanupStack::PopAndDestroy( temp );
    packet->RtpPacketResetPtr();
    
    CleanupStack::PopAndDestroy( packet );
    CleanupStack::PopAndDestroy( streamOut );
    
    }    

void UT_CRtpSessionSrtp::UT_CRtpSessionSrtp_SendSRTCPReportL()
	{
	TInt error( KErrNone );
	TRtpSSRC ssrcOut( 1234 );
	TBuf8<5> data;
    data.Format( _L8( "foo" ) );

	CSRTPStreamOut* streamOut = CSRTPStreamOut::NewL(*iSRTPSession, ssrcOut);
	CleanupStack::PushL( streamOut );
	CRtpPacket* packet = CRtpPacket::NewL( 2048, iProfileRTPTimeRates );
	CleanupStack::PushL( packet );	
    // Initialise lots of values
    
    TRtpPacketStreamParam aStreamParam;
    TRtpPacketIOParam  aIoParam; 
    TRtpSendHeader header;
// Set the parameters
    aStreamParam.TRTP.SSRC = 5678;
    aStreamParam.TRTP.payload= 15;
    aStreamParam.TRTP.seqNum = 30;
    aStreamParam.TRTP.timeStamp = 40;
    aIoParam.TRTP.padding = 0;
    aIoParam.TRTP.marker = 0;
    aIoParam.TRTP.fHeaderExtension = 0;
    aIoParam.TRTP.numCSRC = 0;
    aIoParam.TRTP.payloadData = const_cast<TUint8*>( data.Ptr() );
    aIoParam.TRTP.payloadDataLen = data.Length();
    aIoParam.TRTP.extension.type = 0;
    aIoParam.TRTP.extension.length = 0;
    aIoParam.TRTP.extension.data = 0;
 
 // Build the packet
    packet->SetType( ERTP );
    packet->RtpPacketBuild( &aStreamParam, &aIoParam );
    
    HBufC8* temp =iSRTPSession->ProtectRTPL(1234, packet->Des());
    CleanupStack::PushL( temp );
    
    // Set the session's receive buffer to the packet data
    iSession->iRtpRecvBuf=temp->Des();

 
    //Then start rtcp packet sending
    TBuf8<5> reason;
    reason.Format( _L8( "foo" ) );
 	TUint streamAddress( 0 );
    TInt ret( iSession->FindStream( iTranstreamId, streamAddress ) );

    if ( ret == KErrNone )
        {
        CRtpTranStream* tempStream =
            reinterpret_cast<CRtpTranStream*>( streamAddress );
        tempStream->iFlagFirstPkg=EFalse;   
        TInt retVal = iSession->BuildRTCPReport( tempStream, NULL ); 
        RTP_EUNIT_ASSERT_EQUALS( retVal, KErrNone );       
        }
        
    iSession->SendSRTCPReportL( 1234/* valid Tx stream*/ );
	iSession->iRtcpEnabled = EFalse;
	iSession->SendSRTCPReportL( KErrNone );
	iSession->iRtcpEnabled = ETrue;
	
  	CleanupStack::PopAndDestroy( temp );
  	CleanupStack::PopAndDestroy( packet );
  	CleanupStack::PopAndDestroy( streamOut );
	
	}
	
void UT_CRtpSessionSrtp::UT_CRtpSessionSrtp_OnRtcpReceivedL()
	{
	HBufC8* encSrtcpPacket =HBufC8::NewLC(KRTCPPacket().Length());
	*encSrtcpPacket=KRTCPPacket;
    Hex(*encSrtcpPacket);
    TInt len= KRTCPPacket().Length();
	encSrtcpPacket->Des().SetLength(len/2);
	
  
    // Set the session's receive buffer to the packet data
    iSession->iRtcpRecvBuf= encSrtcpPacket->Des();

    //it leaves because the packet build here has different iMKI key.
   iSession->OnRtcpReceivedL();
    CleanupStack::PopAndDestroy(encSrtcpPacket);
    
    
	}	
void UT_CRtpSessionSrtp::ErrorNotify( TInt /*aErrCode*/ )
	{
	
	}
void UT_CRtpSessionSrtp::RtpPacketReceived( TRtpId /*aStreamId*/,
                                const TRtpRecvHeader& /*aHeaderInfo*/, 
                                const TDesC8& /*aPayloadData*/ )
{
   
}
    	// from MRtcpObserver

void UT_CRtpSessionSrtp::SdesReceived( TRtpSSRC /*aSSRC*/,
                           const TRtpSdesParams& /*aParams*/ )
	{
	
	}    
void UT_CRtpSessionSrtp::ByeReceived( TRtpId /*aStreamId*/, TRtpSSRC /*aSSRC*/,
                          const TDesC8& /*aReason*/ )
	{
	
	}
void UT_CRtpSessionSrtp::AppReceived( TRtpId /*aStreamId*/, TRtpSSRC /*aSSRC*/,const TRtcpApp& /*aApp*/ )
	{
	
	}                          
    
void UT_CRtpSessionSrtp::SrReceived( TRtpId /*aStreamId*/, 
							TRtpSSRC /*aSSRC*/,const TTimeStamps& /*aTimeStamps*/ )
	{
		
	}                         
    
void UT_CRtpSessionSrtp::RrReceived( TRtpId /*aStreamId*/, TRtpSSRC /*aSSRC*/ )
	{
		
	}
		// from MNonRTPDataObserver
void UT_CRtpSessionSrtp::NonRTPDataReceived( TUint /*aPort*/, TBool /*aRTPport*/, 
									const TDesC8& /*aNonRTPData*/ )	
	{
		
	}
void UT_CRtpSessionSrtp::OnExpiredL( TInt /*aStatus*/ )
	{
	}
TRtpId UT_CRtpSessionSrtp::AssignUniqueID()
	{
	return ++iNumOfObjects;
	}
	
TInt UT_CRtpSessionSrtp::AddStreamToSession(TRtpId /*aSessionId*/, TRtpId /*aStreamId*/)
	{
	//call back function
	return KErrNone;
	}

void UT_CRtpSessionSrtp::Write16( TUint8* const aPointer, TUint16 aValue )
    {
    // check value range (16 bits)
    aPointer[0] = static_cast<TUint8>( ( aValue & 0xFF00 ) >> 8 );
    aPointer[1] = static_cast<TUint8>( aValue & 0x00FF );
    }
    
//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CRtpSessionSrtp,
    "CRtpSessionSrtp",
    "UNIT" )

EUNIT_TEST(
    "NewL - test ",
    "CRtpSessionSrtp",
    "NewL",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSessionSrtp_NewLL, Teardown)

EUNIT_TEST(
    "SendRtpPacket - test ",
    "CRtpSessionSrtp",
    "SendRtpPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSessionSrtp_SendRtpPacketL, Teardown)
EUNIT_TEST(
    "SendRtpPacket2 - test ",
    "CRtpSessionSrtp",
    "SendRtpPacketwNoStreamFound",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSessionSrtp_SendRtpPacket1L, Teardown)

EUNIT_TEST(
    "SendRtpPacket3 - test ",
    "CRtpSessionSrtp",
    "SendRtpPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSessionSrtp_SendRtpPacket_OneL, Teardown)

EUNIT_TEST(
    "SendRtpPacket4 - test ",
    "CRtpSessionSrtp",
    "SendRtpPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSessionSrtp_SendRtpPacket_OneAL, Teardown)

EUNIT_TEST(
    "SendRtpPacket5 - test ",
    "CRtpSessionSrtp",
    "SendRtpPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSessionSrtp_SendRtpPacket_2L, Teardown)

EUNIT_TEST(
    "SendRtpPacket6 - test ",
    "CRtpSessionSrtp",
    "SendRtpPacket",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSessionSrtp_SendRtpPacket_TwoAL, Teardown)

EUNIT_TEST(
    "OnRtpReceived1 - test ",
    "CRtpSessionSrtp",
    "OnRtpReceived",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSessionSrtp_OnRtpReceivedL, Teardown)

EUNIT_TEST(
    "OnRtpReceived2 - test ",
    "CRtpSessionSrtp",
    "OnRtpReceived",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSessionSrtp_OnRtpReceived_OneL, Teardown)

EUNIT_TEST(
    "SendSRTCPReport ",
    "CRtpSessionSrtp",
    "SendSRTCPReport",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSessionSrtp_SendSRTCPReportL, Teardown)
EUNIT_TEST(
    "OnRtcpReceived ",
    "CRtpSessionSrtp",
    "OnRtcpReceived",
    "FUNCTIONALITY",
    SetupL, UT_CRtpSessionSrtp_OnRtcpReceivedL, Teardown)

  
EUNIT_END_TEST_TABLE

//  END OF FILE
