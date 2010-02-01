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
* Name          : CSIPQueryNaptr.h
* Part of       : ServerResolver
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/
#ifndef CSIPQUERYNAPTR_H
#define CSIPQUERYNAPTR_H

//INCLUDES
#include <e32std.h>
#include <dns_qry.h>
#include "CSIPQueryBase.h"
#include "_sipcodecdefs.h"

// CLASS DEFINITION
/**
 * CSIPQueryNaptr implements a
 */
class CSIPQueryNaptr : public CSIPQueryBase
	{
	public:// Constructors and destructor

		static CSIPQueryNaptr* NewL( const TDesC8& aTarget, TBool aSipsUri );
		
		~CSIPQueryNaptr();

	public:

		const TDesC8& QueryBuf();
		
		TDes8& QueryResultBuf(); 
		
		TDnsRespNAPTR QueryResult();
		
		void AddL( TDnsRespNAPTR* aNaptr );
		
		static TInt Compare( const TDnsRespNAPTR& aFirst, 
						     const TDnsRespNAPTR& aSecond );
						     
		TDnsRespNAPTR& NAPTRRecordL( TInt aIndex );
			
		TBool MatchToRulesL( const TDnsRespNAPTR& aNaptr );
		
		RPointerArray<TDnsRespNAPTR>& RecordArray();
		
		static RStringF NaptrProtocol( const TDnsRespNAPTR& aNaptr );
		
		void Query( MSIPHostResolver& aResolver );
		
		void HandleQueryResultL( MSIPHostResolver& aResolver );
	
		const TDesC8& ResultTargetL();
		
		RStringF ResultProtocolL();
		
		void RemoveElementL( TInt aIndex );
		
		TInt ArrayCount(); 

	private:// Constructors and destructor

		void ConstructL( const TDesC8& aTarget );

		CSIPQueryNaptr(TBool aSipsUri);
		
	private:// Data
		
		HBufC8* iTarget;
	
		TDnsRespNAPTRBuf iResult;
	
		TDnsQueryBuf iQueryBuf;
	
		RPointerArray<TDnsRespNAPTR>* iRecordArray;
		
		TBool iSipsUri;

	private: // For testing purposes

		UNIT_TEST(CSIPQueryNaptrStub)
		UNIT_TEST(CSIPQueryNaptrTest)
	};
#endif // end of CSIPQUERYNAPTR_H

// End of File
