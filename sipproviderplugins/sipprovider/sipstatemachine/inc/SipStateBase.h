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
// CSipStateBase definition file
// 
//

/**
 @file
 @internalComponent
*/
 
#ifndef SIP_STATE_BASE_H
#define SIP_STATE_BASE_H

#include "siphlcommontypes.h"

class CSipStateMachine;

class CSipStateBase : public CBase
/**
This class will hold the base functionality required for a SIP state
to have in it. Every state implementation should be derived from this
class and implement the virtuals

@internalComponent
@released since v9.2
*/
	{
public:
	/**
	The constructor of the class. 
	
	@param aStateMachine Handle to the state machine on which the state belongs
	*/
    CSipStateBase( CSipStateMachine* aStateMachine, TSipHLConsts::SIP_STATES aCurrState ) :
       iSipSM(aStateMachine), iCurrState(aCurrState)
    {
    }
  	/**
	The implemented version of the function will get called in case of a request
	comes from the client. i.e the entity which instantiated the state machine. 
	
	@param aRequestStatus To be set when the processing is complete
	*/
    virtual CSipStateBase * ProcessClientL(TRequestStatus& aStatus) = 0;
    
    /**
	The implemented version of the function will get called in case of a request
	comes from the server. i.e TE. 
	
	@param aRequestStatus To be set when the processing is complete
	*/
    virtual CSipStateBase * ProcessServerL(TRequestStatus& aStatus) = 0;
    
    /**
	The implemented version of the function can either be called from the client
	ore the TE
	
	@param aRequestStatus To be set when the processing is complete
	*/
    virtual CSipStateBase * ProcessServerErrorL(TRequestStatus & aStatus) = 0;
    
	/**
	The function will set what the client wants the state to do
	
	@param aRequest Request that are supported
	*/
    void SetClientRequest(TSipHLConsts::SIP_REQUESTS aRequest);
    
    /**
	The function will set what the server wants the state to do
	
	@param aRequest Request that are supported
	*/
    void SetServerRequest(TSipHLConsts::SIP_REQUESTS aRequest);

    /**
	The function will set what the client wants the state to do in
	response of a request
	
	@param aResponse Response that are supported
	*/
    void SetClientResponse(TSipHLConsts::SIP_RESPONSES aResponse);
    
    /**
	The function will set what the Server wants the state to do in
	response of a request
	
	@param aResponse Response that are supported
	*/
    void SetServerResponse(TSipHLConsts::SIP_RESPONSES aResponse);
	
	/**
	This function will Get the current state name 
	*/
	TSipHLConsts::SIP_STATES GetStateId();
	
	/**
	This function will reset all the request and response to None
	*/
    void ResetRequestResponse();
    
protected:
	// handle to the Next state
    CSipStateBase* 		iNext; 	
    CSipStateMachine*   		iSipSM;
    TChar						iReqIdentifier;
    TSipHLConsts::SIP_REQUESTS	iClientRequest;
    TSipHLConsts::SIP_REQUESTS	iServerRequest;
    TSipHLConsts::SIP_RESPONSES	iClientResponse;
    TSipHLConsts::SIP_RESPONSES	iServerResponse;
    TSipHLConsts::SIP_STATES	iCurrState;
	};


inline void CSipStateBase::SetClientRequest(TSipHLConsts::SIP_REQUESTS aRequest)
/**
 */
 	{
 	iReqIdentifier = 1;
 	iClientRequest = aRequest;	
 	}	
inline void CSipStateBase::SetServerRequest(TSipHLConsts::SIP_REQUESTS aRequest)
/**
 */
 	{
 	iReqIdentifier = 2;
 	iServerRequest = aRequest;	
 	}

inline void CSipStateBase::SetClientResponse(TSipHLConsts::SIP_RESPONSES aResponse)
/**
 */
 	{
 	iReqIdentifier = 3;
 	iClientResponse = aResponse;	
 	}
inline void CSipStateBase::SetServerResponse(TSipHLConsts::SIP_RESPONSES aResponse)
/**
 */
 	{
 	iReqIdentifier = 4;
 	iServerResponse = aResponse;	
 	}
	
inline void CSipStateBase::ResetRequestResponse()
/**
 */
 	{
 	iReqIdentifier = 0;
 	iClientRequest = iServerRequest = TSipHLConsts::ERequestNone;
 	iClientResponse = iServerResponse = TSipHLConsts::EResponseNone;	
 	}

inline TSipHLConsts::SIP_STATES CSipStateBase::GetStateId()
/**
 */
	{
	return iCurrState;	
	}
	




#endif
