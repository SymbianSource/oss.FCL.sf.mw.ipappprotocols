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
* Name          : CIpSecSadbHandler.h
* Part of       : SIPSec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CIPSECSADBHANDLER_H
#define CIPSECSADBHANDLER_H

#include "MSipSecSaDbSendReceiveObserver.h"

class RSADB;
class MIpSecMechanismParams;
class MSipSecSadbObserver;
class TSipSecSaParams;
class CSIPSecurityClientHeader;
class CSIPSecurityHeaderBase;
class CSipSecSaDbMsgSender;
class TInetAddr;


// Sadb message hanling
class CIpSecSadbHandler : 
    public CBase, 
    public MSipSecSaDbSendReceiveObserver
	{
public:

    enum TSadbHandlerState
        {
        EAvailable,
        EAddingInbound1,
        EAddingInbound2,
        EAddingOutbound1,
        EAddingOutbound2,
        EDeletingInbound1,
        EDeletingInbound2,
        EDeletingOutbound1,
        EDeletingOutbound2
        };
        
    static CIpSecSadbHandler* NewL( MSipSecSadbObserver& aObserver, 
                                    RSADB& aSADB );
    
	~CIpSecSadbHandler();

private:

    CIpSecSadbHandler( MSipSecSadbObserver& aObserver );   

    void ConstructL( RSADB& aSADB );
    
public: // Public methods

   	void AddSasL( MIpSecMechanismParams& aParams,
				  TInetAddr aLocalAddress,
				  TInetAddr aRemoteAddress,
				  TSipSecSaParams& aSaParams );
				  
	void DeleteSasL( MIpSecMechanismParams& aParams,
					 TInetAddr aLocalAddress,
				     TInetAddr aRemoteAddress,
				     TSipSecSaParams& aSaParams );
				     
	void SetSaParamsL( TSipSecSaParams& aParams, 				     
					   CSIPSecurityHeaderBase& aCliHeader, 
				       CSIPSecurityHeaderBase& aSrvHeader,
				       const TDesC8& aAuthKey );
		
	void CancelAddSaOps();
	
	void SADBAddMsgReceived( TUint aMsgSeq, TInt aError );
	      
    
public: // Functions from MSipSecSaDbSendReceiveObserver

	void MessageSentL( TInt aStatus,
					   TInt aMsgType );

	void MessageReceived( TInt aStatus );
	
	
private: // Private methods

    void IssueRequestL( MIpSecMechanismParams& aParams,
	                    TInetAddr& aLocalAddress,
	                    TInetAddr& aRemoteAddress,
	                    TSipSecSaParams& aSaParams );
	                    
	TBool IsAvailable();
	
	void Reset();
	
	void AddCompletedL( TInt aError );
	
	void DeleteCompletedL();
	                    
	void AddSaL( TInetAddr& aSourceAddress,
                 TInetAddr& aDestinationAddress,
                 TUint aSpi );
				     
	void DeleteSaL( TInetAddr& aSourceAddress,
                    TInetAddr& aDestinationAddress,
                    TUint aSpi );
                    
    void SAsAddedL( TBool aSuccess );
	                
    TUint32 ConvertToBigEndian( TUint32 aNum );

private: // Data

    // Not owned
    MSipSecSadbObserver& iObserver;
    
    TSadbHandlerState iState;
    
    RArray<TUint> iOngoingAdds;
    
    CSipSecSaDbMsgSender* iSaDbMsgSender;
    
    // Not owned
    MIpSecMechanismParams* iParams;
    
    // Not owned
    TSipSecSaParams* iSaParams;
    
    TInetAddr iLocalAddress;
    TInetAddr iRemoteAddress;
 
    
#ifdef CPPUNIT_TEST
    friend class CSipSecAgreeRecord;
    friend class CSipIpSecTest;
#endif
    
	};

#endif // CIPSECSADBHANDLER_H


// End of File
