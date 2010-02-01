/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CSIPMsgAssembler.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __CSIPMSGASSEMBLER_H__
#define __CSIPMSGASSEMBLER_H__

#include <e32base.h>
#include "MMsgAssemblerContext.h"


class CSIPMessage;
class MSIPMsgAssemblerOwner;
class MSigCompController;
class TTCPMsgState;

class CSIPMsgAssembler : public CBase, 
                         public MMsgAssemblerContext
	{
public:
	enum
		{
		EMsgBufferSize = 1000
		};
		
		
   /** 
	* Create a instance of CSIPMsgAssembler.
	* 
	* @ param aMsgOwner; a reference of MSIPMsgAssemblerOwner.
	* @ param aSigComp; Signalling compression interface
	*
    * @ return a CSIPMsgAssembler object.
	*/ 
	static CSIPMsgAssembler* NewL ( MSIPMsgAssemblerOwner& aMsgOwner,
		                            MSigCompController&    aSigComp );

   /** 
	* Create a instance of CSIPMsgAssembler and push it to cleanup stack.
	* 
	* @ param aMsgOwner; a reference of MSIPMsgAssemblerOwner.
	* @ param aSigComp; Signalling compression interface
	*
    * @ return a CSIPMsgAssembler object.
	*/ 
	static CSIPMsgAssembler* NewLC ( MSIPMsgAssemblerOwner& aMsgOwner,
		                             MSigCompController&    aSigComp );

  
	~CSIPMsgAssembler ();

   /**
    * Covert data to SIP message and send it to message owner.
	* 
	* @ param aData, data to be converted.
	* @ param aNextLength, next data length to be inputed.
	*/
	void InputL (HBufC8* aData, TUint& aNextLength);
	
	MSIPMsgAssemblerOwner& MsgOwner();
		
	MSigCompController& SigComp();
		
	CBufFlat& MsgBuffer();
	
	/**
    * Obtain a pointer to the stored SIP message.
	* 
	* @return SIP message, ownership is not transferred
	*/	
    CSIPMessage* Message();
    
    /**
    * Store aMessage and discard the previous message.
	* 
	* @param aMessage SIP message, ownership is transferred	
	*/
    void SetMessage( CSIPMessage* aMessage );
        
    void DetachMsg();
    
    void ChangeState( MMsgAssemblerContext::TTCPMsgStateValue aStateValue );
        
    TTCPMsgState& CurrentState();
    
    void SetIsSigComp( TBool aIsSigComp );
    
    TBool IsSigComp() const;
    
    void SetUnConsumedBytes( TInt aUnConsumedBytes );
    
    TInt UnConsumedBytes() const;

private:

	CSIPMsgAssembler (MSIPMsgAssemblerOwner& aMsgOwner,
		              MSigCompController&       aSigComp);

	void ConstructL ();
	
private:

	static void ResetState ( TAny* aMsgAssembler );

private:

	MSIPMsgAssemblerOwner& iMsgOwner;
	CBufFlat*              iMsgBuf;
	CSIPMessage*           iMessage;
	MSigCompController&    iSigComp;
	CArrayVarFlat< TTCPMsgState >           iStates;
	MMsgAssemblerContext::TTCPMsgStateValue iStateValue;
	TBool                  iIsSigComp;
	TInt                   iUnConsumedBytes;

private:

#ifdef CPPUNIT_TEST
	friend class CSIPMsgAssemblerTest;
#endif
	};

#endif // end of __CSIPMSGASSEMBLER_H__

// End of File
