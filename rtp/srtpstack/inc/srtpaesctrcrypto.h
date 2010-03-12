/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Class for AES (in Counter Mode) encryption operations
*
*/




#ifndef __SRTPAESCTRCRYPTO_H__
#define __SRTPAESCTRCRYPTO_H__

#include <e32base.h>

class CAESEncryptor;

class CSrtpAESCTRCrypto : public CBase
  	{
public:
    /**
    * Constructor.
    * @leave KErrNone if success, system-wide error code otherwise
    */
	IMPORT_C static CSrtpAESCTRCrypto* NewL();
		
	/**
    * AES CM cipher, produces generic keystream
    * @param aBitLength  key length.
    * @param aKey  Input key.
    * @param aIV  Integer Value (IV).
	* Returns HBufC8* generated keystream
	*/
	IMPORT_C HBufC8* KeystreamL(TUint aBitLength, const TDesC8& aKey, const TDesC8& aIV );
	
	/**
    * AES CM cipher, produces key stream and performs an XOR operation for the text
    * @param aKey  Input key.
    * @param aIV  Integer Value (IV).
    * @param aSrc source data which will be encrypted.
	* Returns HBufC8* decrypted/encrypted source
	*/
	IMPORT_C HBufC8* EncryptL(const TDesC8& aKey, const TDesC8& aIV, const TDesC8& aSrc);
		
	/**
    * Destructor
	*/
	~CSrtpAESCTRCrypto();
private:
    /**
    * Default constructor.
    */
	CSrtpAESCTRCrypto( );
		
	/**
    * Increases IV's value by one. 
    * Sets IV's last 32 bits to 0 if their value is max
    * @param aIV  Integer Value (IV).
	*/
	void IncreaseIV(TDes8& iv);
	
	void CreateEncryptorL(const TDesC8& aKey);
	
private:
    
    HBufC8* iKey;
    CAESEncryptor* iEncryptor;
	
	#ifdef EUNIT_TESTING
        friend class UT_CSrtpAESCTRCrypto;
    #endif     
	};

#endif // __SrtpAESCTRCrypto_H__
