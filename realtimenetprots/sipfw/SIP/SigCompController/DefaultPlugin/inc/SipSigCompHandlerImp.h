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
* Name          : sipsigcomphandlerimp.h
* Part of       : SIPSigComp
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/
#ifndef __SIPSIGCOMPHANDLERIMP_H__
#define __SIPSIGCOMPHANDLERIMP_H__


#include "msiGcompowner.h"
#include "sipsigcomphandler.h"
#include "MCompartmentCtxOwner.h"
#include "MCompartmentUserOwner.h"

class CSigComp;
class CSipSigCompCompartmentCtx;
class CSipSigCompCompartmentUser;
class CSigCompCompressingLogic;
class TSIPSigCompHandlerInitParams;
class MLocalName;

class CSipSigCompHandlerImpl :
		public CSigCompHandler,
		public MSigCompOwner,
		public MCompartmentCtxOwner,
		public MCompartmentUserOwner
{
public:


	static CSipSigCompHandlerImpl* NewL( TAny* aInitParams );

	static CSipSigCompHandlerImpl* NewLC( TAny* aInitParams );

	/**
	* Destructor.
	*
	* @post All objects and members created by the SigComp Handler are deleted
	*
	*/
    ~CSipSigCompHandlerImpl ();

public: // From MSigCompController

    TUint32 CreateCompartmentL( TUint32 aIapId );

    void RemoveCompartment( TUint32 aCompartmentId );

	CBufBase* DecompressL( const TDesC8& aMessage,
                           TUint& aBytesConsumed,
                           TBool aIsStreamBased );

	CBufBase* EncodeL( const TSIPTransportParams& aTransportParams,
	                   CSIPRequest& aRequest,
                       const TInetAddr& aAddress,
					   TBool aStreambasedProtocol,
                       TBool& aCompressed );

	CBufBase* EncodeL( const TSIPTransportParams& aTransportParams,
	                   CSIPResponse& aResponse,
                       const TInetAddr& aAddress,
                       TBool aStreambasedProtocol,
                       TBool& aCompressed );

	TBool IsCompleteSigCompMessageL( const TDesC8& aMessage ) const;

	TBool IsSigCompMsg( const TDesC8& aMessage ) const;

	TBool IsSupported() const;

	TInt CompartmentCount();

	void SendFailedL( TUint32 aCompartmentId );

	void AllowL( const TInetAddr& aAddress, TUint32 aIapId );

	void Deny();
	

public: // From MSigCompOwner


	CBufBase* EncodeSipL( CSIPResponse& aResponse,
		                  TBool aStreambasedProtocol );

	CBufBase* EncodeSipL( CSIPRequest& aRequest,
		                  TBool aStreambasedProtocol );

	CBufBase* EncodeSipAndCompressL( const TSIPTransportParams& aTransportParams,
	                                 CSIPResponse& aResponse,
            					     const TInetAddr& aAddress,
            						 TBool aStreambasedProtocol );

	CBufBase* EncodeSipAndCompressL( const TSIPTransportParams& aTransportParams,
	                                 CSIPRequest& aRequest,
                                     const TInetAddr& aAddress,
                                     TBool aStreambasedProtocol );

	CSigComp* SigComp() {return iSigComp;}

	TBool Match( const TDesC8& aAddress );

	TBool HasCompartmentUser( const TSIPTransportParams& aTransportParams );
	

public: // From MCompartmentCtxOwner
	
	void KillMe( CSipSigCompCompartmentCtx* aCompartmentCtx );	
	
	
public: // From MCompartmentCtxUser
	
	CSipSigCompCompartmentCtx* CreateCompartmentL( const TInetAddr& aAddress,
                                            	   TUint32 aIapId,
	                                               TBool aDynamicCompression );
	
    CSipSigCompCompartmentCtx* SearchCompartmentByInetAddr( const TInetAddr& aAddress,
	                                                        TUint32 aIapId );
	                                                                                                                
protected:

    void ConstructL();

private:

	CSipSigCompHandlerImpl( TSIPSigCompHandlerInitParams* aInitParams );

private:

    TUint32 CreateCompartmentId();
												   
	TUint32 CreateCompartmentUserL( TUint32 aIapId );

	TInt DeleteCompartmentUser( TUint32 aCompartmentId );					

	CSipSigCompCompartmentUser* SearchCompartmentUserL( TUint32 aCompartmentId,
                                                        const TInetAddr& aAddress );  
                                                                                                      
	CSipSigCompCompartmentUser* SearchCompartmentUserByCompId( TUint32 aCompartmentId );
	
	void RemoveAllCompartments();
     

private: // Data

	CSigComp* iSigComp;
	TSglQue<CSipSigCompCompartmentCtx> iCompartmentList;
	TSglQue<CSipSigCompCompartmentUser> iCompartmentUserList;
	CSigCompCompressingLogic* iCompressingLogic;
	MLocalName& iLocalName;
	TInt iMaxCompartmentCount;
	TUint32 iCompartmentIdCounter;
	
private: //For testing purposes
#ifdef CPPUNIT_TEST	
	friend class CSigCompHandlerTest;
#endif
};

#endif // end of __SIPSIGCOMPHANDLERIMP_H__
