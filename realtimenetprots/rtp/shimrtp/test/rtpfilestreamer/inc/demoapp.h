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
// This file and DemoApp.cpp define a simple textshell user iterface for an
// RTPFileStreamer demo.  See RTPFileStreamer.h/.cpp for example usage of RTP
// APIs
// 
//

/**
 @file
 @internalComponent
*/


#ifndef __DEMOAPP_H__
#define __DEMOAPP_H__

#include <e32test.h>
#include <bacline.h>
#include "rtpfilestreamer.h"

class MActiveConsoleNotify
	{
public:
	virtual void KeyPressed(TChar aKey) =0;
	};


class CActiveConsole : public CActive
	{
	public:
		static CActiveConsole* NewL(MActiveConsoleNotify& aNotify,const TDesC& aTitle,const TSize& aSize);
		void RequestKey();
		
		inline CConsoleBase& Console() const 
			{
			return *iConsole;
			};

		~CActiveConsole();

	private:
		void RunL();
		void DoCancel();
		void DrawCursor();
		CActiveConsole(MActiveConsoleNotify& aNotify);
		void ConstructL(const TDesC& aTitle,const TSize& aSize);


	private:
		CConsoleBase*			iConsole;
		MActiveConsoleNotify&	iNotify;
	};
	
class CDemoApp : public CBase, public MActiveConsoleNotify, public MFileStreamerObserver
	{
public:
	static CDemoApp* NewL();

	void StartL();
	void Stop();
	~CDemoApp();
	//void TestMenu();
	void KeyPressed(TChar aKey);
	void ReadError(TInt aError);
public:
	virtual void NotifyPacketSent();
	virtual void NotifyPacketReceived();
	virtual void NotifyComplete();
	virtual void NotifyError();

private:
	CDemoApp();
	void ConstructL();
	void DrawMonitor();
	
private:
	CActiveConsole*			iActiveConsole;
	
	CActiveConsole*			iMonitorConsole;
	TInt					iSent;
	TInt					iRecv;
	
	CRtpFileStreamer*		iStreamer;
	RSocketServ				iSockServ;
	};

#endif