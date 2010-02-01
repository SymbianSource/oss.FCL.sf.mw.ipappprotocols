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
* Name          : CSipLaunchingStrategies.h
* Part of       : SIP Client Resolver
* Version       : 1.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPLAUNCHINGSTRATEGIES_H
#define CSIPLAUNCHINGSTRATEGIES_H

#include <e32base.h>
#include <stringpool.h>
#include "_sipcodecdefs.h"

class CSIPResponse;
class CSIPRequest;
class MSipClients;
class CSipHeaderStrategyBase;
class CSdpStrategyBase;
class CSIPClientResolver2;
class CSdpMediaField;

class CSipLaunchingStrategies : public CBase
    {
public:

    static CSipLaunchingStrategies* NewL(MSipClients& aSipClients);
    ~CSipLaunchingStrategies();

	CSIPResponse* ApplyL(CSIPRequest& aRequest, RArray<TUid>& aUids,
						CSIPClientResolver2& aClientResolver );

	static void PushLC(RPointerArray<CSdpMediaField>* aArray);
	static void ResetAndDestroyArray(TAny* anArray);

private:

    CSipLaunchingStrategies(MSipClients& aSipClients);
	void ConstructL();

    CSIPResponse* CheckMethodL(CSIPRequest& aRequest, RArray<TUid>& aUids, 
    						CSIPClientResolver2& aClientResolver2);

	TBool HasSDPContent(CSIPRequest& aRequest);
    
    CSIPResponse* CreateOptionsResponseL(CSIPRequest& aRequest,
                                         RArray<TUid>& aUids,
                                        CSIPClientResolver2& aClientResolver2 );
                                         
    TBool ClientForOptionsL( CSIPRequest& aRequest,
                            RArray<TUid>& aUids,
                            CSIPClientResolver2& aClientResolver2 );

private: // Data

    MSipClients& iSipClients;
	RPointerArray<CSipHeaderStrategyBase> iSipStrategies;
	RPointerArray<CSdpStrategyBase> iSdpStrategies;
	
private: // For testing purposes

	UNIT_TEST(CSipLaunchingStrategiesTest)
    };

#endif // CSIPLAUNCHINGSTRATEGIES_H

// End of File
