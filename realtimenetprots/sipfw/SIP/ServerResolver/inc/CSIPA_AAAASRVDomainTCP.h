/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSIPA_AAAASRVDomainTCP.h
* Part of       : ServerResolver
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/
#ifndef CSIPA_AAAASRVDOMAINTCP_H
#define CSIPA_AAAASRVDOMAINTCP_H

//INCLUDES
#include "CSIPQueryConditionBase.h"
#include "_sipcodecdefs.h"

// FORWARD DECLARATIONS
class CSIPQueryBase;
class MSIPServerQuery;

// CLASS DEFINITION
/**
 * CSIPA_AAAASRVDomainTCP implements a
 */
class CSIPA_AAAASRVDomainTCP : public CSIPQueryConditionBase
	{
	public:// Constructors and destructor

		static CSIPA_AAAASRVDomainTCP* NewL( MSIPServerQuery& aServerQuery,
					CSIPQueryBase* aSrvQuery );
		
		~CSIPA_AAAASRVDomainTCP();

	public:

		CSIPQueryBase& QueryBase();
	
		CSIPQueryConditionBase* HandleQueryResultL( TInt aStatus );

	private:
	
		void ConstructL( CSIPQueryBase* aSrvQuery );

		CSIPA_AAAASRVDomainTCP( MSIPServerQuery& aServerQuery );
		
	private:// Data
		
		CSIPQueryBase* iQueryBase;
		
		MSIPServerQuery& iServerQuery;
		
		CSIPQueryBase* iSrvQuery;
		
	private: // For testing purposes

		UNIT_TEST(CSIPA_AAAASRVDomainTCPTest)
	};
#endif // end of CSIPA_AAAASRVDOMAINTCP_H

// End of File
