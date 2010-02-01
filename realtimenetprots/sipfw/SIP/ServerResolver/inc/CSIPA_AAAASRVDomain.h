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
* Name          : CSIPA_AAAASRVDomain.h
* Part of       : ServerResolver
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/
#ifndef CSIPA_AAAASRVDOMAIN_H
#define CSIPA_AAAASRVDOMAIN_H

//INCLUDES
#include "CSIPQueryConditionBase.h"
#include "_sipcodecdefs.h"

// FORWARD DECLARATIONS
class MSIPServerQuery;
class CSIPQueryBase;

// CLASS DEFINITION
/**
 * CSIPA_AAAASRVDomain implements a
 */
class CSIPA_AAAASRVDomain : public CSIPQueryConditionBase
	{
	public:// Constructors and destructor

		static CSIPA_AAAASRVDomain* NewL( MSIPServerQuery& aServerQuery,
				     				  	  CSIPQueryBase* aNaptrQuery,
				     				  	  CSIPQueryBase* aSrvQuery );
				     				  	
		/**
	 	* Destructor.
	 	*/				  	
		~CSIPA_AAAASRVDomain();	     
		
	public:				  	

		CSIPQueryBase& QueryBase();
	
		CSIPQueryConditionBase* HandleQueryResultL( TInt aStatus );

	private:// Constructors and destructor
	
		void ConstructL( CSIPQueryBase* aNaptrQuery,
				     	 CSIPQueryBase* aSrvQuery );

		CSIPA_AAAASRVDomain( MSIPServerQuery& aServerQuery );
	
	private:// Data
	
		CSIPQueryBase* iQueryBase;
		
		CSIPQueryBase* iQueryNaptr;
		
		CSIPQueryBase* iSrvQuery;
		
		MSIPServerQuery& iServerQuery;

	private: // For testing purposes

		UNIT_TEST(CSIPA_AAAASRVDomainTest)
	};
#endif // end of CSIPA_AAAASRVDOMAIN_H

// End of File
