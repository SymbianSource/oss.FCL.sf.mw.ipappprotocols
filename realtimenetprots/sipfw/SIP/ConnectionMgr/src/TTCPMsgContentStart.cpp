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
// Name        : TTCPMsgContentStart.cpp
// Part of     : ConnectionMgr
// Implementation
// Version     : SIP/4.0
//



#include "TTCPMsgContentStart.h"
#include "sipmessage.h"
#include "SipAssert.h"
#include "MMsgAssemblerContext.h"
#include "MSigCompController.h"
#include "CSIPMsgAssembler.h"


// ----------------------------------------------------------------------------
// TTCPMsgContentStart::DataReceivedL
// ----------------------------------------------------------------------------
//
void TTCPMsgContentStart::DataReceivedL( TPtr8 aData, TUint&  aNextLength )
	{
	CSIPMessage* message = iMsgAssembler.Message();
	
	// panic if message == 0 in debug mode. leaves in release mode.
	__SIP_ASSERT_LEAVE( message, KErrGeneral );
	// panic if there is no content length in debug mode. 
	// leaves in release mode.                
	__SIP_ASSERT_LEAVE( message->HasAnnouncedContentLength(), 
	                    KErrGeneral );	     

	TUint contentLen = message->AnnouncedContentLength();
	
	// panic if content length <= 0 in debug mode. 
	// leaves in release mode.                
	__SIP_ASSERT_LEAVE( contentLen > 0, KErrGeneral );	 
	
    // if message contains already some content, received data is complementing
	// it. this kind of situation might occur when sigcomp splits too big sip 
	// message to pieces
	if ( message->Content().Length() > 0 )
	    {
        iMsgAssembler.MsgBuffer().InsertL( iMsgAssembler.MsgBuffer().Size(), 
                                           message->Content() ); 
        iMsgAssembler.MsgBuffer().Compress();
                                           
        // remove old uncomplete content
        HBufC8* uncompletedContent = message->TakeContentOwnershipL(); 
        delete uncompletedContent;          
	    }
		
	if ( iMsgAssembler.SigComp().IsSupported() &&
	     iMsgAssembler.IsSigComp() &&
	     HandleComplementingSigCompDataL( aData, aNextLength, contentLen ) )
		{
	    return;
		}
	
	if ( iMsgAssembler.MsgBuffer().Size() > 0 )
		{
		// insert received data to msg buffer.
		iMsgAssembler.MsgBuffer().InsertL(
						        	iMsgAssembler.MsgBuffer().Size(), aData );
		iMsgAssembler.MsgBuffer().Compress();
		// delete the content of received data
		aData.Delete( 0, aData.Length() ); 

		TPtr8 bufPtr = iMsgAssembler.MsgBuffer().Ptr( 0 );
		
		// check if the msg buffer contains the whole content, if so
		// copy the msg buffer to new data buffer, clean the msg buffer
		// and enter to msg content end state.
		if ( contentLen <= static_cast< TUint >( bufPtr.Length() ) )
			{
			HBufC8* newData = HBufC8::NewLC( bufPtr.Length() );
		    // copy the msg buffer data  new data 
	    	TPtr8 newDataPtr = newData->Des();		    
	   		// delete the content of received data
			aData.Delete( 0, aData.Length() ); 
		    newDataPtr.Append(iMsgAssembler.MsgBuffer().Ptr(0));
		    // clean the msg buffer
			iMsgAssembler.MsgBuffer().Reset();
			iMsgAssembler.MsgBuffer().Compress();	
			
		    iMsgAssembler.ChangeState( MMsgAssemblerContext::EMsgContentEnd );
			iMsgAssembler.CurrentState().DataReceivedL( newDataPtr, 
			                                            aNextLength );
		    CleanupStack::PopAndDestroy( newData );
			}
		// if the msg buffer contains not enough content, ask the next
		// length to be received.
		else
			{
			aNextLength = contentLen - aData.Length();
			}
		}
	else if ( contentLen <= static_cast< TUint >( aData.Length() ) )
		{
		iMsgAssembler.ChangeState(MMsgAssemblerContext::EMsgContentEnd);
		iMsgAssembler.CurrentState().DataReceivedL( aData, aNextLength );
		}
	else
		{
		iMsgAssembler.MsgBuffer().InsertL(
									iMsgAssembler.MsgBuffer().Size(), aData );
		iMsgAssembler.MsgBuffer().Compress();
		aNextLength = contentLen - aData.Length();
		}
	}

// ----------------------------------------------------------------------------
// TTCPMsgContentStart::HandleComplementingSigCompDataL
// ----------------------------------------------------------------------------
//
TBool TTCPMsgContentStart::HandleComplementingSigCompDataL( 
    TPtr8 aData, 
    TUint& aNextLength,
    TInt aAnnouncedContentLen )
    {
    // If still receiving compressed data, current contents have to be
    // stored until we come back from comp states, in sigcomp point of
    // view message can be complete but it ain't necessary complete
    // in sip point of view. All data in aData or msgbuffer are not 
    // necessarily decompressed - these have to be taken with us when 
    // going back to decompression states
    
    TInt unconsumedBytes( iMsgAssembler.UnConsumedBytes() );
    
    TPtr8 msgBufPtr( iMsgAssembler.MsgBuffer().Ptr( 0 ) );
    HBufC8* content = NULL;
    
    TInt decompressedDataLen = aData.Length() - unconsumedBytes;
    
   	__SIP_ASSERT_LEAVE( decompressedDataLen > 0, KErrGeneral );
   	
   	TInt msgBufLen( msgBufPtr.Length() );
    TInt decompressedLen( msgBufLen + decompressedDataLen );
   	if ( aAnnouncedContentLen <= decompressedLen )
   	    {
   	    // Enough decompressed content data
   	    return EFalse;
   	    }
      
    // Store decompressed data to message content
    if ( msgBufLen > 0 )
        {
        content = HBufC8::NewL( decompressedLen );
        TPtr8 contentPtr( content->Des() );
        contentPtr.Copy( msgBufPtr );
        contentPtr.Append( aData.Left( decompressedDataLen ) );
        }
    else
        {
        content = aData.Left( decompressedDataLen ).AllocL();
        }
    
    // Remove decompressed data which was stored to content   
    aData.Delete( 0, decompressedDataLen ); 
        
     // Clean msgbuffer
   	iMsgAssembler.MsgBuffer().Reset();
    iMsgAssembler.MsgBuffer().Compress();
    	   
    iMsgAssembler.SetUnConsumedBytes( 0 );
    
    CSIPMessage* message = iMsgAssembler.Message();
    message->SetContent( content ); // ownership is transferred
	content = NULL;
	
	aNextLength = CSIPMsgAssembler::EMsgBufferSize;
	
	iMsgAssembler.ChangeState( MMsgAssemblerContext::ECompMsgStart );
	iMsgAssembler.CurrentState().DataReceivedL( aData, aNextLength );
	
	return ETrue;
    }

// End of File
