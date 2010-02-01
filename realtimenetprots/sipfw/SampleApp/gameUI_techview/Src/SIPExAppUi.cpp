
// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// INCLUDE FILES
//



#include    "SIPExAppUi.h"
#include    "SIPExGameContainer.h"
#include    "SIPEx.hrh"
#include    "SIPExAddrQueryDlg.h"
#include    "SIPExProfileQueryDlg.h"
#include    "SIPExIMDialog.h"
#include    "SIPExDoc.h"
#include    <sipex.rsg>
#include    <eikenv.h>          // CEikonEnv
#include    <eikmenup.h>        // CEikMenupane
#include    <eikbtgpc.h>        // CEikButtonGroupContainer
#include    <eikinfo.h>         // CEikInfoDialog

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPExAppUi::CSIPExAppUi
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIPExAppUi::CSIPExAppUi()
: iState( CSIPExEngine::EIdle )
    {
    }

// -----------------------------------------------------------------------------
// CSIPExAppUi::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSIPExAppUi::ConstructL()
    {
    BaseConstructL();

    iEngine = ( static_cast< CSIPExDoc* >( Document() ) )->CreateEngineL( *this );
    iGameContainer = CSIPExGameContainer::NewL( ClientRect(), *iEngine );
    iEngine->SetViewNotifier( *iGameContainer );

    RegisterViewL( *iGameContainer );
    SetDefaultViewL( *iGameContainer );

    HandleToolbar();
    
    AddToStackL( iGameContainer );
    }

// -----------------------------------------------------------------------------
// CSIPExAppUi::~CSIPExAppUi
// Destructor
// -----------------------------------------------------------------------------
//
CSIPExAppUi::~CSIPExAppUi()
    {
    if( iGameContainer )
        {
        DeregisterView( *iGameContainer );
        iEikonEnv->RemoveFromStack( iGameContainer );
        delete iGameContainer;
        }
    }

// -----------------------------------------------------------------------------
// CSIPExAppUi::HandleCommandL
// User interaction handling. The views capture most of the events, but some
// might get pushed here
// -----------------------------------------------------------------------------
//
void CSIPExAppUi::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EEikCmdExit:
            {
            // If game is ongoing, notify opponent, ignore errors
            TRAPD( err, iEngine->EndGameL() );
            CBaActiveScheduler::Exit();
            break;
            }

        case ECmdInviteForGame:
            {
            CSIPExAddrQueryDlg* dlg = CSIPExAddrQueryDlg::NewL( iAddress );
            if( dlg->ExecuteLD( R_INVITE_ADDRESS_DLG ) )
                {
                iEngine->InviteL( iAddress );
                }
            iGameContainer->DrawNow();
            break;
            }

        case ECmdEnableProfile:
            {
           	TRAPD( err, iEngine->EnableProfileL());
            if (err == KErrNotFound)
            	{
            	CSIPExProfileQueryDlg* dlg = CSIPExProfileQueryDlg::NewL( );
            	if( dlg->ExecuteLD( R_PROFILE_DLG ) )
	                {
	                // iEngine->InviteL( iAddress );
	                }
	            iGameContainer->DrawNow();
            	iEngine->EnableProfileL();
	            }
            else
            	{
		    	User::Leave(err);
                }
            break;
            }   

        case ECmdDisableProfile:
            {
            iEngine->DisableProfileL();
            break;
            }  

        // Send instant message
        case ECmdSendIM:
            {
            TBuf< 256 > msg;
            CSIPExIMDialog* dlg = CSIPExIMDialog::NewL( iAddress, msg );
            if( dlg->ExecuteLD( R_INSTANT_MESSAGE_DLG ) )
                {
                iEngine->SendInstantMsgL( iAddress, msg );
                }
            iGameContainer->DrawNow();
            break;
            }  

        // End game
        case ECmdEndGame:
            {
            iEngine->EndGameL();
            break;
            }

        default:
            {
            // Do nothing, no-one will handle it
            // No use of leaving though, because this isn't an error - only
            // an unhandled command
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CSIPExAppUi::GameStateChanged
// From MSIPExGameObserver.
// Game state changed. Then the game should be redraw.
// -----------------------------------------------------------------------------
//
void CSIPExAppUi::GameStateChanged( CSIPExEngine::TEngineState aState )
    {
    iState = aState;
    HandleToolbar();
    iGameContainer->DrawDeferred();
    }
    
// -----------------------------------------------------------------------------
// CSIPExAppUi::HandleToolbar
// Sets the toolbar items dimmed/undimmed according game state.
// -----------------------------------------------------------------------------
//
void CSIPExAppUi::HandleToolbar()
    {
    CEikButtonGroupContainer* toolbar = iEikonEnv->AppUiFactory()->ToolBar();
    
    switch( iState )
        {
        case CSIPExEngine::EIdle:
            toolbar->DimCommand( ECmdInviteForGame, ETrue );
            toolbar->DimCommand( ECmdEnableProfile, EFalse );
            toolbar->DimCommand( ECmdDisableProfile, ETrue );
            toolbar->DimCommand( ECmdSendIM, ETrue );
            toolbar->DimCommand( ECmdEndGame, ETrue );
            break;
        case CSIPExEngine::EEnabled:
            toolbar->DimCommand( ECmdInviteForGame, EFalse );
            toolbar->DimCommand( ECmdEnableProfile, ETrue );
            toolbar->DimCommand( ECmdDisableProfile, EFalse );
            toolbar->DimCommand( ECmdSendIM, EFalse );
            toolbar->DimCommand( ECmdEndGame, ETrue );
            break;
        case CSIPExEngine::EActivating:
            toolbar->DimCommand( ECmdInviteForGame, ETrue );
            toolbar->DimCommand( ECmdEnableProfile, ETrue );
            toolbar->DimCommand( ECmdDisableProfile, ETrue );
            toolbar->DimCommand( ECmdSendIM, EFalse );
            toolbar->DimCommand( ECmdEndGame, ETrue );
            break;
        case CSIPExEngine::EActive:
            toolbar->DimCommand( ECmdInviteForGame, ETrue );
            toolbar->DimCommand( ECmdEnableProfile, ETrue );
            toolbar->DimCommand( ECmdDisableProfile, ETrue );
            toolbar->DimCommand( ECmdSendIM, EFalse );
            toolbar->DimCommand( ECmdEndGame, EFalse );
            break;
        default:
            break;
        }
    toolbar->DrawDeferred();
    }

// -----------------------------------------------------------------------------
// CSIPExAppUi::AcceptInvitationL
// From MSIPExGameObserver.
// Show confirmation dialog with summoner's address.
// If accepted return ETrue else EFalse.
// -----------------------------------------------------------------------------
//
TBool CSIPExAppUi::AcceptInvitationL( const TDesC& aFrom )
    {
    HBufC* title = 
        iEikonEnv->AllocReadResourceLC( R_SIP_EX_ACCEPT_INVITATION_DLG_TITLE );
    
    HBufC* temp = iEikonEnv->AllocReadResourceLC( R_SIP_EX_ACCEPT_INVITATION );
    
    HBufC* buf = HBufC::NewL( temp->Length() + aFrom.Length() );
    TPtr pbuf( buf->Des() );
    pbuf.AppendFormat( *temp, &aFrom );
    
    CleanupStack::PopAndDestroy( temp );
    CleanupStack::PushL( buf );
    
    TBool retVal( EFalse );
    if( CEikInfoDialog::RunDlgLD( *title, pbuf, CEikInfoDialog::EAllowEnter ) )
        {
        retVal = ETrue;
        }

    CleanupStack::PopAndDestroy( buf );
    CleanupStack::PopAndDestroy( title );
    
    iGameContainer->DrawDeferred();
    return retVal;
    }

// -----------------------------------------------------------------------------
// CSIPExAppUi::IMReceivedL
// From MSIPExGameObserver.
// Show the received instant message in info editor.
// -----------------------------------------------------------------------------
//
void CSIPExAppUi::IMReceivedL( const TDesC8& aFrom, const TDesC8& aMsg )
    {
    HBufC* from = HBufC::NewLC( aFrom.Length() );
    from->Des().Copy( aFrom );

    HBufC* msg = HBufC::NewLC( aMsg.Length() );
    msg->Des().Copy( aMsg );
    
    HBufC* temp = iEikonEnv->AllocReadResourceLC( R_SIP_EX_IM_RECEIVED );
    
    HBufC* buf = 
            HBufC::NewLC( temp->Length() + aFrom.Length() + aMsg.Length() );
    TPtr pbuf( buf->Des() );
    pbuf.AppendFormat( *temp, from, msg );

    iGameContainer->ShowInfoL( pbuf );
    CleanupStack::PopAndDestroy( 4 );
    }

//  End of File
