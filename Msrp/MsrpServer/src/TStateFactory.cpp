/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html."
* Initial Contributors:
* Nokia Corporation - initial contribution.
* Contributors:
*
* Description:
* MSRP Implementation
*
*/

#include "TStateFactory.h"

#define ADD_STATE_TO_ARRAY(state) \
    {\
    CleanupStack::PushL( state ); \
    iStateArray.InsertL( state, state->identity() ); \
    CleanupStack::Pop( state ); \
    }

CStateFactory* CStateFactory::NewL()
    {
    CStateFactory* factory = CStateFactory::NewLC();
    CleanupStack::Pop( factory );       
    return factory;
    }

CStateFactory* CStateFactory::NewLC()
    {
    CStateFactory* factory = new ( ELeave ) CStateFactory;
    CleanupStack::PushL( factory );
    factory->ConstructL();
    return factory;       
    }

void CStateFactory::ConstructL( )
    {
    TStateBase *state;

    state = new ( ELeave ) TStateIdle;
    ADD_STATE_TO_ARRAY( state );

    state = new ( ELeave ) TStateConnecting;
    ADD_STATE_TO_ARRAY( state );

    state = new ( ELeave ) TStateWaitForClient;
    ADD_STATE_TO_ARRAY( state );
    
    state = new ( ELeave ) TStateActive;
    ADD_STATE_TO_ARRAY( state );

    state = new ( ELeave ) TStateActiveSend;
    ADD_STATE_TO_ARRAY( state ); 
	
	state = new ( ELeave ) TStateFileShare;
    ADD_STATE_TO_ARRAY( state ); 
    
    state = new ( ELeave ) TStateError;
    ADD_STATE_TO_ARRAY( state );  
    }

CStateFactory::CStateFactory()
    {   
    }

CStateFactory::~CStateFactory()
    {    
    iStateArray.ResetAndDestroy();
    iStateArray.Close();    
    }
  
TStateBase* CStateFactory::getStateL(TStates aState)
    {    
    return iStateArray[aState];    
    }

