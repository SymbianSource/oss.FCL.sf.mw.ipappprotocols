/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSipDialogItem.h
* Part of       : SIPDialogs
* Version       : SIP/6.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPDIALOGITEM_H
#define CSIPDIALOGITEM_H

#include <e32base.h>
#include <stringpool.h>
#include "SipStackServerDefs.h"
#include "MTransactionOwner.h"
#include "MRefreshOwner.h"
#include "sip.h"
#include "MDialogContext.h"
#include "MSIPSecUser.h"
#include "DialogStateBase.h"
#include "Establishing.h"
#include "Early.h"
#include "Confirmed.h"
#include "Terminated.h"

class MSipRefreshMgr;
class MTransactionUser;
class CSIPSec;
class MDialogOwner;
class CDeleteMgr;
class CDialogsStore;
class CTransactionItemStore;
class CSIPMessage;
class CSIPRouteHeaderBase;
class CSIPRouteHeader;
class CSIPCallIDHeader;
class CSIPContactHeader;
class CURIContainer;
class CSIPURI;
class CSIPAddress;
class CSigCompMgr;
class MSigComp;


class CSipDialogItem : public CBase,
                       public MDialogContext,
                       public MTransactionOwner,
                       public MRefreshOwner,
                       public MSIPSecUser
                       
    {
public: // Constructors and destructor

    static CSipDialogItem* NewLC(
        CDialogsStore& aDialogStore,
        MDialogOwner* aDialogOwner,
        MSIPSecUser& aSIPSecUser,
        MTransactionUser& aTransactionMgr, 
        MSipRefreshMgr& aRefreshMgr,
        CSIPSec& aSIPSec,
        CDeleteMgr& aDeleteMgr,
        MSigComp& aSigComp,
        TRegistrationId aRegistrationId=KEmptyRegistrationId);

    ~CSipDialogItem();
    
    
public: // From MDialogContext
    
    MDialogOwner* DialogOwner();
    
    TDialogId DialogId() const;
    
    TRegistrationId RegistrationId() const;
   
    void SelfDestruct();
    
    TTransactionId FirstTransactionId();
    
    void ChangeState(TDialogStateBase& aState);
    
    void TerminateForkedDialogs(TTransactionId aTransactionId);

    void SetCallId(CSIPCallIDHeader* aCallId);

    CSIPCallIDHeader* CallId();
    
    void SetLocalTag(RStringF aTag);
    
    RStringF LocalTag();

    void SetRemoteTagL(CSIPFromToHeaderBase* aFromTo);
    
    RStringF RemoteTag();
    
    TBool RemoteTagSet();
    
    void SetLocalSeqNum(TUint aNum);
    
    void SetRemoteTargetWithMsgL(CSIPMessage& aMsg);

    void SetRemoteSeqNum(TUint aNum);

    void SetRouteSetUsingRecordRouteL(CSIPMessage& aMessage,
                                      TBool aIsUAC,
                                      TInt aLeaveCode);

    TInt RemoveTransactionItem(TTransactionId& aTransactionId);
        
    TBool HasRouteSet() const;    
    
    void AddCallIdHeaderInDialogL(CSIPMessage& aMessage);
    
    void AddCSeqHeaderInDialogL(CSIPRequest& aRequest);

    
public: // From MDialogContext and MRefreshOwner

    TSIPTransportParams TransportParams(const TRegistrationId& aRegistrationId);    

    
public: // From MTransactionOwner

    void ReceiveL(TUint32 aIapId,
                  TTransactionId aTransaction,
                  CSIPRequest* aReq);
    
    void ReceiveL(TTransactionId aTransaction,
                  CSIPResponse* aMsg);
                  
    TInt TransactionEnded(TUint32 aIapId,
                          TTransactionId aTransaction,
                          TInt aReason);
                          
    TInt NextCSeq(TUint& aCSeq);
    
    const MSIPSecUser* SIPSecUser() const;
    

public: // From MRefreshOwner    
    
    void RefreshReceivedL (TTransactionId aTransactionId,
                           TRefreshId aRefreshId,
                           CSIPResponse* aSIPResponse);

    TInt RefreshError (TRefreshId aRefreshId,
                       TTransactionId aTransactionId,
                       TInt aReason);

    TInt NextRefreshCSeq (TUint& aCSeq);
    
    const MSIPSecUser* SIPSecUser(const TRegistrationId& aRegistrationId);
    
    TUint RefreshDurationL();

public: // From MSIPSecUser

	TBool PassOnlyRealmsToUser() const;
	
    void RequestCredentialsL(const TDesC8& aRealm);
	
    void RequestCredentialsL(CSIPResponse& aResponse,
	                         TTransactionId aTransactionId,
	                         TRefreshId aRefreshId);
    
    const MSIPSecUser* TrustedUser(TRegistrationId aRegistrationId);
    
    TBool ByPassSIPSec() const;
    

public: // New functions    
   
    TUint32 IapId();
   
    static TInt Offset();
 
    void ResetStateL();
 
    TBool IsEarly() const;

    CDeleteMgr& DeleteMgr();
    
    TBool Match(CSIPRequest& aRequest) const;
    
    static TBool ContactOk(const CSIPContactHeader& aContact);

    void SetFirstTransactionId(const TTransactionId& aTransactionId);
    
    CTransactionItemStore& TransactionItems();
    
    TBool HasTransactionItem(TTransactionId aTransactionId);

    TDialogStateBase& CurrentState();
    
    TUint LocalSeqNum();
    
    TUint RemoteSeqNum();

    void SetLocalAddressL(const CSIPAddress& aAddr);

    const CSIPAddress* LocalAddress();

    void SetRemoteAddressL(const CSIPAddress& aAddr);

    const CSIPAddress* RemoteAddress();
    
    void SetInitialRemoteTargetL(CURIContainer& aUri);
    
    void SetRemoteTargetL(CURIContainer& aUri);
    
    CURIContainer* RemoteTarget();

    void SetLocalTargetWithMsgL(CSIPMessage& aMsg,
                                TBool aUpdateSigCompParam=ETrue);
    
    CSIPContactHeader* LocalTarget();
    
    void SetRouteSetUsingRouteL(CSIPMessage& aMessage);
    
    void SetRouteSetL (RPointerArray<CSIPRouteHeaderBase>& aRoute);
    
    void UpdateLocalTargetL(CSIPMessage& aMessage);
    
    void AddRouteToMessageL(CSIPMessage& aMessage);
    
    void AddRecordRouteToMessageL(CSIPMessage& aMessage);
    
    void AddToHeaderInDialogL(CSIPRequest& aRequest);
    
    void AddFromHeaderInDialogL(CSIPRequest& aRequest);    
    
    void EndTransaction(TTransactionId aTransactionId, TInt aReason);


private: // New functions

    CSipDialogItem (CDialogsStore& aDialogStore,
                    MSIPSecUser& aSIPSecUser,
                    MTransactionUser& aTransactionMgr,
                    MSipRefreshMgr& aRefreshMgr,
                    CSIPSec& aSIPSec,
                    CDeleteMgr& aDeleteMgr,
                    MSigComp& aSigComp,
                    TRegistrationId aRegistrationId);

    void ConstructL (MDialogOwner* aDialogOwner);

    void UpdateSigCompL (CURIContainer& aUri);
    void SetLocalTargetL (CSIPContactHeader& aContact, 
                          TBool aUpdateSigCompParam=ETrue);
    void UpdateRemoteTagForRefreshesL (RStringF aRemoteTag);
    void SetRouteSet (RPointerArray<CSIPRouteHeader>* aRouteSet);
    TBool CheckResponse(CSIPResponse* aResponse);
    TBool CheckRequest(CSIPRequest* aRequest);
    TBool CheckContacts(CSIPMessage& aMessage, 
                        TBool aContactRequired=EFalse) const;
    void CheckContactsL(TTransactionId aTransactionId,
                        CSIPResponse& aResponse) const;
    CSipDialogItem* CreateForkingDialogLC();

    void UpdateTransportForNextHopAndLocalTargetL(
        CSIPContactHeader* aLocalContactInOutgoingRequest);

    CSIPURI& ExtractUriL(CSIPContactHeader* aContact);

    // Needed for cleanup of Route-set:
    static void CleanupRouteSet (TAny* aRouteSet);

private: // Data

    TSglQueLink iLink;
    TDialogId iDialogId;
    TRegistrationId iRegistrationId; // Used for filling the Contact hostport
    TTransactionId iFirstTransactionId;
    CDialogsStore& iDialogStore;
    MTransactionUser& iTU;
    MSipRefreshMgr& iRefreshMgr;
    CSIPSec& iSIPSec;
    CDeleteMgr& iDeleteMgr;
    MSigComp& iSigComp;
    MDialogOwner* iDialogOwner;
    MSIPSecUser& iSIPSecUser;
    CTransactionItemStore* iTransactionItemStore;
    CSigCompMgr* iSigCompMgr;
    RStringF iTagParamName;
    RStringF iTransportParamName;
    RStringF iContactHeaderName;

    // States
    TTerminated iTerminated;
    TConfirmed iConfirmed;
    TEarly iEarly;
    TEstablishing iEstablishing;
    TDialogStateBase* iCurrentState;

    // Dialog state data
    CSIPCallIDHeader* iCallId;
    RStringF iLocalTag;
    RStringF iRemoteTag;
    TBool iRemoteTagSet;
    TUint iLocalSeqNum;
    TUint iRemoteSeqNum;
    CSIPAddress* iLocalAddress;
    CSIPAddress* iRemoteAddress;
    CURIContainer* iRemoteTarget;
    CSIPContactHeader* iLocalTarget;
    RPointerArray<CSIPRouteHeader> iRouteSet;
    
private: // For testing purposes

    UNIT_TEST(CSipDialogItemTest)
    UNIT_TEST(CDialogStoreTest)
    UNIT_TEST(CSipDialogMgrTest)
    };

#endif // CSIPDIALOGITEM_H

// End of File
