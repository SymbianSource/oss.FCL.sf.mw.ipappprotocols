// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CSIPClient.cpp
// Part of       : SIPClient
// Version       : SIP/6.0
//



#include "sipclient.h"
#include "RSIP.h"
#include "CSIPClientReceiver.h"
#include "sipclientconnection.h"


const TInt KSupportedSecurityMechanismsMaxLength = 300;
const TInt KNegotiatedSecurityMechanismMaxLength = 100;
const TInt KCredentialsBufExpandSize = 100;

// -----------------------------------------------------------------------------
// CSIPClient::NewL
// -----------------------------------------------------------------------------
//
CSIPClient* CSIPClient::NewL (const TUid& aUid, 
                              MSIPClientObserver& aSIPObserver)
	{
    CSIPClient* self = CSIPClient::NewLC (aUid,aSIPObserver);
    CleanupStack::Pop (self);
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPClient::NewLC
// -----------------------------------------------------------------------------
//
CSIPClient* CSIPClient::NewLC (const TUid& aUid, 
                               MSIPClientObserver& aSIPObserver)
	{
	CSIPClient* self = new(ELeave)CSIPClient;
    CleanupStack::PushL (self);
    self->ConstructL (aUid, aSIPObserver);
    return self;
	}

// -----------------------------------------------------------------------------
// CSIPClient::CSIPClient
// -----------------------------------------------------------------------------
//
CSIPClient::CSIPClient ()
	{
	}

// -----------------------------------------------------------------------------
// CSIPClient::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPClient::ConstructL (const TUid& aUid, MSIPClientObserver& aSIPObserver)
	{
	iSip = new(ELeave)RSIP;
    if (aUid.iUid == 0) 
        {
        // Allowing ProfileAgent to use a zero UID
	    User::LeaveIfError(iSip->Connect());
        }
    else
        {
        User::LeaveIfError(iSip->Connect(aUid));
        }
	iReceiver = new(ELeave)CSIPClientReceiver(*iSip,aSIPObserver);
	}

// -----------------------------------------------------------------------------
// CSIPClient::~CSIPClient
// -----------------------------------------------------------------------------
//
CSIPClient::~CSIPClient()
	{
	delete iReceiver;
	if (iSip)
        {
        iSip->Close();
        }
	delete iSip;

	for (TInt i=0; i < iConnections.Count(); ++i)
        {
        iConnections[i]->DetachClient();
        }    
    iConnections.Close();
	}

// -----------------------------------------------------------------------------
// CSIPClient::SupportedSecurityMechanismsL
// -----------------------------------------------------------------------------
//
CDesC8Array* CSIPClient::SupportedSecurityMechanismsL () const
    {
    HBufC8* buf = HBufC8::NewLC(KSupportedSecurityMechanismsMaxLength);
	TPtr8 bufPtr = buf->Des();
    TIpcArgs args(TIpcArgs::ENothing);
    args.Set (ESipItcArgAuthenticationMechanism,&bufPtr);
	
    TInt err = iSip->Send(ESipItcSupportedSecurityMechanisms,args);
	if (err != KErrNone)
        {
        User::Leave(err);
        }
    CDesC8ArraySeg* mechanisms = new(ELeave)CDesC8ArraySeg(1);
    CleanupStack::PushL(mechanisms);
	if (bufPtr.Length() > 0)
		{
	    RDesReadStream readStream(bufPtr);
	    readStream.PushL();
        TUint32 mechanismLength = readStream.ReadUint32L();
        while (mechanismLength > 0)
            {
	        HBufC8* mechanism = HBufC8::NewLC (mechanismLength);
		    TPtr8 mechanismPtr(mechanism->Des());
		    readStream.ReadL (mechanismPtr,mechanismLength);
            mechanisms->AppendL(mechanismPtr);
            CleanupStack::PopAndDestroy(mechanism);
            mechanismLength = readStream.ReadUint32L();
            }
	    readStream.Pop();
	    readStream.Close();
        }
    CleanupStack::Pop(mechanisms);
    CleanupStack::PopAndDestroy(buf);
	return mechanisms;
    }
    
// -----------------------------------------------------------------------------
// CSIPClient::NegotiatedSecurityMechanismL
// -----------------------------------------------------------------------------
//    
HBufC8* CSIPClient::NegotiatedSecurityMechanismL(const TDesC8& aHop)
	{
    HBufC8* mechanism = HBufC8::NewLC(KNegotiatedSecurityMechanismMaxLength);
	TPtr8 mechanismPtr = mechanism->Des();
    TIpcArgs args(TIpcArgs::ENothing);
    args.Set (ESipItcArgAuthenticationMechanism,&mechanismPtr);
    args.Set (ESipItcArgNextHop,&aHop);    
    TInt err = iSip->Send(ESipItcNegotiatedSecurityMechanism,args);
	if (err != KErrNone)
        {
        User::Leave(err);
        }    
    CleanupStack::Pop(mechanism);
    if (mechanism->Length() == 0)
    	{
    	delete mechanism;
    	mechanism = NULL;
    	}
    return mechanism;
	}

// -----------------------------------------------------------------------------
// CSIPClient::IsSigCompSupportedL
// -----------------------------------------------------------------------------
//
TBool CSIPClient::IsSigCompSupportedL () const
    {
    TIpcArgs args(TIpcArgs::ENothing);
    TInt err = iSip->Send(ESipItcIsSigCompSupported, args);
        
    if (!(err == KErrNone || err == KErrNotSupported))
        {
        User::Leave(err);
        }
    return (err == KErrNone);
    }

// -----------------------------------------------------------------------------
// CSIPClient::Connection
// -----------------------------------------------------------------------------
//
CSIPClientConnection* CSIPClient::Connection (TUint32 aIapId) const
    {
    TInt index = FindIndex (aIapId);
    if (index >= 0)
        {
        return iConnections[index];
        }
    return 0;
    }

// -----------------------------------------------------------------------------
// CSIPClient::SetSecurityHandlingL
// -----------------------------------------------------------------------------
//
void CSIPClient::SetSecurityHandlingL(TBool aEnabled)
    {
    TIpcArgs args(TIpcArgs::ENothing);
    args.Set(ESipItcArgSecurityHandling,aEnabled);        
    User::LeaveIfError(iSip->Send(ESipItcSetSecurityHandling,args));
    }
    
// -----------------------------------------------------------------------------
// CSIPClient::SetHttpDigestObserver
// -----------------------------------------------------------------------------
//    
TInt CSIPClient::SetHttpDigestObserver(
    MSIPHttpDigestChallengeObserver& aObserver)
    {   
    TInt err = 
        SendHttpDigestObserverTypeToServer(ESipRealmHttpDigestObserver);
    
    if (!err)
        {
        iReceiver->SetHttpDigestObserver(aObserver);
        }
    
    return err;
    }

// -----------------------------------------------------------------------------
// CSIPClient::SetHttpDigestObserver
// -----------------------------------------------------------------------------
//    
TInt CSIPClient::SetHttpDigestObserver(
    MSIPHttpDigestChallengeObserver2& aObserver)
    {   
    TInt err = 
        SendHttpDigestObserverTypeToServer(ESipResponseHttpDigestObserver);
    
    if (!err)
        {
        iReceiver->SetHttpDigestObserver(aObserver);
        }
    
    return err;
    }

// -----------------------------------------------------------------------------
// CSIPClient::RemoveHttpDigestObserver
// -----------------------------------------------------------------------------
//  
void CSIPClient::RemoveHttpDigestObserver()
    {
    SendHttpDigestObserverTypeToServer(ESipNoHttpDigestObserver);
    iReceiver->RemoveHttpDigestObserver();
    }
	
// -----------------------------------------------------------------------------
// CSIPClient::SetCredentialsL
// -----------------------------------------------------------------------------
//	
void CSIPClient::SetCredentialsL(TUint32 aRequestId,
                                 TUint32 aRefreshId,
                                 const TDesC8& aRealm,
                                 const TDesC8& aUsername,
                                 const TDesC8& aPasswd,
                                 const TDesC8& aOutboundProxy)
    {    
    TIpcArgs args(TIpcArgs::ENothing);  
    args.Set(ESipItcArgRealm, &aRealm);
    
    TSIPIds ids;
    ids.iRequestId = aRequestId;
    ids.iRefreshId = aRefreshId;
    TPckgBuf<TSIPIds> idsPckg(ids);
    args.Set(ESipItcArgIds, &idsPckg);

    CBufFlat* credentialsBuf = CBufFlat::NewL(KCredentialsBufExpandSize);
	CleanupStack::PushL(credentialsBuf);
	RBufWriteStream writeStream(*credentialsBuf,0);
	writeStream.PushL();
    writeStream.WriteUint32L(aUsername.Length());
    writeStream.WriteL(aUsername);
    writeStream.WriteUint32L(aPasswd.Length());
    writeStream.WriteL(aPasswd);
    TInt outboundProxyLength = aOutboundProxy.Length();
    writeStream.WriteUint32L(outboundProxyLength);
    if (outboundProxyLength > 0)
        {
        writeStream.WriteL(aOutboundProxy);
        writeStream.WriteUint32L(0); // end of credentials
        }
    writeStream.Pop();
    writeStream.Close();
    TPtr8 credentialsPtr(credentialsBuf->Ptr(0));
    args.Set(ESipItcArgCredentials, &credentialsPtr);

    User::LeaveIfError(iSip->Send(ESipItcSetCredentials,args));

    CleanupStack::PopAndDestroy(credentialsBuf);
    }
			            
// -----------------------------------------------------------------------------
// CSIPClient::RemoveCredentials
// -----------------------------------------------------------------------------
//	
TInt CSIPClient::RemoveCredentials(const TDesC8& aRealm)
    {
    TIpcArgs args(TIpcArgs::ENothing);
    args.Set(ESipItcArgRealm, &aRealm);
    return iSip->Send(ESipItcRemoveCredentials,args);
    }
		
// -----------------------------------------------------------------------------
// CSIPClient::IgnoreChallenge
// -----------------------------------------------------------------------------
//	
TInt CSIPClient::IgnoreChallenge(TUint32 aRequestId,
                                 TUint32 aRefreshId,
                                 const TDesC8& aRealm)
    {
    TIpcArgs args(TIpcArgs::ENothing);
    args.Set(ESipItcArgRealm, &aRealm);
    
    TSIPIds ids;
    ids.iRequestId = aRequestId;
    ids.iRefreshId = aRefreshId;
    TPckgBuf<TSIPIds> idsPckg(ids);
    args.Set(ESipItcArgIds, &idsPckg);
    
    return iSip->Send(ESipItcIgnoreChallenge,args);
    }

// -----------------------------------------------------------------------------
// CSIPClient::AddL
// -----------------------------------------------------------------------------
//
void CSIPClient::AddL (CSIPClientConnection* aConnection)
    {
    __ASSERT_ALWAYS (aConnection, User::Leave(KErrArgument)); 
    TInt index = FindIndex (aConnection->IapId());
    if (index >= 0)
        {
        User::Leave (KErrAlreadyExists);
        }
    User::LeaveIfError (iConnections.Append(aConnection));
    }
    
// -----------------------------------------------------------------------------
// CSIPClient::Remove
// -----------------------------------------------------------------------------
//
TInt CSIPClient::Remove (const CSIPClientConnection* aConnection)
    {
    if (aConnection == 0)
        {
        return KErrArgument;
        }
    TInt index = FindIndex (aConnection->IapId());
    if (index >= 0)
        {
        iConnections.Remove(index);
        return KErrNone;
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CSIPClient::FindIndex
// -----------------------------------------------------------------------------
//
TInt CSIPClient::FindIndex (TUint32 aIapId) const   
    {
    for (TInt i=0; i < iConnections.Count(); i++)
        {
        if (iConnections[i]->IapId() == aIapId)
            {
            return i;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CSIPClient::SendHttpDigestObserverTypeToServer
// -----------------------------------------------------------------------------
//
TInt CSIPClient::SendHttpDigestObserverTypeToServer(
    const TSipHttpDigestObserverType& aObserverType)
    {
    TIpcArgs args(TIpcArgs::ENothing);
    args.Set(ESipItcArgHttpDigestObserverType,aObserverType);        
    return iSip->Send(ESipItcSetHttpDigestObserver,args);
    }

// -----------------------------------------------------------------------------
// CSIPClient::SIP
// -----------------------------------------------------------------------------
//
RSIP& CSIPClient::SIP ()
    {
    return *iSip;
    }
