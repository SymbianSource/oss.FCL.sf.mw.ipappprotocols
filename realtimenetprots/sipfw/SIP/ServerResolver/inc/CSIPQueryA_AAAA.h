/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSipQueryA_AAAA.h
* Part of       : ServerResolver
* Version       : SIP/6.0 
*
*/




/**
 @internalComponent
*/
#ifndef CSIPQUERYA_AAAA_H
#define CSIPQUERYA_AAAA_H

//INCLUDES
#include <dns_qry.h>
#include "CSIPQueryBase.h"
#include "_sipcodecdefs.h"

// CLASS DEFINITION
/**
 * CSIPQueryA_AAAA implements a
 */
class CSIPQueryA_AAAA : public CSIPQueryBase
	{
	public:

		static CSIPQueryA_AAAA* NewL( const TDesC8& aTarget );
		
		~CSIPQueryA_AAAA();

	public:

		TInetAddr QueryResultBuf(); 
		
		void SetTargetL( const TDesC8& aTarget );
		
		void Query( MSIPHostResolver& aResolver );
		
		TBool QueryNext( MSIPHostResolver& aResolver );
		
		void HandleQueryResultL( MSIPHostResolver& aResolver );
		
		TBool DnsNoRoute() const;
			
	private:// Constructors and destructor

		void ConstructL( const TDesC8& aTarget );

		CSIPQueryA_AAAA();
		
	private:// Data
		
		HBufC* iTarget;
		
		TInetAddr iResult;
		
		TNameEntry iNResult;

	private: // For testing purposes

		UNIT_TEST(CSIPQueryA_AAAATest)
		
		UNIT_TEST(CSIPQueryA_AAAAStub)
		
		UNIT_TEST(CSIPA_AAAAOrigDomainTest)	
	};
#endif // end of CSIPQUERYA_AAAA_H

// End of File
