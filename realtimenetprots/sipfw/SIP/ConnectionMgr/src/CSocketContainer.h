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
* Name        : CSocketContainer.h
* Part of     : ConnectionMgr
* See class definition below.
* Version     : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef CSOCKETCONTAINER_H
#define CSOCKETCONTAINER_H
  
// INCLUDES
#include <e32base.h>
#include <in_sock.h>

// FORWARD DECLARATIONS
class CSecureSocket;

// CLASS DEFINITION
//
class CSocketContainer : public CBase
    {
    public: // Constructor and destructor
        
        static CSocketContainer* NewL( RSocket& aSocket );
        
        static CSocketContainer* NewLC( RSocket& aSocket );   
         
        ~CSocketContainer();
    
    public: // Function wrappers for socket
    
        virtual void Send( const TDesC8& aDesc, 
                           TUint aFlags, 
                           TRequestStatus& aStatus );
                   
        virtual void SendTo( const TDesC8& aDesc,
                             TSockAddr& aAddr,
                             TUint aFlags,
                             TRequestStatus& aStatus );
        
        virtual void CancelSend();
        
        virtual void RecvOneOrMore( TDes8& aDes, 
                                    TUint aFlags, 
                                    TRequestStatus& aStatus, 
                                    TSockXfrLength& aLen );
                            
        virtual void CancelRecv();
        
        virtual void Connect( TSockAddr& aAddr, TRequestStatus& aStatus );
        
        virtual void CancelConnect();
        
        virtual void CancelAll();
        
        virtual void RemoteName( TSockAddr& aAddr );
        
        virtual TInt SetOpt( TUint aOptionName, 
                             TUint aOptionLevel, 
                             const TDesC8& aOption );
        
        virtual TInt SetOpt( TUint aOptionName, 
                             TUint aOptionLevel, 
                             TInt aOption );
        
        virtual TBool HasSecureSocket() const;
        
        virtual CSecureSocket* SecureSocket();
        
        RSocket& Socket();
                     
    protected:

        CSocketContainer( RSocket& aSocket );
        
    protected: // Data
        
        RSocket& iSocket;
    
    };
    
#endif // CSOCKETCONTAINER_H
