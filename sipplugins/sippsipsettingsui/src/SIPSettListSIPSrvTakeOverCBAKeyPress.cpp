/*
* Copyright (c) 2003 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Catches right soft key press
*
*/


// INCLUDE FILES

#include "SIPSettListSIPSrvTakeOverCBAKeyPress.h"
#include <coedef.h>
#include <eikenv.h>
#include <coeaui.h>
#include "gssippluginlogger.h"

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvTakeOverCBAKeyPress::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
// 
CSIPSettListSIPSrvTakeOverCBAKeyPress* 
		CSIPSettListSIPSrvTakeOverCBAKeyPress::NewL()
    {
    __GSLOGSTRING("CSIPSettListSIPSrvTakeOverCBAKeyPress::NewL" )
    CSIPSettListSIPSrvTakeOverCBAKeyPress* self =
        CSIPSettListSIPSrvTakeOverCBAKeyPress::NewLC();
	CleanupStack::Pop( self );
    return self;
    } 

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvTakeOverCBAKeyPress::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIPSettListSIPSrvTakeOverCBAKeyPress* 
		CSIPSettListSIPSrvTakeOverCBAKeyPress::NewLC()
    {
    __GSLOGSTRING("CSIPSettListSIPSrvTakeOverCBAKeyPress::NewLC" )
    CSIPSettListSIPSrvTakeOverCBAKeyPress* self =
        new ( ELeave ) CSIPSettListSIPSrvTakeOverCBAKeyPress();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
// ----------------------------------------------------------------------------
// CSIPSettListSIPSrvTakeOverCBAKeyPress::CSIPSettListSIPSrvTakeOverCBAKeyPress
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CSIPSettListSIPSrvTakeOverCBAKeyPress::CSIPSettListSIPSrvTakeOverCBAKeyPress()
	: iBackCalled(EFalse)
	{
    __GSLOGSTRING("CSIPSettListSIPSrvTakeOverCBAKeyPress::CSIPSettListSIPSrvTakeOverCBAKeyPress" )
	}

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvTakeOverCBAKeyPress::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSIPSettListSIPSrvTakeOverCBAKeyPress::ConstructL()
	{
    __GSLOGSTRING("CSIPSettListSIPSrvTakeOverCBAKeyPress::ConstructL" )
    CCoeEnv::Static()->AppUi()->AddToStackL( this, ECoeStackPriorityCba, 
    										 ECoeStackFlagRefusesFocus );	
	this->SetFocusing( EFalse );
	}

// Destructor
CSIPSettListSIPSrvTakeOverCBAKeyPress::~CSIPSettListSIPSrvTakeOverCBAKeyPress()
	{
    __GSLOGSTRING("CSIPSettListSIPSrvTakeOverCBAKeyPress::~CSIPSettListSIPSrvTakeOverCBAKeyPress" )
    CCoeEnv::Static()->AppUi()->RemoveFromStack( this );
	}

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvTakeOverCBAKeyPress::OfferKeyEventL
// Handles the key events, pushes them to item list
// -----------------------------------------------------------------------------
//
TKeyResponse CSIPSettListSIPSrvTakeOverCBAKeyPress::OfferKeyEventL
			(const TKeyEvent &aKeyEvent, TEventCode aType)
	{
    __GSLOGSTRING("CSIPSettListSIPSrvTakeOverCBAKeyPress::OfferKeyEventL" )
	if ( aType == EEventKeyDown && aKeyEvent.iScanCode == EStdKeyDevice1 )
		{
		iBackCalled = ETrue;
		}
		 	
	return EKeyWasNotConsumed;
	}

// -----------------------------------------------------------------------------
// CSIPSettListSIPSrvTakeOverCBAKeyPress::BackCalledL
// Returns true, if right soft key is pressed
// -----------------------------------------------------------------------------
//	
TBool CSIPSettListSIPSrvTakeOverCBAKeyPress::BackCalled()
	{
	return iBackCalled;
	}
