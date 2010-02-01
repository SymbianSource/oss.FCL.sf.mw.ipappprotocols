// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name        : CProxyQuery.cpp
// Part of     : ProxyResolver
// Version     : SIP/4.0
//



#include "CProxyQuery.h"
#include "MSIPProxyResolverObserver.h"
#include "MQueryObserver.h"
#include <utf.h>
#include <nifman.h>


const TInt CProxyQuery::iOffset = _FOFF(CProxyQuery, iLink);

// -----------------------------------------------------------------------------
// CProxyQuery::NewL
// -----------------------------------------------------------------------------
//
CProxyQuery* CProxyQuery::NewL( TUint                      aRequestId, 
                                TUint32                    aIAPId,
                                MSIPProxyResolverObserver& aResolverObserver,
                                MQueryObserver&            aQueryObserver,
                                RSocketServ&               aServer )
    {
    CProxyQuery* self = CProxyQuery::NewLC( aRequestId, 
                                            aIAPId, 
                                            aResolverObserver, 
                                            aQueryObserver, 
                                            aServer );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CProxyQuery::NewLC
// -----------------------------------------------------------------------------
//
CProxyQuery* CProxyQuery::NewLC( TUint                      aRequestId,
                                 TUint32                    aIAPId,
                                 MSIPProxyResolverObserver& aResolverObserver,
                                 MQueryObserver&            aQueryObserver,
                                 RSocketServ&               aServer )
    {
    CProxyQuery* self = new( ELeave )CProxyQuery( aRequestId,
                                                  aIAPId,
                                                  aResolverObserver,
                                                  aQueryObserver,
                                                  aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
// -----------------------------------------------------------------------------
// CProxyQuery::~CProxyQuery
// -----------------------------------------------------------------------------
//  
CProxyQuery::~CProxyQuery ()
    {
    delete iConnCont;    
    delete iResultsArray;
    }

// -----------------------------------------------------------------------------
// CProxyQuery::Connection()
// returns the RConnection object
// -----------------------------------------------------------------------------
//  
RConnection & CProxyQuery::Connection() 
    {  
    return iConnCont->Connection(); 
    }
    
// -----------------------------------------------------------------------------
// CProxyQuery::ConnectionOpened()
// returns connection opening status
// -----------------------------------------------------------------------------
//  
TBool CProxyQuery::ConnectionOpened()     
    { 
    return iConnectionOpened;   
    }

// -----------------------------------------------------------------------------
// CProxyQuery::ConstructL
// -----------------------------------------------------------------------------
// 
void CProxyQuery::ConstructL()
    {
    iConnCont = CProxyConnectionContainer::NewL( *this, iIAPId, iServer );   
    iSipServerAddrBuf().index = 0;  
    iSipServerDomainBuf().index = 0;  
    iCurrentQueryType = KConnGetSipServerDomain;
    iResultsArray = new (ELeave) CDesC8ArrayFlat(1);    
    }
    
// -----------------------------------------------------------------------------
// CProxyQuery::CProxyQuery
// -----------------------------------------------------------------------------
//
CProxyQuery::CProxyQuery( TUint                        aRequestId,
                          TUint32                      aIAPId, 
                          MSIPProxyResolverObserver&   aResolverObserver,
                          MQueryObserver&              aQueryObserver,
                          RSocketServ&                 aServer )
    :   iRequestId( aRequestId ),
        iIAPId( aIAPId ),
        iResolverObserver( aResolverObserver ),
        iQueryObserver( aQueryObserver ),
        iConnectionOpened( EFalse ),
        iServer( aServer )
    {
    }

// -----------------------------------------------------------------------------
// CProxyQuery::ResolvingRequestComplete
// -----------------------------------------------------------------------------
//
void CProxyQuery::ResolvingRequestComplete()
    {
    if ( iResultsArray->Count() > 0 )
        {
        iResolverObserver.ProxyResolvingRequestComplete( iRequestId, 
                                                         iResultsArray ); 
        iResultsArray = NULL; // ownership transferred       
        }
    else
        {
        ResolvingRequestFailed( KErrNotFound );
        }
    }
    
// -----------------------------------------------------------------------------
// CProxyQuery::ResolvingRequestFailed
// -----------------------------------------------------------------------------
//
void CProxyQuery::ResolvingRequestFailed( TInt aError )
    {
    iResolverObserver.ProxyResolvingRequestFailed( iRequestId, aError );
    }

// -----------------------------------------------------------------------------
// CProxyQuery::RequestId
// -----------------------------------------------------------------------------
//
TUint CProxyQuery::RequestId() const
    {
    return iRequestId;
    }
    
// -----------------------------------------------------------------------------
// CProxyQuery::IAPId
// -----------------------------------------------------------------------------
//
TUint32 CProxyQuery::IAPId() const
    {
    return iIAPId;
    }

// -----------------------------------------------------------------------------
// CProxyQuery::ConnectionReady
// -----------------------------------------------------------------------------
//
void CProxyQuery::ConnectionReady()
    {
    iConnectionOpened = ETrue;
    iQueryObserver.ConnectionReady(this);    
    }

// -----------------------------------------------------------------------------
// CProxyQuery::ErrorOccured
// -----------------------------------------------------------------------------
//
void CProxyQuery::ErrorOccured( TInt aError )
    {
    iConnectionOpened = EFalse;   //just to be sure..
    iQueryObserver.ErrorOccured( aError, this );
    }


// -----------------------------------------------------------------------------
// CProxyQuery::CurrentQueryType
// -----------------------------------------------------------------------------
//
TUint CProxyQuery::CurrentQueryType()
    {
    return iCurrentQueryType;
    }
    
// -----------------------------------------------------------------------------
// CProxyQuery::SetQueryType
// -----------------------------------------------------------------------------
//
void CProxyQuery::SetQueryType( TUint aNewType )
    {    
    iCurrentQueryType = aNewType;
    }            

// -----------------------------------------------------------------------------
// CProxyQuery::SetNextIndex
// -----------------------------------------------------------------------------
//
void CProxyQuery::SetNextIndex()
    {
    if ( iCurrentQueryType == KConnGetSipServerAddr )
        {        
	    (iSipServerAddrBuf().index)++;
        }
    else
        {
	    (iSipServerDomainBuf().index)++;
        }
    }            

// -----------------------------------------------------------------------------
// CProxyQuery::GetBuf
// -----------------------------------------------------------------------------
//
TDes8* CProxyQuery::GetBuf()
    {
    if ( iCurrentQueryType == KConnGetSipServerAddr )
        {        
	    return &iSipServerAddrBuf;
        }
    else
        {
        return &iSipServerDomainBuf;            
        }
    }            
   
// -----------------------------------------------------------------------------
// CProxyQuery::AddResultsL
// -----------------------------------------------------------------------------
//
void CProxyQuery::AddResultsL()
    {
    THostName addr;
    if ( iCurrentQueryType == KConnGetSipServerAddr )
        {
        iSipServerAddrBuf().address.Output( addr );    
	     
        }        
    else
        {
        addr.Copy( iSipServerDomainBuf().domainName );
        }
        
    HBufC8* addrBuf = HBufC8::NewLC( 2*addr.Length() );
    TPtr8 addrBufPtr = addrBuf->Des();
    User::LeaveIfError( 
        CnvUtfConverter::ConvertFromUnicodeToUtf8( addrBufPtr, addr ) );   
    if ( addrBufPtr.Length() > 0 )
        {
        iResultsArray->AppendL( addrBufPtr );   	                   	
        }
    CleanupStack::PopAndDestroy( addrBuf );
    }


