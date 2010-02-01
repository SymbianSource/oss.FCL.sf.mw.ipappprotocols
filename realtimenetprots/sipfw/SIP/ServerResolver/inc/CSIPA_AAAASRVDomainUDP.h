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
* Name          : CSIPA_AAAASRVDomainUDP.h
* Part of       : ServerResolver
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/
#ifndef CSIPA_AAAASRVDOMAINUDP_H
#define CSIPA_AAAASRVDOMAINUDP_H

//INCLUDES
#include "CSIPQueryConditionBase.h"
#include "_sipcodecdefs.h"

// FORWARD DECLARATIONS
class MSIPServerQuery;
class CSIPQueryBase;

// CLASS DEFINITION
/**
 * CSIPA_AAAASRVDomainUDP implements a
 */
class CSIPA_AAAASRVDomainUDP : public CSIPQueryConditionBase
	{
	public:// Constructors and destructor

		static CSIPA_AAAASRVDomainUDP* NewL( MSIPServerQuery& aServerQuery,
					CSIPQueryBase* aSrvQuery );
		
		~CSIPA_AAAASRVDomainUDP();

	public:

		CSIPQueryBase& QueryBase();
	
		CSIPQueryConditionBase* HandleQueryResultL( TInt aStatus );
	
	private:// Constructors and destructor
	
		void ConstructL( CSIPQueryBase* aSrvQuery );

		CSIPA_AAAASRVDomainUDP( MSIPServerQuery& aServerQuery );
		
	private:// Data
		
		CSIPQueryBase* iQueryBase;
		
		MSIPServerQuery& iServerQuery;
		
		CSIPQueryBase* iSrvQuery;
		
	private: // For testing purposes

		UNIT_TEST(CSIPA_AAAASRVDomainUDPTest)
	};
#endif // end of CSIPAAAAASRVDOMAINUDP_H

// End of File
