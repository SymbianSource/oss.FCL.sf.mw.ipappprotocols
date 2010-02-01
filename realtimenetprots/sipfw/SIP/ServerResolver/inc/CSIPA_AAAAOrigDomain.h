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
* Name          : CSIPA_AAAAOrigDomain.h
* Part of       : ServerResolver
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/
#ifndef CSIPA_AAAAORIGDOMAIN_H
#define CSIPA_AAAAORIGDOMAIN_H

//INCLUDES
#include "CSIPQueryConditionBase.h"
#include "_sipcodecdefs.h"

class CSIPQueryBase;
class MSIPServerQuery;

// CLASS DEFINITION
/**
 * CSIPA_AAAAOrigDomain implements a
 */
class CSIPA_AAAAOrigDomain : public CSIPQueryConditionBase
	{
	public:// Constructors and destructor

    	/**
	 	* Creates a new CSIPA_AAAAOrigDomain instance 
	 	* @param aServerQuery an MSIPServerQuery reference
	 	* @returns a new instance of CSIPA_AAAAOrigDomain
	 	*/
		static CSIPA_AAAAOrigDomain* NewL( MSIPServerQuery& aServerQuery );
		
		/**
	 	* Destructor.
	 	*/
		~CSIPA_AAAAOrigDomain();
		
	public:

		CSIPQueryBase& QueryBase();
	
		CSIPQueryConditionBase* HandleQueryResultL( TInt aStatus );
	
	private:// Constructors and destructor

		void ConstructL();
		
		CSIPA_AAAAOrigDomain( MSIPServerQuery& aServerQuery );
		
	private:// Data
	
		CSIPQueryBase* iQueryBase;
		
		MSIPServerQuery& iServerQuery;
		
	private: // For testing purposes

		UNIT_TEST(CSIPA_AAAAOrigDomainTest)	
	};
#endif // end of CSIPA_AAAAORIGDOMAIN_H

// End of File
