// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation file for SIP dummy PRT
// 
//

/**
 @file
 @internalComponent
*/

#include <es_prot.h>
#include "SIPDummy.h"
#include <es_prot_internal.h>


extern "C"
	{
	IMPORT_C CProtocolFamilyBase* InstallSIPDummy(void);	// Force export 

	EXPORT_C CProtocolFamilyBase * InstallSIPDummy(void)
	//
	// Create a new protocol family
	//
		{
		return CDProtocolFamily::NewL();
		}
	}

CDProtocolFamily* CDProtocolFamily::NewL()
//
//
//
	{
	CDProtocolFamily* pf=new(ELeave) CDProtocolFamily();
	return pf;
	}

CDProtocolFamily::CDProtocolFamily() : CProtocolFamilyBase()
	{
	_LIT(KDummySIPProtocol, "SIPDummyProtocol");

	iProtocolDescs[0].iAddrFamily = KAFSip;
	iProtocolDescs[0].iSockType = KSockDatagram;
	iProtocolDescs[0].iProtocol = 1;	// dummy
	iProtocolDescs[0].iVersion = TVersion(0, 1, 0);	// dummy
	iProtocolDescs[0].iByteOrder = EBigEndian;
	iProtocolDescs[0].iServiceInfo = 0;
	iProtocolDescs[0].iSecurity = KSocketNoSecurity;
	iProtocolDescs[0].iMessageSize = 0x300;	// dummy
	iProtocolDescs[0].iName = KDummySIPProtocol;
	iProtocolDescs[0].iServiceTypeInfo = ESocketSupport|ECantProcessMBufChains;
	iProtocolDescs[0].iNamingServices = 0;
	iProtocolDescs[0].iNumSockets=1;
	}

TInt CDProtocolFamily::Install()
	{
	return KErrNone;
	}

TInt CDProtocolFamily::Remove()
	{
	return KErrNone;
	}

TUint CDProtocolFamily::ProtocolList(TServerProtocolDesc *& aProtocolDescPointer)
	{
	const TInt KNumProts = sizeof(iProtocolDescs) / sizeof(iProtocolDescs[0]);
  	aProtocolDescPointer=new TServerProtocolDesc[KNumProts];
  	if (!aProtocolDescPointer)
  		return 0;

	Mem::Copy(aProtocolDescPointer, iProtocolDescs, sizeof(TServerProtocolDesc)*KNumProts);

	return KNumProts;
	};

CProtocolBase * CDProtocolFamily::NewProtocolL(TUint /* aSockType */,TUint aProtocol)
	{
	ASSERT(aProtocol == 1);
	CDummyProtocol* p=CDummyProtocol::NewL(aProtocol, &iProtocolDescs[aProtocol-1]);	
	return p;
	}


//---------------------------------------------------------------------------------------------------------

//
CDummyProtocol::CDummyProtocol(TServerProtocolDesc* aProtoDesc)
//
//
//
:CProtocolBase(),iProtoDesc(aProtoDesc),iProtocols(0x16)
	{
	}

CDummyProtocol *CDummyProtocol::NewL(TInt /*aType*/, TServerProtocolDesc* aProtoDesc)
	{
	return new(ELeave) CDummyProtocol(aProtoDesc);
	}

CServProviderBase * CDummyProtocol::NewSAPL(TUint /*aProtocol*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}


CHostResolvProvdBase* CDummyProtocol::NewHostResolverL()
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CServiceResolvProvdBase* CDummyProtocol::NewServiceResolverL()
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CNetDBProvdBase* CDummyProtocol::NewNetDatabaseL()
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CDummyProtocol::~CDummyProtocol()
	{
	}


void CDummyProtocol::InitL(TDesC& /*aTag*/)
	{
	}

void CDummyProtocol::BindL(CProtocolBase* /*aProtocol*/, TUint /*anId*/)
	{
	}

void CDummyProtocol::BindToL(CProtocolBase* /*aProtocol*/)
	{
	}


void CDummyProtocol::StartL()
	{
	}

TInt CDummyProtocol::Send(RMBufChain &,CProtocolBase* /*aSourceProtocol*/)
	{
	return 1;
	}

TInt CDummyProtocol::Send(TDes8 &, TSockAddr* /*to*/,TSockAddr* /*from*/,CProtocolBase* /*aSourceProtocol*/)
	{
	return 1;
	}

void CDummyProtocol::Process(RMBufChain &,CProtocolBase* /*aSourceProtocol*/)
	{
	}

void CDummyProtocol::Process(TDes8 & ,TSockAddr* /*from*/,TSockAddr* /*to*/,CProtocolBase* /*aSourceProtocol*/)
	{
	}

void CDummyProtocol::Identify(TServerProtocolDesc *aDesc) const
	{
	Mem::Copy(aDesc, iProtoDesc, sizeof(TServerProtocolDesc));
	}

TInt CDummyProtocol::GetOption(TUint /*level*/,TUint,TDes8&,CProtocolBase* /*aSourceProtocol*/)
	{
	return KErrNotSupported;
	}

TInt CDummyProtocol::SetOption(TUint /*level*/,TUint /*name*/,const TDesC8& /*option*/,CProtocolBase* /*aSourceProtocol*/)
	{
	return KErrNotSupported;
	}

void CDummyProtocol::Error(TInt /*anerror*/,CProtocolBase* /*aSourceProtocol*/)
	{
	}
