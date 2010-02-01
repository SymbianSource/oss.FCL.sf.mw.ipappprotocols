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
* Name          : CSIPA_AAAASRVDomainOrig.h
* Part of       : ServerResolver
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/
#ifndef CSIPA_AAAASRVDOMAINORIG_H
#define CSIPA_AAAASRVDOMAINORIG_H

//INCLUDES
#include "CSIPQueryConditionBase.h"
#include "_sipcodecdefs.h"

// FORWARD DECLARATIONS
class MSIPServerQuery;
class CSIPQueryBase;

// CLASS DEFINITION
/**
 * CSIPA_AAAASRVDomainOrig implements a
 */
class CSIPA_AAAASRVDomainOrig : public CSIPQueryConditionBase
	{
	public:// Constructors and destructor

		static CSIPA_AAAASRVDomainOrig* NewL( MSIPServerQuery& aServerQuery );
		
		~CSIPA_AAAASRVDomainOrig();

	public:

		CSIPQueryBase& QueryBase();
	
		CSIPQueryConditionBase* HandleQueryResultL( TInt aStatus );
	
	private:// Constructors and destructor
	
		void ConstructL();

		CSIPA_AAAASRVDomainOrig( MSIPServerQuery& aServerQuery );
		
	private:// Data
		
		CSIPQueryBase* iQueryBase;
		
		MSIPServerQuery& iServerQuery;

	private: // For testing purposes

		UNIT_TEST(CSIPA_AAAASRVDomainOrigTest)
	};
#endif // end of CSIPA_AAAASRVDOMAINORIG_H

// End of File
