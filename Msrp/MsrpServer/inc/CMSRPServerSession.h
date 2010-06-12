/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html."
* Initial Contributors:
* Nokia Corporation - initial contribution.
* Contributors:
*
* Description:
* MSRP Implementation
*
*/

#ifndef CMSRPSERVERSESSION_H_
#define CMSRPSERVERSESSION_H_

#include "E32base.h"

// Forward declarations
class CMSRPServer;
class RMessage2;
class CMSRPServerSubSession;
class MMSRPConnectionManager;


// Class to handle multiple sub sessions

class CMsrpHandleObj : public CBase
    {
public:
    static CMsrpHandleObj* NewL( CMSRPServerSubSession* aSubSession);
    CMSRPServerSubSession* Subsession() const;
    TUint Handle() const;
    inline TBool Compare( TUint aHandle ) const;
    virtual ~CMsrpHandleObj();
    
private:
    void ConstructL();     
    CMsrpHandleObj( CMSRPServerSubSession* aSubSession);

public:
    TSglQueLink iLink;    
    
private:      
    CMSRPServerSubSession* iSubSession;
    TUint iHandle;    
    };



class CMSRPServerSession : public CSession2
    {
public:
    static CMSRPServerSession* NewL( CMSRPServer& aServer );
    static CMSRPServerSession* NewLC( CMSRPServer& aServer );
            
    ~CMSRPServerSession();
    void ServiceL( const RMessage2& aMessage ); 
    MMSRPConnectionManager& ConnectionManager();
        
private:
        
    void ConstructL();     
    CMSRPServerSession( CMSRPServer& aServer );

    void CreateSubSessionL( const RMessage2& aMessage );
    void CloseSubSessionL( const RMessage2& aMessage );
    void DispatchToSubSessionL( const RMessage2& aMessage );

    // ITC Write functions. Must be moved out to a common location.
    void WriteResponseL( const RMessage2& aMessage,
                         TServerClientResponses aResponseItem, 
      					 TInt aValue );
        
    CMsrpHandleObj* CMSRPServerSession::FindHandle( TInt aHandle );        

private:        
    CMSRPServer& iMSRPServer;
    TSglQue<CMsrpHandleObj> iSubSessionList;
    };

#endif /* CMSRPSERVERSESSION_H_ */
