// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : sipsecrequestdata.cpp
// Part of       : SIPSec/DigestPlugin
// Version       : SIP/6.0
//



#include "CSIPSecDigest.h"
#include "sipsecrequestdata.h"
#include "sipsecdigestcontext.h"

_LIT8( KSIPSecReqDataSeparator, ":" );
const TInt KSSIPSecReqDataMethod( 1 );
const TInt KSSIPSecReqDataURI( 2 );
const TInt KSSIPSecReqDataMessage( 3 );
const TInt KSSIPSecReqDataServer( 4 );


// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// RSIPSecRequestDataField::RSIPSecRequestDataField
// ----------------------------------------------------------------------------
//

RSIPSecRequestDataField::RSIPSecRequestDataField() :
	iValue ( TPtrC8() ),
    iNeedsHashing( EFalse ),
    iBuffer( NULL )
    {
    }
    
// ----------------------------------------------------------------------------
// RSIPSecRequestDataField::RSIPSecRequestDataField
// ----------------------------------------------------------------------------
//		
RSIPSecRequestDataField::RSIPSecRequestDataField( const TDesC8& aValue,
												  TBool aNeedsHashing ) :
    iValue( TPtrC8( aValue ) ),
    iNeedsHashing( aNeedsHashing ),
    iBuffer( NULL )
    {
    }

// ----------------------------------------------------------------------------
// RSIPSecRequestDataField::RSIPSecRequestDataField
// ----------------------------------------------------------------------------
//		
RSIPSecRequestDataField::RSIPSecRequestDataField( TPtrC8 aValue,
												  TBool aNeedsHashing ) :
    iValue( TPtrC8( aValue ) ),
    iNeedsHashing( aNeedsHashing ),
    iBuffer( NULL )
    {
    }

// ----------------------------------------------------------------------------
// RSIPSecRequestDataField::RSIPSecRequestDataField
// ----------------------------------------------------------------------------
//		
RSIPSecRequestDataField::RSIPSecRequestDataField( HBufC8* aValue,
												  TBool aNeedsHashing ) :
    iValue( TPtrC8( aValue->Des() ) ),
    iNeedsHashing( aNeedsHashing ),
    iBuffer( aValue )
    {
    }
    
// ----------------------------------------------------------------------------
// RSIPSecRequestDataField::NeedsHashing
// ----------------------------------------------------------------------------
//		
TBool RSIPSecRequestDataField::NeedsHashing() const
    {
    return iNeedsHashing;
    }
    
// ----------------------------------------------------------------------------
// RSIPSecRequestDataField::Value
// ----------------------------------------------------------------------------
//		
TPtrC8 RSIPSecRequestDataField::Value()
    {
    return iValue;
    }

// ----------------------------------------------------------------------------
// RSIPSecRequestDataField::Close
// ----------------------------------------------------------------------------
//		
void RSIPSecRequestDataField::Close()
    {
    delete iBuffer;
    iBuffer = NULL;
    }



// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CSIPSecRequestData::NewL
// ----------------------------------------------------------------------------
//
CSIPSecRequestData* CSIPSecRequestData::NewL( TSIPSecDigestCtxProcess& aContext,
						  					  CSIPSecRequestData::TQop aQop )
    {    
    CSIPSecRequestData* self = new ( ELeave ) CSIPSecRequestData( aContext,
    															  aQop );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CSIPSecRequestData::CSIPSecRequestData
// ----------------------------------------------------------------------------
//		
CSIPSecRequestData::CSIPSecRequestData( TSIPSecDigestCtxProcess& aContext,
										CSIPSecRequestData::TQop aQop ) :
	iDigestContext( aContext ),
	iIterator( 0 ),
	iQop( aQop )
    {    
    if ( iQop == CSIPSecRequestData::EDoesNotExist )
    	{
    	//Previously iQop was set to auth-int if qop didn't exist.
	    //But shouldn't it be set to auth, since A2 is calculated differently
	    //for case when qop=auth or unspecified and when qop=auth-int.
	    //(see RFC 2617 3.2.2.3)
    	iQop = CSIPSecRequestData::EAuth;    	
    	}
    }

// ----------------------------------------------------------------------------
// CSIPSecRequestData::Size
// ----------------------------------------------------------------------------
//		
TInt CSIPSecRequestData::Size() const
    {
    return iSize;        
    }
    
// ----------------------------------------------------------------------------
// CSIPSecRequestData::ConstructL
//
// Calculating A2:
//   If the "qop" directive's value is "auth" or is unspecified, then A2 is:
//     A2 = Method ":" digest-uri-value
//   If the "qop" value is "auth-int", then A2 is:
//     A2 = Method ":" digest-uri-value ":" H(entity-body)
//
// Note: if qop was unspecified, "auth" was selected in the constructor.
// ----------------------------------------------------------------------------
//		
void CSIPSecRequestData::ConstructL()
    {
	__ASSERT_ALWAYS( iQop != EUnknown ,User::Leave( KErrArgument ) );

    const TDesC8& method = iDigestContext.Method();
    const TDesC8& requestURI = iDigestContext.DigestURI();

	iSize = method.Length() +
			KSIPSecReqDataSeparator().Length() +       		
       		requestURI.Length();
    if ( iQop != EAuth )
    	{
    	iSize = iSize +
    			KSIPSecReqDataSeparator().Length() +
    			KSIPSecDigestHashHexSize;
    	}
    }
    
// ----------------------------------------------------------------------------
// CSIPSecRequestData::Next
// ----------------------------------------------------------------------------
//		
RSIPSecRequestDataField CSIPSecRequestData::NextL()
    {    
    __ASSERT_ALWAYS( !EndOfData() , User::Leave( KErrGeneral ) );
    
    iIterator++;
    
    switch( iIterator )
        {
        case KSSIPSecReqDataMethod:
            {
            return RSIPSecRequestDataField ( 
                        iDigestContext.Method(), EFalse );
            }
        case KSSIPSecReqDataURI:
            {
            return RSIPSecRequestDataField( 
                        iDigestContext.DigestURI(), EFalse );
            }
        case KSSIPSecReqDataMessage:
            {
            return RSIPSecRequestDataField( 
                        iDigestContext.Message(), ETrue );            
            }
        default:
            {
        	User::Leave( KErrGeneral );
            }
        }
        
    return RSIPSecRequestDataField();
    }

// ----------------------------------------------------------------------------
// CSIPSecRequestData::EndOfData
// ----------------------------------------------------------------------------
//		
TBool CSIPSecRequestData::EndOfData() const
    {    
    if ( iQop == EAuthInt )
        {
        return iIterator >= KSSIPSecReqDataMessage;
        }
    
    return iIterator >= KSSIPSecReqDataURI;
    }
    
// ----------------------------------------------------------------------------
// CSIPSecRequestData::Separator
// ----------------------------------------------------------------------------
//		
const TDesC8& CSIPSecRequestData::Separator() const
    {
	if ( EndOfData() )
    	{
    	return KNullDesC8;
		}
	
    return KSIPSecReqDataSeparator;
    }



// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CSIPSecDigestVerifyData::NewL
// ----------------------------------------------------------------------------
//		
CSIPSecDigestVerifyData*
CSIPSecDigestVerifyData::NewL( TSIPSecDigestVerifyContext& aContext,
							   CSIPSecRequestData::TQop aQop )
    {    
    CSIPSecDigestVerifyData* self =
    	new ( ELeave ) CSIPSecDigestVerifyData( aContext, aQop );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CSIPSecDigestVerifyData::CSIPSecDigestVerifyData
// ----------------------------------------------------------------------------
//		
CSIPSecDigestVerifyData::CSIPSecDigestVerifyData(
	TSIPSecDigestVerifyContext& aContext,
	CSIPSecRequestData::TQop aQop ) :
    CSIPSecRequestData( aContext, aQop )
    {    
    }
    
// ----------------------------------------------------------------------------
// CSIPSecDigestVerifyData::ConstructL
// ----------------------------------------------------------------------------
//		
void CSIPSecDigestVerifyData::ConstructL()
    {
    TSIPSecDigestVerifyContext& digestCtx =
    	static_cast<TSIPSecDigestVerifyContext&>( iDigestContext );
    
    const TDesC8& method = iDigestContext.Method();
    const TDesC8& requestURI = iDigestContext.DigestURI();
    
    HBufC8* secServerBuf = digestCtx.SecurityServerHeaderL();
    __ASSERT_ALWAYS( secServerBuf, User::Leave( KErrArgument ) );

    CleanupStack::PushL( secServerBuf );
    const TDesC8& secServer = secServerBuf->Des();
    
    if ( iQop == EAuth )
        {        
        iSize = KSIPSecReqDataSeparator().Length() * 2;
        iSize += method.Length() + requestURI.Length() + secServer.Length();
        }
    else 
        {
        iSize = KSIPSecReqDataSeparator().Length() * 3;
        iSize += method.Length() + requestURI.Length() +
        		 KSIPSecDigestHashHexSize + secServer.Length();
        }

    CleanupStack::PopAndDestroy( secServerBuf );        
    }
    
// ----------------------------------------------------------------------------
// CSIPSecDigestVerifyData::NextL
//
// RFC3329 2.2:
// If the qop is "auth" or unspecified, then A2 is:
//   A2 = Method ":" digest-uri-value ":" security-server
//
// If the "qop" value is "auth-int", then A2 is:
//   A2 = Method ":" digest-uri-value ":" H(entity-body) ":" security-server
// ----------------------------------------------------------------------------
//		
RSIPSecRequestDataField CSIPSecDigestVerifyData::NextL()
    {
    __ASSERT_ALWAYS( !EndOfData() , User::Leave( KErrGeneral ) );

    TSIPSecDigestVerifyContext& digestCtx =
    	static_cast<TSIPSecDigestVerifyContext&>( iDigestContext );

    switch( ++iIterator )
        {
        case KSSIPSecReqDataMethod:
            {
            return RSIPSecRequestDataField(
                    iDigestContext.Method(), EFalse );
            }
        case KSSIPSecReqDataURI:
            {
            return RSIPSecRequestDataField(
                     iDigestContext.DigestURI(), EFalse );
            }
        case KSSIPSecReqDataMessage:
            {
            if ( iQop == EAuthInt )
                {
                return RSIPSecRequestDataField(
                            iDigestContext.Message(), ETrue );
                }

            return RSIPSecRequestDataField(
              	digestCtx.SecurityServerHeaderL(), EFalse );
            }
        case KSSIPSecReqDataServer:
            {
            return RSIPSecRequestDataField(
            	digestCtx.SecurityServerHeaderL(), EFalse );
            }
        default:
            {
            User::Leave( KErrGeneral );
            }
        }

    return RSIPSecRequestDataField();    
    }    
    
// ----------------------------------------------------------------------------
// CSIPSecDigestVerifyData::EndOfData
// This function differs from CSIPSecRequestData::EndOfData.
// ----------------------------------------------------------------------------
//		
TBool CSIPSecDigestVerifyData::EndOfData() const
    {    
    if ( iQop == EAuthInt )
        {
        return iIterator >= KSSIPSecReqDataServer;
        }

    return iIterator >= KSSIPSecReqDataMessage;    
    }    
