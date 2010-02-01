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
* Name          : TTlsTransStateBase.h
* Part of       : ConnectionMgr
* Version       : SIP/5.0 
*
*/




/**
 @internalComponent
*/


#ifndef TTLSTRANSSTATEBASE_H
#define TTLSTRANSSTATEBASE_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class TSIPTransportParams;
class TInetAddr;
class CSIPMessage;
class MTlsTransStateOwner;

// CLASS DECLARATION
/**
* A base class for TLS transport states.
*
*/
class TTlsTransStateBase
	{
    public:	// State enums
    
        enum TState
            {
            ETcpConnecting,
            ETlsConnecting,
            ETlsConnected,
            EMaxStates
            };
	    
	public: // New functions
	
	    virtual void EnterL();
	    
	    virtual TBool ConnectionOpenL();
	    
	    virtual TBool DisconnectedL();
	    
	    virtual void SendToNetL( const TSIPTransportParams& aParams,
                                 const TInetAddr& aAddress, 
                                 CSIPMessage& aMessage, 
                                 TUint aOrigTransport,
                                 TRequestStatus& aStatus );

    protected: // Constructor
    
        TTlsTransStateBase( MTlsTransStateOwner& aOwner );
        
    protected: // Data

        MTlsTransStateOwner& iOwner;
	};
	
#endif // TTLSTRANSSTATEBASE_H

// End of File
