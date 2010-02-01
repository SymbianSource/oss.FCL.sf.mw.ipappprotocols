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
* Name          : CSIPSRVNaptrDomain.h
* Part of       : ServerResolver
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/
#ifndef CSIPSRVNAPTRDOMAIN_H
#define CSIPSRVNAPTRDOMAIN_H

//INCLUDES
#include "CSIPQueryConditionBase.h"
#include "_sipcodecdefs.h"

// FORWARD DECLARATIONS
class MSIPServerQuery;
class CSIPQueryBase;

// CLASS DEFINITION
/**
 * CSIPSRVNaptrDomain implements a
 */
class CSIPSRVNaptrDomain : public CSIPQueryConditionBase
	{
	public:// Constructors and destructor

		static CSIPSRVNaptrDomain* NewL( MSIPServerQuery& aServerQuery,
									     CSIPQueryBase* aNaptrQuery );
									
		~CSIPSRVNaptrDomain();
		
	public: 

		CSIPQueryBase& QueryBase();
	
		CSIPQueryConditionBase* HandleQueryResultL( TInt aStatus );

	private:// Constructors and destructor
	
		void ConstructL( CSIPQueryBase* aNaptrQuery );
		
		CSIPSRVNaptrDomain( MSIPServerQuery& aServerQuery );
		
	private:// Data
	
		CSIPQueryBase* iQueryBase;
		
		CSIPQueryBase* iQueryNaptr;
		
		MSIPServerQuery& iServerQuery;

	private: // For testing purposes

		UNIT_TEST(CSIPSRVNaptrDomainTest)
	};
#endif // end of CSIPSRVNAPTRDOMAIN_H

// End of File
