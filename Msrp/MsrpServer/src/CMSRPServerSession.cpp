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

#include "E32cmn.h"

#include "MSRPCommon.h"
#include "CMSRPServer.h"
#include "CMSRPServerSession.h"
#include "CMSRPServerSubSession.h"
#include <e32math.h>


CMSRPServerSession::CMSRPServerSession(CMSRPServer& aServer) 
: CSession2(), iMSRPServer(aServer), iSubSessionList(_FOFF(CMsrpHandleObj,iLink))
	{
	}


CMSRPServerSession::~CMSRPServerSession()
	{
	CMsrpHandleObj* pHandleObj;
    TSglQueIter<CMsrpHandleObj> iter(iSubSessionList);
	pHandleObj = iter++;
    while ( pHandleObj )
        {
        iSubSessionList.Remove(*pHandleObj);
        delete pHandleObj;
        pHandleObj = iter++;
        }

	iMSRPServer.SessionDeleted();
	}


CMSRPServerSession* CMSRPServerSession::NewLC( CMSRPServer& aServer )
	{
	CMSRPServerSession* self = new(ELeave) CMSRPServerSession(aServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


CMSRPServerSession* CMSRPServerSession::NewL( CMSRPServer& aServer )
	{
	CMSRPServerSession* self = CMSRPServerSession::NewLC(aServer);
	CleanupStack::Pop(self);
	return self;
	}


void CMSRPServerSession::ConstructL()
	{
	MSRPLOG( "CMSRPServerSession::ConstructL" );
	iMSRPServer.SessionCreated();
	}

/* The Client Server Session protocol 
   MSRP sub session creation and deletion
   1. Client creates a MSRP sub session  by sending a code EMSRPCreateSession. 
   	  No other parameters are required.
   	  As a result of this the server returns it a handle to a the created sub-session.
   	  The returned handle is used in the client side reflection and is used by the 
   	  client sub-session to communicate with the server side subsession.
   2. The Client deletes the MSRP sub session by sending a code EMSRPDeleteSession.
      The handle of the sub session to be deleted is send as a argument.
      
*/
void CMSRPServerSession::ServiceL( const RMessage2& aMessage )
	{
   /* RProcess proc;
    RThread thread;
    MSRPLOG2( "CMSRPServerSession::ServiceL... Server Process Priority%d", proc.Priority());
    MSRPLOG2( "CMSRPServerSession::ServiceL... Server Thread Priority%d", thread.Priority());*/
	// Implements
	// - Handling of Session requests like Capability Query.
	// - Handling of Session requests like Application Capability checks. 
	// - Creation/Deletion request for MSRP SubSessions.
	// - Dispatch of messages to relevant MSRP subsessions.

    MSRPLOG( "CMSRPServerSession::ServiceL() Entered" );
	switch(aMessage.Function())
		{
		case EMSRPCreateSubSession:
		    MSRPLOG( "CMSRPServerSession::ServiceL... EMSRPCreateSubSession" );
			CreateSubSessionL(aMessage);
			break;
		
		case EMSRPCloseSubSession:
		    MSRPLOG( "CMSRPServerSession::ServiceL... EMSRPCloseSubSession" );
			CloseSubSessionL(aMessage);
			break;
			
		default:	
		    MSRPLOG( "CMSRPServerSession::ServiceL... default" );
			// Message for the subSession
			DispatchToSubSessionL(aMessage);
			break;
		}

    MSRPLOG( "CMSRPServerSession::ServiceL() Exit" );

	}


void CMSRPServerSession::CreateSubSessionL( const RMessage2& aMessage )
	{	
	MSRPLOG( "Entered createSubSessionL()" );
	
	/* First notify the Server so that connections can be started */
	iMSRPServer.CreatingSubSessionL( aMessage.Int0() );
    TBuf8< KMaxLengthOfSessionId > messageId;
    aMessage.ReadL( 1, messageId );
	
	CMSRPServerSubSession* subSession = CMSRPServerSubSession::NewL( 
	        *this, iMSRPServer.StateFactory(), messageId );
	CleanupStack::PushL(subSession);
			
	//check handle doesnt preexist or add current time to handle
	//generate new handle
	CMsrpHandleObj* handleObj = CMsrpHandleObj::NewL(subSession);
	
	CleanupStack::Pop(subSession); //Ownership with handleobj
	CleanupStack::PushL(handleObj);	//dummy push
	iSubSessionList.AddLast(*handleObj); //ownership of handle with list 
	CleanupStack::Pop(handleObj); //dummy pop
	
	WriteResponseL(aMessage, ESubSessionHandle, handleObj->Handle());	


	MSRPLOG2("CMSRPServerSession::CreateSubSessionL Created subsession with handle %d", handleObj->Handle())
	aMessage.Complete(KErrNone);
	}


void CMSRPServerSession::CloseSubSessionL( const RMessage2& aMessage )
	{	
	MSRPLOG( "Entered deleteSubSessionL()" );
	// Locate the subSession and delete.
	TInt handle = aMessage.Int3();
	CMsrpHandleObj* pHandleObj = FindHandle(handle);

	if(!pHandleObj)
	    {
        _LIT(KBadHandle,"Bad Handle");      
        aMessage.Panic(KBadHandle,EBadSubsessionHandle);	    
	    }
	
	iSubSessionList.Remove(*pHandleObj);
	delete pHandleObj;
	aMessage.Complete(KErrNone);
	}


void CMSRPServerSession::DispatchToSubSessionL( const RMessage2& aMessage )
	{	
	MSRPLOG( "Entered dispatchToSubSession()" );
	// Get the sub session handle	
	TInt handle = aMessage.Int3();
	MSRPLOG2( "Received aMessage for subSession ( %d ) ", aMessage.Int3());
	CMsrpHandleObj* pHandleObj = FindHandle(handle);
	if(!pHandleObj)
		{
		_LIT(KBadHandle,"Bad Handle");		
		aMessage.Panic(KBadHandle,EBadSubsessionHandle);		
		}
    pHandleObj->Subsession()->ServiceL(aMessage);
	}


void CMSRPServerSession::WriteResponseL( const RMessage2& aMessage,
                                         TServerClientResponses aResponseItem, 
                                         TInt aValue )
    {
    MSRPLOG( "CMSRPServerSession::WriteResponseL..." );
	//Package the interger value and do a write.
    TPckg< TInt > valuePackage(aValue);
        
	aMessage.WriteL(aResponseItem, valuePackage);
	}


MMSRPConnectionManager& CMSRPServerSession::ConnectionManager()
	{
	return iMSRPServer.ConnectionManager();
	}

CMsrpHandleObj* CMSRPServerSession::FindHandle( TInt aHandle )
    {       
    CMsrpHandleObj* pHandleObj = NULL;
    TSglQueIter<CMsrpHandleObj> iter(iSubSessionList);  
	
    pHandleObj = iter++;
    while ( pHandleObj )
        {
        if(pHandleObj->Compare(aHandle))
            break;  
        pHandleObj = iter++;
        }
    return pHandleObj;
    }




// CHandleObj class function definitions

CMsrpHandleObj* CMsrpHandleObj::NewL( CMSRPServerSubSession* aSubSession)
    {
    CMsrpHandleObj* self = new(ELeave) CMsrpHandleObj(aSubSession);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }


CMSRPServerSubSession* CMsrpHandleObj::Subsession() const
    {
    return iSubSession;
    }


TUint CMsrpHandleObj::Handle() const
    {
    return iHandle;
    }
    

TBool CMsrpHandleObj::Compare( TUint aHandle ) const
    {
    if (iHandle == aHandle)
        return ETrue;
    
    return EFalse;    
    }
    

CMsrpHandleObj::~CMsrpHandleObj()
    {
    delete iSubSession;        
    }
    

void CMsrpHandleObj::ConstructL()
    {
    iHandle = Math::Random();
    }
    
    
CMsrpHandleObj::CMsrpHandleObj( CMSRPServerSubSession* aSubSession)
    {
    iSubSession = aSubSession;
    }
