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
* Name          : MTlsTransStateOwner.h
* Part of       : ConnectionMgr
* Version       : SIP/5.0 
*
*/




/**
 @internalComponent
*/


#ifndef MTLSTRANSSTATEOWNER_H
#define MTLSTRANSSTATEOWNER_H

//  INCLUDES
#include "TTlsTransStateBase.h"

// FORWARD DECLARATIONS
class CSender;

// CLASS DECLARATION
/**
*  Defines an interface of CTlsTransport used by state classes.
*/
class MTlsTransStateOwner
	{
    public:
    
        virtual void ChangeStateL( TTlsTransStateBase::TState aState ) = 0;
    	
    	virtual void ConnectTcpL() = 0;
    	
    	virtual void ConnectTlsL() = 0;
    	
    	virtual TBool Retry() const = 0;
    	
    	virtual void ResetL() = 0;
    	
    	virtual void Destroy() = 0;

        virtual CSender& GetSender() = 0;
        
        virtual void StartTimerL( TInt aTimeout ) = 0;
        
        virtual  void StopTimer() = 0;
        
        virtual TBool HasSecureSocket() = 0;
		
        virtual TBool MatchCertificateL() = 0;
        
	};
	
#endif // MTLSTRANSSTATEOWNER_H

// End of File

