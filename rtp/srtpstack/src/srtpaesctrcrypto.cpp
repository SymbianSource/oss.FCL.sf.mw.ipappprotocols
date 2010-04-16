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




#include "srtpaesctrcrypto.h"
#include <e32uid.h>
#include <symmetric.h>
#include "srtpdef.h"
#include "srtputils.h"
// ---------------------------------------------------------------------------
// CSrtpAESCTRCrypto::NewL()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CSrtpAESCTRCrypto* CSrtpAESCTRCrypto::NewL()
	{
	CSrtpAESCTRCrypto* self=new (ELeave) CSrtpAESCTRCrypto( );
	return self;
	}

// ---------------------------------------------------------------------------
// CSrtpAESCTRCrypto::~CSrtpAESCTRCrypto()
// 
// ---------------------------------------------------------------------------
//
CSrtpAESCTRCrypto::~CSrtpAESCTRCrypto()
	{
	delete iEncryptor;
	delete iKey;
	}


// ---------------------------------------------------------------------------
// CSrtpAESCTRCrypto::CSrtpAESCTRCrypto
// 
// ---------------------------------------------------------------------------
//
CSrtpAESCTRCrypto::CSrtpAESCTRCrypto( ) 
	{
	}


// ---------------------------------------------------------------------------
// CSrtpAESCTRCrypto::KeystreamL
// 
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC8* CSrtpAESCTRCrypto::KeystreamL(TUint aBitLength, 
                                                const TDesC8& aKey, 
                                                const TDesC8& aIV )
	{
    TBuf8<16> iv;           //temp for IV    
    TBuf8<16> data;         //for a 128-bit piece of a keystream    
    CAESEncryptor*  encryptor = NULL;
    TUint buffLength;       //length of the result (as 8 bit descriptor)
    TUint count;          // how many 128-bit AES calls needed 
    
    buffLength = aBitLength/8;  
    if (aBitLength%8)
        {
        buffLength++;            
        }

    count = aBitLength/128;    
    if (aBitLength%128)
        {
        count++;            
        }
    
	HBufC8* outputBuff = HBufC8::NewLC(16*count);
	TPtr8 ptrOutputBuff = outputBuff->Des();	
    
    SRTP_DEBUG_TUINT_VALUE( "KeyStreamL, Check the length of aKey==16 and the length is", (aKey.Length()) );
    
    if(aKey.Length() != 16)
    	{
	    CleanupStack::Pop(outputBuff);
        delete outputBuff;    
        outputBuff=NULL;    
    	User::Leave(KErrArgument);
    	}
    
    SRTP_DEBUG_TUINT_VALUE( "KeyStreamL, Check the length of aIV==16 and the length is", (TUint)aIV.Length() );
    
    //if IV's length is not valid
    if(aIV.Length() != 16)
    	{
	    CleanupStack::Pop(outputBuff);
        delete outputBuff;    
        outputBuff= NULL; 
    	User::Leave(KErrArgument);
    	}    
            
    iv.Copy(aIV);
    
    SRTP_DEBUG_TUINT_VALUE( "KeyStreamL, Check aBitLength/128>KSRTPMaxKeyStreamBlocks and the length is", 
                (TUint)aBitLength/128  );
    if(aBitLength/128 > KSRTPMaxKeyStreamBlocks)
    	{
	    CleanupStack::Pop(outputBuff);
        delete outputBuff;    
        outputBuff=NULL;   
    	User::Leave(KErrArgument);
    	}

    encryptor = CAESEncryptor::NewLC(aKey);
                   
    for(int x = 0; x < count; x++)
    	{    	    	   	
    	data.Copy(iv);
    
    	encryptor->Transform(data);

    	IncreaseIV(iv);
    	    			
		ptrOutputBuff.Append(data);		    	
    	}
    
    //set the size if it was changed
    TInt size= ptrOutputBuff.Size();
    ptrOutputBuff.SetLength(buffLength);
    	
	CleanupStack::PopAndDestroy(encryptor);
    CleanupStack::Pop(outputBuff); 
       	    
    SRTP_DEBUG_TUINT_VALUE( "CSrtpAESCTRCrypto::KeystreamL ptrOutputBuff.Size()", 
    			(TUint)size );
    
    return outputBuff;	
	}


// ---------------------------------------------------------------------------
// CSrtpAESCTRCrypto::EncryptL
// 
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC8* CSrtpAESCTRCrypto::EncryptL(const TDesC8& aKey, 
                                            const TDesC8& aIV, 
                                            const TDesC8& aSrc)
	{
		
		HBufC8* outputBuff = HBufC8::NewLC(aSrc.Length());
		TPtr8 ptrOutputBuff = outputBuff->Des();
			
	    TBuf8<16> iv; //temp for IV
	    TBuf8<16> data; //for a 128-bit piece of a keystream
	    TBuf8<16> msg; //for a 128-bit piece of a message
	    TInt count=0; // how many full 128-bit pieces can be made of a source
	    
	    SRTP_DEBUG_TINT_VALUE( "EncryptL, Check aBitLengh ==16 and the length is", 
	                aKey.Length()  );
	      
	    //if key's length is not valid
	    if(aKey.Length() != 16)
	    	{
		    CleanupStack::Pop(outputBuff);
	        delete outputBuff;    
	        outputBuff= NULL;
	    	User::Leave(KErrArgument);
	    	}
	    SRTP_DEBUG_TINT_VALUE( "EncryptL, Check the length of aIV==16 and the length is", 
	                aIV.Length() );
	               
	    //if IV's length is not valid
	    if(aIV.Length() != 16)
	    	{
		    CleanupStack::Pop(outputBuff);
	        delete outputBuff;    
	        outputBuff= NULL;  
	    	User::Leave(KErrArgument);
	    	}    
		
		SRTP_DEBUG_TINT_VALUE("EncryptL, Check aSrc.Length()>KSRTPMaxTextLength and the length is", 
	                aSrc.Length()  );
	          
	    //check that text length is less than 2^23
	    if(aSrc.Length() > KSRTPMaxTextLength)
	    	{
		    CleanupStack::Pop(outputBuff);
	        delete outputBuff;    
	        outputBuff= NULL;    
	    	User::Leave(KErrArgument);
	    	}
	            
	    iv.Copy(aIV);
	        
	    count = aSrc.Length()/16;
		
		SRTP_DEBUG_TINT_VALUE("EncryptL, count>KSRTPMaxKeyStreamBlocks and the length is", 
	                count  );
	         

	    if(count > KSRTPMaxKeyStreamBlocks)
	    	{
		    CleanupStack::Pop(outputBuff);
	        delete outputBuff;    
	        outputBuff= NULL;  
	    	User::Leave(KErrArgument);
	    	}

	    if ( !iEncryptor || !iKey || (*iKey != aKey) )
	        {
	        CreateEncryptorL(aKey);
            }
	                   
	    for(int x = 0; x < count; x++)
	    	{
	    	msg.Copy(aSrc.Mid(x*16, 16));
	    	
	    	data.Copy(iv);
	    	   	
	    	iEncryptor->Transform(data);

	    	IncreaseIV(iv);
	    	    	
		    // XOR 128-bits of message with encrypted IV
		    for(int i = 0; i < 16; i++)
				{
				msg[i] ^= data[i];
				}
			
			ptrOutputBuff.Append(msg);
			    	
	    	}
	    
	    //if source's length isn't an exact multiple of 128-bits
	    if(aSrc.Length()%16)
	    	{
	    	TInt bytesleft;
	    	bytesleft = aSrc.Length() - count*16;
	    	
	    	msg.Copy(aSrc.Mid(count*16, bytesleft));
	    	data.Copy(iv);
	    	iEncryptor->Transform(data);
	    		
		    // XOR last piece of message with encrypted IV
		    for(int i = 0; i < bytesleft; i++)
			{
					msg[i] ^= data[i];
			}
			
			ptrOutputBuff.Append(msg);
			    		
	    	}
		
	    CleanupStack::Pop(outputBuff);    	    
	    
	    return outputBuff;
	}

// ---------------------------------------------------------------------------
// CSrtpAESCTRCrypto::IncreaseIV
// 
// ---------------------------------------------------------------------------
//
void CSrtpAESCTRCrypto::IncreaseIV(TDes8& iv)
	{
	
	if(iv[15] != 0xFF)
		{
		iv[15] += 1;	
		}
	else if(iv[14] != 0xFF)
		{
		iv[15] = 0;
		iv[14] += 1;
		}
	else if(iv[13] != 0xFF)
		{
		iv[15] = 0;
		iv[14] = 0;
		iv[13] += 1;
		}	
	else if (iv[12] != 0xFF)
		{
		iv[15] = 0;
		iv[14] = 0;
		iv[13] = 0;
		iv[12] += 1;
		}		
	else
		{
		iv[15] = 0;
		iv[14] = 0;
		iv[13] = 0;
		iv[12] = 0;
		}

	} 

// ---------------------------------------------------------------------------
// CSrtpAESCTRCrypto::CreateEncryptorL
// ---------------------------------------------------------------------------
//
void CSrtpAESCTRCrypto::CreateEncryptorL(const TDesC8& aKey)
    {
    delete iEncryptor;
    iEncryptor = 0;
    delete iKey;
    iKey = 0;
    iKey = aKey.AllocL();
    iEncryptor = CAESEncryptor::NewL(*iKey);
    }
