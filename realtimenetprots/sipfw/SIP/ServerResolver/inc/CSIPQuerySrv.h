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
* Name          : CSIPQuerySrv.h
* Part of       : ServerResolver
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/
#ifndef CSIPQUERYSRV_H
#define CSIPQUERYSRV_H

//INCLUDES
#include <dns_qry.h>
#include <e32std.h>
#include "CSIPQueryBase.h"
#include "_sipcodecdefs.h"

// CLASS DEFINITION
/**
 * CSIPQuerySrv implements a
 */
class CSIPQuerySrv : public CSIPQueryBase
	{
	public:// Constructors and destructor

		static CSIPQuerySrv* NewL( const TDesC8& aTarget, TBool aSipsUri );
		
		~CSIPQuerySrv();
		
	public:
		
		static HBufC8* SetTargetProtocolL( const TDesC8& aTarget,
										   RStringF aProtocol,
										   TBool aIsSipsUri );
										   
		static HBufC8* SetTargetTLSL( const TDesC8& aTarget );
		const TDesC8& QueryBuf();
		
		TDes8& QueryResultBuf(); 
		
		TDnsRespSRV QueryResult();
		
		void AddL( TDnsRespSRV aSrv );
		
		static TInt Compare( const TDnsRespSRV& aFirst, 
							 const TDnsRespSRV& aSecond );
							 
		TDnsRespSRV& SRVRecordL( TInt aIndex );
			
		RArray<TDnsRespSRV>& RecordArray();
		
		void SetTargetL( const TDesC8& aTarget );
		
		void Query( MSIPHostResolver& aResolver );
		
		void HandleQueryResultL( MSIPHostResolver& aResolver );
		
		const TDesC8& ResultTargetL();
		
		TUint ResultPortL();
		
		void RemoveElementL( TInt aIndex );
		
		TInt ArrayCount(); 

	private:// Constructors and destructor
	
		void ConstructL( const TDesC8& aTarget );

		CSIPQuerySrv(TBool aSipsUri);
		
	private:// Data
	
		HBufC8* iTarget;
		
		TDnsRespSRVBuf iResult;
		
		TDnsQueryBuf iQueryBuf;
		
		RArray<TDnsRespSRV> iRecordArray;
		
		TBool iSipsUri;

	private: // For testing purposes

		UNIT_TEST(CSIPQuerySrvTest)
		UNIT_TEST(CSIPQuerySrvStub)		
	};
#endif // end of CSIPQUERYSRV_H

// End of File
