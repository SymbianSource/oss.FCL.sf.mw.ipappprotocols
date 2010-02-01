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

#if (!defined __NEWL_STEP_H__)
#define __NEWL_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_ut_rtpcollisionmgrsuitestepbase.h"
#include "ut_stubclasses.h"

class CnewlStep : public CTe_UT_RtpCollisionMgrSuiteStepBase
	{
public:
	CnewlStep();
	~CnewlStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	};

_LIT(KnewlStep,"newlStep");

#endif
