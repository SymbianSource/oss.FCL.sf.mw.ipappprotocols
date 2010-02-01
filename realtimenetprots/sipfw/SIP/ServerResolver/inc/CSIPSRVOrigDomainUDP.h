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
* Name          : CSIPSRVOrigDomainUDP.h
* Part of       : ServerResolver
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/
#ifndef CSIPSRVORIGDOMAINUDP_H
#define CSIPSRVORIGDOMAINUDP_H

//INCLUDES
#include "CSIPQueryConditionBase.h"
#include "_sipcodecdefs.h"

// FORWARD DECLARATIONS
class MSIPServerQuery;
class CSIPQueryBase;

// CLASS DEFINITION
/**
 * CSIPSRVOrigDomainUDP implements a
 */
class CSIPSRVOrigDomainUDP : public CSIPQueryConditionBase
	{
	public:// Constructors and destructor

		static CSIPSRVOrigDomainUDP* NewL( MSIPServerQuery& aServerQuery );
		
		~CSIPSRVOrigDomainUDP();
		
	public: 

		CSIPQueryBase& QueryBase();
	
		CSIPQueryConditionBase* HandleQueryResultL( TInt aStatus );
	
	private:// Constructors and destructor
	
		void ConstructL();

		CSIPSRVOrigDomainUDP( MSIPServerQuery& aServerQuery );
		
	private:// Data
		
		CSIPQueryBase* iQueryBase;
		
		MSIPServerQuery& iServerQuery;
		
	private: // For testing purposes

		UNIT_TEST(CSIPSRVOrigDomainUDPTest)		
	};
#endif // end of CSIPSRVORIGDOMAINUDP_H

// End of File
