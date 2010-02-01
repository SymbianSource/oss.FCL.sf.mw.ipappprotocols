/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : MMsgAssemblerContext.h
* Part of     : ConnectionMgr
* Version     : SIP/3.0
*
*/




/**
 @internalComponent
*/
#ifndef __MMSGASSEMBLERCONTEXT_H__
#define __MMSGASSEMBLERCONTEXT_H__

// INCLUDES

// FORWARD DECLARATIONS
class MSIPMsgAssemblerOwner;
class MSigCompController;
class CSIPMessage;
class TTCPMsgState;

// CLASS DEFINITION
/**
 * defines a interface of CSIPMessageAssembler used for state classes.
 */
class MMsgAssemblerContext
	{
	public: // Enumerations

		// the states of handling received TCP messages.
		enum TTCPMsgStateValue
			{
			EMsgInit,
			EMsgHeaderStart,
			EMsgHeaderEnd,
			EMsgContentStart,
			EMsgContentEnd,
			ECompMsgStart,
			ECompMsgEnd,
			EMsgComplete,
			EMsgMaxStates
			};

	public:	// Constructors and destructor

		/// Virtual destructor
		virtual ~MMsgAssemblerContext() {};

	public: // Abstract methods

		virtual MSIPMsgAssemblerOwner& MsgOwner() = 0;
		
		virtual MSigCompController& SigComp() = 0;
		
		virtual CBufFlat& MsgBuffer() = 0;
		
        virtual CSIPMessage* Message() = 0;
        
        virtual void SetMessage( CSIPMessage* aMessage ) = 0;
        
        virtual void DetachMsg() = 0;
        
        virtual void ChangeState( MMsgAssemblerContext::TTCPMsgStateValue aState ) = 0;
        
        virtual TTCPMsgState& CurrentState() = 0;
        
        virtual void SetIsSigComp( TBool aIsSigComp ) = 0;
    
        virtual TBool IsSigComp() const = 0;
        
        virtual void SetUnConsumedBytes( TInt aUnConsumedBytes ) = 0;
    
        virtual TInt UnConsumedBytes() const = 0;
	};

#endif // end of __MMSGASSEMBLERCONTEXT_H__

// end of file

