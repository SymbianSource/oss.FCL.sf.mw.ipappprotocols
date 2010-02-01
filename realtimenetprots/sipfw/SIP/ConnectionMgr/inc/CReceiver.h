/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CReceiver.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __CRECEIVER_H__
#define __CRECEIVER_H__

#include <e32base.h>
#include <in_sock.h>

class MContext;
class CSocketContainer;


class CReceiver : public CActive
	{

public:

    enum TReceiverStates
        {
        EIdle,
        EListening,
        EReceiving
        };
        
public: // Constructors and destructor

	static CReceiver* NewL( MContext& aContext, TUint aIPAddrFamily );
	static CReceiver* NewLC( MContext& aContext, TUint aIPAddrFamily );
		
	~CReceiver();

public: // New methods
	
	void Listen();
	
public: // From CActive

    void RunL();
    
	void DoCancel();
	
	TInt RunError( TInt aError );
		
private:
    
    CReceiver( MContext& aContext, TUint aIPAddrFamily );
    
    void ReceivedDataL( HBufC8* aData );
    
    void ReceivePendingL();    

private:
   
    TInetAddr iAddr;
    MContext& iContext;
    TUint iIPAddrFamily;
    
    TReceiverStates iState;

	HBufC8* iReceivedMsg;
	TPtr8 iReceivedMsgPtr;
	
	TPckgBuf< TUint > iFlags;
	
private: // For unit testing purposes
#ifdef CPPUNIT_TEST
    friend class CTransportUdpTest;	
#endif
	};
	
#endif // end of __CRECEIVER_H__

// End of File
