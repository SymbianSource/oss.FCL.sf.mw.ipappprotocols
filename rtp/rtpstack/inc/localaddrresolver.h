/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/





#ifndef __LOCALADDRRESOLVER_H__
#define __LOCALADDRRESOLVER_H__

//Includes
#include <in_sock.h>

//Class declaration
class CLocalAddrResolver : public CBase
	{ 
    #if defined( _DEBUG ) && defined( RTP_UNIT_TEST )
	    //Friend class used for testing
	    friend class CRtpUtCLocalAddrResolver; 
    #endif 
	 
	public:
		static CLocalAddrResolver* NewL( RSocketServ& aServer );
		static CLocalAddrResolver* NewLC( RSocketServ& aServer );
		
		void GetLocalAddrL(TInetAddr& aAddr, TUint32 aIap);
		~CLocalAddrResolver();

	private://functions
		CLocalAddrResolver(RSocketServ& aServer);
		void ConstructL();
	    
	    void SetAddr( TInetAddr& aTarget, TInetAddr& aSource );
	    void CheckAndSetAddr( TInetAddr& aTarget, TInetAddr& aCandidate,
	                          TUint32 aCandidateIap, TUint32 aSpecifiedIap );

	private: //data
		RSocketServ* iServer;
		TInetAddr iResultAddr;
		TUint iCount;
	
	private: 	
	
	#ifdef EUNIT_TESTING
	  friend class UT_CLocalAddrResolver;
	#endif     	
	};
#endif // end of __LOCALADDRRESOLVER_H__

// End of File

