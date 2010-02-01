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
* Name          : CIpSecPolicyHandler.h
* Part of       : SIPSec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CIPSECPOLICYHANDLER_H
#define CIPSECPOLICYHANDLER_H

#include <e32base.h>
#include <ipsecpolapi.h>
#include <in_sock.h>

class MSipSecPolicyObserver;
class TSipSecSaParams;

// IpSec policy hanling
class CIpSecPolicyHandler : public CActive
	{
public:

    enum TPolicyHandlerState
        {
        EIdle,
        ELoading,
        EActivating,
        EActive,
        EUnloading
        };

	static CIpSecPolicyHandler* NewL( MSipSecPolicyObserver& aObserver,
	                                  RIpsecPolicyServ& aPolicyServ );
						 
	static CIpSecPolicyHandler* NewLC( MSipSecPolicyObserver& aObserver,
	                                   RIpsecPolicyServ& aPolicyServ );
							  
	~CIpSecPolicyHandler();
	
    
    void CreatePolicyL( const TInetAddr& aLocalAddress,
	                    const TInetAddr& aRemoteAddress,
	                    const TSipSecSaParams& aSaParams );
	                    
	void RemovePolicy();

protected:
		
	void RunL();
	
	TInt RunError( TInt aError );
    	
	void DoCancel();

private:

    CIpSecPolicyHandler( MSipSecPolicyObserver& aObserver,
                         RIpsecPolicyServ& aPolicyServ );

    HBufC8* GetAddrAsTextLC( const TInetAddr& aAddr );
    
    void RemovePolicyData();
    
    void NotifyActiveL();
    
    void NotifyUnload();
    
    void NotifyError( TInt aError );
    
private: // Data

    MSipSecPolicyObserver& iObserver;
    
    RIpsecPolicyServ& iPolicyServ;
    
    TPolicyHandlerState iHandlerState;
    
    HBufC8* iPolicyData;
    
    TPolicyHandlePckg iPolicyHandle;
    
						  
#ifdef CPPUNIT_TEST
    friend class CActiveObjController1;
#endif
	};

#endif // CIPSECPOLICYHANDLER_H


// End of File
