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
* Name          : Sipimsprofilesimrecord.h
* Part of       : ProfileAgent
* Version       : SIP/4.1
*
*/




/**
 @internalComponent
*/

#ifndef CSIPPROFILESIMRECORD_H
#define CSIPPROFILESIMRECORD_H

#include <e32base.h>

/**
 * @brief Class represents record stored in SIM 
 * 
 * @class CSIPProfileSIMRecord CSIPProfileSIMRecord.h "CSIPProfileSIMRecord.h"
 *
 */
class CSIPProfileSIMRecord : public CBase
	{	
public:  //enums
	
	enum TFailureType
	    {
	    ENoFailure = 0,
	    ESQN,
	    EMAC,
	    EPrivateIdentity,
	    };
	    
public:  // Constructors and destructor

    /**
    * Constructs an object; leaves on failure.
    * @return new instance
    */
    static CSIPProfileSIMRecord* NewL();

    /**
    * Constructs an object and adds the pointer to the cleanup stack;
    * leaves on failure.
    * @return new instance
    */		
    static CSIPProfileSIMRecord* NewLC();

    /**
    * Destructor
    */
    ~CSIPProfileSIMRecord();

public:  // New functions

    /**
    * Sets the type of the record to be ISIM.
    * @param aISIMPresent
    */		
    void SetISIMPresent( TBool aISIMPresent );

    /**
    * Sets 
    * @param aValue privateid
    */		
    void SetPrivateIdL( const TDesC8& aValue );

    /**
    * Sets
    * @param aValue privateid
    */		
    void SetFirstPublicUserNameL( const TDesC8& aValue );

    /**
    * Sets 
    * @param aValue privateid
    */		
    void SetHomeNetworkDomainNameL( const TDesC8& aValue );

	/**
    * Sets 
    * @param aValue privateid
    */		
    void SetSIMPrivateIdL( const TDesC8& aValue );

    /**
    * Sets
    * @param aValue privateid
    */		
    void SetSIMFirstPublicUserNameL( const TDesC8& aValue );

    /**
    * Sets 
    * @param aValue privateid
    */		
    void SetSIMHomeNetworkDomainNameL( const TDesC8& aValue );

	/**
    * Authorization has failed
    */		
    void UserRecordFailed( TInt aError );

    /**
    * @return ETrue if ISIM application is present 
    * (and the public user identity is not temporary).
    */		
    TBool IsISIMPresent();

    /**
    * Returns failure
    * @return failure
    */		
    TInt Failure() const;
    
    /**
    * returns PrivateIdentity value
    * @return value
    */		
    const TDesC8& PrivateIdentity() const;

    /**
    * returns PublicUserIdentity value
    * @return  value
    */		
    const TDesC8& PublicIdentity() const;
    
    /**
    * returns HomeNetworkDomainName value
    * @return  value
    */		
    const TDesC8& HomeNetworkDomainName() const;  
    
      /**
    * returns PrivateIdentity value
    * @return value
    */		
    const TDesC8& SIMPrivateIdentity() const;

    /**
    * returns PublicUserIdentity value
    * @return  value
    */		
    const TDesC8& SIMPublicIdentity() const;
    
    /**
    * returns HomeNetworkDomainName value
    * @return  value
    */		
    const TDesC8& SIMHomeNetworkDomainName() const;
    
    /**
    * Reset all the member values and set them to KNullDesC8
    */
    void ResetValuesL();  
    
    /**
    * Reset all the ISIM member values and set them to KNullDesC8
    */
    void ResetISIMValuesL();  
    
    /**
    * Function add prefix "sip:" to aValue
    * and put the created new value to cleanupstack
    * return values ownership is transfered
    */
    HBufC8* AddSIPPrefixLC(const TDesC8& aValue);

private: //methods

    /**
    * Constructor
    */		
    CSIPProfileSIMRecord();
    
    /**
    * 2nd phase constructor
    */		
    void ConstructL();
    
    TBool FindSIPSchema( const TDesC8& aValue );
    
    TPtrC8 RemoveSIPSchemaL( const TDesC8& aValue );

private: //data

    TBool iISIMPresent;

    /**
    * failure count
    */		
    TInt iFailureCount;
    
    /**
    * Home Network Domain Name
    */		
    HBufC8* iHNDN;
    HBufC8* iHNDNsim;

    /**
    * Internet Multimedia Public Identity
    */		
    HBufC8* iIMPU;
    HBufC8* iIMPUsim;

    
    /**
    * Internet Multimedia Private Identity
    */		
    HBufC8* iIMPI;
    HBufC8* iIMPIsim;


	TInt 	iFailure;

#ifdef CPPUNIT_TEST
	friend class CSIPProfileUserSIMRecordTest;
#endif
	};

#endif// CSIPPROFILESIMRECORD_H

// End of File
