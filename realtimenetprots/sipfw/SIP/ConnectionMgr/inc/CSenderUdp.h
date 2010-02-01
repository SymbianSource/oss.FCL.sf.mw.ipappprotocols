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
* Name          : CSenderUdp.h
* Part of       : ConnectionMgr
* Version       : SIP/5.1 
*
*/




/**
 @internalComponent
*/
#ifndef CSENDERUDP_H
#define CSENDERUDP_H

#include "CSender.h"
#include <e32base.h>

class CSIPMessage;
class MContext;
class COwnerSettingsList;


class CSenderUdp : public CSender
	{
public:

	static CSenderUdp* NewL( MContext& aContext, COwnerSettingsList& aSettingsList );
	static CSenderUdp* NewLC( MContext& aContext, COwnerSettingsList& aSettingsList );
	~CSenderUdp();
	
	TBool SendL( const TSIPTransportParams& aTransportParams,
	             CSIPMessage& aMessage,
			     const TInetAddr& aAddr,
			     TUint aOrigTransport,
			     TRequestStatus& aStat,
			     TBool aStore,
			     TBool aForceUDP );
	void SendL( COutgoingData* aData );
	void SendNextL();
	void SetSendPermission( TBool aPermissionToSend );
	
private:

	CSenderUdp( MContext& aContext, COwnerSettingsList& aSettingsList );
	           
	void EncodeAndSendL( TBool aRemoveFromList );
	
private: // Data

    TBool iPermissionToSend;
	
	};
	
#endif // end of CSENDERUDP_H

// End of File
