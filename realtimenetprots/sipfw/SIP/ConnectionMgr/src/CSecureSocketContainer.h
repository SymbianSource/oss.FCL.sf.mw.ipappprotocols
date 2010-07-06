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
* Name        : CSecureSocketContainer.h
* Part of     : ConnectionMgr
* See class definition below.
* Version     : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef CSECURESOCKETCONTAINER_H
#define CSECURESOCKETCONTAINER_H
  
// INCLUDES
#include "CSocketContainer.h"

// FORWARD DECLARATIONS
class CSecureSocket;

// CLASS DEFINITION
//
class CSecureSocketContainer : public CSocketContainer
    {
    public: // Constructor and destructor
        
        static CSecureSocketContainer* NewL( RSocket& aSocket );
        
        static CSecureSocketContainer* NewLC( RSocket& aSocket );   
         
        ~CSecureSocketContainer();
    
    public: // From CSocketContainer
    
        void Send( const TDesC8& aDesc, 
                   TUint aFlags, 
                   TRequestStatus& aStatus );
        
        void CancelSend();
        
        void RecvOneOrMore( TDes8& aDes, 
                            TUint aFlags, 
                            TRequestStatus& aStatus, 
                            TSockXfrLength& aLen );
                            
        void CancelRecv();
        
        void Connect( TSockAddr& aAddr, TRequestStatus& aStatus );
        
        void CancelConnect();
        
        void CancelAll();
        
        void RemoteName( TSockAddr& aAddr );
        
        TInt SetOpt( TUint aOptionName, TUint aOptionLevel, const TDesC8& aOption );
        
        TInt SetOpt( TUint aOptionName, TUint aOptionLevel, TInt aOption );
        
        TBool HasSecureSocket() const;
        
        CSecureSocket* SecureSocket();
    
    public: // New methods
    
        void SetSecureSocket( CSecureSocket* aSecureSocket );
          
    private: // Private constructor

        CSecureSocketContainer( RSocket& aSocket );
        
    private: // Data
        
        CSecureSocket* iSecureSocket; // Not owned
        
    };
    
#endif // CSECURESOCKETCONTAINER_H
