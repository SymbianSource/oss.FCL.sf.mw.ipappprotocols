// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


// HEADERS
#include <connpref.h>
#include <es_sock.h>
#include <commdbconnpref.h>
#include "in_sock.h"
#include <e32cons.h>
#include <e32err.h>
#include <e32std.h>
#include <e32base.h>
#include <badesca.h>
#include <random.h>
#include "rtpheader.h"
#include "rtptestapp.h"

const TUint KIAPID = 1;
const TInt KLength = 20;
_LIT8(KHello, "RTP APIs...testing");
_LIT8(KNoCName, "");

CConsoleBase *gConsole;
CActiveScheduler *sched;

void MainL()
    {
    // Create a Console 
    gConsole = Console::NewL(_L("Test Console"), TSize(KConsFullScreen, KConsFullScreen));
    CleanupStack::PushL(gConsole);
    
    // Create an Active scheduler 
    sched = new (ELeave) CActiveScheduler;
    CActiveScheduler::Install(sched);
    
    //IAP(Internet Access Point) id to be chaged as per comms dat
    CRtpTestApp * iRtpTestApp = CRtpTestApp::NewL();   
    
    // Send RTP Packet with CSRC and Check RTP Stack 
    // Supports maximum 15 contributing sources
    iRtpTestApp->SendRtpPacketL();
    
    // Send RTP packet with zero length paylpoad
    iRtpTestApp->SendZeroLengthRtpPacketL();
    
    // Send RTP Packets and Modify packet in callback function
    iRtpTestApp->SendRtpPacketLoopbackL();
    
    
    User::After(5000000);
    delete iRtpTestApp;
    delete sched;
    CleanupStack::PopAndDestroy(gConsole);
    
    }

CRtpTestApp * CRtpTestApp::NewL()
{
    gConsole->Printf(_L("RtpTestApp::NewL\n"));
    CRtpTestApp* self = new( ELeave ) CRtpTestApp(); 
    CleanupStack::PushL( self );
    
    // To construct the objects in the second-phase of the construction
    self->ConstructL( );
    CleanupStack::Pop( self);
    return self;
}

CRtpTestApp::CRtpTestApp()
    {
    
    }


void CRtpTestApp::ErrorNotify( TInt /*aErrCode*/ )
    {
    
    }

void CRtpTestApp::SdesReceived( TRtpSSRC /*aSSRC*/,
                   const TRtpSdesParams& /*aParams*/ )
    {
    
    }

void CRtpTestApp::ByeReceived( TRtpId /*aStreamId*/, TRtpSSRC /*aSSRC*/,
                  const TDesC8& /*aReason*/ )
    {
    
    }

void CRtpTestApp::AppReceived( TRtpId /*aStreamId*/, TRtpSSRC /*aSSRC*/,
                  const TRtcpApp& /*aApp*/ )
    {
    
    }

void CRtpTestApp::SrReceived( TRtpId /*aStreamId*/, TRtpSSRC /*aSSRC*/,
                 const TTimeStamps& /*aTimeStamps*/ )
    {
    
    }

void CRtpTestApp::RrReceived( TRtpId /*aStreamId*/, TRtpSSRC /*aSSRC*/ )
    {
    
    }

void CRtpTestApp::RtpPacketReceived( TRtpId /*aStreamId*/, const TRtpRecvHeader& aHeaderInfo, const TDesC8& /*aPayloadData*/ )
    {
    gConsole->Printf(_L("RtpPacketReceived .. \n"));
    
    for(int i=0; i<aHeaderInfo.iCsrcCount; i++)
        {
        if ( (*iCsrclist)[i] == *(aHeaderInfo.iCsrcList+i) )
            {
            gConsole->Printf(_L("RtpPacketReceived, Successfully\n"));
            }
        else
            {
            gConsole->Printf(_L("Malformed RTP Packet Received\n"));
            gConsole->Printf(_L("Test Case Failed\n"));
            }
        }
    
    // Check for modified Padding, header extension, Marker bit values 1
    if( aHeaderInfo.iMarker == 0 )
        {
        gConsole->Printf(_L("Test Case Failed\n"));
        }
    }

void CRtpTestApp::ReadyToSendRtpPacket(  TRtpId /*aTranStreamId*/, TPtr8 &aPacket  )
    {
    gConsole->Printf(_L("ReadyToSendRtpPacket Callback..\n"));
    
    // Change Marker bit value from 0 to 1
    aPacket[1] = aPacket[1] | 0x80;
    
    TUint32* data32 = reinterpret_cast<TUint32*>(&(aPacket[8]));
    //Change SSRC Value from
    *data32 = ByteOrder::Swap32(0xDEADBEEF);
    }

void CrtpDataSender::RunL()
    {
    gConsole->Printf(_L("CrtpDataSender::RunL\n"));
    CActiveScheduler::Stop();
    }

void CRtpTestApp::ConstructL()
    {
    iRtpApi = CRtpAPI::NewL(*this );

    TVersion  version = iRtpApi->Version();
    
    TInt result(KErrNone);
    
    iParams.iCName.Set(KNoCName());

    iRtpApi->OpenL( iParams, NULL, NULL, NULL );
    
    result = iRtpApi->StartConnection(KIAPID);
    gConsole->Printf(_L("Local Ip Address %d\n"),result);
    
    TBuf<50> addrStr;
    TInetAddr inetaddr = iRtpApi->GetLocalIPAddressL();
    inetaddr.Output(addrStr);
    
    gConsole->Printf(_L("Local Ip Address %s\n"),addrStr.PtrZ());
    
    iSessionParams.iPriority = TCreateSessionParams::EPriorityStandard;
    iSessionParams.iSocketBufSize = KSocketBufSize;
    
    
    iPort1 = 8000 ;
    iPort2 = 9000;

    iRtcpParams.iRtcpFraction = 10;
    iRtcpParams.iRtcpTimeOut = 5000000;
    iRtcpParams.iSessionBWidth = 300;
    
    iEnableRtcp = EFalse;
    
    //Creating New session
    iSessionId1  = KNullId;
    iSessionId2 = KNullId;
    iSessionId1 = iRtpApi->CreateSessionL(iSessionParams, iPort1, iEnableRtcp, &iRtcpParams);
    iSessionId2 = iRtpApi->CreateSessionL(iSessionParams,iPort2,EFalse,&iRtcpParams);
    
    TInetAddr remAddress = iRtpApi->GetLocalIPAddressL(); 
    remAddress.SetPort(iPort2); //Session 1 sends data to Session 2
    
    iRtpApi->SetRemoteAddress(iSessionId1, remAddress);
     
    iRtpApi->RegisterRtpObserver(iSessionId1, *this);
    iRtpApi->RegisterRtcpObserver(iSessionId1, *this);
    iRtpApi->RegisterRtpObserver(iSessionId2, *this);
    iRtpApi->RegisterRtcpObserver(iSessionId2, *this);
    iRtpApi->RegisterRtpPostProcessingObserver(iSessionId1, *this);
    
    TTranStreamParams streamParams;
    streamParams.iPayloadType = 98;
    TRtpSSRC locSSRC;
    
    iS1T1 = iRtpApi->CreateTransmitStreamL(iSessionId1,streamParams,locSSRC);
    
    TRcvStreamParams rcvParams;
    rcvParams.iPayloadType = 98;

    iS2R1 = iRtpApi->CreateReceiveStreamL(iSessionId2,rcvParams);
    
    
    /* Start both the sessions */
    TInt ret = iRtpApi->StartSession(iSessionId1);
    ret = iRtpApi->StartSession(iSessionId2);
    
    iRtpApi->SetSamplingRate(98,8000);
    }


void CRtpTestApp::SendRtpPacketL()
    {
    gConsole->Printf(_L("SendRtpPackets  TO Loopback address and Verify Header fields.. \n"));
    
    TRtpSendHeader headerinfo;
    headerinfo.iHeaderExtension = NULL;
    headerinfo.iMarker = 0;
    headerinfo.iPadding = 0;
    headerinfo.iPayloadType = 98;
    headerinfo.iTimestamp = 1000;
    
    HBufC8* data = HBufC8::NewLC(KLength);
    data->Des().Copy(KHello);

    
    TRtpSendPktParams *header = new(ELeave) TRtpSendPktParams(headerinfo);
    CleanupDeletePushL(header);
    header->iTranStreamId = iS1T1;
    header->iPayloadData.Set(data->Des()); 
    
    const TUint KArrayGranularity = 15;
    
    iCsrclist = new(ELeave) CArrayFixFlat<TUint32>(KArrayGranularity);
    
    // Send RTP Packets with CSRC list - 17 CSRC. Max Supported CSRC list is 15.
    iCsrclist->AppendL(100001);
    iCsrclist->AppendL(100002);
    iCsrclist->AppendL(100003);
    iCsrclist->AppendL(100004);
    iCsrclist->AppendL(100005);
    iCsrclist->AppendL(100006);
    iCsrclist->AppendL(100007);
    iCsrclist->AppendL(100008);
    iCsrclist->AppendL(100009);
    iCsrclist->AppendL(100010);
    iCsrclist->AppendL(100011);
    iCsrclist->AppendL(100012);
    iCsrclist->AppendL(100013);
    iCsrclist->AppendL(100014);
    iCsrclist->AppendL(100015);
    iCsrclist->AppendL(100016);
    iCsrclist->AppendL(100017);
    
    iRtpApi->SetSamplingRate(headerinfo.iPayloadType, 8000);
    TInt ret1 = iRtpApi->SendRtpPacket(*header, iCsrclist->Array());
    
    CrtpDataSender *datasend = CrtpDataSender::NewL(iRtpApi, iS1T1);
    CleanupStack::PushL(datasend);
    
    datasend->SendRtpData(*header, iCsrclist->Array());
    
    gConsole->Printf(_L("SendRtpPacket %d\n"),ret1);
    
    CActiveScheduler::Start();
    
    CleanupStack::PopAndDestroy(3);
    delete iCsrclist;
    iCsrclist = 0;
    }


void CRtpTestApp::SendRtpPacketLoopbackL()
    {
    gConsole->Printf(_L("SendRtpPackets  TO Loopback address and Verify Header fields.. \n"));

    TRtpSendHeader header; 
    header.iHeaderExtension = NULL;
    header.iMarker = 0;
    header.iPadding = 0;
    header.iPayloadType = 98;
    header.iTimestamp = 1000;
    
    HBufC8* data = HBufC8::NewLC(KLength);
    data->Des().Copy(KHello);

    TRtpSendPktParams *headerinfo = new(ELeave) TRtpSendPktParams(header);
    CleanupDeletePushL(headerinfo);
    headerinfo->iTranStreamId = iS1T1;
    headerinfo->iPayloadData.Set(data->Des()); 
    
    const TUint KArrayGranularity = 15;
    
    // Create CSRC list and Append CSRC identifiers
    iCsrclist = new(ELeave) CArrayFixFlat<TUint32>(KArrayGranularity);
    
    iCsrclist->AppendL(123456);
    iCsrclist->AppendL(999999);
     
    TInt error2( KErrNone );
    // Try sending synchronously with Csrc
    error2 = iRtpApi->SendRtpPacket(*headerinfo, iCsrclist->Array());
    gConsole->Printf(_L("SendRtpPacket.. %d\n"), error2);
    
    // Try sending asynchronously with CSRC
    CrtpDataSender *datasend = CrtpDataSender::NewL(iRtpApi, iS1T1);
    CleanupStack::PushL(datasend);
    datasend->SendRtpData(*headerinfo, iCsrclist->Array());
    gConsole->Printf(_L("SendRtpPacket.. %d\n"), error2);
    
    CActiveScheduler::Start();
    
    CleanupStack::PopAndDestroy(3);
    delete iCsrclist;
    iCsrclist = 0;
    }

void CRtpTestApp::SendZeroLengthRtpPacketL()
    {
    // Start Session on Session id
    iRtpApi->StartSession(iSessionId1);
    
    TInetAddr remAddress = iRtpApi->GetLocalIPAddressL();
    remAddress.SetPort(9000); //Session 1 sends data to Session 2
    
    iRtpApi->SetRemoteAddress(iSessionId1, remAddress);
    
    iRtpApi->RegisterRtpObserver(iSessionId1, *this);
    iRtpApi->RegisterRtcpObserver(iSessionId1, *this);
    iRtpApi->RegisterRtpPostProcessingObserver(iSessionId1, *this);
    
    //iRtpApi->SetNonRTPDataObserver(iSessionId1, *this);
    
    TTranStreamParams tranStreamParams;
    tranStreamParams.iPayloadType = 96;
    TRtpSSRC ssrc = 4567890;
    
    TRtpId streamId = iRtpApi->CreateTransmitStreamL(iSessionId1, tranStreamParams, ssrc);

    
    TRtpSendHeader headerinfo;
    headerinfo.iHeaderExtension = 0;
    headerinfo.iPayloadType = 96;
    headerinfo.iTimestamp = 12345678;
    
    TRtpSendPktParams *header = new(ELeave) TRtpSendPktParams(headerinfo);
    CleanupDeletePushL(header);
    header->iTranStreamId = streamId;
    
    const TUint KArrayGranularity = 15;
    
    iCsrclist = new(ELeave) CArrayFixFlat<TUint32>(KArrayGranularity);
    
    // Send RTP Packets with CSRC list
    iCsrclist->AppendL(100001);
    iCsrclist->AppendL(100002);
    
    iRtpApi->SetSamplingRate(headerinfo.iPayloadType, 8000);
    
    TInt ret1 = iRtpApi->SendRtpPacket(*header, iCsrclist->Array());
    
    CrtpDataSender *datasend = CrtpDataSender::NewL(iRtpApi, streamId);
    CleanupStack::PushL(datasend);
    datasend->SendRtpData(*header, iCsrclist->Array());
    
    CActiveScheduler::Start();
    
    CleanupStack::PopAndDestroy(2);
    delete iCsrclist;
    iCsrclist = 0;
    }

CRtpTestApp::~CRtpTestApp()
    {
    delete iRtpApi;
    }

GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on exit
 */
    {
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    if(!cleanup)
        {
        return KErrNoMemory;
        }
    TInt err = 0;
    TRAP(err,MainL());
    delete cleanup;
    __UHEAP_MARKEND;
    return err;
    }


