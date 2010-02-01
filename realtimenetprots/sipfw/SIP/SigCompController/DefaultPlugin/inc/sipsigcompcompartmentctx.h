/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : sipsigcompcompartmentctx.h
* Part of       : SIPSigComp
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __SIPSIGCOMPCOMPARTMENTCTX_H__
#define __SIPSIGCOMPCOMPARTMENTCTX_H__

#include <in_sock.h>
#include <e32base.h>
#include "sigcomp.h"

// FORWARD DECLARATIONS
class MCompartmentCtxOwner;
class MCompartmentCtxUser;


// CLASS DEFINITION
class CSipSigCompCompartmentCtx : public CBase
{
public: // Constructors and destructor

	enum TState
		{
		EAlive = 100,
		EIdle
		};

	static CSipSigCompCompartmentCtx* NewL( MCompartmentCtxOwner& aMyOwner,
	                                        const CSigComp& aSigComp, 
	                                        const TInetAddr& aAddress,
								            TUint32 aIapId,
								            TBool aDynamicCompression=ETrue );

	

	static CSipSigCompCompartmentCtx* NewLC( MCompartmentCtxOwner& aMyOwner,
	                                         const CSigComp& aSigComp, 
	                                         const TInetAddr& aAddress,
								             TUint32 aIapId,
								             TBool aDynamicCompression=ETrue );
	

    ~CSipSigCompCompartmentCtx();

public:

	CSigCompCompartment* Compartment();

	void AttachCompartmentUserL( MCompartmentCtxUser* aUser );
	
	void DetachCompartmentUser( MCompartmentCtxUser* aUser );

	TBool Match( const TInetAddr& aAddress, TUint32 aIapId ) const;
	
	void SetCurrentState( CSipSigCompCompartmentCtx::TState aCurrentState );
	
	void SendFailedL();
	
	static const TInt iOffset;
	
protected: // 2nd phase constructor

	void ConstructL();

private:

	CSipSigCompCompartmentCtx( MCompartmentCtxOwner& aMyOwner,
	                           const CSigComp& aSigComp, 
	                           const TInetAddr& aAddress,
							   TUint32 aIapId,
    						   TBool aDynamicCompression );
    						   
    void NotifyUsersAboutSelfDestruction();
    
	void ResetCompartmentL();

private: // data

    TSglQueLink iLink;
    
	MCompartmentCtxOwner& iMyOwner;
	const CSigComp& iSigComp;
	TInetAddr iAddress;
	TUint32 iIapId;
	TBool iDynamicCompression;
	RPointerArray<MCompartmentCtxUser> iUserArray;
	CSipSigCompCompartmentCtx::TState iCurrentState;
	
	CSigCompCompartment* iSigCompCompartment;
	
private: //For testing purposes
#ifdef CPPUNIT_TEST	
	friend class CSigCompTest;
#endif
};

#endif // end of __SIPSIGCOMPCOMPARTMENTCTX_H__
