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
* Name          : TTlsTransTlsConnected.h
* Part of       : ConnectionMgr
* Version       : SIP/5.0 
*
*/




/**
 @internalComponent
*/


#ifndef TTLSTRANSTLSCONNECTED_H
#define TTLSTRANSTLSCONNECTED_H

//  INCLUDES
#include "TTlsTransStateBase.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
* TLS Transport in connecting TCP state.
*
*/
class TTlsTransTlsConnected : public TTlsTransStateBase
	{	    
	public: // New functions
	
	    TTlsTransTlsConnected( MTlsTransStateOwner& aOwner, 
	                           TInt aIdleTimeout );
	
	public: // From TTlsTransStateBase
	
	    void EnterL();
	
	    TBool DisconnectedL();
	    
	    void SendToNetL( const TSIPTransportParams& aParams,
                         const TInetAddr& aAddress, 
                         CSIPMessage& aMessage, 
                         TUint aOrigTransport,
                         TRequestStatus& aStatus );
	
	private:
	
	    TInt iIdleTimeout;
	        
	};
	
#endif // TTLSTRANSTLSCONNECTED_H

// End of File
