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
// Name          : SIPMessageUtility.cpp
// Part of       : TransactionUser
// Version       : SIP/5.0
//



#include <in_sock.h>
#include <e32math.h>
#include <utf.h>

#include "SipAssert.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "sipuri.h"
#include "uricontainer.h"
#include "sipviaheader.h"
#include "sipfromheader.h"
#include "siptoheader.h"
#include "sipaddress.h"
#include "siphostport.h"
#include "sipcseqheader.h"
#include "sipstrings.h"
#include "sipstrconsts.h"

#include "SIPMessageUtility.h"
#include "CUserAgent.h"


const TInt KMaxBitsReturned = 6;
const TInt KBitsInByte = 8;

//Length of CSIPMessageUtility::iCounter as characters in hex representation
const TInt KCounterLength = 8;


// -----------------------------------------------------------------------------
// CSIPMessageUtility::CSIPMessageUtility
// -----------------------------------------------------------------------------
//
CSIPMessageUtility::CSIPMessageUtility()
	{	
	TUint ticks = User::TickCount();
	TTime now;
	now.UniversalTime();
	TInt64 us = now.Int64();

	iSeed = static_cast<TInt64>(ticks) + us;
	iCounter = I64LOW(us) - ticks;
	}

// -----------------------------------------------------------------------------
// CSIPMessageUtility::~CSIPMessageUtility
// -----------------------------------------------------------------------------
//
CSIPMessageUtility::~CSIPMessageUtility()
	{
	}

// -----------------------------------------------------------------------------
// CSIPMessageUtility::CompareTInetAddr
// aAddr2 always has port filled.
// -----------------------------------------------------------------------------
//
TBool CSIPMessageUtility::CompareTInetAddr(const TInetAddr& aAddr,
										   RStringF aTransportProtocol,
                                           const TInetAddr& aAddr2)
	{
    return aAddr.Match(aAddr2) &&
    	   (SIPPort(aAddr, aTransportProtocol) == aAddr2.Port());
	}

// -----------------------------------------------------------------------------
// CSIPMessageUtility::SIPPort
// -----------------------------------------------------------------------------
//
TUint CSIPMessageUtility::SIPPort(const TInetAddr& aAddr,
								  RStringF aTransportProtocol)
	{
	const TUint KDefaultSipPort 	  = 5060;
	const TUint KDefaultSipPortForTLS = 5061;

    if (aAddr.Port() == 0)
        {
        if (aTransportProtocol == SIPStrings::StringF(SipStrConsts::ETLS))
        	{
        	return KDefaultSipPortForTLS;
        	}

        return KDefaultSipPort;
        }
    return aAddr.Port();
	}

// -----------------------------------------------------------------------------
// CSIPMessageUtility::ConvertUtf8LC
// -----------------------------------------------------------------------------
//
HBufC* CSIPMessageUtility::ConvertUtf8LC(const TDesC8& aUtf8)
	{
	HBufC* unicode = HBufC::NewLC(aUtf8.Size());
	TPtr ptr = unicode->Des();

	User::LeaveIfError(CnvUtfConverter::ConvertToUnicodeFromUtf8(ptr, aUtf8));

    return unicode;
	}

// -----------------------------------------------------------------------------
// CSIPMessageUtility::AddRandomStringL
// -----------------------------------------------------------------------------
//
void CSIPMessageUtility::AddRandomStringL(TDes8& aBuf,
										  TInt aLength,
										  TBool aCaseSensitive,
										  CSIPMessage* aMsg,
										  TTransactionId aTransactionId,
										  const CUserAgent* aUserAgent)
	{
    __SIP_ASSERT_LEAVE(aLength <= aBuf.MaxLength() - aBuf.Length(),
		               KErrOverflow);

	HBufC8* data = BuildInputDataL(aLength, aMsg, aTransactionId, aUserAgent);

    //2^KMaxBitsReturned (64) valid chars that can be put to descriptor
	_LIT8(KCharStore,
          "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-_");
	TInt amountOfBits = KMaxBitsReturned;
	if (!aCaseSensitive)
		{
		//The capital letters are not used
		amountOfBits--;
		}

	TUint counter = 0;
	for (TInt i = 0; i < aLength; i++)
		{
		aBuf.Append(KCharStore()[CSIPMessageUtility::GetNextBits(
            		*data, amountOfBits, counter) %	KCharStore().Length()]);
		}

	delete data;
	}

// -----------------------------------------------------------------------------
// CSIPMessageUtility::BuildInputDataL
// Use the best source of randomness first, and add pseudorandom numbers until
// there is enough data.
// -----------------------------------------------------------------------------
//
HBufC8* CSIPMessageUtility::BuildInputDataL(TUint aLength,
											CSIPMessage* aMsg,
											TTransactionId aTransactionId,
											const CUserAgent* aUserAgent)
	{
	//Extra bytes at end to have enough data for getting aLength amount of
	//KMaxBitsReturned bit sequences.
	const TInt KExtra = 4;
	HBufC8* buf =
		HBufC8::NewLC(((aLength * KMaxBitsReturned) / KBitsInByte) + KExtra);
	TPtr8 ptr = buf->Des();

	//First two bytes are filled later. Terminate string with a zero.
	const TUint8 reserveString[] = {1, 1, 0};
	ptr = reserveString;

	//Increment the counter every time the input data is created so even if To,
	//From etc. headers are same, output differs.
	iCounter++;

    AddCheckSumOfSipMessageL(aMsg, ptr);
    AddCheckSumOfTaIdL(aTransactionId, ptr);
	AddSystemInfo(ptr);

	if (aUserAgent && FitsInBuf(ptr, sizeof(TUint16)))
		{
		//UserAgent's current state
        ComputeChecksum(ptr, aUserAgent, sizeof(*aUserAgent));
		}

	TInt random = 0;
	while (FitsInBuf(ptr, sizeof(random)))
		{
		random = Math::Rand(iSeed);
		ptr.Append(reinterpret_cast<const TUint8*>(&random), sizeof(random));
        }

    //Write buf's checksum to the two reserved bytes.
	TUint16 cs = 0;
	Mem::Crc(cs, ptr.Ptr(), buf->Length());
	ptr[0] = static_cast<TUint8>((cs & 0xff00) >> KBitsInByte);
	ptr[1] = static_cast<TUint8>(cs & 0xff);

	CleanupStack::Pop(buf);
	return buf;
	}

// -----------------------------------------------------------------------------
// CSIPMessageUtility::GetNextBits
// -----------------------------------------------------------------------------
//
TUint8
CSIPMessageUtility::GetNextBits(const TDesC8& aBuf, TInt aBits, TUint& aCounter)
	{
	if (aBuf.Length() == 0)
		{
		return 0;
		}

	//Amount of aBits long bit sequences in aBuf
	TUint sixBitItems = aBuf.Length() * KBitsInByte / aBits;

	if (aCounter >= sixBitItems)
		{
		aCounter = 0;
		}

	//The position in aBuf, of the byte containing the first bit of the aBits
	//long bit sequence. Zero means the first byte.
	TInt startByte = aCounter * aBits / KBitsInByte;
	TUint16 result = static_cast<TUint16>(aBuf[startByte] << KBitsInByte);

	if (++startByte >= aBuf.Length())
		{
		startByte = 0;
		}
	
	result = static_cast<TUint16>(result | aBuf[startByte]);
	
	
	//The position of the first bit of the aBits long bit sequence, within the
	//byte. Zero means the first bit.
	TUint offsetInsideByte = (aCounter * aBits) % KBitsInByte;

	//Remove excess bits from the result
	result = static_cast<TUint16>(result << offsetInsideByte);	
	result >>= ((2 * KBitsInByte) - aBits);

    __ASSERT_DEBUG(result < (1 << aBits),
        		   User::Panic(_L("CSIPMsgUtil:GetNextBits"), KErrOverflow));
    aCounter++;
    return static_cast<TUint8>(result);
	}

// -----------------------------------------------------------------------------
// CSIPMessageUtility::FitsInBuf
// -----------------------------------------------------------------------------
//
TBool CSIPMessageUtility::FitsInBuf(const TDes8& aBuf, TInt aSize)
	{	
	return (aBuf.Size() <= aBuf.MaxSize() - aSize);
	}

// -----------------------------------------------------------------------------
// CSIPMessageUtility::AddCheckSumOfSipMessageL
// -----------------------------------------------------------------------------
//
void CSIPMessageUtility::AddCheckSumOfSipMessageL(CSIPMessage* aMsg,
                                                  TDes8& aBuf)
    {
    if (aMsg)
        {
        if (aMsg->HasHeader(SIPStrings::StringF(SipStrConsts::EToHeader)))
		    {
		    AddCheckSumOfFromToHeaderL(*aMsg->To(), aBuf);
		    }
	
	    if (aMsg->HasHeader(SIPStrings::StringF(SipStrConsts::EFromHeader)))
		    {
		    AddCheckSumOfFromToHeaderL(*aMsg->From(), aBuf);
		    }
	
	    AddCheckSumOfCSeq(*aMsg, aBuf);
	    AddCheckSumOfRequestLineL(*aMsg, aBuf);
        }
    }

// -----------------------------------------------------------------------------
// CSIPMessageUtility::AddCheckSumOfFromToHeaderL
// -----------------------------------------------------------------------------
//
void
CSIPMessageUtility::AddCheckSumOfFromToHeaderL(CSIPFromToHeaderBase& aHeader,
                                               TDes8& aBuf)
	{
	if (FitsInBuf(aBuf, sizeof(TUint16)))
		{
		HBufC8* toBuf = aHeader.SIPAddress().ToTextLC();
		HBufC8* buf = HBufC8::NewLC(KCounterLength + toBuf->Length());
		TPtr8 ptr = buf->Des();

		//Add iCounter at the beginning, to get a greater effect on output
		ptr.AppendFormat(_L8("%x"), iCounter);
		ptr.Append(*toBuf);

        ComputeChecksum(aBuf, buf, buf->Length());

        CleanupStack::PopAndDestroy(buf);
		CleanupStack::PopAndDestroy(toBuf);
		}
	}

// -----------------------------------------------------------------------------
// CSIPMessageUtility::AddCheckSumOfCSeq
// -----------------------------------------------------------------------------
//
void CSIPMessageUtility::AddCheckSumOfCSeq(CSIPMessage& aMsg, TDes8& aBuf)
	{
	if (aMsg.HasHeader(SIPStrings::StringF(SipStrConsts::ECSeqHeader)) &&
		FitsInBuf(aBuf, sizeof(TUint16)))
		{
		TUint cseq = aMsg.CSeq()->Seq();
        ComputeChecksum(aBuf, &cseq, sizeof(cseq));
		}
	}

// -----------------------------------------------------------------------------
// CSIPMessageUtility::AddCheckSumOfClock
// -----------------------------------------------------------------------------
//
void CSIPMessageUtility::AddCheckSumOfClock(TDes8& aBuf) const
	{
	if (FitsInBuf(aBuf, sizeof(TUint16)))
		{
		TTime now;
		now.UniversalTime();
		TInt64 timeAsInt = now.Int64();
		TBuf8<20> timeBuf;

		//Add iCounter before timeAsInt. If this function is called very often,
		//timeAsInt can be same as in the previous call.
		timeBuf.Append(reinterpret_cast<const TUint8*>(&iCounter),
					   sizeof(iCounter));
		timeBuf.Append(reinterpret_cast<const TUint8*>(&timeAsInt),
					   sizeof(timeAsInt));
        ComputeChecksum(aBuf, &timeBuf, timeBuf.Length());
	
		if (FitsInBuf(aBuf, sizeof(TUint16)))
			{
			timeBuf.Zero();
			timeBuf.Append(reinterpret_cast<const TUint8*>(&iCounter),
						   sizeof(iCounter));
			TUint ticks = User::TickCount();
			timeBuf.Append(reinterpret_cast<const TUint8*>(&ticks),
						   sizeof(ticks));
            ComputeChecksum(aBuf, &timeBuf, timeBuf.Length());
            }
		}
	}

// -----------------------------------------------------------------------------
// CSIPMessageUtility::AddCheckSumOfTaIdL
// Add counter first, so it has greater effect on output.
// -----------------------------------------------------------------------------
//
void CSIPMessageUtility::AddCheckSumOfTaIdL(TTransactionId aTransactionId,
											TDes8& aBuf)
	{
	if (aTransactionId != KEmptyTransactionId &&
		FitsInBuf(aBuf, sizeof(TUint16)))
		{
		const TInt KTransactionIdLengthInHex = 8;
		HBufC8* buf = HBufC8::NewLC(KCounterLength + KTransactionIdLengthInHex);
		TPtr8 ptr = buf->Des();
		ptr.AppendFormat(_L8("%x%x"), iCounter, aTransactionId);

        ComputeChecksum(aBuf, buf, buf->Length());
        CleanupStack::PopAndDestroy(buf);
		}
	}

// -----------------------------------------------------------------------------
// CSIPMessageUtility::AddCheckSumOfRequestLineL
// -----------------------------------------------------------------------------
//
void CSIPMessageUtility::AddCheckSumOfRequestLineL(CSIPMessage& aMsg,
                                                   TDes8& aBuf)
	{
	if (aMsg.IsRequest() && FitsInBuf(aBuf, sizeof(TUint16)))
		{
		CSIPRequest& req = static_cast<CSIPRequest&>(aMsg);	
		if (req.RequestURI())
			{
			HBufC8* uriBuf = req.RequestURI()->ToTextL();
			CleanupStack::PushL(uriBuf);

			HBufC8* buf =
                HBufC8::NewLC(uriBuf->Length() + req.Method().DesC().Length());
			TPtr8 ptr = buf->Des();
			ptr.Append(*uriBuf);
			ptr.Append(req.Method().DesC());

            ComputeChecksum(aBuf, buf, buf->Length());

			CleanupStack::PopAndDestroy(buf);
			CleanupStack::PopAndDestroy(uriBuf);
			}
		}
	}

// -----------------------------------------------------------------------------
// CSIPMessageUtility::AddSystemInfo
// -----------------------------------------------------------------------------
//
void CSIPMessageUtility::AddSystemInfo(TDes8& aBuf) const
	{
	AddCheckSumOfClock(aBuf);

	if (FitsInBuf(aBuf, sizeof(TUint16)))
		{
		TInt largest = 0;
		TInt total = User::Available(largest);		
		TInt value = (largest + total - User::CountAllocCells()) + iCounter;
        ComputeChecksum(aBuf, &value, sizeof(value));
		}

	if (FitsInBuf(aBuf, sizeof(TUint8)))	
		{
		TTimeIntervalSeconds inactivity = User::InactivityTime();
		if (inactivity.Int() > 0)
			{
			TUint8 byteVal = static_cast<TUint8>(inactivity.Int() & 0xff);
			aBuf.Append(&byteVal, sizeof(byteVal));
			}
		}
	}

// -----------------------------------------------------------------------------
// CSIPMessageUtility::ComputeChecksum
// -----------------------------------------------------------------------------
//
void
CSIPMessageUtility::ComputeChecksum(TDes8& aBuf, const TAny* aPtr, TInt aLength)
    {
    __SIP_ASSERT_RETURN(aPtr != NULL, KErrArgument);

    TUint16 cs = 0;
	Mem::Crc(cs, aPtr, aLength);
	aBuf.Append(reinterpret_cast<const TUint8*>(&cs), sizeof(cs));
    }

// -----------------------------------------------------------------------------
// CSIPMessageUtility::MessageMethod
// -----------------------------------------------------------------------------
//
RStringF CSIPMessageUtility::MessageMethod(CSIPMessage& aMsg)
	{
	if (aMsg.IsRequest())
		{
		return static_cast<const CSIPRequest&>(aMsg).Method();
		}

    __ASSERT_DEBUG(aMsg.CSeq() != NULL,
        		   User::Panic(_L("CSIPMsgUtil:MsgMethod"), KErrArgument));
    return aMsg.CSeq()->Method();
	}

// -----------------------------------------------------------------------------
// CSIPMessageUtility::TransactionType
// -----------------------------------------------------------------------------
//
CTransactionBase::TTransactionType
CSIPMessageUtility::TransactionType(CSIPMessage& aMsg, TBool aIncomingMsg)
	{
	__ASSERT_ALWAYS(
		aMsg.HasHeader(SIPStrings::StringF(SipStrConsts::ECSeqHeader)),
		User::Panic(_L("MsgUt:TType CSeq"), KErrArgument));

	RStringF cseqMethod = aMsg.CSeq()->Method();
	if (cseqMethod == SIPStrings::StringF(SipStrConsts::EInvite) ||
        cseqMethod == SIPStrings::StringF(SipStrConsts::EAck))
		{
		if (aMsg.IsRequest() ^ aIncomingMsg)
			{
			//Outgoing request or incoming response
			return CTransactionBase::KClientInviteTransaction;
			}

		return CTransactionBase::KServerInviteTransaction;
		}

	if (aMsg.IsRequest() ^ aIncomingMsg)
		{
		return CTransactionBase::KClientTransaction;
		}

	return CTransactionBase::KServerTransaction;
	}

// -----------------------------------------------------------------------------
// CSIPMessageUtility::HasViaMagicCookie
// -----------------------------------------------------------------------------
//
TBool CSIPMessageUtility::HasViaMagicCookie(CSIPMessage& aMsg)
	{
    CSIPViaHeader* topVia = TopVia(aMsg);
    RStringF branch = SIPStrings::StringF(SipStrConsts::EBranch);

	return topVia &&
		   topVia->HasParam(branch) &&
           topVia->ParamValue(branch).DesC().Left(
               BranchMagicCookie().Length()).Compare(BranchMagicCookie()) == 0;
	}

// -----------------------------------------------------------------------------
// CSIPMessageUtility::CheckTransport
// -----------------------------------------------------------------------------
//
TBool CSIPMessageUtility::CheckTransport(RStringF aTransport)
    {
    return aTransport == SIPStrings::StringF(SipStrConsts::EUDP) ||
    	   aTransport == SIPStrings::StringF(SipStrConsts::ETCP) ||
    	   aTransport == SIPStrings::StringF(SipStrConsts::ETLS);
    }

// -----------------------------------------------------------------------------
// CSIPMessageUtility::TransportProtocol
// -----------------------------------------------------------------------------
//
TBool CSIPMessageUtility::TransportProtocol(CSIPMessage& aMsg, 
											RStringF& aTransport)
	{
    CSIPViaHeader* topVia = TopVia(aMsg);
    if (topVia && CheckTransport(topVia->Transport()))
		{
		aTransport.Close();
		aTransport = topVia->Transport().Copy();
		return ETrue;
		}

	return EFalse;
	}

// -----------------------------------------------------------------------------
// CSIPMessageUtility::UpdateViaTransportL
// -----------------------------------------------------------------------------
//
void CSIPMessageUtility::UpdateViaTransportL(CSIPMessage& aMsg, 
											 RStringF aTransport)
	{
    __SIP_ASSERT_LEAVE(CheckTransport(aTransport), KErrArgument);

    CSIPViaHeader* topVia = TopVia(aMsg);
    __SIP_ASSERT_LEAVE(topVia != NULL, KErrArgument);

    topVia->SetTransportL(aTransport);
	}

// -----------------------------------------------------------------------------
// CSIPMessageUtility::CompareTags
// If either tag is missing, tags are considered equal
// -----------------------------------------------------------------------------
//
TBool CSIPMessageUtility::CompareTags(const CSIPFromToHeaderBase& aHeader,
									  const CSIPFromToHeaderBase& aHeader2)
	{
	RStringF tag = SIPStrings::StringF(SipStrConsts::ETag);
	if (aHeader.HasParam(tag) && aHeader2.HasParam(tag))
		{
		return aHeader.ParamValue(tag) == aHeader2.ParamValue(tag);
		}
	
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSIPMessageUtility::CopyHeadersL
// If aDest already has headers, they are deleted.
// -----------------------------------------------------------------------------
//
void CSIPMessageUtility::CopyHeadersL(CSIPMessage& aSrc,
							 	      CSIPMessage& aDest,
									  RStringF aHeaderName)
	{	
	if (aDest.HasHeader(aHeaderName))
		{
		aDest.DeleteHeaders(aHeaderName);
		}

	if (aSrc.HasHeader(aHeaderName))
		{
		TSglQueIter<CSIPHeaderBase> iter = aSrc.Headers(aHeaderName);

		for (CSIPHeaderBase* srcHeader = iter++; srcHeader; srcHeader= iter++)
			{
			CSIPHeaderBase* destHeader = srcHeader->CloneL();
			CleanupStack::PushL(destHeader);
			aDest.AddHeaderL(destHeader);
			CleanupStack::Pop(destHeader);
			}
		}
	}

// -----------------------------------------------------------------------------
// CSIPMessageUtility::CopyAuthorizationHeadersL
// -----------------------------------------------------------------------------
//
void CSIPMessageUtility::CopyAuthorizationHeadersL(CSIPMessage* aSrc,
												   CSIPMessage& aDest)
	{
	if (aSrc)
		{
		CopyHeadersL(*aSrc,
					 aDest,
					 SIPStrings::StringF(SipStrConsts::EAuthorizationHeader));
		CopyHeadersL(*aSrc,
				aDest,
				SIPStrings::StringF(SipStrConsts::EProxyAuthorizationHeader));
		}
	}

// -----------------------------------------------------------------------------
// CSIPMessageUtility::CopyHeaderFromMsgL
// -----------------------------------------------------------------------------
//
CSIPHeaderBase*
CSIPMessageUtility::CopyHeaderFromMsgL(CSIPMessage& aMsg, RStringF aHeaderName)
	{
	if (aMsg.HasHeader(aHeaderName))
		{
		TSglQueIter<CSIPHeaderBase> iter = aMsg.Headers(aHeaderName);
		CSIPHeaderBase* header = iter;
		return header->CloneL();
		}

	return NULL;
	}

// -----------------------------------------------------------------------------
// CSIPMessageUtility::FillCSeqL
// -----------------------------------------------------------------------------
//
void
CSIPMessageUtility::FillCSeqL(CSIPMessage& aMsg, TUint aSeq, RStringF aMethod)
	{
	if (!aMsg.HasHeader(SIPStrings::StringF(SipStrConsts::ECSeqHeader)))
		{
		CSIPCSeqHeader* cseq = CSIPCSeqHeader::NewLC(aSeq, aMethod);
		aMsg.AddHeaderL(cseq);
		CleanupStack::Pop(cseq);
		}
	}

// -----------------------------------------------------------------------------
// CSIPMessageUtility::HasSigCompParam
// -----------------------------------------------------------------------------
//
TBool CSIPMessageUtility::HasSigCompParam(const CURIContainer& aUri)
    {
    RStringF comp = SIPStrings::StringF(SipStrConsts::EComp);
	return aUri.IsSIPURI() &&
		   aUri.SIPURI()->HasParam(comp) &&
		   aUri.SIPURI()->ParamValue(comp) ==
		   SIPStrings::StringF(SipStrConsts::ESigComp);
    }

// -----------------------------------------------------------------------------
// CSIPMessageUtility::IsAck
// -----------------------------------------------------------------------------
//
TBool CSIPMessageUtility::IsAck(const CSIPMessage& aMsg)
	{
	return aMsg.IsRequest() &&
		   (static_cast<const CSIPRequest&>(aMsg).Method() ==
			SIPStrings::StringF(SipStrConsts::EAck));
	}

// -----------------------------------------------------------------------------
// CSIPMessageUtility::IsFinalResponse
// -----------------------------------------------------------------------------
//
TBool CSIPMessageUtility::IsFinalResponse(const CSIPResponse& aResp)
	{
	return aResp.Type() != CSIPResponse::E1XX;
	}

// -----------------------------------------------------------------------------
// CSIPMessageUtility::Is2xxResponse
// -----------------------------------------------------------------------------
//
TBool CSIPMessageUtility::Is2xxResponse(const CSIPMessage& aMsg)
	{
	return !aMsg.IsRequest() &&
    	   static_cast<const CSIPResponse&>(aMsg).Type() == CSIPResponse::E2XX;
	}

// -----------------------------------------------------------------------------
// CSIPMessageUtility::TopVia()
// -----------------------------------------------------------------------------
//
CSIPViaHeader* CSIPMessageUtility::TopVia(CSIPMessage& aMsg)
    {
    if (aMsg.HasHeader(SIPStrings::StringF(SipStrConsts::EViaHeader)))
		{
		TSglQueIter<CSIPHeaderBase> headers =
            aMsg.Headers(SIPStrings::StringF(SipStrConsts::EViaHeader));
		return &static_cast<CSIPViaHeader&>(*headers);
        }

    return NULL;
    }

// -----------------------------------------------------------------------------
// CSIPMessageUtility::BranchMagicCookie()
// If Via's Branch begins with this value, the SIP message <-> transaction
// mapping is done using the Branch.value.
// -----------------------------------------------------------------------------
//
const TDesC8& CSIPMessageUtility::BranchMagicCookie()
    {    
    _LIT8(KMagicCookie, "z9hG4bK");
    return KMagicCookie;
    }

// -----------------------------------------------------------------------------
// CSIPMessageUtility::UriDescriptor
// -----------------------------------------------------------------------------
//
const TDesC8& CSIPMessageUtility::UriDescriptor(const CURIContainer& aUri)
	{	
    if (aUri.IsSIPURI())
    	{
    	const CSIPURI& sipuri = *aUri.SIPURI();
    	RStringF maddr = SIPStrings::StringF(SipStrConsts::EMaddr);
    	
    	if (sipuri.HasParam(maddr))
    		{
    		return sipuri.ParamValue(maddr).DesC();
    		}    	

    	return sipuri.HostPort().Host();
    	}    
    return aUri.Uri8()->Uri().UriDes();    
	}

// -----------------------------------------------------------------------------
// CSIPMessageUtility::IsPrivateAddressL
// -----------------------------------------------------------------------------
//
TBool CSIPMessageUtility::IsPrivateAddressL(MSIPTransportMgr& aTransportMgr,
										    TUint32 aIapId)
	{	
	TInetAddr localAddr;
	User::LeaveIfError(aTransportMgr.GetLocalAddress(aIapId, localAddr));

    // The following addresses are private (RFC 1918):
	// 10.0.0.0 - 10.255.255.255
	const TUint32 KPrivateRange1Low  = INET_ADDR(10, 0, 0, 0);
	const TUint32 KPrivateRange1High = INET_ADDR(10, 255, 255, 255);
	// 172.16.0.0 - 172.31.255.255
	const TUint32 KPrivateRange2Low  = INET_ADDR(172, 16, 0, 0);
	const TUint32 KPrivateRange2High = INET_ADDR(172, 31, 255, 255);
	// 192.168.0.0 - 192.168.255.255 
	const TUint32 KPrivateRange3Low  = INET_ADDR(192, 168, 0, 0);
	const TUint32 KPrivateRange3High = INET_ADDR(192, 168, 255, 255);

    TUint32 addr = localAddr.Address();
    //First check if a IPv4 address. IPv6 addresses are always public.
    return ( localAddr.Address() &&
            ((addr >= KPrivateRange1Low && addr <= KPrivateRange1High) ||
             (addr >= KPrivateRange2Low && addr <= KPrivateRange2High) ||
             (addr >= KPrivateRange3Low && addr <= KPrivateRange3High)));
	}
