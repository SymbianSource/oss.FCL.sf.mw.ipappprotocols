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
* Name          : sipsigcompcompartmentuser.h
* Part of       : SIPSigComp
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __SIPSIGCOMPCOMPARTMENTUSER_H__
#define __SIPSIGCOMPCOMPARTMENTUSER_H__

#include <in_sock.h>
#include <e32base.h>

#include "MCompartmentCtxUser.h"

// FORWARD DECLARATIONS
class CSigCompCompartment;
class CSipSigCompCompartmentCtx;
class MCompartmentUserOwner;


// CLASS DEFINITION
class CSipSigCompCompartmentUser : public CBase, public MCompartmentCtxUser
{
public: // Constructors and destructor

	static CSipSigCompCompartmentUser* NewL( MCompartmentUserOwner& aMyOwner,
                                             TUint32 aCompartmentId,
                                             TUint32 aIapId,
                                             TBool aDynamicCompression=ETrue );	

	static CSipSigCompCompartmentUser* NewLC( MCompartmentUserOwner& aMyOwner,
                                              TUint32 aCompartmentId,
                                              TUint32 aIapId,
                                              TBool aDynamicCompression=ETrue );
	
    ~CSipSigCompCompartmentUser();

public: // From MCompartmentCtxUser

    void CompartmentDeleted();
    
public:
    
	CSigCompCompartment* Compartment();

	void SetAddressL( const TInetAddr& aAddress );

	TBool Match( const TInetAddr& aAddress ) const;
	
	TBool Match( TUint32 aCompartmentId ) const;
	
	void SendFailedL();

    static const TInt iOffset;
	
protected: // 2nd phase constructor

	void ConstructL();

private:

	CSipSigCompCompartmentUser( MCompartmentUserOwner& aMyOwner,
                                TUint32 aCompartmentId,
                                TUint32 aIapId,
                                TBool aDynamicCompression);


private: // data

    TSglQueLink iLink;

    MCompartmentUserOwner& iMyOwner;
	
	TUint32 iCompartmentId;
	TUint32 iIapId;
	TBool iDynamicCompression;
	
	CSipSigCompCompartmentCtx* iCompartmentCtx;

private: //For testing purposes
#ifdef CPPUNIT_TEST	
	friend class CSigCompTest;
#endif
};

#endif // end of __SIPSIGCOMPCOMPARTMENTUSER_H__
