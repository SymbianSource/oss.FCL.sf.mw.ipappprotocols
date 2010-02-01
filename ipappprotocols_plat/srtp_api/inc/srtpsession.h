/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    SRTP session class
*
*/




#ifndef __SRTP_SESSION_H__
#define __SRTP_SESSION_H__

// INCLUDES
#include <in_sock.h>
// FORWARD DECLARATIONS
class CSRTPStream;
class CSRTPCryptoContext;
class MSRTPReKeyingObserver;

/**
* Contains an SRTP session
*/
class CSRTPSession : public CBase
    {
    public: // construction
        /**
        * Two-phased constructor.
        * Use this function if each stream will have its own cryptographic
        * context.
        * @param aDestination  The destination address, including the port.
        * @leave KErrNone if success, system-wide error code otherwise
        */
        IMPORT_C static CSRTPSession* NewL(const TInetAddr& aDestination );

        /**
        * Two-phased constructor.
        * Use this function if all streams will use the same cryptographic
        * context.
        * @param aDestination  The destination address, including the port
        * @param aCon  The default cryptographic context for the session,
        *              ownership is transferred.
        * @param aObs  The callback object
        * @leave KErrNone if success, system-wide error code otherwise
        */
        IMPORT_C static CSRTPSession* NewL( const TInetAddr& aDestination,
                                     CSRTPCryptoContext* aCon,
                                     MSRTPReKeyingObserver& aObs );

        /**
        * Two-phased constructor. Creates a new CSRTPSession instance 
        * and pushes it to CleanupStack.
        * Use this function if each stream will have its own cryptographic
        * context.
        * @param aDestination  The destination address, including the port.
        * @leave KErrNone if success, system-wide error code otherwise
        */
        IMPORT_C static CSRTPSession* NewLC(const TInetAddr& aDestination );

        /**
        * Two-phased constructor. Creates a new CSRTPSession instance 
        * and pushes it to CleanupStack.
        * Use this function if all streams will use the same cryptographic
        * context.
        * @param aDestination  The destination address, including the port
        * @param aCon  The default cryptographic context for the session,
        *              ownership is transferred.
        * @param aObs  The callback object
        * @leave KErrNone if success, system-wide error code otherwise
        */
        IMPORT_C static CSRTPSession* NewLC(const TInetAddr& aDestination,
                                     CSRTPCryptoContext* aCon,
                                     MSRTPReKeyingObserver& aObs);
        /**
        * Destructor, remove and delete all the stream in the session
        */
        ~CSRTPSession();

        /**
        * Retrieve CSSRTPStream object associated with given SSRC.
        * Note that if there are any duplicates the first one is returned.
        * Otherwise, Multiple SRTP streams with identical SSRCs and with same 
        * stream type (In/Out stream) are not supported.
        * @param aSSRC - [input] SSRC of interest
        * @param aInInStream - True if stream type is CStreamIn, False otherwise
        * @return CSSRTPStream object if successful; Leave if not found.
        */
	    IMPORT_C CSRTPStream& StreamL(TUint aSSRC, TBool aIsInStream);

        /**
        * Session should find the match stream and protect stream
        * Note that if there are no matching stream, the function leave
        * @param aSSRC - [input] SSRC of interest
        * @param aPacket - [input] packet to be protected, the length of descriptor
        *  should be the same with the actual data, otherwise encoding will not be 
        *  correct.        
        * @return HBufC8 protected packet
        */
		IMPORT_C HBufC8* ProtectRTPL(TUint aSSRC, const TDesC8& aPacket);
		
		
        /**
        * Session should find the match stream and protect packet
        * Note that if there are no matching stream, the function leave
        * @param aSSRC - [input] SSRC of interest
        * @param aPacket - [input] packet to be protected, the length of descriptor
        *  should be the same with the actual data.              
        * @return HBufC8 protected packet
        */
		IMPORT_C HBufC8* ProtectRTCPL(TUint aSSRC, const TDesC8& aPacket);
		
				
        /**
        * SRTPSession find the match stream and unprotect stream
        * Note that if there are no matching stream, the function leave
        * In late binding cases, session will try to find the stream 
        * which SSRC is equal to zero and unprotect the packet. 
        * If the autenticate for the packet is successful, SSRC will be set to this stream.
        * @param aSSRC - [input] SSRC of interest
        * @param aPacket - [input] packet to be Unprotected      
        * @return HBufC8 Unprotected packet
        */		
		IMPORT_C HBufC8* UnprotectRTPL(TUint aSSRC, const TDesC8& aPacket);
		
       /**
        * SRTPSession find the match stream and unprotect packet
        * Note that if there are no matching stream, the function leaves
        * SRTP must have been recive earlier otherwise the function will leave
        * with KErrNotFound--no SSRC matching stream found
        * Note that the streamIn must have to be created to receive RTCP report if 
        * there is only one streamOut.
        * @param aSSRC - [input] SSRC of interest
        * @param aPacket - [input] packet to be Unprotected      
        * @return HBufC8 Unprotected packet
        */		
		IMPORT_C HBufC8* UnprotectRTCPL(TUint aSSRC, const TDesC8& aPacket);		

       /**
        * Return the destination address set in NewL
        * @return aDestination  The destination address, including the port.
        */		
		IMPORT_C const TInetAddr& RemoteAddr() ;
        
        /**
        * Set/Update crypto context sharing between streams 
        * Crypto context ownershíp is transfered.
        * Note that to update crypto context, the states and the number of packets received
        * will be reset. ROC will not be re-intialized. This can be used 
        * only when keys lifetime has not expired.
        * RFC 4568 6.5, 6.4
        * @param CSRTPCryptoContext
        * @leave KErrNone if success, system-wide error code otherwise
        */
        IMPORT_C void SetCryptoContextL(CSRTPCryptoContext* aCon);
        
        /**
        * Get cryptographic context associated for this session
        * @return CSRTPCryptoContext
        */
        CSRTPCryptoContext& GetCryptoContext();
        
        /**
        * Add given stream to the list of CSRTPStream objects
        * @param aStream  The stream to be added to list
        * @return void
        */
        void AddStreamToList(CSRTPStream *aStream);        

        /**
        * Remove given stream from the list of CSRTPStream objects
        * @param aStream  The stream to be removed
        * @return void
        */
        void RemoveStreamFromList(CSRTPStream *aStream);   
 
         /**
        * Call for Rekey 
        * @param aStream the stream which needs to be rekey
        * @param aIsStrmCrypto True if stream has it's own crypto, false otherwise
        * @return void
        */
        void ReKeyNeeded(const CSRTPStream& aStream, TBool aIsStrmCrypto);        
                       
        
    private: //methods

        CSRTPSession (const TInetAddr& aDestination,
                      MSRTPReKeyingObserver& aObs);
                      
        CSRTPSession(const TInetAddr& aDestination);
                                     
        void ConstructL ( CSRTPCryptoContext* aCon );
        
        TBool FindStream(TUint aSSRC, TBool aIsInStream);
        
        HBufC8* FindLateBindingStreamAndUnprotectRTPL(TUint aSSRC, 
												const TDesC8& aPacket);
		HBufC8* FindLateBindingStreamAndUnprotectRTCPL(TUint aSSRC, 
												const TDesC8& aPacket);	
																					
        void RemoveAllStream( );        
    
    private: // from Array
        /**
        * iStreamLink, Link to list.
        */
        TSglQueLink iStreamLink;

        /**
        * list of CSRTPStream objects
        */
        TSglQue<CSRTPStream> iStreamList;

        /**
        * CSRTPStream iterator.
        */
        TSglQueIter<CSRTPStream> iStreamIter;
        
    private: //data
        /*
        destination address
        */
        const TInetAddr&          iDestination;  
        /*
        crypto context, owned
        */
        CSRTPCryptoContext* iContext;
        /*
        observer (application)
        */        
        MSRTPReKeyingObserver* iObserver;
        
        TUint8 iStreamCount;
        
    protected:
        /*
        * Session is created with cryptographic context
        */
        TBool   iSesssionCrypto; 
        /*
        * True if Rekey is called, False otherwise
        */
        TBool 	iRekey;  
        
    private:  
    	friend class CSRTPStream;
    	#ifdef EUNIT_TESTING
        friend class UT_CSRTPSession;
    #endif     
        
    };

#endif // __SRTP_SESSION_H__
