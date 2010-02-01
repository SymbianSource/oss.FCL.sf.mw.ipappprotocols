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
// Name          : CIpSecSadbHandler.cpp
// Part of       : SIPSec
// Version       : SIP/4.0 
//



#include <pfkey_send.h>
#include <lib_pfkey.h>

#include "siprequest.h"
#include "sipresponse.h"
#include "sipsecurityclientheader.h"
#include "sipsecurityserverheader.h"
#include "sipsecurityverifyheader.h"

#include "CIpSecSadbHandler.h"
#include "MSipSecSaDbSendReceiveObserver.h"
#include "SipSecIpsecParams.h"
#include "SipSecUtils.h"
#include "CSipSecSaDbMsgSender.h"
#include "CSipSecSaDbMsgReceiver.h"
#include "MIpSecMechanismParams.h"
#include "MSipSecSadbObserver.h"

const TInt KOngoingAddsGranularity = 4;

// ----------------------------------------------------------------------------
// CIpSecSadbHandler::NewL
// ----------------------------------------------------------------------------
//
CIpSecSadbHandler* CIpSecSadbHandler::NewL( 
    MSipSecSadbObserver& aObserver, 
    RSADB& aSADB )
	{
   	CIpSecSadbHandler* self = new ( ELeave ) CIpSecSadbHandler( aObserver );
	CleanupStack::PushL( self );
	self->ConstructL( aSADB );
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CIpSecSadbHandler::ConstructL
// ----------------------------------------------------------------------------
//	
void CIpSecSadbHandler::ConstructL( RSADB& aSADB )
    {
   	iSaDbMsgSender = CSipSecSaDbMsgSender::NewL( *this, aSADB );
    }
   

// ----------------------------------------------------------------------------
// CIpSecSadbHandler::CIpSecSadbHandler
// ----------------------------------------------------------------------------
//	
CIpSecSadbHandler::CIpSecSadbHandler( MSipSecSadbObserver& aObserver ) : 
    iObserver( aObserver ),
    iState( EAvailable ),
    iOngoingAdds( KOngoingAddsGranularity )
	{
	}

// ----------------------------------------------------------------------------
// CIpSecSadbHandler::~CIpSecSadbHandler
// ----------------------------------------------------------------------------
//	
CIpSecSadbHandler::~CIpSecSadbHandler()
	{
	delete iSaDbMsgSender;
	iOngoingAdds.Reset();
	iOngoingAdds.Close();
	}

	
// ----------------------------------------------------------------------------
// CIpSecSadbHandler::AddSasL
// ----------------------------------------------------------------------------
//	
void CIpSecSadbHandler::AddSasL(
	MIpSecMechanismParams& aParams,
	TInetAddr aLocalAddress,
	TInetAddr aRemoteAddress,
	TSipSecSaParams& aSaParams )
	{
	IssueRequestL( aParams, 
	               aLocalAddress, 
	               aRemoteAddress, 
	               aSaParams );

    // Kick-start SA adding looping
    AddCompletedL( KErrNone );
	}
	
// ----------------------------------------------------------------------------
// CIpSecSadbHandler::DeleteSasL
// ----------------------------------------------------------------------------
//	
void CIpSecSadbHandler::DeleteSasL( 
	MIpSecMechanismParams& aParams,
	TInetAddr aLocalAddress,
	TInetAddr aRemoteAddress,
	TSipSecSaParams& aSaParams )
	{
	CancelAddSaOps();
	
	IssueRequestL( aParams, 
	               aLocalAddress, 
	               aRemoteAddress, 
	               aSaParams );
	
	// Kick-start SA deleting looping
    DeleteCompletedL();	
	}

// ----------------------------------------------------------------------------
// CIpSecSadbHandler::SetSaParamsL
// ----------------------------------------------------------------------------
//				
void CIpSecSadbHandler::SetSaParamsL( 
	TSipSecSaParams& aParams, 				     
	CSIPSecurityHeaderBase& aCliHeader, 
	CSIPSecurityHeaderBase& aSrvHeader,
	const TDesC8& aAuthKey )
	{
	TInt len = aAuthKey.Length();
	__ASSERT_ALWAYS( len == KAuthKeyFromSIMSize && len <= KAuthKeyMaxSize, 
	                 User::Leave( KErrArgument ) );
	                 
	aParams.iSpi_uc  = SipSecUtils::ValueL( aCliHeader, KSpic );
	aParams.iSpi_us  = SipSecUtils::ValueL( aCliHeader, KSpis );
	aParams.iPort_uc = SipSecUtils::ValueL( aCliHeader, KPortc );
	aParams.iPort_us = SipSecUtils::ValueL( aCliHeader, KPorts );
	aParams.iSpi_pc  = SipSecUtils::ValueL( aSrvHeader, KSpic );
	aParams.iSpi_ps  = SipSecUtils::ValueL( aSrvHeader, KSpis );
	aParams.iPort_pc = SipSecUtils::ValueL( aSrvHeader, KPortc );
	aParams.iPort_ps = SipSecUtils::ValueL( aSrvHeader, KPorts );
	
	RStringF v = SipSecUtils::ValueFL( aCliHeader, KAlgorithm );
	const TDesC8& alg = v.DesC();
	__ASSERT_ALWAYS( alg.Length(),User::Leave( KErrNotReady ) );	  
	if ( alg == KHmacMd596 )
	    {
	    aParams.iAuthAlg = SADB_AALG_MD5HMAC;
	    aParams.iAuthKey.Copy( aAuthKey );
	    }
	else
	    {
	    aParams.iAuthAlg = SADB_AALG_SHA1HMAC;
	    
	    // In hmac-sha-1-96 160 bit key has to be used, aAuthKey retrieved
	    // from SIM should be only 128 bits, pad with zeros from right
        const TUint KPadLength = 4;
        
        __ASSERT_ALWAYS( KAuthKeyFromSIMSize + KPadLength <= KAuthKeyMaxSize,
                         User::Leave( KErrOverflow ) );

	    TBuf8<KAuthKeyMaxSize> authkey;
	    authkey.Append( aAuthKey );
	    const TUint KPosKeyEnd = authkey.Length();
	    authkey.SetMax();
	    LittleEndian::Put32( &authkey[ KPosKeyEnd ], 0 );
	    aParams.iAuthKey.Copy( authkey );
	    }
	
	}

// ----------------------------------------------------------------------------
// CIpSecSadbHandler::CancelAddSAOps
// ----------------------------------------------------------------------------
//	
void CIpSecSadbHandler::CancelAddSaOps()
    {
    if ( iSaDbMsgSender->Type() == SADB_ADD )
        {
        iSaDbMsgSender->Cancel();
        Reset();
        }
    }
    
// ----------------------------------------------------------------------------
// CIpSecSadbHandler::SADBAddMsgReceived
// ----------------------------------------------------------------------------
//       
void CIpSecSadbHandler::SADBAddMsgReceived( TUint aMsgSeq, TInt aError )
    {
    TInt index = iOngoingAdds.FindInOrder( aMsgSeq );
    if ( index != KErrNotFound )
        {
        iOngoingAdds.Remove( index );
        iOngoingAdds.Compress();
        
        TBool aSuccess( !aError );
        
        TRAP_IGNORE( SAsAddedL( aSuccess ) )
        }
    }
    
// ----------------------------------------------------------------------------
// CIpSecSadbHandler::MessageSentL
// ----------------------------------------------------------------------------
//
void CIpSecSadbHandler::MessageSentL( TInt aStatus, TInt aMsgType )
	{
	if ( aMsgType == SADB_ADD )
	    {
	    AddCompletedL( aStatus );
	    }
	else
	    {
	    // Error is ignored since anyway we should try to delete existing SAs
	    DeleteCompletedL();
	    }
	}

// ----------------------------------------------------------------------------
// CIpSecSadbHandler::MessageReceived
// ----------------------------------------------------------------------------
//
void CIpSecSadbHandler::MessageReceived( TInt /* aStatus */ )
	{
	// Not used in this context
	}

// ----------------------------------------------------------------------------
// CIpSecSadbHandler::IssueRequestL
// ----------------------------------------------------------------------------
//
void CIpSecSadbHandler::IssueRequestL(
    MIpSecMechanismParams& aParams,
	TInetAddr& aLocalAddress,
	TInetAddr& aRemoteAddress,
	TSipSecSaParams& aSaParams )
    {
   	if ( !IsAvailable() )
	    {
	    User::Leave( KErrInUse );
	    }
	    
	// Remember parameters regarding current request
	iParams = &aParams;
	iSaParams = &aSaParams;
	
	iLocalAddress = aLocalAddress;
	iRemoteAddress = aRemoteAddress;
    }
 
// ----------------------------------------------------------------------------
// CIpSecSadbHandler::IsAvailable
// ----------------------------------------------------------------------------
//   
TBool CIpSecSadbHandler::IsAvailable()
    {
    return ( iState == EAvailable );
    }

// ----------------------------------------------------------------------------
// CIpSecSadbHandler::Reset
// ----------------------------------------------------------------------------
//    
void CIpSecSadbHandler::Reset()
    {
    iState = EAvailable;
    iOngoingAdds.Reset();
    iOngoingAdds.Compress();
    }

// ----------------------------------------------------------------------------
// CIpSecSadbHandler::AddCompletedL
// ----------------------------------------------------------------------------
//   
void CIpSecSadbHandler::AddCompletedL( TInt aError )
    {
    // No use to continue if failure occured
    if ( aError != KErrNone )
	    {
	    SAsAddedL( EFalse );
	    }
	
	switch ( iState )
	    {
	    case EAvailable:
	        {
	        // 1. Inbound  spi_us: port_us <- port_pc

             // Note: Family is KAfInet
        	iLocalAddress.SetPort( iSaParams->iPort_us );
        	iRemoteAddress.SetPort( iSaParams->iPort_pc );
	        AddSaL( iRemoteAddress, iLocalAddress, iSaParams->iSpi_us );
	        iState = EAddingInbound1;
	        break;
	        }
	    case EAddingInbound1:
	        {
	        // 2. Inbound  spi_uc: port_uc <- port_ps
	
	        iLocalAddress.SetPort( iSaParams->iPort_uc );
	        iRemoteAddress.SetPort( iSaParams->iPort_ps );
	        AddSaL(  iRemoteAddress, iLocalAddress, iSaParams->iSpi_uc );
	        iState = EAddingInbound2;
	        break;
	        }
	    case EAddingInbound2:
	        {
	        // 3. Outbound spi_ps: port_uc -> port_ps

	        iLocalAddress.SetPort( iSaParams->iPort_uc );
	        iRemoteAddress.SetPort( iSaParams->iPort_ps );
	        AddSaL( iLocalAddress, iRemoteAddress, iSaParams->iSpi_ps );
	        iState = EAddingOutbound1;
	        break;
	        }
	    case EAddingOutbound1:
	        {
	       	// 4. Outbound spi_pc: port_us -> port_pc
	
        	iLocalAddress.SetPort( iSaParams->iPort_us );
        	iRemoteAddress.SetPort( iSaParams->iPort_pc );
	        AddSaL( iLocalAddress, iRemoteAddress, iSaParams->iSpi_pc );
	        iState = EAddingOutbound2;
	        break;
	        }
	    case EAddingOutbound2:
	        {
	        iState = EAvailable;
	        SAsAddedL( ETrue );
	        break;
	        }
	    default:
	        {
	        break;
	        }
	    }
    }
 
// ----------------------------------------------------------------------------
// CIpSecSadbHandler::DeleteCompletedL
// ----------------------------------------------------------------------------
//      
void CIpSecSadbHandler::DeleteCompletedL()
    {
   	switch ( iState )
	    {
	    case EAvailable:
	        {
	        // 1. Inbound  spi_us: port_us <- port_pc

            // Note: Family is KAfInet
        	iLocalAddress.SetPort( iSaParams->iPort_us ); 
        	iRemoteAddress.SetPort( iSaParams->iPort_pc );
	        DeleteSaL( iRemoteAddress, iLocalAddress, iSaParams->iSpi_us );
	        iState = EDeletingInbound1;
	        break;
	        }
	    case EDeletingInbound1:
	        {
	        // 2. Inbound  spi_uc: port_uc <- port_ps
	
	        iLocalAddress.SetPort( iSaParams->iPort_uc );
	        iRemoteAddress.SetPort( iSaParams->iPort_ps );
	        DeleteSaL( iRemoteAddress, iLocalAddress, iSaParams->iSpi_uc );
	        iState = EDeletingInbound2;
	        break;
	        }
	    case EDeletingInbound2:
	        {
	        // 3. Outbound spi_ps: port_uc -> port_ps

	        iLocalAddress.SetPort( iSaParams->iPort_uc );
	        iRemoteAddress.SetPort( iSaParams->iPort_ps );
	        DeleteSaL( iLocalAddress, iRemoteAddress, iSaParams->iSpi_ps );
	        iState = EDeletingOutbound1;
	        break;
	        }
	    case EDeletingOutbound1:
	        {
	       	// 4. Outbound spi_pc: port_us -> port_pc
	
        	iLocalAddress.SetPort( iSaParams->iPort_us );
        	iRemoteAddress.SetPort( iSaParams->iPort_pc );
	        DeleteSaL( iLocalAddress, iRemoteAddress, iSaParams->iSpi_pc );
	        iState = EDeletingOutbound2;
	        break;
	        }
	    case EDeletingOutbound2:
	        {
	        // All done, inform observer.
	        // Might lead to destruction of this handler.
	        Reset();
	        iObserver.SAsDeletedL();
	        break;
	        }
	    default:
	        {
	        break;
	        }
	    }
    }
    
// ----------------------------------------------------------------------------
// CIpSecSadbHandler::AddSaL
// ----------------------------------------------------------------------------
//	
void CIpSecSadbHandler::AddSaL( 
    TInetAddr& aSourceAddress,
    TInetAddr& aDestinationAddress,
    TUint aSpi )
	{
	TUint seqNum( iParams->SeqNumber() );
	
	// Store sequence numbers of add operations
	User::LeaveIfError( iOngoingAdds.Append( seqNum ) );
	
	TPfkeySendMsg*
		msg = new ( ELeave ) TPfkeySendMsg( SADB_ADD, SADB_SATYPE_ESP, 
		   							  	    seqNum, PID );
	CleanupStack::PushL( msg );
	
	// SPI has to be converted to network byte order
	TUint32 spi = ConvertToBigEndian( aSpi );
	
	msg->Add( Int2Type<SADB_EXT_SA>(), spi, iSaParams->iAuthAlg, SADB_EALG_NULL, 
			  SADB_SASTATE_MATURE, 0,0 ); 
	msg->Add( Int2Type<SADB_EXT_ADDRESS_SRC>(), aSourceAddress, 0, 0 );
	msg->Add( Int2Type<SADB_EXT_ADDRESS_DST>(), aDestinationAddress, 0, 0 );	
	msg->Add( Int2Type<SADB_EXT_KEY_AUTH>(), iSaParams->iAuthKey, 0 );

	sadb_msg &m = msg->MsgHdr();
	m.sadb_msg_len = static_cast<TUint16>( msg->Length() / KWordLen );
	
	iSaDbMsgSender->SendL( msg, SADB_ADD );
	
	CleanupStack::Pop( msg ); // msg ownership was transferred
	}

	
// ----------------------------------------------------------------------------
// CIpSecSadbHandler::DeleteSaL
// ----------------------------------------------------------------------------
//				     
void CIpSecSadbHandler::DeleteSaL( 
    TInetAddr& aSourceAddress,
    TInetAddr& aDestinationAddress,
    TUint aSpi )
	{
	TPfkeySendMsg*
		msg = new ( ELeave ) TPfkeySendMsg( SADB_DELETE, SADB_SATYPE_ESP, 
		   							  	    iParams->SeqNumber(), PID );
	CleanupStack::PushL( msg );
	
    
    // SPI has to be converted to network byte order
	TUint32 spi = ConvertToBigEndian( aSpi );
	
	msg->Add( Int2Type<SADB_EXT_SA>(), spi, 0, 0, 
			  SADB_SASTATE_MATURE, 0,0 ); 
	msg->Add( Int2Type<SADB_EXT_ADDRESS_SRC>(), aSourceAddress, 0, 0 );
	msg->Add( Int2Type<SADB_EXT_ADDRESS_DST>(), aDestinationAddress, 0, 0 );
	
	sadb_msg &m = msg->MsgHdr();
	m.sadb_msg_len = static_cast<TUint16>( msg->Length() / KWordLen );
	
	iSaDbMsgSender->SendL( msg, SADB_DELETE );
	
	CleanupStack::Pop( msg ); // msg ownership was transferred	
	}

// ----------------------------------------------------------------------------
// CIpSecSadbHandler::SAsAddedL
// ----------------------------------------------------------------------------
//	
void CIpSecSadbHandler::SAsAddedL( TBool aSuccess )
    {
    // Haven't necassarily yet received all the sadb messages related
    // to add operations. In that case we have to wait for them since they
    // contain also information about success.
    if ( !aSuccess || ( iOngoingAdds.Count() == 0 && iState == EAvailable ) )
        {
        iObserver.SAsAddedL( aSuccess );
        Reset();
        }
    }
	
// ----------------------------------------------------------------------------
// CIpSecSadbHandler::ConvertToBigEndian
// ----------------------------------------------------------------------------
//		
TUint32 CIpSecSadbHandler::ConvertToBigEndian( TUint32 aNum )
    {
    const TInt KMaxTUint32CStringLen = 11;
    TUint8 temp[ KMaxTUint32CStringLen ];	
	LittleEndian::Put32( temp, aNum );
	return BigEndian::Get32( temp );
    }
	
// End of File
