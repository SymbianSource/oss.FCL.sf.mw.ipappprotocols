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

#if (!defined RTP_CORE_STEP_H)
#define RTP_CORE_STEP_H
#include <testexecutestepbase.h>

class CTRtpCoreServer;


class CTRtpCoreStep : public CTestStep
	{
public:
	~CTRtpCoreStep() {};
	inline void SetServer(CTRtpCoreServer* aServer);
	virtual TVerdict doTestStepL();
	virtual TVerdict doRtpTestStepL() = 0;
	TInt iError;
	TBool iIgnoreIncrease;
protected:

	CTRtpCoreServer* iServer;
	};

inline void CTRtpCoreStep::SetServer(CTRtpCoreServer* aServer)
	{
	iServer = aServer;
	}


class COom : public CTRtpCoreStep
	{
public:
	virtual TVerdict doRtpTestStepL();
	};	
	
class CSDESSetAttribute : public CTRtpCoreStep
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doRtpTestStepL();
	};
	
class CSDESGetLength : public CTRtpCoreStep
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doRtpTestStepL();
	};
	
class CSDESGetAttribute : public CTRtpCoreStep
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doRtpTestStepL();
	};
	
class CSDESGetType : public CTRtpCoreStep
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doRtpTestStepL();
	};
	
class CAPPSetSubType : public CTRtpCoreStep
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doRtpTestStepL();
	};

class CAPPGetSubType : public CTRtpCoreStep
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doRtpTestStepL();
	};

class CAPPGetAppName : public CTRtpCoreStep
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doRtpTestStepL();
	};

class CBYESetSSRC : public CTRtpCoreStep
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doRtpTestStepL();
	};

class CBYESetReason : public CTRtpCoreStep
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doRtpTestStepL();
	};

class CBYEGetSSRC : public CTRtpCoreStep
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doRtpTestStepL();
	};

class CBYEGetReason : public CTRtpCoreStep
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doRtpTestStepL();
	};

class CSMTestBase : public CTRtpCoreStep
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doRtpTestStepL()=0;
	virtual TVerdict doTestStepPostambleL();
	};

class CSMNewL : public CSMTestBase
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doRtpTestStepL();
	};
	
class CSMSetRtpStreamParameters : public CSMTestBase
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doRtpTestStepL();
	};
	
class CSMMaxDropout : public CSMTestBase
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doRtpTestStepL();
	};
	
class CSMMaxMisorder : public CSMTestBase
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doRtpTestStepL();
	};
	
class CSMMinSequential : public CSMTestBase
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doRtpTestStepL();
	};

class CSMByeTimeout : public CSMTestBase
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doRtpTestStepL();
	};

class CSMShortTimeout : public CSMTestBase
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doRtpTestStepL();
	};

class CSMLongTimeout : public CSMTestBase
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doRtpTestStepL();
	};

class CSMNext : public CSMTestBase
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doRtpTestStepL();
	};

	
class CRtpApiTest : public CSMTestBase
{
	public:
	virtual TVerdict doRtpTestStepL();
};

class CRtpUtilsTest : public CSMTestBase
{
	public:
	virtual TVerdict doRtpTestStepL();
};

_LIT(KOom,"Oom");
_LIT(KSDESSetAttribute,"SDESSetAttribute");
_LIT(KSDESGetLength,"SDESGetLength");
_LIT(KSDESGetAttribute,"SDESGetAttribute");
_LIT(KSDESGetType,"SDESGetType");
_LIT(KAPPSetSubType,"APPSetSubType");
_LIT(KAPPGetSubType,"APPGetSubType");
_LIT(KAPPGetAppName,"APPGetAppName");
_LIT(KBYESetSSRC,"BYESetSSRC");
_LIT(KBYESetReason,"BYESetReason");
_LIT(KBYEGetSSRC,"BYEGetSSRC");
_LIT(KBYEGetReason,"BYEGetReason");
_LIT(KSMNewL,"SMNewL");
_LIT(KSMSetRtpStreamParameters,"SMSetRtpStreamParameters");
_LIT(KSMMaxDropout,"SMMaxDropout");
_LIT(KSMMaxMisorder,"SMMaxMisorder");
_LIT(KSMMinSequential,"SMMinSequential");
_LIT(KSMByeTimeout,"SMByeTimeout");
_LIT(KSMShortTimeout,"SMShortTimeout");
_LIT(KSMLongTimeout,"SMLongTimeout");
_LIT(KSMNext,"SMNext");


_LIT(KRtpApiTest,"RtpApiTest");
_LIT(KRtpUtilsTest,"RtpUtilsTest");

#endif

