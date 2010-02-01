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
* Name          : MSigCompOwner.h
* Part of       : SIP SigComp Controller
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/


#ifndef __MSIGCOMPOWNER_H__
#define __MSIGCOMPOWNER_H__

#include <e32base.h>
#include <in_sock.h>
class CSIPResponse;
class CSIPRequest;
class CSigComp;
class TSIPTransportParams;

class MSigCompOwner
	{
public:
	virtual ~MSigCompOwner() {}

	virtual CBufBase* EncodeSipL( CSIPResponse& aResponse,
							      TBool aStreambasedProtocol ) = 0;
	virtual CBufBase* EncodeSipL( CSIPRequest& aRequest,
							      TBool aStreambasedProtocol ) = 0;
	virtual CBufBase* EncodeSipAndCompressL( const TSIPTransportParams& aTransportParams,
	                                         CSIPResponse& aResponse,
											 const TInetAddr& aAddress,
											 TBool aStreambasedProtocol ) = 0;

	virtual CBufBase* EncodeSipAndCompressL( const TSIPTransportParams& aTransportParams,
	                                         CSIPRequest& aRequest,
											 const TInetAddr& aAddress,
											 TBool aStreambasedProtocol ) = 0;
	virtual CSigComp* SigComp() = 0;
	virtual TBool HasCompartmentUser( const TSIPTransportParams& aTransportParams ) = 0;
	virtual TBool Match( const TDesC8& aAddress ) = 0;
	};

#endif // end of __MSIGCOMPOWNER_H__

// End of File
