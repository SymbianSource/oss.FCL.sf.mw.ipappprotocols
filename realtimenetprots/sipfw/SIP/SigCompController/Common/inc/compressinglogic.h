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
* Name          : compressinglogic.h
* Part of       : SIP SigComp Controller
* Version       : SIP/3.0 
*
*/




/**
 @internalComponent
*/


#ifndef __COMPRESSINGLOGIC_H__
#define __COMPRESSINGLOGIC_H__
#include <e32base.h>
#include <in_sock.h>

// FORWARD DECLARATIONS
class CSIPResponse;
class CSIPRequest;
class CSIPMessage;
class CSIPURI;
class MSigCompOwner;
class CSIPViaHeader;
class TSIPTransportParams;

// CLASS DEFINITION
class CSigCompCompressingLogic : public CBase
	{
public: // Constructors and destructor

	static CSigCompCompressingLogic* NewL( MSigCompOwner& aSigCompOwner );
	static CSigCompCompressingLogic* NewLC( MSigCompOwner& aSigCompOwner );
	
	~CSigCompCompressingLogic();

public:

	CBufBase* EncodeL( const TSIPTransportParams& aTransportParams,
	                   CSIPResponse& aResponse,
                       const TInetAddr& aAddress,
                       TBool aStreambasedProtocol, 
                       TBool& aCompressed );
	
	CBufBase* EncodeL( const TSIPTransportParams& aTransportParams,
	                   CSIPRequest& aRequest,
                       const TInetAddr& aAddress,
                       TBool aStreambasedProtocol, 
                       TBool& aCompressed );
	

private:

	CSigCompCompressingLogic( MSigCompOwner& aSigCompOwner );

	TBool CompInVia( CSIPMessage& aMessage );
	void SetCompToTopViaL( CSIPMessage& aMessage );
	void DeleteCompFromTopVia( CSIPMessage& aMessage );
	CSIPViaHeader* TopViaHeader( CSIPMessage& aMessage );
	
	TBool HasSigCompParam( CSIPURI& aURI ) const;
	TBool ShouldBeCompressL( const TSIPTransportParams& aTransportParams,
	                         CSIPRequest& aRequest );
	TBool ShouldBeCompressL( CSIPResponse& aResponse );
	void AddSigCompToContactsL( CSIPMessage& aMessage );
	void SetCompEqualSigCompL( CSIPURI& aURI );
	void RemoveCompEqualSigComp( CSIPURI& aURI );
	CSIPURI* SigCompUriFromRequest( CSIPRequest& aRequest );
	
private: // Data

	MSigCompOwner& iSigCompOwner;
	
	};



#endif // end of __COMPRESSINGLOGIC_H__

// End of File
