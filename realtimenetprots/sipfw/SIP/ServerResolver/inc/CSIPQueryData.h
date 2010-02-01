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
* Name          : CSIPQueryData.h
* Part of       : ServerResolver
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/
#ifndef CSIPQUERYDATA_H
#define CSIPQUERYDATA_H

//INCLUDES
#include <stringpool.h>
#include <dns_qry.h>
#include "CSIPQueryConditionBase.h"
#include "CSIPQueryBase.h"
#include "_sipcodecdefs.h"

// FORWARD DECLARATIONS
class MSIPServerResolverObserver;
class MSIPHostResolver;
class MSIPResolvingResult;
class CSIPQueryA_AAAA;

// CLASS DEFINITION
/**
 * CSIPQueryData implements a
 */
class CSIPQueryData : public CBase
	{
	public:// Constructors and destructor

		static CSIPQueryData* NewL( MSIPServerResolverObserver* aObserver,
                     				TUint aPort,
                     				RStringF aProtocol,
                     				const TDesC8& aOriginalTarget,
				     				MSIPHostResolver& aResolver,
				     				RPointerArray< MSIPResolvingResult >& aResult,
				     				TBool aSipsUri,
				     				TBool aForceUDP );

	
	
		~CSIPQueryData();
		
	public:
	
		RPointerArray<MSIPResolvingResult>& ResultArray();
		 
		MSIPServerResolverObserver* Observer();
		
		TUint Port();
		
		RStringF Protocol();
		
		const TDesC8& Target() const;
		
		const TDesC8& OriginalTarget() const;
		
		MSIPHostResolver& Resolver();
		 
		void SetPort( TUint aPort );
		
		void SetProtocol( RStringF aProtocol );
		
		void SetTargetL( const TDesC8& aTarget );
		
		void SetOriginalTargetL( const TDesC8& aOriginalTarget );
		
		TBool CreateResultL( CSIPQueryA_AAAA& aQuery );
		
		void CompleteQueryL();
		
		TBool IsSipsUri();
		
		void SetForceUDP();

	private:// Constructors and destructor
	
		void ConstructL( MSIPServerResolverObserver* aObserver,
                         TUint aPort,
                         RStringF aProtocol,
                         const TDesC8& aOriginalTarget );

		CSIPQueryData( MSIPHostResolver& aResolver,
				RPointerArray< MSIPResolvingResult >& aResult,
				TBool aSipsUri,
				TBool aForceUDP );
	
    	TBool AddResultToArrayL();

	private:// Data
	
		HBufC8* iTarget;
		
		HBufC8* iOriginalTarget;
		
		//Not owned
		CSIPQueryA_AAAA* iQuery; 
		
		TUint iPort;
		
		RStringF iProtocol;
		
		//Not owned
		MSIPServerResolverObserver* iObserver;
		
		MSIPHostResolver& iResolver;
		
		RPointerArray<MSIPResolvingResult>& iResultArray;
		
		TBool iSipsUri;
		
		TBool iForceUDP;
		
	private: // For testing purposes

		UNIT_TEST(CSIPQueryDataTest)
		UNIT_TEST(CSIPA_AAAAOrigDomainTest)		
	};
#endif // end of CSIPQUERYDATA_H

// End of File
