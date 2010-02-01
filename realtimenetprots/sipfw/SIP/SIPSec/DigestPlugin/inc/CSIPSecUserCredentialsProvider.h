/*
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
* Name          : CSIPSecUserCredentialsProvider.h
* Part of       : SIPSec
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/
#ifndef __SIPSEC_USERCREDENTIALSPROVIDER_H__
#define __SIPSEC_USERCREDENTIALSPROVIDER_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "Lwtimer.h"

// CONSTANTS
const TUint KSIPSecWaitBeforeRequestUC = 100;

// FORWARD DECLARATIONS
class MSIPSecUser;
class CSIPSecUserRecord;
class TSIPSecDigestCtxSetup;
class CSIPSecDigestPlugin;

// CLASS DECLARATION
/**
 * @brief Class is base class for user credentials provider
 * 
 * @class CSIPSecCredentialsProvider CSIPSecUserCredentialsProvider.h 
 * "CSIPSecUserCredentialsProvider.h"
 *
 *
 */
class CSIPSecCredentialsProvider : public CBase
	{
	
public:  // Constructors and destructor

    /**
    * Constructor
    * @param aUserRecord user record, whose provider this is
    */
    CSIPSecCredentialsProvider( CSIPSecUserRecord& aUserRecord );

    /**
    * Destructor
    */
    ~CSIPSecCredentialsProvider(); 
        
public: // New pure virtual functions

    /**
    * Starts requesting credentials
    * @param aContext Context
    */
    virtual void RequestL( TSIPSecDigestCtxSetup& aContext ) = 0;
	
    /**
    * Cancels pending operations
    */
    virtual void Cancel() = 0;

public: // New virtual functions

	/**
    * Request credentials again, this time from another SIPSec user.
    */
	virtual void RequestAgainL();

public: // New functions

    /**
    * returns user record associated to this provider
    * @return user record associated to this provider
    */		
    CSIPSecUserRecord& UserRecord() const;

protected:

     // Not owned
     CSIPSecDigestPlugin* iPlugin;
    
private:

    // Not owned	
    CSIPSecUserRecord& iUserRecord;
	};
        



/**
 * @brief Class represents user credentials provider from user
 * 
 * @class CSIPSecUserCredentialsProvider CSIPSecUserCredentialsProvider.h 
 * "CSIPSecUserCredentialsProvider.h"
 *
 *
 */
class CSIPSecUserCredentialsProvider : public CSIPSecCredentialsProvider, 
                                       public MExpirationHandler
	{
public:  // Constructors and destructor

    /**
    * Constructor
    * @param aUserRecord user record, whose provider this is
    */
    CSIPSecUserCredentialsProvider( CSIPSecUserRecord& aUserRecord );

    /**
    * Destructor
    */
    ~CSIPSecUserCredentialsProvider();

public: // From MExpirationHandler

    void TimerExpiredL( TTimerId aTimerId, TAny* aTimerParam );

public: // From CSIPSecCredentialsProvider

    void RequestL( TSIPSecDigestCtxSetup& aContext );

    void Cancel();

	void RequestAgainL();

private: // New functions, for internal use

	void RequestFromUserL();

private: // Data

    // Not owned
    MTimerManager* iTimer;

    // Not owned
    MSIPSecUser* iUser;

    TTimerId iTimerId;
	};


#endif// __SIPSEC_USERCREDENTIALSPROVIDER_H__

// End of File
