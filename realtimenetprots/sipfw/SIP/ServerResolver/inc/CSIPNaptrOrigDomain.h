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
* Name          : CSIPNaptrOrigDomain.h
* Part of       : ServerResolver
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/
#ifndef CSIPNAPTRORIGDOMAIN_H
#define CSIPNAPTRORIGDOMAIN_H

//INCLUDES
#include "CSIPQueryConditionBase.h"
#include "_sipcodecdefs.h"

// FORWARD DECLARATIONS
class CSIPQueryBase;
class MSIPServerQuery;

// CLASS DEFINITION
/**
 * CSIPNaptrOrigDomain implements a
 */
class CSIPNaptrOrigDomain : public CSIPQueryConditionBase
	{
	public:// Constructors and destructor
	
    	/**
	 	* Creates a new CSIPNaptrOrigDomain instance 
	 	* @param aServerQuery an MSIPServerQuery reference
	 	* @returns a new instance of CSIPNaptrOrigDomain
	 	*/
		static CSIPNaptrOrigDomain* NewL( MSIPServerQuery& aServerQuery );
		
		/**
	 	* Destructor.
	 	*/
		~CSIPNaptrOrigDomain();
		
	public:

		CSIPQueryBase& QueryBase();
	
		CSIPQueryConditionBase* HandleQueryResultL( TInt aStatus );
	
	private:// Constructors and destructor
	
		void ConstructL();

		CSIPNaptrOrigDomain( MSIPServerQuery& aServerQuery );

	private:// New functions for internal use

		void CheckIfOnlyUDPResultsL() const;

	private:// Data
	
		CSIPQueryBase* iQueryBase;
		
		MSIPServerQuery& iServerQuery;
		
	private: // For testing purposes

		UNIT_TEST(CSIPNaptrOrigDomainTest)		
	};
#endif // end of CSIPNAPTRORIGDOMAIN_H

// End of File
