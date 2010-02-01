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

#if (!defined __CHANGESSRC_STEP_H__)
#define __CHANGESSRC_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_ut_rtpcollisionmgrsuitestepbase.h"

class CchangessrcStep : public CTe_UT_RtpCollisionMgrSuiteStepBase
	{
public:
	CchangessrcStep();
	~CchangessrcStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	};

_LIT(KchangessrcStep,"changessrcStep");

#endif
