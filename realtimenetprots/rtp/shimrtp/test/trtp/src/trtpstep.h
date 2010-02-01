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

#if (!defined TRTPSTEP_H)
#define TRTPSTEP_H

#include <testexecutestepbase.h>
#include "trtpserver.h"

const TInt KDefaultMaxPcktSize = 1024;
const TInt KDefaultPriority = 0;

class CTrtpStep : public CTestStep
	{
public:
	inline void SetServer(CTrtpServer* aServer);
	virtual TVerdict doTestStepL();
	virtual TVerdict trtpDoTestStepL() = 0;
	TInt iError;
	TBool iIgnoreIncrease;
	void GotEvent(TRtpEventType aType);
	void ResetEvents();
	

	
protected:

	TBool iGotSdes;
	TBool iGotApp;
	TBool iGotSr;
	TBool iGotRR;
	TBool iGotBye;

	CTrtpServer* iServer;
	};

inline void CTrtpStep::SetServer(CTrtpServer* aServer)
	{
	iServer = aServer;
	}

class CMakePacket : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};

class CClosePacket : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};

class CSetPayloadType : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};

class CPayloadType : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};

class CMarker : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};

class CSetMarker : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};

class CExtension : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};

class CFlags : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};

class CSetFlags : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};

class CSequenceNumber : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};

class CTimestamp : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};

class CSetTimestamp : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};

class CSSRC : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};

class COpenSession : public CTrtpStep
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict trtpDoTestStepL();
	};

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
class COpenSession2 : public CTrtpStep
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict trtpDoTestStepL();
	};
class CCloseSession2 : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};
class CTidyUp2 : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};
#endif

class CCloseSession : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};
class CTidyUp : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};
class CRxPacket : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	virtual TVerdict doTestStepPostambleL();
	};

class CPayload : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};

class CSetRxSize : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};

class CCheckEvent : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};

class CEvent : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};

class COom : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};

class CCSRCs : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};

class CExtensionContent : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};

class CSend : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};

class CSetPayload : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};

class CSetAlignment : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};

class CSetStreamPayloadType : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};

class CCheckStatus : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};

class CWorkConnected : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};

class COpenSessionRConnect : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	virtual TVerdict doTestStepPreambleL();
	};
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
class COpenSessionRConnect2 : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	virtual TVerdict doTestStepPreambleL();
	};
class COpenSessionRtcp2 : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	virtual TVerdict doTestStepPreambleL();
	};

class COpenSessionRtcpRemoved2 : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};
class COpenSessionRemoved2 : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};
#endif
class COpenSessionRtcpRemoved : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};
class COpenSessionRemoved : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	};
class COpenSessionRtcp : public CTrtpStep
	{
public:
	virtual TVerdict trtpDoTestStepL();
	virtual TVerdict doTestStepPreambleL();
	};


_LIT(KMakePacket,"MakePacket");
_LIT(KClosePacket,"ClosePacket");
_LIT(KPayloadType,"PayloadType");
_LIT(KSetPayloadType,"SetPayloadType");
_LIT(KMarker,"Marker");
_LIT(KSetMarker,"SetMarker");
_LIT(KExtension,"Extension");
_LIT(KFlags,"Flags");
_LIT(KSetFlags,"SetFlags");
_LIT(KSequenceNumber,"SequenceNumber");
_LIT(KTimestamp,"Timestamp");
_LIT(KSetTimestamp,"SetTimestamp");
_LIT(KSSRC,"SSRC");
_LIT(KOpenSession,"OpenSession");
_LIT(KOpenSessionRemoved, "OpenSessionRemoved");
_LIT(KCloseSession,"CloseSession");
_LIT(KTidyUp,"TidyUp");
_LIT(KRxPacket,"RxPacket");
_LIT(KPayload,"Payload");
_LIT(KSetRxSize,"SetRxSize");
_LIT(KCheckEvent,"CheckEvent");
_LIT(KEvent,"Event");
_LIT(KOom,"Oom");
_LIT(KCSRCs,"CSRCs");
_LIT(KExtensionContent,"ExtensionContent");
_LIT(KSend,"Send");
_LIT(KSetPayload,"SetPayload");
_LIT(KSetAlignment,"SetAlignment");
_LIT(KSetStreamPayloadType,"SetStreamPayloadType");
_LIT(KCheckStatus,"CheckStatus");
_LIT(KWorkConnected,"WorkConnected");
_LIT(KOpenSessionRConnect, "OpenSessionRConnect");
_LIT(KOpenSessionRtcp, "OpenSessionRtcp");
_LIT(KOpenSessionRtcpRemoved, "OpenSessionRtcpRemoved");
_LIT(KRtcpTest, "RtcpTest");

#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
_LIT(KOpenSession2,"OpenSession2");
_LIT(KOpenSessionRemoved2, "OpenSessionRemoved2");
_LIT(KOpenSessionRConnect2, "OpenSessionRConnect2");
_LIT(KOpenSessionRtcp2, "OpenSessionRtcp2");
_LIT(KOpenSessionRtcpRemoved2, "OpenSessionRtcpRemoved2");
_LIT(KCloseSession2,"CloseSession2");
_LIT(KTidyUp2,"TidyUp2");
#endif

#endif
