/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : sipsecrequestdata.h
* Part of       : SIPSec/DigestPlugin
* Version       : SIP/6.0
*
*/




/**
 @internalComponent
*/

#ifndef __SIPSEC_REQUESTDATA_H__
#define __SIPSEC_REQUESTDATA_H__

// INCLUDES
#include <e32base.h>
#include <e32std.h>

// FORWARD DECLARATIONS
class TSIPSecDigestCtxProcess;
class TSIPSecDigestVerifyContext;


// CLASS DECLARATION
/**
 * @brief Class represents data field of A2 
 *
 * @class RSIPSecRequestDataField sipsecrequestdata.h "sipsecrequestdata.h" 
 */
class RSIPSecRequestDataField 
    {
public:
    
    /**
    * Default Constructor
    */
    RSIPSecRequestDataField();

    /**
    * Closes
    */
    void Close();
    
    /**
    * Constructor
    * @param aValue value of field
    * @param aNeedsHashing does it need hashing
    */
    RSIPSecRequestDataField( const TDesC8& aValue, TBool aNeedsHashing );

    /**
    * Constructor
    * @param aValue value of field
    * @param aNeedsHashing does it need hashing
    */
    RSIPSecRequestDataField( TPtrC8 aValue, TBool aNeedsHashing );  

    /**
    * Constructor
    * @param aValue value of field
    * @param aNeedsHashing does it need hashing
    */
    RSIPSecRequestDataField( HBufC8* aValue, TBool aNeedsHashing );

    /**
    * Does field need hashing
    * @return ETrue, if field needs hashing
    */
    TBool NeedsHashing() const;

    /**
    * Value of the field
    * @return Value of the field
    */
    TPtrC8 Value();

private: // Data

    // Value    
    TPtrC8 iValue;
    
    // Does field need hashing    
    TBool iNeedsHashing;

    // Holds pointer to buffer. Owned.
    HBufC8* iBuffer;
    };



/**
 * @brief Class represents A2 of response of credentials or authentication info
 * 
 * @class CSIPSecRequestData sipsecrequestdata.h "sipsecrequestdata.h"
 */
class CSIPSecRequestData : public CBase
    {
public:

    enum TQop
        {
        EUnknown = -1, 	// qop exists, but isn't auth or auth-int
        EAuth,			// auth
        EAuthInt,		// auth-int
        EDoesNotExist	// qop doesn't exist
        };

    /**
    * Constructor
    * @param aContext Digest context
    * @param aQop Qop value
    */
    static CSIPSecRequestData* NewL( TSIPSecDigestCtxProcess& aContext,
    								 CSIPSecRequestData::TQop aQop );

    /**
    * Size of data
    * @return Size of data
    */
    TInt Size() const;

    /**
    * Next field
    * @return Next field
    */
    virtual RSIPSecRequestDataField NextL();
    
    /**
    * Is end of data
    * @return ETrue if end of data
    */
    virtual TBool EndOfData() const;

    /**
    * Separator of the field
    * @return Separator of the field
    */
    virtual const TDesC8& Separator() const;

protected: // New functions

    /**
    * Constructor
    * @param aContext Digest context
    * @param aQop Qop value
    */
    CSIPSecRequestData( TSIPSecDigestCtxProcess& aContext,
    					CSIPSecRequestData::TQop aQop );

private: // New functions, for internal use
    
    /**
    * 2nd phase Constructor
    */
    void ConstructL();
    
protected: // Data

    // Digest context
    TSIPSecDigestCtxProcess& iDigestContext;

    // Field iterator    
    TInt iIterator;

    // Qop    
    TQop iQop;
    
    // Size of data    
    TInt iSize;    
    };



/**
 * @brief Class represents A2 of response of security verify
 * 
 * @class CSIPSecDigestVerifyData sipsecrequestdata.h "sipsecrequestdata.h"
 *
 */
class CSIPSecDigestVerifyData : public CSIPSecRequestData
    {    
public:

    /**
    * Constructor
    * @param aContext Digest context
    * @param aQop Qop value
    */
    static CSIPSecDigestVerifyData* NewL( TSIPSecDigestVerifyContext& aContext,
    		  							  CSIPSecRequestData::TQop aQop );

    /**
    * Next field
    * @return Next field
    */
    RSIPSecRequestDataField NextL();
    
    /**
    * Is end of data
    * @return ETrue if end of data
    */
    TBool EndOfData() const;    

private: // Constructors, for internal use
    
    /**
    * Constructor
    * @param aContext Digest context
    * @param aQop Qop value
    */
    CSIPSecDigestVerifyData( TSIPSecDigestVerifyContext& aContext,
    						 CSIPSecRequestData::TQop aQop );

    /**
    * 2nd phase Constructor
    */
    void ConstructL();
    };

#endif //__SIPSEC_REQUESTDATA_H__
