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
* Name          : SipProfileAgentConfigRetriever.h
* Part of       : SIP / SIP Profile Server
*
*/



#ifndef SIPPROFILEAGENTCONFIGRETRIEVER_H_
#define SIPPROFILEAGENTCONFIGRETRIEVER_H_

// INCLUDES
#include <e32base.h>
#include "sipprofiletypeinfo.h"
#include <sipprofileagentextensionparams.h>

// FORWARD DECLARATIONS
class CRepository;

// CLASS DECLARATION

/**
*@internalComponent
*/

/**
 * This class reads configuration extension values from Central Repository,
 *  and listens for their change in central repository.
 */
class CSipProfileAgentConfigRetriever : public CActive
	{
public: // Constructors and destructor

	/**
	 * Creates a CSipProfileAgentConfigRetriever object.
	 *
	 * @return value New CSipProfileAgentConfigRetriever object, ownership is transferred.
	 */
	static CSipProfileAgentConfigRetriever* NewL();
	
	~CSipProfileAgentConfigRetriever();
	
public: // From CActive

	void DoCancel();

	void RunL();

	TInt RunError(TInt aError);

public: //New functions
	/**
	* Returns the expiry value based on the profile type.
	*/
	TUint ExpiryValueL(TSIPProfileTypeInfo::TSIPProfileClass aClass,
	        CSipProfileAgentConfigExtension::TProfileProperty aProperty);

private: // New functions, for internal use

	CSipProfileAgentConfigRetriever();

	void ConstructL();

    /**
	 * Reads the timer values from repository. If they are not found, function
     * leaves.
	 */
    void ReadValuesL();    

    /**
    * Listens for notifcations from central repository
    * leaves.
    */
	void ListenForChangesL();

private: // Data

	// Provides access to Central Repository, owned.
	CRepository* iRepository;
	
	// Stores the keys for specific property againt a specific profile type
	TUint32 iProfilePropertyKeys[TSIPProfileTypeInfo::EMaxSIPProfileClass][CSipProfileAgentConfigExtension::EMaxProfileProperty];
	
	// Stores the Default registation duration specific to a profile type
	TUint32 iDefaultRegistrationValue[TSIPProfileTypeInfo::EMaxSIPProfileClass];
	
	// Stores the Default subscription duration specific to a profile type
	TUint32 iDefaultSubscriptionValue[TSIPProfileTypeInfo::EMaxSIPProfileClass];

private: // For testing purposes
#ifdef CPPUNIT_TEST
	friend class CSipProfileAgentConfigRetriever_Test;
#endif

	};

#endif /*SIPPROFILEAGENTCONFIGRETRIEVER_H_*/
// End of File
