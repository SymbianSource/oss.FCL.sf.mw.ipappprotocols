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
// Header for class specific to unit tests
// 
//

/**
 @file
 @internalComponent
*/

#if (!defined T_RTPCORESERVER_H__)
#define T_RTPCORESERVER_H__
#include <testexecuteserverbase.h>

#if (!defined __DATA_CAGING__)
IMPORT_C TInt NewServer();
#endif


//#include "rtpsession.h"
#include "rtpapi.h"
#include "rtpcontroller.h"
#include "rtpcontrollerdummy.h"

class CTRtpCoreStep;

class CTRtpCoreServer : public CTestServer
	{
public:
	static CTRtpCoreServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	~CTRtpCoreServer();

	// Members used by the steps
	
	TBool iOom;
	CTRtpCoreStep* iCurrentStep;
	TBool iStopping;
	TInt iStopCount;
	CRtpController* iRtpSession;

	
	CActiveScheduler* iScheduler;

 private:
	CTRtpCoreServer();
	};

#endif
