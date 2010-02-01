/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



/**
 @file 
 @internalAll
*/

#if (!defined __CFRTP_BASICTESTS_H__)
#define __CFRTP_BASICTESTS_H__
#include <test/testexecutestepbase.h>
#include "te_cfrtpsuitestepbase.h"

const TInt KRtpHeaderSize = 12;
const TInt KBufferSize1 = 1000;
const TInt KBufferSize2 = 2000;


class CSubConStep : public CTe_CfrtpSuiteStepBase
	{
public:
	CSubConStep();
	~CSubConStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KSubConTestStep,"CSubConTest_01");

/////////////////////////////////////////////////////////////

class CRtpSockTest : public CTe_CfrtpSuiteStepBase
	{
public:
	CRtpSockTest();
	~CRtpSockTest();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};


_LIT(KRtpSockTestStep,"CRtpSockTest");

/////////////////////////////////////////////////////////////

class CRtcpSockTest : public CTe_CfrtpSuiteStepBase
	{
public:
	CRtcpSockTest();
	~CRtcpSockTest();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};


_LIT(KRtcpSockTestStep,"CRtcpSockTest");

/////////////////////////////////////////////////////////////

class CRtpSendRecv : public CTe_CfrtpSuiteStepBase
	{
public:
	CRtpSendRecv();
	~CRtpSendRecv();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KRtpSendRecvStep,"CRtpSendRecv");

/////////////////////////////////////////////////////////////

class CRtcpSendRecv : public CTe_CfrtpSuiteStepBase
	{
public:
	CRtcpSendRecv();
	~CRtcpSendRecv();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KRtcpSendRecvTestStep,"CRtcpSendRecv");

/////////////////////////////////////////////////////////////

class CRtpConnect : public CTe_CfrtpSuiteStepBase
	{
public:
	CRtpConnect();
	~CRtpConnect();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KRtpConnectTestStep,"CRtpConnect");

/////////////////////////////////////////////////////////////

class CRtcpConnect : public CTe_CfrtpSuiteStepBase
	{
public:
	CRtcpConnect();
	~CRtcpConnect();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KRtcpConnectTestStep,"CRtcpConnect");

/////////////////////////////////////////////////////////////

class CRtpNoBind : public CTe_CfrtpSuiteStepBase
	{
public:
	CRtpNoBind();
	~CRtpNoBind();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KRtpNoBindTestStep,"CRtpNoBind");

/////////////////////////////////////////////////////////////

class CRtpBind : public CTe_CfrtpSuiteStepBase
	{
public:
	CRtpBind();
	~CRtpBind();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KRtpBindTestStep,"CRtpBind");

/////////////////////////////////////////////////////////////

class CRtpSockOpt : public CTe_CfrtpSuiteStepBase
	{
public:
	CRtpSockOpt();
	~CRtpSockOpt();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KRtpSockOptTestStep,"CRtpSockOpt");

/////////////////////////////////////////////////////////////

class CRtcpSockOpt : public CTe_CfrtpSuiteStepBase
	{
public:
	CRtcpSockOpt();
	~CRtcpSockOpt();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KRtcpSockOptTestStep,"CRtcpSockOpt");

/////////////////////////////////////////////////////////////

class CRtpIoctl : public CTe_CfrtpSuiteStepBase
	{
public:
	CRtpIoctl();
	~CRtpIoctl();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KRtpIoctlTestStep,"CRtpIoctl");

/////////////////////////////////////////////////////////////

class CBigSendRecvRtp : public CTe_CfrtpSuiteStepBase
	{
public:
	CBigSendRecvRtp();
	~CBigSendRecvRtp();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KBigSendRecvRtp,"CBigSendRecvRtp");

/////////////////////////////////////////////////////////////

class CRtpNegTests_01 : public CTe_CfrtpSuiteStepBase
	{
public:
	CRtpNegTests_01();
	~CRtpNegTests_01();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KRtpNegTests_01TestStep,"CRtpNegTests_01");

/////////////////////////////////////////////////////////////

class CRtpNegTests_02 : public CTe_CfrtpSuiteStepBase
	{
public:
	CRtpNegTests_02();
	~CRtpNegTests_02();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KRtpNegTests_02TestStep,"CRtpNegTests_02");

/////////////////////////////////////////////////////////////

class CRtpNegTests_03 : public CTe_CfrtpSuiteStepBase
	{
public:
	CRtpNegTests_03();
	~CRtpNegTests_03();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KRtpNegTests_03TestStep,"CRtpNegTests_03");

/////////////////////////////////////////////////////////////

class CRtpNegTests_04 : public CTe_CfrtpSuiteStepBase
	{
public:
	CRtpNegTests_04();
	~CRtpNegTests_04();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KRtpNegTests_04TestStep,"CRtpNegTests_04");

/////////////////////////////////////////////////////////////

class CRtpNegTests_05 : public CTe_CfrtpSuiteStepBase
	{
public:
	CRtpNegTests_05();
	~CRtpNegTests_05();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KRtpNegTests_05TestStep,"CRtpNegTests_05");

/////////////////////////////////////////////////////////////

class CRtpNegTestsFailNoBearer : public CTe_CfrtpSuiteStepBase
	{
public:
	CRtpNegTestsFailNoBearer();
	~CRtpNegTestsFailNoBearer();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:

private:
	};
	
_LIT(KRtpNegTests_FailNoBearer,"CRtpNegTestsFailNoBearer");

/////////////////////////////////////////////////////////////

class CRtpShutDownImmediate : public CTe_CfrtpSuiteStepBase
	{
public:
	CRtpShutDownImmediate();
	~CRtpShutDownImmediate();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	};
	
_LIT(KRtpShutDownImmediate,"CRtpShutDownImmediate");

/////////////////////////////////////////////////////////////

class CRtcpShutDownImmediate : public CTe_CfrtpSuiteStepBase
	{
public:
	CRtcpShutDownImmediate();
	~CRtcpShutDownImmediate();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:

private:
	};
	
_LIT(KRtcpShutDownImmediate,"CRtcpShutDownImmediate");

/////////////////////////////////////////////////////////////

class CShutDownImmediate : public CTe_CfrtpSuiteStepBase
	{
public:
	CShutDownImmediate();
	~CShutDownImmediate();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:

private:
	};

_LIT(KShutDownImmediate,"CShutDownImmediate");
#endif

