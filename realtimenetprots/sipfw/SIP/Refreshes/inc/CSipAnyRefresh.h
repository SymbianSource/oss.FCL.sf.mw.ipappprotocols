/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSipAnyRefresh.h
* Part of       : SIPRefreshes
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPANYREFRESH_H
#define CSIPANYREFRESH_H

#include "CSipRefreshBase.h"

class CSipAnyRefresh : public CSipRefreshBase
	{
public:

	static CSipAnyRefresh* NewLC (MTransactionUser& aTU,
                                  MTimerManager& aTimerMgr,
                                  CSIPSec& aSIPSec,
							      CSipRefreshMgr& aRefreshMgr,
							      TRegistrationId aRegistrationId,
							      CSIPRequest* aRequest,
                                  MRefreshOwner* aOwner,
                                  MSIPSecUser& aSIPSecUser,
							      CURIContainer& aRemoteTarget,
								  TBool aPassAllResponsesToOwner);


    ~CSipAnyRefresh ();

private:

	CSipAnyRefresh (MTransactionUser& aTU,
                    MTimerManager& aTimerMgr,
                    CSIPSec& aSIPSec,
				    CSipRefreshMgr& aRefreshMgr,
				    TRegistrationId aRegistrationId,
                    MRefreshOwner* aOwner,
                    MSIPSecUser& aSIPSecUser,
					TBool aPassAllResponsesToOwner);


	void ConstructL(CSIPRequest* aRequest, CURIContainer& aRemoteTarget);

	void Forward2xxResponseToCallerL(CSIPResponse* aResponse);

    void SetExpirationTimeToRequestL(CSIPRequest& aRequest,
                                     TUint aExpirationTimeInSeconds);

	void SetExpirationTimeToResponseL(CSIPResponse& aResponse, 
		                              TUint         aExpirationTimeInSeconds);

	TInt ExpirationTimeFromRequestL(CSIPRequest& aRequest);

	TInt ExpirationTimeFromResponse(CSIPResponse& aResponse);

private:

	void CheckUpdateRequestExpiresValueL(CSIPRequest& aRequest);

    void CheckTerminateRequestExpiresValueL(CSIPRequest& aRequest);

private: // Data

	TBool iPassAllResponsesToOwner;

private: // For testing purposes

	UNIT_TEST(CSipAnyRefreshTest)
};

#endif // CSIPANYREFRESH_H

// End of File


