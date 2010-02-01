/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSenderTcp.h
* Part of       : ConnectionMgr
* Version       : SIP/5.1
*
*/




/**
 @internalComponent
*/
#ifndef CSENDERTCP_H
#define CSENDERTCP_H

#include "CSender.h"
#include <e32base.h>

class COwnerSettingsList;


class CSenderTcp : public CSender
	{
public:
	static CSenderTcp* NewL(MContext& aContext, COwnerSettingsList& aSettingsList);
	static CSenderTcp* NewLC(MContext& aContext, COwnerSettingsList& aSettingsList);
	~CSenderTcp();
	
	TBool SendL(const TSIPTransportParams& aTransportParams,
	            CSIPMessage& aMessage,
			    const TInetAddr& aAddr,
			    TUint aOrigTransport,
			    TRequestStatus& aStat,
			    TBool aStore,
			    TBool aForceUDP);
	void SendL(const TSIPTransportParams& aTransportParams,
	           CSIPMessage& aMessage, 
	           TRequestStatus& aStat);
	void SendNextL();
	
	COutgoingData* StoredData();
	COutgoingData* Data();	
	
private:
	CSenderTcp(MContext& aContext, COwnerSettingsList& aSettingsList);
	void EncodeAndSendL(const TInetAddr& aAddr);
	TBool HandleFirstSendL(const TSIPTransportParams& aTransportParams,
                           CSIPMessage& aMessage, 
                           const TInetAddr& aAddr, 
                           TUint aOrigTransport, 
                           TRequestStatus& aStat,
                           TBool aStore);
    void DoSend( const TDesC8& aData );
    
	};

#endif // end of CSENDERTCP_H

// End of File
