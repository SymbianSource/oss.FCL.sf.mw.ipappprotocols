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
* Name          : CSipRegisterRefresh.h
* Part of       : SIPRefreshes
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIPREGISTERREFRESH_H
#define CSIPREGISTERREFRESH_H

#include <stringpool.h>
#include "CSipRefreshBase.h"

class CSipRegisterRefresh : public CSipRefreshBase
	{
public:


	static CSipRegisterRefresh* NewLC (
		MTransactionUser& aTU,
		MTimerManager& aTimerMgr,
		CSIPSec& aSIPSec,
		CSipRefreshMgr& aRefreshMgr,
		TRegistrationId aRegistrationId,
		CSIPRequest* aRequest,
		MRefreshOwner* aOwner,
		MSIPSecUser& aSIPSecUser,
		CURIContainer& aRemoteTarget);


    ~CSipRegisterRefresh ();

public: // From MTransactionOwner
    
    const MSIPSecUser* SIPSecUser() const;

private:

	CSipRegisterRefresh (MTransactionUser& aTU,
                         MTimerManager& aTimerMgr,
                         CSIPSec& aSIPSec,
				         CSipRefreshMgr& aRefreshMgr,
				         TRegistrationId aRegistrationId,
                         MRefreshOwner* aOwner,
                         MSIPSecUser& aSIPSecUser);


	void ConstructL(CSIPRequest* aRequest, CURIContainer& aRemoteTarget);

	void Forward2xxResponseToCallerL(CSIPResponse* aResponse);

    void SetExpirationTimeToRequestL(CSIPRequest& aRequest,
                                     TUint aExpirationTimeInSeconds);

	void SetExpirationTimeToResponseL(CSIPResponse& aResponse, 
		                              TUint         aExpirationTimeInSeconds);

	TInt ExpirationTimeFromRequestL(CSIPRequest& aRequest);

	TInt ExpirationTimeFromResponse(CSIPResponse& aResponse);

	TInt ExpirationTimeFromResponseContact(CSIPResponse& aResponse);

private:

	void CheckUpdateRequestExpiresValueL(CSIPRequest& aRequest);

    void CheckTerminateRequestExpiresValueL(CSIPRequest& aRequest);

private: // Data
    
    RStringF iContactHeaderName;

private: // For testing purposes

	UNIT_TEST(CSipRegisterRefreshTest)
	};

#endif // CSIPREGISTERREFRESH_H

// End of File


