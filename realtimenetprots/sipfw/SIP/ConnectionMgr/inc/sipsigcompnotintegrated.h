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
* Name          : sipsigcompnotintegrated.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/
#ifndef __SIPSIGCOMPNOTINTEGRATED_H__
#define __SIPSIGCOMPNOTINTEGRATED_H__

#include <e32base.h>
#include "MSigCompController.h"


class CSigCompNotIntegrated : public CBase, public MSigCompController
	{

public:
	~CSigCompNotIntegrated() {}
	static CSigCompNotIntegrated* NewL();
	static CSigCompNotIntegrated* NewLC();

	TBool IsSupported () const;

    TUint32 CreateCompartmentL (TUint32 aIapId);

    void RemoveCompartment (TUint32 aCompartmentId);

	CBufBase* EncodeL (const TSIPTransportParams& aTransportParams,
	                   CSIPRequest& aRequest,
                       const TInetAddr& aAddress,
					   TBool aStreambasedProtocol,
					   TBool& aCompressed);

	CBufBase* EncodeL (const TSIPTransportParams& aTransportParams,
	                   CSIPResponse& aResponse,
                       const TInetAddr& aAddress,
					   TBool aStreambasedProtocol,
					   TBool& aCompressed);

	TBool IsCompleteSigCompMessageL (const TDesC8& aMessage) const;

	TBool IsSigCompMsg (const TDesC8& aMessage) const;

	CBufBase* DecompressL (const TDesC8& aMessage,
                           TUint& aBytesConsumed,
                           TBool aIsStreamBased);

	TInt CompartmentCount ();

	void AllowL(const TInetAddr& aAddr, TUint32 aIapId);

	void Deny();

	void SendFailedL (TUint32 aCompartmentId);

private:
	CSigCompNotIntegrated() {}
	};
	
#endif //__SIPSIGCOMPNOTINTEGRATED_H__

// End of File
