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

#if (!defined __TE_UT_RTPCOLLISIONMGR_STEP_BASE__)
#define __TE_UT_RTPCOLLISIONMGR_STEP_BASE__
#include <test/testexecutestepbase.h>
// Please add your include here if you have 



#include "ut_stubclasses.h"

/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
class CTe_UT_RtpCollisionMgrSuiteStepBase : public CTestStep
	{
public:
	virtual ~CTe_UT_RtpCollisionMgrSuiteStepBase();
	CTe_UT_RtpCollisionMgrSuiteStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();
	CRtpController *iRtpController;

//Please add your class members which will be common to all individual test steps:
protected:
	HBufC8*		iReadData;
	HBufC8*		iWriteData;
	TInt        iPort1,iPort2,iPort3,iPort4;
	TInt	    iSSRC1,iSSRC2,iSSRC3,iSSRC4;
	TUint32     iLocalIpAddr,iIpAddr1,iIpAddr2,iIpAddr3,iIpAddr4;
	TBuf8<80>   iCname1,iCname2,iCname3,iCname4;
	};

#endif
