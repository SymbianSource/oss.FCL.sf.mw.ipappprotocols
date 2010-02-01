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
* Name          : SipSecUtils.h
* Part of       : SIPSec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef SIPSECUTILS_H
#define SIPSECUTILS_H

// INCLUDES
#include "sipstrconsts.h"

// FORWARD DECLARATIONS
class TInetAddr;
class CSIPMessage;
class CSIPRequest;
class CSIPHeaderBase;
class CSIPSecurityHeaderBase;
class CSIPParameterHeaderBase;
class RStringF;

// MACROS
#define _STRINGF(sc) SIPStrings::Pool().StringF( sc, SIPStrings::Table() )


// CLASS DECLARATION
/**
 * @brief SipSec utility functions
 */
class SipSecUtils
	{
public: // New Functions
	static TBool Match( SipStrConsts::TStrings aStringConst,
				 	    const RStringF& aString );

	static TSglQueIter<CSIPHeaderBase>
		SecHeaders( CSIPMessage& aMessage,
					SipStrConsts::TStrings aStringConst,
					TBool& aFound );
							 					   
	static CSIPHeaderBase*
		SingleHeader( CSIPMessage& aMessage,
					  SipStrConsts::TStrings aHeaderStrConst );
							 					   
	static CSIPSecurityHeaderBase*
		HeaderWithMechanism( CSIPMessage& aMessage,
							 SipStrConsts::TStrings aHeaderStrConst,
							 const TDesC8& aMechanism );
							
	static RPointerArray< CSIPSecurityHeaderBase >* HeadersWithMechanismLC(
                    CSIPMessage& aMessage,
                	SipStrConsts::TStrings aHeaderStrConst,
	                const TDesC8& aMechanism );
	                
	static void ResetAndDeleteArray( TAny* aHeaders );
														
    static CSIPSecurityHeaderBase*
    	HeaderWithHighestVaL( RPointerArray<CSIPSecurityHeaderBase>& aHeaders,
							  const TDesC8& aMechanism,
						      const TDesC8& aParameter );
							 
	static TBool HasHeaderWithValue( CSIPMessage& aMessage,
									 SipStrConsts::TStrings aHeader,
									 SipStrConsts::TStrings aValue );
					 
	static CSIPSecurityHeaderBase*
		HeaderWithParamVaL( RPointerArray<CSIPSecurityHeaderBase>& aHeaders,
							const TDesC8& aMechanism,
							const TDesC8& aParameter,
							const TDesC8& aValue );
									 
	static void AddSupportedHeaderL( CSIPMessage& aMessage,
									 SipStrConsts::TStrings aValue );
									 
	static void AddRequireHeadersL( CSIPMessage& aMessage,
		    						SipStrConsts::TStrings aValue );
		    						
	static TUint RandomNumber( TInt64& aSeed,
							   TUint aMinLimit,
							   TUint aMaxLimit );
	
	static TUint ValueL( const CSIPParameterHeaderBase& aSecHeader, 
						 const TDesC8& aParameter );
						 
	static void ValueL( const CSIPParameterHeaderBase& aSecHeader, 
						const TDesC8& aParameter,
						TReal64& aValue );
						
	static RStringF ValueFL( const CSIPParameterHeaderBase& aHeader, 
						 	 const TDesC8& aParameter );
						
	static void SetParamL( CSIPParameterHeaderBase& aSecHeader,
						   const TDesC8& aParameter,
						   const TDesC8& aValue );
						   
	static void SetParamL( CSIPParameterHeaderBase& aSecHeader,
						   const TDesC8& aParameter,
						   TUint aValue );
						   
	static TBool HasParamL( CSIPParameterHeaderBase& aSecHeader,
						    const TDesC8& aParameter );
						    
    static CBufBase* EncodeHeadersL( TSglQueIter<CSIPHeaderBase>& aHeaders );    
	};

#endif // SIPSECUTILS_H

// End of File
