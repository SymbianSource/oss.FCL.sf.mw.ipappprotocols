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
// class for the rtp demo app
// 
//

/**
 @file
 @internalComponent
*/
 
#include "rtpfilestreamer.h"
#include "demoapp.h"

// ----------------------------------------------------------------------------
//							Active Console
// ----------------------------------------------------------------------------


CActiveConsole::CActiveConsole(MActiveConsoleNotify& aNotify)
: CActive(EPriorityStandard), iNotify(aNotify)
	{
	CActiveScheduler::Add(this);
	}


CActiveConsole::~CActiveConsole()
	{
	delete iConsole;
	Cancel();
	}

CActiveConsole* CActiveConsole::NewL(MActiveConsoleNotify& aNotify,const TDesC& aTitle,const TSize& aSize)
	{
	CActiveConsole* console = new (ELeave) CActiveConsole(aNotify);
	CleanupStack::PushL(console);
	console->ConstructL(aTitle,aSize);
	CleanupStack::Pop();
	return console;
	}

void CActiveConsole::ConstructL(const TDesC& aTitle,const TSize& aSize)
	{
	iConsole = Console::NewL(aTitle,aSize);
	}

void CActiveConsole::DoCancel()
	{
	iConsole->ReadCancel();
	}

void CActiveConsole::RequestKey()
	{
	DrawCursor();
	iConsole->Read(iStatus);
	SetActive();
	}

void CActiveConsole::DrawCursor()
	{
	iConsole->Printf(_L(">>"));
	}

void CActiveConsole::RunL()
	{
	// key has been pressed
	TChar ch = iConsole->KeyCode();
	iNotify.KeyPressed(ch);
	}


CDemoApp::CDemoApp()
	{
	}

CDemoApp::~CDemoApp()
	{
	delete iActiveConsole;
	delete iMonitorConsole;
	iSockServ.Close();
	}

CDemoApp* CDemoApp::NewL()
	{
	CDemoApp* thisapp = new (ELeave) CDemoApp();
	CleanupStack::PushL(thisapp);
	thisapp->ConstructL();
	CleanupStack::Pop();
	return thisapp;
	}

GLDEF_D TSize gMainConsole(KConsFullScreen,KConsFullScreen);
GLDEF_D TSize gTinyConsole(40,10);

void CDemoApp::ConstructL()
	{
	iActiveConsole = CActiveConsole::NewL(*this,_L("Rtp App"),gMainConsole);
	iMonitorConsole = CActiveConsole::NewL(*this,_L("Status"),gTinyConsole);
	User::LeaveIfError(iSockServ.Connect());
	
	}

void CDemoApp::StartL()
	{	
	// go ....
	CCommandLineArguments* args = CCommandLineArguments::NewLC();
	TInt argc = args->Count();
	
	if ((args->Count()!=7) && (args->Count()!=8) )
		{
		iActiveConsole->Console().Printf(_L("Usage:\n\
		rtpfilestreamer destaddr destport localport srcfilename destfilename payloadsize [connid]"));		
		iActiveConsole->Console().Getch();
		}
	else
		{
		TPtrC daddrDes = args->Arg(1);
		TPtrC dportDes = args->Arg(2);
		TPtrC lportDes = args->Arg(3);
		TPtrC sfilename = args->Arg(4);
		TPtrC dfilename = args->Arg(5);
		TPtrC psizDes = args->Arg(6);
		
		TPtrC conIdDes(KNullDesC);
		if (args->Count()==8)
			{
			conIdDes.Set(args->Arg(7));
			}
		
		TInt dport;
		TInt lport;
		TInt psize;
		TInetAddr daddr;
		TLex parser;
		parser.Assign(dportDes);
		User::LeaveIfError(parser.Val(dport));
		parser.Assign(psizDes);
		User::LeaveIfError(parser.Val(psize));
		parser.Assign(lportDes);
		User::LeaveIfError(parser.Val(lport));
		
		RHostResolver resolver;
		User::LeaveIfError(resolver.Open(iSockServ,KAfInet,KProtocolInetUdp));
		CleanupClosePushL(resolver);
		TNameEntry entry;
		TRequestStatus status;
		resolver.GetByName(daddrDes,entry,status);
		User::WaitForRequest(status);
		User::LeaveIfError(status.Int());
		CleanupStack::PopAndDestroy(1);//close resolver
		
		TSockAddr remote = entry().iAddr;
		remote.SetPort(dport);
		
		TInt connId=KErrNotFound;
		if (args->Count()==8)
			{
			parser.Assign(conIdDes);
			User::LeaveIfError(parser.Val(connId));
			}
		
		iActiveConsole->Console().Printf(_L("Streaming %S to %S:%d, saving received frames in %S.\n"),&sfilename,&daddrDes,dport,&dfilename);
		iStreamer = CRtpFileStreamer::NewL(iSockServ,sfilename,dfilename,psize,remote,lport,connId);
		iStreamer->SetObserver(*this);
		iStreamer->StartL();
		CActiveScheduler::Start();	
		}
	CleanupStack::PopAndDestroy(args);
	}

void CDemoApp::Stop()
	{
	if (iStreamer)
		{
		delete iStreamer;
		iStreamer = 0;
		}
	CActiveScheduler::Stop();
	}
	
	
void CDemoApp::DrawMonitor()
	{
	iMonitorConsole->Console().ClearScreen(); //SetPos(0,0);
	iMonitorConsole->Console().Printf(_L("Sent: %d\nRecv: %d"),iSent,iRecv);
	}
	
void CDemoApp::NotifyPacketSent()
	{
	iSent++;
	DrawMonitor();
	iActiveConsole->Console().Printf(_L("s"));
	}
	
void CDemoApp::NotifyPacketReceived()
	{
	iRecv++;
	DrawMonitor();
	iActiveConsole->Console().Printf(_L("r"));
	}
	
void CDemoApp::NotifyComplete()
	{
	iMonitorConsole->Console().Printf(_L("Complete\n"));
	if (iStreamer)
		{
		delete iStreamer;
		iStreamer = 0;
		}
	CActiveScheduler::Stop();
	}
	
void CDemoApp::NotifyError()
	{
	iMonitorConsole->Console().Printf(_L("Error\n"));
	iActiveConsole->Console().Printf(_L("E"));
	}

void CDemoApp::KeyPressed(TChar /*aKey*/)	
	{
	
	}

	
	
void MainL()
	{
	CDemoApp* app = CDemoApp::NewL();
	CleanupStack::PushL(app);
	app->StartL();
	CleanupStack::PopAndDestroy(app);
	//finished
	}

TInt E32Main()
	{
	__UHEAP_MARK;

	CTrapCleanup* cleanupStack=CTrapCleanup::New();	// Get CleanupStack

	CActiveScheduler* activescheduler=new CActiveScheduler;
	CActiveScheduler::Install(activescheduler);

	TRAPD(err, MainL());
	if (err != KErrNone)
		{
		return err;
		}
	delete activescheduler;
	delete cleanupStack;	
	
	__UHEAP_MARKEND;
	return KErrNone;
	}

