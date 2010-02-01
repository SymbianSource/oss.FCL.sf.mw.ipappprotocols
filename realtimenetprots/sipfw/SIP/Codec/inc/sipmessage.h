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
* Name        : sipmessage.h
* Part of     : SIP Codec
* Interface   : SIP Codec API
* Version     : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPMESSAGE_H
#define CSIPMESSAGE_H

//  INCLUDES
#include <e32base.h>
#include "sipheaderbase.h"
#include "_sipcodecdefs.h"

// FORWARD DECLARATIONS
class CSIPFromHeader;
class CSIPToHeader;
class CSIPCallIDHeader;
class CSIPCSeqHeader;

// CLASS DECLARATION
/**
* A base class for SIP messages that provides functions 
* for setting and getting headers and SIP version of a SIP message. 
*
* @lib sipcodec.lib
*/
class CSIPMessage : public CBase
    {
    public: // Constructors and destructor

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CSIPMessage();

    public: // New functions

        /**
        * Checks if the actual class of this object is CSIPRequest and 
        * can be safely casted to CSIPRequest
        * @return ETrue if the object is of class CSIPRequest
        */
        IMPORT_C virtual TBool IsRequest() const = 0;

        /**
        * Adds a header to the SIP message. Note that the first header inserted
        * within a group of headers with the same name (for example "Route") is
        * also guaranteed to appear first in the encoded message.
        * 
        * @pre aSIPHeader != 0
        * @pre !(aSIPHeader->MoreThanOneAllowed()&&HasHeader
        *                                            (aSIPHeader->Name())
        * @param aSIPHeader a SIP header to be added, ownership is transferred.
        */
        IMPORT_C void AddHeaderL(CSIPHeaderBase* aSIPHeader);

        /**
        * Adds a deep copy of a given header to the SIP message. 
        * Note that the first header inserted
        * within a group of headers with the same name (for example "Route") is
        * also guaranteed to appear first in the encoded message.
        * 
        * @pre !(aSIPHeader.MoreThanOneAllowed()&&HasHeader(aSIPHeader->Name())
        * @param aSIPHeader a SIP header to be added
        */
        IMPORT_C void AddHeaderL(const CSIPHeaderBase& aSIPHeader);

        /**
        * Replaces a header to the SIP message based on pointer comparison.
        * The updated header has to have the same type.
        * This means that a Contact-header cannot replace a To-header.
        * Note that the new header will be placed last in the list of headers.
        *
        * @pre aOldSIPHeader != 0
        * @pre aNewSIPHeader != 0
        * @pre aOldSIPHeader->Name() == aNewSIPHeader->Name()
        * @param aOldSIPHeader the header to be replaced.
        *       Deleted if the function returns without leaving.
        * @param aNewSIPHeader the header that replaces the old header.
        *        The ownership is transferred.
        */
        IMPORT_C void ReplaceHeaderL(
            CSIPHeaderBase* aOldSIPHeader,
            CSIPHeaderBase* aNewSIPHeader);

        /**
        * Replaces all the headers with a common name.
        * All the headers must be of same type.
        *
        * @pre aNewHeaders.Count() > 0
        * @param aNewHeaders the headers that will replace the old headers.
        *        The ownership of the headers is transferred and 
        *        the array is resetted on return.
        */
        IMPORT_C void ReplaceHeadersL(
            RPointerArray<CSIPHeaderBase>& aNewHeaders);

        /**
        * Checks if a one or more headers with aName are present
        * @param aName the full or compact name of the header
        * @return ETrue if present, otherwise EFalse
        */
        IMPORT_C TBool HasHeader(RStringF aName) const;

        /**
        * Get a count of headers with a given name in this SIP message.
        * @param aName the full or compact name of the header
        * @return number of headers with the given name.
        */
        IMPORT_C TInt HeaderCount(RStringF aName);

        /**
        * Gets an iterator to a list of headers with aName 
        * @pre HasHeader(aName) == ETrue
        * @param aName the full or compact name of the headers to get
        * @return an iterator to a list of headers with aName.
        *         The headers can be modified through the iterator, but are 
        *         supposed to be removed using RemoveHeader.
        */
        IMPORT_C TSglQueIter<CSIPHeaderBase> Headers(RStringF aName);

        /**
        * Gets a header with aName and an index in list of header with aName
        * Note that using HeaderCount(...) and this function to loop through
        * the headers with a given name is MUCH less efficient than using
        * iterators through Headers(...).
        *
        * @pre aIndex < HeaderCount(aName) 
        * @param aName the full or compact name of the header to get
        * @param aIndex the index in the header list
        * @return 0 if the header with the given name or index was not found,
        *          otherwise the header. The ownership is NOT transferred.
        */
        IMPORT_C CSIPHeaderBase* Header(RStringF aName, TInt aIndex);

        /**
        * Removes a header from the SIP message based on pointer comparison
        * @param aSIPHeader the header to be removed, ownership is NOT 
        *        transferred, the caller is supposed to do the deletion
        * @return KErrNone if succesfully removed, KErrNotFound if the
        *         header was not found.
        */
        IMPORT_C TInt RemoveHeader(const CSIPHeaderBase* aSIPHeader);

        /**
        * Removes and deletes all the headers with name aName
        * @pre HasHeader(aName)==ETrue
        * @param aName the full or compact name of the headers to delete. 
        * @return KErrNone if succesfully removed and deleted,
        *         KErrNotFound if headers with aName header were not found.
        */
        IMPORT_C TInt DeleteHeaders(RStringF aName);

        /**
        * Creates copies of the pointers to all the headers in the SIP message.
        * Note that the caller must not delete any of the headers returned.
        * @return an array of all the headers in the SIP message.
        */
        IMPORT_C const RPointerArray<CSIPHeaderBase>& AllHeadersL();

        /**
        * Gets the From-header in the message.
        * @return The From-header or zero if the header is not present.
        *         The ownership is NOT transferred.
        */
        IMPORT_C CSIPFromHeader* From();

        /**
        * Gets the To-header in the message.
        * @return The To-header or zero if the header is not present.
        *          The ownership is NOT transferred.
        */
        IMPORT_C CSIPToHeader* To();

        /**
        * Gets the Call-ID-header in the message.
        * @return The Call-ID-header or zero if the header is not present.
        *         The ownership is NOT transferred.
        */
        IMPORT_C CSIPCallIDHeader* CallID();

        /**
        * Gets the CSeq-header in the message.
        * @return The CSeq-header or zero if the header is not present.
        *         The ownership is NOT transferred.
        */
        IMPORT_C CSIPCSeqHeader* CSeq();

        /**
        * Gets the SIP version
        * @return the SIP version as text
        */
        IMPORT_C RStringF SIPVersion() const;

        /**
        * Set the content
        * @param aContent, the ownership is transferred
        */
        IMPORT_C void SetContent(HBufC8* aContent);

        /**
        * Gets the content.
        * @return the content, the ownership is NOT transferred.
        */
        IMPORT_C const TDesC8& Content() const;

        /**
        * Gets the ownership to the content
        * @return the content, the ownership is transferred.
        */
        IMPORT_C HBufC8* TakeContentOwnershipL();

        /**
        * Checks if "Content-Length"-header was present 
        * when the message was decoded.
        * @return ETrue if set, otherwise EFalse
        */
        IMPORT_C TBool HasAnnouncedContentLength() const;

        /**
        * Gets the value of "Content-Length"-header 
        * that was present when the message was decoded.
        * This might be different than the actual length of the message content.
        * @pre HasAnnouncedContentLength() == ETrue
        * @return the value of "Content-Length"-header
        */
        IMPORT_C TUint AnnouncedContentLength() const;

        /**
        * Using this function the user can get the length of the
        * encoded header part without actually encoding the whole header part.
        * Note that this function only saves memory, but not necessarily 
        * processing time compared to the actual encoding.
        * @return the length of the header part when encoded
        */
        IMPORT_C TInt EncodedHeaderPartLengthL();

        /**
        * Encodes the header part of the SIP message object 
        * into its textual representation..
        * @return a textual representation of the object,
        *         the ownership is transferred
        */
        IMPORT_C CBufBase* ToTextHeaderPartL();

        /**
        * Encodes the header part of the SIP message object 
        * into its textual representation and pushes it to the CleanupStack.
        * @return a textual representation of the object,
        *         the ownership is transferred
        */
        IMPORT_C CBufBase* ToTextHeaderPartLC();

        /**
        * Encodes the SIP message object into its textual representation.
        * @return a textual representation of the object,
        *         the ownership is transferred
        */
        IMPORT_C CBufBase* ToTextL();

        /**
        * Encodes the SIP message object into its textual representation
        * and pushes it to the CleanupStack.
        * @return a textual representation of the object,
        *         the ownership is transferred
        */
        IMPORT_C CBufBase* ToTextLC();

        /**
        * Externalizes all the headers that support serialization
        * to a write stream.
        * @param aWriteStream a stream where the objects will be externalized
        */
        IMPORT_C void ExternalizeHeadersL(RWriteStream& aWriteStream);

        /**
        * Adds headers to the CSIPMessage from a read stream. 
        * @param aReadStream a stream containing the values of the
        *        externalized SIP header objects 
        */
        IMPORT_C void InternalizeHeadersL(RReadStream& aReadStream);

    public: // New functions, for internal use

        void SetSIPVersionL(const TDesC8& aSIPVersion);

    protected: // Constructors
    
        void ConstructL();
        CSIPMessage();
        
    protected: // New functions
        
        virtual HBufC8* ToTextFirstLineLC() const = 0; 

    private: // New functions

        RStringF ReadFromStreamL (RReadStream& aReadStream) const;
        
        TBool IsContentLengthHeader(const CSIPHeaderBase& aHeader) const;
        
        TInt FindHeaderListIndex(RStringF aName) const;
        
        TInt FindHeaderListIndex(const CSIPHeaderBase& aHeader) const;
    
        void InsertL(TSglQue<CSIPHeaderBase>& aHeaderList);
        
        static TInt HeaderOrder(
            const TSglQue<CSIPHeaderBase>& aFirstList, 
            const TSglQue<CSIPHeaderBase>& aSecondList);
            
        void GetEncodedFirstLineSizeL(TInt& aSize) const;
    
        void GetEncodedContentLengthHeaderSizeL(TInt& aSize) const;
    
        HBufC8* ToTextContentLengthLC() const;

    private: // Data

        RStringF iSIPVersion;
        RArray<TSglQue<CSIPHeaderBase> > iSIPHeaderListArray;
        HBufC8* iContent;
        TInt iAnnouncedContentLength;
        TBool iAnnouncedContentLengthSet;
        RPointerArray<CSIPHeaderBase> iTmpAllHeaders;

    private: // For testing purposes

        UNIT_TEST(CSIPMessageTest)
    };


#endif // CSIPMESSAGE_H

// End of File
