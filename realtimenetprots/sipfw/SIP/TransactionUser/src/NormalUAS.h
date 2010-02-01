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
* Name          : NormalUAS.h
* Part of       : TransactionUser
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef NORMALUAS_H
#define NORMALUAS_H

// INCLUDES
#include "UserAgentServer.h"

// FORWARD DECLARATIONS


// CLASS DECLARATION

class CNormalUAS : public CUserAgentServer
	{
public: // Constructor and destructor
	static CNormalUAS* NewL(CUserAgentCreateParams& aParams,	
							MSipConnectionMgr& aConnectionMgr,							
							MSIPRequestRouter& aRouter,
                            MSipDialogs& aDialogs);

    ~CNormalUAS();

public: // From CUserAgent
	void DeleteTimer(const CUserAgentTimer& aTimer);	
	
public: // From CUserAgentServer
	void CreateTransactionL();
	
public: // New functions
	static CNormalUAS& Ptr(CUserAgent& aUserAgent);

protected:
	CNormalUAS(CUserAgentCreateParams& aParams,
               MSipDialogs& aDialogs,
               MSIPRequestRouter& aRouter);

private:
	void ConstructL(MSipConnectionMgr& aConnectionMgr);

    void CancelAllTimers();
	};

#endif // end of NORMALUAS_H

// End of File
