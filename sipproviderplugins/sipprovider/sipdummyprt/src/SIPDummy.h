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

#ifndef SIPDUMMY_H
#define SIPDUMMY_H

#include <sipconnpref.h>

//---------------------------------------------------------------------------------------------------------

NONSHARABLE_CLASS(CDProtocolFamily): public CProtocolFamilyBase
	{
private:
	CDProtocolFamily();	
public:
	static CDProtocolFamily * NewL();
	TInt Install();
	TInt Remove();
	CProtocolBase * NewProtocolL(TUint aSockType,TUint aProtocol);
	TUint ProtocolList(TServerProtocolDesc *& aProtocolList);
private:
	TServerProtocolDesc iProtocolDescs[1];
	};

//---------------------------------------------------------------------------------------------------------

NONSHARABLE_CLASS(CDummyProtocol): public CProtocolBase
	{
public:
	CDummyProtocol(TServerProtocolDesc* aProtoDesc);
	~CDummyProtocol();
	static CDummyProtocol *NewL(TInt aProtocol, TServerProtocolDesc* aProtoDesc);
	CServProviderBase *NewSAPL(TUint aProtocol);
	virtual CHostResolvProvdBase *NewHostResolverL();
	virtual CServiceResolvProvdBase *NewServiceResolverL();
	virtual CNetDBProvdBase* NewNetDatabaseL();
	TBool CanCreateSockets();
	void InitL(TDesC& aTag);
	void BindL(CProtocolBase *aProtocol, TUint anId);
	virtual void BindToL(CProtocolBase *protocol);
	void StartL(void);
	TInt Send(RMBufChain &,CProtocolBase* aSourceProtocol);
	void Process(RMBufChain &,CProtocolBase* aSourceProtocol);
	TInt Send(TDes8 &, TSockAddr *to,TSockAddr *from,CProtocolBase* aSourceProtocol);
	void Process(TDes8 & ,TSockAddr *from,TSockAddr *to,CProtocolBase* aSourceProtocol);
	void Identify(TServerProtocolDesc *)const;
	TInt GetOption(TUint level,TUint name,TDes8 &anOption,CProtocolBase* aSourceProtocol);
	TInt SetOption(TUint level,TUint name,const TDesC8& option,CProtocolBase* aSourceProtocol);
	void Error(TInt anError,CProtocolBase* aSourceProtocol);
	TBool IsStarted(){return iIsStarted;}
protected:
	TInt iType;
	TServerProtocolDesc* iProtoDesc;
	TBool iIsStarted;
	CArrayFixFlat<CProtocolBase *> iProtocols;
	};





#endif //SIPDUMMY_H
