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
// Name          : CSIPMessage.cpp
// Part of       : SIP Codec
// Version       : SIP/4.0 
//




#include "sipmessage.h"
#include "CSIPContentLengthHeader.h"
#include "sipcodecerr.h"
#include "SIPSyntaxCheck.h"
#include "SIPHeaderLookup.h"
#include "sipfromheader.h"
#include "siptoheader.h"
#include "sipcallidheader.h"
#include "sipcseqheader.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "_sipcodecdefs.h"

_LIT8 (KCRLF, "\r\n");
_LIT8 (KCRLFCRLF, "\r\n\r\n");
_LIT8 (KComma, ",");
const TInt KSIPHeaderOffset = _FOFF(CSIPHeaderBase,iLink);

// -----------------------------------------------------------------------------
// CSIPMessage::CSIPMessage
// -----------------------------------------------------------------------------
//
CSIPMessage::CSIPMessage()
 : iAnnouncedContentLengthSet (EFalse)
	{
	}

// -----------------------------------------------------------------------------
// CSIPMessage::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPMessage::ConstructL()
	{
	SIPHeaderLookup::OpenL();
	iContent = HBufC8::NewL(0);
	iSIPVersion = SIPStrings::StringF(SipStrConsts::EDefaultProtocolVersion);
	}

// -----------------------------------------------------------------------------
// CSIPMessage::~CSIPMessage
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPMessage::~CSIPMessage()
	{
	iSIPVersion.Close();
	for (TInt i=0; i < iSIPHeaderListArray.Count(); i++)
		{
		TSglQueIter<CSIPHeaderBase> iter(iSIPHeaderListArray[i]);
		while (iter)
			{
			CSIPHeaderBase* header = iter++;
			iSIPHeaderListArray[i].Remove (*header);
			delete header;
			}
		}
	iSIPHeaderListArray.Close();
	// NEVER ResetAndDestroy because the array contains copies of the
	// pointers in iSIPHeaderListArray (causes double-deletion).
	iTmpAllHeaders.Close();
	delete iContent;
	SIPHeaderLookup::Close();
	}

// -----------------------------------------------------------------------------
// CSIPMessage::AddHeaderL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPMessage::AddHeaderL (CSIPHeaderBase* aSIPHeader)
	{
	__ASSERT_ALWAYS (aSIPHeader != 0, User::Leave(KErrArgument));

	__ASSERT_ALWAYS (!(aSIPHeader->IsExtensionHeader() &&
	                   SIPHeaderLookup::IsSupported(aSIPHeader->Name())),
					   User::Leave(KErrSipCodecNotAllowed));

	// Never add the Content-Length header. Instead store its value.
	if (IsContentLengthHeader(*aSIPHeader))
		{
		if (iAnnouncedContentLengthSet) 
			{
			User::Leave(KErrAlreadyExists);
			}
		iAnnouncedContentLengthSet = ETrue;
		iAnnouncedContentLength = 
			(static_cast<CSIPContentLengthHeader*>(aSIPHeader))->Value();
		delete aSIPHeader;
		return;
		}
	TInt index = FindHeaderListIndex (*aSIPHeader);
	if (index == KErrNotFound) // Insert as the head of a new header list.
		{
		TSglQue<CSIPHeaderBase> headerList(KSIPHeaderOffset);
		headerList.AddLast (*aSIPHeader);
		InsertL (headerList);
		}
	else // Try to add to existing header list
		{
		if (!aSIPHeader->MoreThanOneAllowed()) 
            {
            User::Leave(KErrAlreadyExists);
            }
		iSIPHeaderListArray[index].AddLast(*aSIPHeader);
		}
	}

// -----------------------------------------------------------------------------
// CSIPMessage::AddHeaderL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPMessage::AddHeaderL (const CSIPHeaderBase& aSIPHeader)
    {
    CSIPHeaderBase* header = aSIPHeader.CloneL();
    CleanupStack::PushL(header);
    AddHeaderL(header);
    CleanupStack::Pop(header);
    }

// -----------------------------------------------------------------------------
// CSIPMessage::ReplaceHeaderL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPMessage::ReplaceHeaderL (CSIPHeaderBase* aOldSIPHeader,
                                           CSIPHeaderBase* aNewSIPHeader)
    {
    __ASSERT_ALWAYS (aOldSIPHeader != 0, User::Leave(KErrArgument));
    __ASSERT_ALWAYS (aNewSIPHeader != 0, User::Leave(KErrArgument));
    
    RStringF oldHeaderName = aOldSIPHeader->Name();
    __ASSERT_ALWAYS (oldHeaderName == aNewSIPHeader->Name(),
                     User::Leave(KErrArgument));

    TInt headerListIndex = FindHeaderListIndex(*aOldSIPHeader);
    __ASSERT_ALWAYS (headerListIndex >= 0, User::Leave(KErrNotFound));

    TBool found = EFalse;
	TSglQueIter<CSIPHeaderBase> iter(iSIPHeaderListArray[headerListIndex]);
	while (iter && !found)
		{
		CSIPHeaderBase* header = iter++;
		if (header == aOldSIPHeader)
			{
			iSIPHeaderListArray[headerListIndex].Remove(*header);
            delete aOldSIPHeader;
            iSIPHeaderListArray[headerListIndex].AddLast(*aNewSIPHeader);
            found = ETrue;
			}
		}
    __ASSERT_ALWAYS (found, User::Leave(KErrNotFound));
    }

// -----------------------------------------------------------------------------
// CSIPMessage::ReplaceHeadersL
// -----------------------------------------------------------------------------
//
EXPORT_C void 
CSIPMessage::ReplaceHeadersL (RPointerArray<CSIPHeaderBase>& aNewHeaders)
    {
    __ASSERT_ALWAYS (aNewHeaders.Count() > 0, User::Leave(KErrArgument));
    __ASSERT_ALWAYS (aNewHeaders[0] != 0, User::Leave(KErrArgument));
    
    RStringF name = aNewHeaders[0]->Name();
    for (TInt i=1; i < aNewHeaders.Count(); i++)
        {
        __ASSERT_ALWAYS (aNewHeaders[i] != 0, User::Leave(KErrArgument));
        __ASSERT_ALWAYS (name == aNewHeaders[i]->Name(),
                         User::Leave(KErrArgument));
        }

    TInt headerListIndex = FindHeaderListIndex (name);
    __ASSERT_ALWAYS (headerListIndex >= 0, User::Leave(KErrArgument));

    // Remove old headers
    TSglQueIter<CSIPHeaderBase> iter(iSIPHeaderListArray[headerListIndex]);
	while (iter)
		{
		CSIPHeaderBase* header = iter++;
		delete header;
		}
    iSIPHeaderListArray[headerListIndex].Reset();

    // Add new headers
    for (TInt j=0; j < aNewHeaders.Count(); j++)
        {
        iSIPHeaderListArray[headerListIndex].AddLast(*aNewHeaders[j]);
        }
    aNewHeaders.Reset();
    }

// -----------------------------------------------------------------------------
// CSIPMessage::HasHeader
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPMessage::HasHeader (RStringF aName) const
	{
	return (FindHeaderListIndex(aName) >= 0);
	}

// -----------------------------------------------------------------------------
// CSIPMessage::HeaderCount
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPMessage::HeaderCount (RStringF aName)
	{
	TInt index = FindHeaderListIndex (aName);
	if (index < 0) 
        {
        return 0;
        }
	TInt headerCount=0;
	TSglQueIter<CSIPHeaderBase> iter(iSIPHeaderListArray[index]);
	while (iter++) 
        {
        headerCount++;
        }
	return headerCount;
	}

// -----------------------------------------------------------------------------
// CSIPMessage::Headers
// -----------------------------------------------------------------------------
//
EXPORT_C TSglQueIter<CSIPHeaderBase> CSIPMessage::Headers (RStringF aName)
	{
	TInt index = FindHeaderListIndex (aName);
	if (index == KErrNotFound)
		{
		// Return and iterator initialized to an empty list
		TSglQue<CSIPHeaderBase> emptyList(KSIPHeaderOffset);
		return TSglQueIter<CSIPHeaderBase>(emptyList);
		}
	return TSglQueIter<CSIPHeaderBase>(iSIPHeaderListArray[index]);
	}

// -----------------------------------------------------------------------------
// CSIPMessage::Header
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPHeaderBase* CSIPMessage::Header (RStringF aName, TInt aIndex)
	{
	TInt headerListIndex = FindHeaderListIndex (aName);
	if (headerListIndex < 0) 
        {
        return 0;
        }
	TSglQueIter<CSIPHeaderBase> iter(iSIPHeaderListArray[headerListIndex]);
	TInt positionInList=0;
	while (iter != 0 && positionInList <= aIndex)
		{
        CSIPHeaderBase* header = iter++;
		if (positionInList == aIndex) 
            {
            return header;
            }
		++positionInList;
		}
	return 0;
	}

// -----------------------------------------------------------------------------
// CSIPMessage::RemoveHeader
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPMessage::RemoveHeader (const CSIPHeaderBase* aSIPHeader)
	{
    if (aSIPHeader == 0)
        {
        return KErrNotFound;
        }
    TInt headerListIndex = FindHeaderListIndex (*aSIPHeader);
    if (headerListIndex < 0) 
        {
        return KErrNotFound;
        }
	TSglQueIter<CSIPHeaderBase> iter(iSIPHeaderListArray[headerListIndex]);
	while (iter)
		{
		CSIPHeaderBase* header = iter++;
		if (header == aSIPHeader)
			{
			iSIPHeaderListArray[headerListIndex].Remove (*header);
			if (iSIPHeaderListArray[headerListIndex].IsEmpty())
				{
				iSIPHeaderListArray.Remove(headerListIndex);
				}
			return KErrNone;
			}
		}
	return KErrNotFound;
	}

// -----------------------------------------------------------------------------
// CSIPMessage::DeleteHeaders
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPMessage::DeleteHeaders (RStringF aName)
    {
    TInt headerListIndex = FindHeaderListIndex (aName);
    if (headerListIndex < 0) 
        {
        return KErrNotFound;
        }
    TSglQueIter<CSIPHeaderBase> iter(iSIPHeaderListArray[headerListIndex]);
	while (iter)
		{
		CSIPHeaderBase* header = iter++;
		iSIPHeaderListArray[headerListIndex].Remove(*header);
		delete header;
		}
    iSIPHeaderListArray.Remove(headerListIndex);
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSIPMessage::AllHeadersL
// -----------------------------------------------------------------------------
//
EXPORT_C const RPointerArray<CSIPHeaderBase>& CSIPMessage::AllHeadersL ()
	{
	iTmpAllHeaders.Reset();
	for (TInt i=0; i < iSIPHeaderListArray.Count(); i++)
		{
		TSglQueIter<CSIPHeaderBase> iter(iSIPHeaderListArray[i]);
		while (iter)
			{
			CSIPHeaderBase* header = iter++;
			User::LeaveIfError(iTmpAllHeaders.Append(header));
			}
		}
	return iTmpAllHeaders;
	}

// -----------------------------------------------------------------------------
// CSIPMessage::From
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPFromHeader* CSIPMessage::From ()
    {
    CSIPHeaderBase* from = 
    	Header(SIPStrings::StringF(SipStrConsts::EFromHeader),0);
    return static_cast<CSIPFromHeader*>(from);
    }

// -----------------------------------------------------------------------------
// CSIPMessage::To
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPToHeader* CSIPMessage::To ()
    {
    CSIPHeaderBase* to = 
    	Header(SIPStrings::StringF(SipStrConsts::EToHeader),0);
    return static_cast<CSIPToHeader*>(to);
    }

// -----------------------------------------------------------------------------
// CSIPMessage::CallID
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPCallIDHeader* CSIPMessage::CallID ()
    {
    CSIPHeaderBase* callid = 
    	Header(SIPStrings::StringF(SipStrConsts::ECallIDHeader),0);
    return static_cast<CSIPCallIDHeader*>(callid);
    }

// -----------------------------------------------------------------------------
// CSIPMessage::CSeq
// -----------------------------------------------------------------------------
//
EXPORT_C CSIPCSeqHeader* CSIPMessage::CSeq ()
    {
    CSIPHeaderBase* cseq = 
    	Header(SIPStrings::StringF(SipStrConsts::ECSeqHeader),0);
    return static_cast<CSIPCSeqHeader*>(cseq);
    }

// -----------------------------------------------------------------------------
// CSIPMessage::SIPVersion
// -----------------------------------------------------------------------------
//
EXPORT_C RStringF CSIPMessage::SIPVersion() const
	{
	return iSIPVersion;
	}

// -----------------------------------------------------------------------------
// CSIPMessage::SetContent
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPMessage::SetContent (HBufC8* aContent)
	{
	delete iContent;
	iContent = aContent;
	}

// -----------------------------------------------------------------------------
// CSIPMessage::Content
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CSIPMessage::Content () const
	{
	return *iContent;
	}

// -----------------------------------------------------------------------------
// CSIPMessage::TakeContentOwnershipL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CSIPMessage::TakeContentOwnershipL ()
    {
    HBufC8* tmp = iContent;
    iContent = HBufC8::NewL(0);
    return tmp;
    }

// -----------------------------------------------------------------------------
// CSIPMessage::HasAnnouncedContentLength
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CSIPMessage::HasAnnouncedContentLength() const
	{
	return iAnnouncedContentLengthSet;
	}

// -----------------------------------------------------------------------------
// CSIPMessage::AnnouncedContentLength
// -----------------------------------------------------------------------------
//
EXPORT_C TUint CSIPMessage::AnnouncedContentLength() const
	{
	return iAnnouncedContentLength;
	}

// -----------------------------------------------------------------------------
// CSIPMessage::EncodedHeaderPartLengthL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CSIPMessage::EncodedHeaderPartLengthL ()
    {
    TInt encodedLength = 0;
    GetEncodedFirstLineSizeL(encodedLength);
    encodedLength += KCRLF().Length();
    // Calculate encoded header lengths
	for (TInt i=0; i < iSIPHeaderListArray.Count(); i++)
		{
		TSglQueIter<CSIPHeaderBase> iter(iSIPHeaderListArray[i]);
        TInt headerCountInList=0;
		while (iter)
			{
			CSIPHeaderBase* header = iter++;
            HBufC8* headerAsText = 0;
            if (++headerCountInList == 1 || header->IsExtensionHeader())
                {
			    headerAsText = header->ToTextL();
                }
            else
                {
                headerAsText = header->ToTextValueL();
                }
            encodedLength += headerAsText->Length();
            delete headerAsText; headerAsText = 0;
            if (!iter || header->IsExtensionHeader())
                {
                encodedLength += KCRLF().Length();
                }
            else // non-extension headers are encoded as a comma separated list
                {
                encodedLength += KComma().Length();
                }
			}
		}
	TInt encodedContentLengthHeaderSize = 0;
	GetEncodedContentLengthHeaderSizeL(encodedContentLengthHeaderSize);
    encodedLength += encodedContentLengthHeaderSize;
    encodedLength += KCRLFCRLF().Length(); // CRLF CRLF ending the header part
    return encodedLength;
    }

// -----------------------------------------------------------------------------
// CSIPMessage::ToTextHeaderPartL
// -----------------------------------------------------------------------------
//
EXPORT_C CBufBase* CSIPMessage::ToTextHeaderPartL ()
    {
    CBufBase* encodedHeaderPart = ToTextHeaderPartLC();
    CleanupStack::Pop(encodedHeaderPart);
    return encodedHeaderPart;
    }

// -----------------------------------------------------------------------------
// CSIPMessage::ToTextHeaderPartLC
// -----------------------------------------------------------------------------
//
EXPORT_C CBufBase* CSIPMessage::ToTextHeaderPartLC ()
    {
    const TInt KResultBufExpandSize = 100;
    CBufFlat* encodedHeaderPart = CBufFlat::NewL(KResultBufExpandSize);
    CleanupStack::PushL(encodedHeaderPart);
	HBufC8* firstLine = ToTextFirstLineLC ();    
    encodedHeaderPart->InsertL (0,*firstLine);
    TInt encodedLength = firstLine->Length();
    CleanupStack::PopAndDestroy(firstLine);
    encodedHeaderPart->InsertL (encodedLength,KCRLF);
    encodedLength += KCRLF().Length();

    // Add headers
	for (TInt i=0; i < iSIPHeaderListArray.Count(); i++)
		{
		TSglQueIter<CSIPHeaderBase> iter(iSIPHeaderListArray[i]);
        TInt headerCountInList=0;
		while (iter)
			{
			CSIPHeaderBase* header = iter++;
            HBufC8* headerAsText = NULL;
            if (++headerCountInList == 1 || !header->EncodeMultipleToOneLine())
                {
			    headerAsText = header->ToTextLC();
                }
            else
                {
                headerAsText = header->ToTextValueLC();
                }
            encodedHeaderPart->InsertL (encodedLength,*headerAsText);
            encodedLength += headerAsText->Length();
            CleanupStack::PopAndDestroy(headerAsText);
            if (!iter || !header->EncodeMultipleToOneLine())
                {
                encodedHeaderPart->InsertL (encodedLength,KCRLF);
                encodedLength += KCRLF().Length();
                }
            else 
                {
                encodedHeaderPart->InsertL (encodedLength,KComma);
                encodedLength += KComma().Length();
                }
			}
		}

    // Add Content-Length header
	HBufC8* contentLengthHeaderAsText = ToTextContentLengthLC ();
    encodedHeaderPart->InsertL (encodedLength,*contentLengthHeaderAsText);
    encodedLength += contentLengthHeaderAsText->Length();
    CleanupStack::PopAndDestroy(contentLengthHeaderAsText);

    // Add CRLF CRLF ending the header part
    encodedHeaderPart->InsertL (encodedLength,KCRLFCRLF);
    encodedLength += KCRLFCRLF().Length();

    return encodedHeaderPart;
    }

// -----------------------------------------------------------------------------
// CSIPMessage::ToTextL
// -----------------------------------------------------------------------------
//
EXPORT_C CBufBase* CSIPMessage::ToTextL ()
	{
    CBufBase* encodedMessage = ToTextLC();
    CleanupStack::Pop(encodedMessage);
    return encodedMessage;
    }

// -----------------------------------------------------------------------------
// CSIPMessage::ToTextLC
// -----------------------------------------------------------------------------
//
EXPORT_C CBufBase* CSIPMessage::ToTextLC ()
	{
    CBufBase* encodedMessage = ToTextHeaderPartLC();
    TInt length = encodedMessage->Ptr(0).Length();
    encodedMessage->InsertL (length,*iContent);
	return encodedMessage;
	}

// -----------------------------------------------------------------------------
// CSIPMessage::ExternalizeHeadersL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPMessage::ExternalizeHeadersL (RWriteStream& aWriteStream)
	{
	const RPointerArray<CSIPHeaderBase>& headers = AllHeadersL();
    const CSIPHeaderBase* header = 0;
	for (TInt i=0; i < headers.Count(); i++)
		{
		header = headers[i];
		if (SIPHeaderLookup::IsAPIHeader(header->Name()) ||
		    header->IsExtensionHeader())
			{
			aWriteStream.WriteUint8L(1); // more headers in the stream flag
			header->ExternalizeL(aWriteStream);
			}
		}
	aWriteStream.WriteUint8L(0); // no more headers in the stream flag
	}

// -----------------------------------------------------------------------------
// CSIPMessage::InternalizeHeadersL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSIPMessage::InternalizeHeadersL (RReadStream& aReadStream)
	{
	TUint8 moreHeaders = aReadStream.ReadUint8L();
    CSIPHeaderBase* header = 0;
	while (moreHeaders)
		{
		RStringF headerName = ReadFromStreamL(aReadStream);
		CleanupClosePushL(headerName);
		header = SIPHeaderLookup::InternalizeL(headerName,aReadStream);
		CleanupStack::PopAndDestroy(1); // headerName
		if (header != 0)
			{
			CleanupStack::PushL(header);
			AddHeaderL(header);
			CleanupStack::Pop(header);
			}
		moreHeaders = aReadStream.ReadUint8L();
		}
	}

// -----------------------------------------------------------------------------
// CSIPMessage::ReadFromStreamL
// -----------------------------------------------------------------------------
//
RStringF CSIPMessage::ReadFromStreamL (RReadStream& aReadStream) const
	{
	TUint32 bufLength = aReadStream.ReadUint32L();
	HBufC8* buf = HBufC8::NewLC (bufLength);
	TPtr8 bufPtr(buf->Des());	
	if (bufLength > 0)
		{
		aReadStream.ReadL (bufPtr,bufLength);
		}
	RStringF str = SIPStrings::Pool().OpenFStringL(bufPtr);
	CleanupStack::PopAndDestroy(buf);	
	return str;
	}

// -----------------------------------------------------------------------------
// CSIPMessage::SetSIPVersionL
// -----------------------------------------------------------------------------
//
void CSIPMessage::SetSIPVersionL (const TDesC8& aSIPVersion)
	{
	__ASSERT_ALWAYS (SIPSyntaxCheck::SIPVersion(aSIPVersion),
	                 User::Leave(KErrSipCodecSIPVersion));

	HBufC8* tmp = aSIPVersion.AllocLC();
	tmp->Des().UpperCase();
	RStringF tmpString = SIPStrings::Pool().OpenFStringL(*tmp);
	CleanupStack::PopAndDestroy(tmp);		
	iSIPVersion.Close();
	iSIPVersion = tmpString;
	}

// -----------------------------------------------------------------------------
// CSIPMessage::IsContentLengthHeader
// -----------------------------------------------------------------------------
//
TBool CSIPMessage::IsContentLengthHeader(const CSIPHeaderBase& aHeader) const
	{
	if (aHeader.Name() == 
	    SIPStrings::StringF(SipStrConsts::EContentLengthHeader) ||
		aHeader.CompactName() ==
		SIPStrings::StringF(SipStrConsts::EContentLengthHeaderCompact))
		{
		return ETrue;	
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPMessage::FindHeaderListIndex
// -----------------------------------------------------------------------------
//
TInt CSIPMessage::FindHeaderListIndex (RStringF aName) const
	{
	for (TInt i=0; i < iSIPHeaderListArray.Count(); i++)
		{
		if (!iSIPHeaderListArray[i].IsEmpty())
			{
			CSIPHeaderBase* header = iSIPHeaderListArray[i].First();
			if (header != 0)
				{
				if (header->Name() == aName ||
					(header->HasCompactName() && 
					 header->CompactName() == aName))
					{
					return i;
					}
				}
			}
		}
	return KErrNotFound;
	}

// -----------------------------------------------------------------------------
// CSIPMessage::FindHeaderListIndex
// -----------------------------------------------------------------------------
//
TInt CSIPMessage::FindHeaderListIndex (const CSIPHeaderBase& aHeader) const
	{
	for (TInt i=0; i < iSIPHeaderListArray.Count(); i++)
		{
		if (!iSIPHeaderListArray[i].IsEmpty())
			{
			CSIPHeaderBase* header = iSIPHeaderListArray[i].First();
			if (header != 0)
				{
				if (header->Name() == aHeader.Name() ||
					(header->HasCompactName() && aHeader.HasCompactName() &&
					 header->CompactName() == aHeader.CompactName()))
					{
					return i;
					}
				}
			}
		}
	return KErrNotFound;
	}

// -----------------------------------------------------------------------------
// CSIPMessage::InsertL
// -----------------------------------------------------------------------------
//
void CSIPMessage::InsertL (TSglQue<CSIPHeaderBase>& aHeaderList)
	{
	TLinearOrder<TSglQue<CSIPHeaderBase> > order(&HeaderOrder);
	TInt err=iSIPHeaderListArray.InsertInOrderAllowRepeats(aHeaderList,order);
	User::LeaveIfError (err);
	}

// -----------------------------------------------------------------------------
// CSIPMessage::HeaderOrder
// -----------------------------------------------------------------------------
//
TInt CSIPMessage::HeaderOrder (const TSglQue<CSIPHeaderBase>& aFirstList,
							   const TSglQue<CSIPHeaderBase>& aSecondList)
	{
	if (aFirstList.IsEmpty() && aSecondList.IsEmpty()) 
        {
        return 0;
        }
	if (aFirstList.IsEmpty()) 
        {
        return 1;
        }
	if (aSecondList.IsEmpty()) 
        {
        return -1;
        }

	CSIPHeaderBase* firstHeader = aFirstList.First();
	CSIPHeaderBase* secondHeader = aSecondList.First();

	if (firstHeader->PreferredPlaceInMessage() <
		secondHeader->PreferredPlaceInMessage()) 
        {
        return -1;
        }

	if (firstHeader->PreferredPlaceInMessage() >
		secondHeader->PreferredPlaceInMessage()) 
        {
        return 1;
        }
	
	return 0;
	}

// -----------------------------------------------------------------------------
// CSIPMessage::GetEncodedFirstLineSizeL
// -----------------------------------------------------------------------------
//
void CSIPMessage::GetEncodedFirstLineSizeL(TInt& aSize) const
    {
	HBufC8* firstLine = ToTextFirstLineLC ();   
    aSize = firstLine->Length();
    CleanupStack::PopAndDestroy(firstLine);
    }

// -----------------------------------------------------------------------------
// CSIPMessage::EncodedContentLengthHeaderSizeL
// -----------------------------------------------------------------------------
//
void CSIPMessage::GetEncodedContentLengthHeaderSizeL(TInt& aSize) const
    {
    HBufC8* headerAsText = ToTextContentLengthLC();
    aSize = headerAsText->Length();
    CleanupStack::PopAndDestroy(headerAsText);
    }

// -----------------------------------------------------------------------------
// CSIPMessage::ToTextContentLengthLC
// -----------------------------------------------------------------------------
//
HBufC8* CSIPMessage::ToTextContentLengthLC () const
	{
	CSIPContentLengthHeader* contentLengthHeader 
		= new(ELeave)CSIPContentLengthHeader;
	CleanupStack::PushL (contentLengthHeader);
	if (iAnnouncedContentLengthSet)
		{	
		contentLengthHeader->SetValue(iAnnouncedContentLength);
		}
	else
		{
		contentLengthHeader->SetValue(iContent->Length());
		}
	HBufC8* headerAsText = contentLengthHeader->ToTextL();
	CleanupStack::PopAndDestroy(contentLengthHeader);
	CleanupStack::PushL(headerAsText);
	return headerAsText;
	}
