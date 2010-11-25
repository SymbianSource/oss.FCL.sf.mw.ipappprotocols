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

// INCLUDES

// CLASS HEADER
#include "CMSRPMsgParser.h"
#include "MSRPCommon.h"

#include "CMSRPMessage.h"
#include "CMSRPToPathHeader.h"

// -----------------------------------------------------------------------------
// CMSRPMsgParser::NewL
// Static constructor
// -----------------------------------------------------------------------------
//
/*static*/CMSRPMsgParser* CMSRPMsgParser::NewL(MMSRPParserObserver& aConnection)
    {
    MSRPLOG( "CMSRPMsgParser::NewL enter" )
    CMSRPMsgParser* self = new (ELeave) CMSRPMsgParser(aConnection );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    MSRPLOG( "CMSRPMsgParser::NewL exit" )
    return self;
    }

// -----------------------------------------------------------------------------
// CMSRPMsgParser::CMSRPMsgParser
// Constructor
// -----------------------------------------------------------------------------
//
CMSRPMsgParser::CMSRPMsgParser(MMSRPParserObserver& aConnection)        
    : iConnection( aConnection )
    {  
    
    }

// -----------------------------------------------------------------------------
// CMSRPMsgParser::~CMSRPMsgParser
// Destructor
// -----------------------------------------------------------------------------
//
CMSRPMsgParser::~CMSRPMsgParser()
    {
    MSRPLOG( "CMSRPMsgParser::~CMSRPMsgParser enter" )
    delete iLocalBuf;
    delete iEndToken;
    delete iMessage;
    iParseBuffers.ResetAndDestroy();
    iParseBuffers.Close();    
    iIncomingMessageChunks.ResetAndDestroy();
    iIncomingMessageChunks.Close();
    MSRPLOG( "CMSRPMsgParser::~CMSRPMsgParser exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPMsgParser::ConstructL
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CMSRPMsgParser::ConstructL()
    {
    iState = ETitleLine;
    //to avoid leak on newl leave
    RMsrpBuf buf(iBufPool.ProvideBufferL());
    iLocalBuf = new (ELeave) RMsrpBuf(buf);
    //iLocalBuf = new (ELeave) RMsrpBuf(iBufPool.ProvideBufferL());
    iEndToken = HBufC8::NewL(KMSRPEndTokenMax);   
    }

// -----------------------------------------------------------------------------
// CMSRPMsgParser::ParseL
// -----------------------------------------------------------------------------
//
TBool CMSRPMsgParser::ParseL()
    {
    TMatchType match = EFullMatch;
    TBool ret = TRUE;
    iByteRangeHeaderFound = EFalse;

    /*parse until a parse element spans buffers. 
      if parse element completes exactly at buffer boundaries,
      might end up parsing greater than bufsize+1_element in one runl. 
      to avoid, use transitioned flag (set in handle full match)*/
    while (match == EFullMatch) 
    //while (ret == TRUE)/*parse all parseable data*/
        {    
        MSRPLOG( "CMSRPMsgParser::ParseL enter" )    
        if(!iParseBuffers.Count())
		{
			MSRPLOG( "CMSRPMsgParser::ParseL exit" ) 
            return FALSE;
		}
                               
        TPtrC8 token(KCRAndLF());        
        if (iState == EBody)
            {
            token.Set(iEndToken->Des());                
            }   
        
        TInt matchPos( 0 );
        match = FindToken(iParseBuffers[0]->Ptr(),token,matchPos);    
        if(match == EFullMatch)
            {
            ret = HandleFullMatchL(token, matchPos);
            }
        else if (match == EPartialMatch)
            {
            ret = HandlePartialMatchL(token, matchPos);    
            }
        else //if (match == ENoMatch)        
            {              
            ret = HandleNoMatchL(token, matchPos);
            }
        }
    MSRPLOG( "CMSRPMsgParser::ParseL exit" )
    return ret;
    }

// -----------------------------------------------------------------------------
// CMSRPMsgParser::HandleFullMatchL
// -----------------------------------------------------------------------------
//
TBool CMSRPMsgParser::HandleFullMatchL(TPtrC8& aToken, TInt aMatchPos, TBool aCopyToLocal)
    {
    MSRPLOG( "CMSRPMsgParser::HandleFullMatchL enter" )
    
    TPtrC8 buf = iParseBuffers[0]->Ptr().Left(aMatchPos + aToken.Length());
    if(aCopyToLocal)
        {        
        iLocalBuf->Append(buf);
        }
    else
        {
        HandleStateL(buf, aMatchPos);
        }
    
    if((aMatchPos + aToken.Length()) != iParseBuffers[0]->Ptr().Length())
        {
        iParseBuffers[0]->MsrpMidTPtr(aMatchPos + aToken.Length());
        }
    else
        {
        //transitioned
        RMsrpBuf* headBuf = iParseBuffers[0];
        iParseBuffers.Remove(0);
        delete headBuf;
        if(!iParseBuffers.Count())
            return FALSE;
        }
    MSRPLOG( "CMSRPMsgParser::HandleFullMatchL exit" )
    return TRUE;
    }
    
    

// -----------------------------------------------------------------------------
// CMSRPMsgParser::HandlePartialMatchL
// -----------------------------------------------------------------------------
//
TBool CMSRPMsgParser::HandlePartialMatchL(TPtrC8& aToken, TInt aCurBufMatchPos)
    {
    MSRPLOG( "CMSRPMsgParser::HandlePartialMatchL enter" )
    TBool ret = TRUE;
    TInt matchTokenLen = iParseBuffers[0]->Ptr().Length()- aCurBufMatchPos;
    TPtrC8 matchToken = aToken.Left(matchTokenLen);
    TPtrC8 remainingToken = aToken.Mid(matchTokenLen);
    TPtrC8 emptyToken;
    
    if(iParseBuffers.Count() < 2)
        {
        if (iState == EBody && aCurBufMatchPos)
            //if some bytes to return in body, return now (xcluding token part) rather than parse again 
            {            
            ret = HandleFullMatchL(emptyToken, aCurBufMatchPos);//parse true return nxt parse false
            }
        return FALSE;//buffer count greater than 0 but content size smaller than complete lexical element  
        }
    
    //Look to complete token
    TInt nxtBufMatchPos;
    TMatchType match;                      
    TPtrC8 headOfNxtBuf = iParseBuffers[1]->Ptr().Left(remainingToken.Length());                    
    match = FindToken(headOfNxtBuf, remainingToken, nxtBufMatchPos);                       
    
    if(nxtBufMatchPos!=0) //nxt buf does not complete token
        {
        if (iState != EBody)
            {
            User::LeaveIfError(KErrCorrupt);
            }
        //false partial alarm for body, dont retain token part
        //need to update curbuf as false tokenpart also needs to be returned
        aCurBufMatchPos += matchToken.Length();
        //ret = HandleFullMatchL(matchToken, aCurBufMatchPos);//parse true return true
        ret = HandleFullMatchL(emptyToken, aCurBufMatchPos);
        }
    else
        {
        if(match == EPartialMatch) //completes part token but bytes insufficient
            {
            if (iState == EBody && aCurBufMatchPos)
                {
                ret = HandleFullMatchL(emptyToken, aCurBufMatchPos);//parse true ret false                                   
                }            
            return FALSE;
            }
        else //complete token
            {  
            TBool addToLocal = FALSE;
            if(iState != EBody)
                {
                addToLocal = TRUE;                
                }

            ret = HandleFullMatchL(matchToken, aCurBufMatchPos, addToLocal);
            ret = HandleFullMatchL(remainingToken, nxtBufMatchPos, addToLocal);//if token.length() and matches state token upto length, transition state
            //ret cud be false, if nxtbuf exactly completes token

            if(addToLocal == TRUE)
                {
                HandleStateL(*iLocalBuf, aCurBufMatchPos+nxtBufMatchPos);//nxtBuf==0, just token
                iLocalBuf->Zero();
                }            
            }
        }
    MSRPLOG( "CMSRPMsgParser::HandlePartialMatchL exit" )
    return ret;
    }



// -----------------------------------------------------------------------------
// CMSRPMsgParser::HandleNoMatchL
// -----------------------------------------------------------------------------
//
TBool CMSRPMsgParser::HandleNoMatchL(TPtrC8& aToken, TInt aCurBufMatchPos)
    {
    MSRPLOG( "CMSRPMsgParser::HandleNoMatchL enter" )

    TPtrC8 emptyToken;    
    TBool ret = FALSE;
    
    aCurBufMatchPos = iParseBuffers[0]->Ptr().Length();
    
    if (iState == EBody /*&& aCurBufMatchPos*/)
        {        
        ret = HandleFullMatchL(emptyToken, aCurBufMatchPos);        
        }        
    else if(iParseBuffers.Count() >= 2)
        {
        //if header find token in nxt buf
        TInt nxtBufMatchPos;
        TMatchType match;                      
        match = FindToken(iParseBuffers[1]->Ptr(),aToken,nxtBufMatchPos);
        
        if(match == EFullMatch)
            {       
            ret = HandleFullMatchL(emptyToken, aCurBufMatchPos, TRUE);
            ret = HandleFullMatchL(aToken, nxtBufMatchPos, TRUE);
            HandleStateL(*iLocalBuf, aCurBufMatchPos+nxtBufMatchPos);
            iLocalBuf->Zero();
            }
        else if (iParseBuffers.Count()>2)
            {
            User::LeaveIfError(KErrCorrupt);        
            }
        //else count==2, bytes insufficient in nxt buf, return false        
        }
    MSRPLOG( "CMSRPMsgParser::HandleNoMatchL exit" )
    return ret;
    }

// -----------------------------------------------------------------------------
// CMSRPMsgParser::HandleStateL
// -----------------------------------------------------------------------------
//
void CMSRPMsgParser::HandleStateL(const TDesC8& aString, TInt aMatchPos)
    {
    MSRPLOG( "CMSRPMsgParser::HandleStateL enter" )
    switch(iState)
         {
         case ETitleLine:
             HandleTitleLineL(aString, aMatchPos);             
             break;
         
         case EToPath:
         case EFromPath:    
         //case EMandatoryHeader:
             HandleHeaderL(aString, aMatchPos);            
             break;
                          
         case EOptionalHeaders:
             HandleOptionalHeaderL(aString, aMatchPos);
             break;
                                      
         case EExtraCRLF:
             HandleXtraCrlfL(aString, aMatchPos);
             break;
             
         case EBody:
             HandleBodyL(aString, aMatchPos);
             break;             
             
         case EEndofEndLine:
             HandleEndofEndLineL(aString, aMatchPos);
             break;
                          
         default:
             User::LeaveIfError(KErrUnknown);
             break;        
         }   
    MSRPLOG( "CMSRPMsgParser::HandleStateL exit" )
    }


// -----------------------------------------------------------------------------
// CMSRPMsgParser::HandleTitleLineL
// -----------------------------------------------------------------------------
//
void CMSRPMsgParser::HandleTitleLineL(const TDesC8& aString, TInt /*aMatchPos*/)
    {
    MSRPLOG( "CMSRPMsgParser::HandleTitleLineL enter" )
    
    TPtrC8 msrp(aString.Left(KMSRP().Length()));
    if(msrp.Compare(KMSRP()))
        User::LeaveIfError(KErrCorrupt);
    if(aString[KMSRP().Length()] != ' ')
        User::LeaveIfError(KErrCorrupt);
    
    TPtrC8 trans_id = aString.Mid(KMSRP().Length()+1);    
    TInt pos = trans_id.Locate(MSRP_SPACE);
    /*if(pos == KErrNotFound)
        User::LeaveIfError(KErrCorrupt);*/    
    if (pos < KMSRPIDLenMin || pos > KMSRPIDLenMax)
        User::LeaveIfError(KErrCorrupt);           
    trans_id.Set(trans_id.Left(pos));
    
    TPtr8 ptr = iEndToken->Des();
    ptr.Zero();
    ptr.Append(KCRAndLF);
    ptr.Append(KDashLine());
    ptr.Append(trans_id);
    
    TPtrC8 method = aString.Mid(KMSRP().Length()+1+trans_id.Length()+1);
    pos = method.Locate(MSRP_SPACE);
    
    if(pos == KErrNotFound)
        {
        pos = method.Locate(MSRP_CARRIAGE);        
    
        if(pos == KErrNotFound)
            User::LeaveIfError(KErrCorrupt);
        
        method.Set(method.Left(pos));
                
        delete iMessage;
        iMessage = NULL;
         if (!method.Compare(KMSRPSend()))
             {
             iMessage = CMSRPMessageHandler::NewL(MMSRPIncomingMessage::EMSRPMessage);
             MSRPLOG2( "CMSRPMsgParser::HandleTitleLineL new message = %d", iMessage )
             }
         else if ( !method.Compare( KMSRPReport() ) )
             {
             iMessage = CMSRPMessageHandler::NewL( MMSRPIncomingMessage::EMSRPReport );
             MSRPLOG2( "CMSRPMsgParser::HandleTitleLineL new report = %d", iMessage )
             }
         else //extn
             {
             iMessage = CMSRPMessageHandler::NewL(MMSRPIncomingMessage::EMSRPNotDefined);
             MSRPLOG2( "CMSRPMsgParser::HandleTitleLineL new not defined = %d", iMessage )
             }        
        }
    else //response
        {
        TPtrC8 null;
        method.Set(method.Left(pos));
        delete iMessage;
        iMessage = NULL;
        iMessage = CMSRPMessageHandler::NewL(MMSRPIncomingMessage::EMSRPResponse);
        MSRPLOG2( "CMSRPMsgParser::HandleTitleLineL new response = %d", iMessage )
        iMessage->SetStatusOfResponseL(method, null);
        }    
    iMessage->SetTransactionId(trans_id);
    iState = EToPath;    
    MSRPLOG( "CMSRPMsgParser::HandleTitleLineL exit" )
    }

#if 0
// -----------------------------------------------------------------------------
// CMSRPMsgParser::HandleTitleLineL
// -----------------------------------------------------------------------------
//
void CMSRPMsgParser::HandleTitleLineL(const TDesC8& aString, TInt /*aMatchPos*/)
    {
    MSRPLOG( "CMSRPMsgParser::HandleTitleLineL enter" )
    TLex8 lex(aString);

    //MSRP
    lex.Mark();
    lex.SkipCharacters();
    if(lex.TokenLength() != 4)
        {   
        User::LeaveIfError(KErrCorrupt);
        }    
    TPtrC8 first_token = lex.MarkedToken();        
    if (first_token.Compare(KMSRP()))
        {
        User::LeaveIfError(KErrCorrupt);
        }
    
    //SP
    if(lex.Peek() != MSRP_SPACE)
    {
    User::LeaveIfError(KErrCorrupt);
    }
    lex.Inc();
    
    //trans-id
    lex.Mark();
    lex.SkipCharacters();    
    if (lex.TokenLength() < KMSRPIDLenMin || lex.TokenLength() > KMSRPIDLenMax)
        {
        User::LeaveIfError(KErrCorrupt);
        }
    
    //SP
    if(lex.Peek() != MSRP_SPACE)
    {
    User::LeaveIfError(KErrCorrupt);
    }
    lex.Inc();
    
    TPtrC8 trans_id = lex.MarkedToken();
    TPtr8 ptr = iEndToken->Des();
    ptr.Zero();
    ptr.Append(kEndMarker());
    ptr.Append(trans_id);
    
    //method
    lex.Mark();
    lex.SkipCharacters();
    if(lex.TokenLength() != 0)
        {   
        User::LeaveIfError(KErrCorrupt);
        }      
    TPtrC8 method = lex.MarkedToken();      
    
    //create msg
    //set trans_id
    if (!method.Compare(KMSRPSend()))
        {
        //type = ESend        
        //set type
        }
    else if (!method.Compare(KMSRPReport()))
        {
        //type = EReport        
        //set type
        }
    else //if 3 digit string
        { 
        //type = EResponse
        //set type
        //set status code
        
        //remaining SP comment\r\n
        /*ignore comment*/
                     
        }
    //else extn method
    
    //remaining \r\n
    //TODO: comment, extn method and check remaining bytes is as xpected
        
    iState = EToPath;
    
    MSRPLOG( "CMSRPMsgParser::HandleTitleLineL exit" )
    }
#endif

// -----------------------------------------------------------------------------
// CMSRPMsgParser::HandleHeaderL
// -----------------------------------------------------------------------------
//
void CMSRPMsgParser::HandleHeaderL(const TDesC8& aString, TInt /*aMatchPos*/)
    {
    MSRPLOG( "CMSRPMsgParser::HandleHeaderL enter" )
    TInt pos = aString.Locate(MSRP_COLON);
    if(pos == KErrNotFound)
        User::LeaveIfError(KErrCorrupt);
    
    TPtrC8 header_name = aString.Left(pos);
    TUint8 nxtChar = aString[pos+1];
       
    if(nxtChar != MSRP_SPACE)
        User::LeaveIfError(KErrCorrupt);
    
    TPtrC8 header_val = aString.Mid(pos+2); 
    header_val.Set(header_val.Left(header_val.Length()-2));
    
    if(!header_val.Length())
        User::LeaveIfError(KErrCorrupt);

    MMSRPMessageHandler::TMsrpHeaderType headerType = MMSRPMessageHandler::EHeaderUnknown;
    if(iState == EToPath)
        {        
        if(header_name.Compare(KMSRPToPath()))
            User::LeaveIfError(KErrCorrupt);
        headerType = MMSRPMessageHandler::EToPath;
        iState = EFromPath;
        }
    else if(iState == EFromPath)
        {
        headerType = MMSRPMessageHandler::EFromPath;
        if(header_name.Compare(KMSRPFromPath()))
            User::LeaveIfError(KErrCorrupt);  
        iState = EOptionalHeaders;
        }
    else //find header id
        {
        if(!header_name.Compare(KMSRPMessageID()))
            {            
            headerType = MMSRPMessageHandler::EMessageId;
            }
        else if(!header_name.Compare(KMSRPByteRange()))
            {
            iByteRangeHeaderFound = ETrue;
            headerType = MMSRPMessageHandler::EByteRange;
            }
        else if(!header_name.Compare(KMSRPSuccessReport()))
            {
            headerType = MMSRPMessageHandler::ESuccessReport;
            }            
        else if(!header_name.Compare(KMSRPFailureReport()))
            {
            headerType = MMSRPMessageHandler::EFailureReport;
            }            
        else if(!header_name.Compare(KMSRPStatus()))
            {
            headerType = MMSRPMessageHandler::EStatus;
            }
        else if(!header_name.Compare(KMSRPContentType()))
            {
            headerType = MMSRPMessageHandler::EContentType;
            iState = EExtraCRLF;             
            }           
        }      
    
    //message->add_header
    TPtrC8 fullHeader(aString);
    iMessage->AddHeaderL(headerType, header_val, fullHeader);
    if ( headerType == MMSRPMessageHandler::EMessageId )
        {
        // let's check if this message chunk belong to one
        // of the chunks already received. If so, combine
        // the messages
        CheckMessageChunkL( );
        }
    //TODO: //only for to-path from-path add_header error
    //switch iState = EBody, mode = EError //essentially start looking for end token
    //if mode is error don't issue callbacks, on transition from EndofEndLine,
    //delete message and switch to normal mode
    
    MSRPLOG( "CMSRPMsgParser::HandleHeaderL exit" )
        
    }
    
// -----------------------------------------------------------------------------
// CMSRPMsgParser::HandleOptionalHeaderL
// -----------------------------------------------------------------------------
//
void CMSRPMsgParser::CheckMessageChunkL( )
    {
    MSRPLOG( "-> CMSRPMsgParser::HandleHeaderL" )
    for ( TInt i = 0; i < iIncomingMessageChunks.Count(); i++ )
        {
        if ( iMessage->CheckMessageChunkL( *iIncomingMessageChunks[ i ] ) )
            {
            delete iMessage;
            iMessage = iIncomingMessageChunks[ i ];
            iIncomingMessageChunks.Remove( i );
            break;
            }
        }
    MSRPLOG( "<- CMSRPMsgParser::HandleHeaderL" )
    }

// -----------------------------------------------------------------------------
// CMSRPMsgParser::HandleOptionalHeaderL
// -----------------------------------------------------------------------------
//
void CMSRPMsgParser::HandleOptionalHeaderL(const TDesC8& aString, TInt aMatchPos)
    {
    MSRPLOG( "CMSRPMsgParser::HandleOptionalHeaderL enter" )
    TPtrC8 endTokenwoStartCrlf = (*iEndToken).Mid(KCRAndLF().Length()); 
    TPtrC8 fullToken(aString.Left(endTokenwoStartCrlf.Length()));
    if (fullToken.Compare(endTokenwoStartCrlf))
        {
        HandleHeaderL(aString,aMatchPos);
        return;
        }
    //iState = EEndOfEndLine;
    aMatchPos -= fullToken.Length();
    HandleEndofEndLineL(aString.Mid(fullToken.Length()),aMatchPos);
    MSRPLOG( "CMSRPMsgParser::HandleOptionalHeaderL exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPMsgParser::HandleXtraCrlfL
// -----------------------------------------------------------------------------
//
void CMSRPMsgParser::HandleXtraCrlfL(const TDesC8& /*aString*/, TInt aMatchPos)
    {
    MSRPLOG( "CMSRPMsgParser::HandleXtraCrlfL enter" )
    if(aMatchPos)
        User::LeaveIfError(KErrCorrupt);
    iState = EBody;
    MSRPLOG( "CMSRPMsgParser::HandleXtraCrlfL exit" )
    }

// -----------------------------------------------------------------------------
// CMSRPMsgParser::HandleBodyL
// -----------------------------------------------------------------------------
//
void CMSRPMsgParser::HandleBodyL(const TDesC8& aString, TInt aMatchPos)
    {
    MSRPLOG( "CMSRPMsgParser::HandleBodyL enter" )
    TPtrC8 token = aString.Mid(aMatchPos);
    
    //if(!aString.Length())
     //   return;
    
    if(aMatchPos!=0)
        {
        TPtrC8 content(aString.Left(aMatchPos));
        if ( !iMessage->IsTransmissionTerminated() )
            {
            MSRPLOG2( "CMSRPMsgParser::HandleBodyL instance = %d", iMessage )
            iMessage->AddContentL( content, iByteRangeHeaderFound ); 
            iConnection.ReportReceiveprogressL( iMessage );
            }
        }

    //partial or full match
    if(token.Length())
        {   
        MSRPLOG( "CMSRPMsgParser::HandleBodyL enter partial/token" )
         
        //if(aMatchPos == 0)
            {
            //TPtrC8 firstPart = (*iEndToken).Left(token.Length());
            TPtrC8 lastPart = (*iEndToken).Right(token.Length());
            
            //if(firstPart == lastPart)
            /*Solns: set iFlagFirstPartMatch on first match and then transition                      
              false transition to endline and transition back to body on endline parse error
              add initial \r\n (disallowed in trans_id) to searchEndToken on transition to body - 
              for optional header, token has no init crlf*/                    
            
            
            
            if (!token.Compare(lastPart))
                {
                iState = EEndofEndLine;
                }
            /*else if(!token.Compare(firstPart)); 
            else User::LeaveIfError(KErrCorrupt);*/                                                        
            }
        //else //full token matches
        }
    MSRPLOG( "CMSRPMsgParser::HandleBodyL exit" )
    return; 
    }

// -----------------------------------------------------------------------------
// CMSRPMsgParser::HandleEndofEndLineL
// -----------------------------------------------------------------------------
//
void CMSRPMsgParser::HandleEndofEndLineL(const TDesC8& aString, TInt aMatchPos)
    {
    MSRPLOG2( "CMSRPMsgParser::HandleEndofEndLineL enter, char = %d", aString[0] )
    if(aMatchPos != 1)
        User::LeaveIfError(KErrCorrupt);
    
    MMSRPMessageHandler::TMsrpMsgEndStatus endType = MMSRPMessageHandler::EMessageEnd;  
    if(aString[0] == MSRP_END)
        endType = MMSRPMessageHandler::EMessageEnd;        
    else if (aString[0] == MSRP_CONTINUE)
        endType = MMSRPMessageHandler::EMessageContinues;
    else if (aString[0] == MSRP_TERMINATE)
        endType = MMSRPMessageHandler::EMessageTerminated;
    else
        User::LeaveIfError(KErrCorrupt);
    
    if ( !iMessage->IsTransmissionTerminated() )
        {
        MSRPLOG2( "CMSRPMsgParser::HandleEndofEndLineL instance = %d", iMessage )
        iMessage->EndOfMessageL( endType );
        TInt status = iConnection.ParseStatusL( iMessage, KErrNone );
        if ( endType == MMSRPMessageHandler::EMessageContinues &&
            status == MMSRPParserObserver::EParseStatusMessageHandled ) 
            {
            iIncomingMessageChunks.AppendL( iMessage );
            }
        else if ( status == MMSRPParserObserver::EParseStatusError )
            {
            delete iMessage;
            iMessage = NULL;
            }
        iMessage = NULL;
        }
        
    iState = ETitleLine;
    MSRPLOG( "CMSRPMsgParser::HandleEndofEndLineL exit" )    
    }


// -----------------------------------------------------------------------------
// CMSRPMsgParser::FindToken
// -----------------------------------------------------------------------------
//
CMSRPMsgParser::TMatchType CMSRPMsgParser::FindToken( const TDesC8& aString, const TDesC8& aToken, TInt& aMatchPos)
    {
    MSRPLOG( "CMSRPMsgParser::FindToken enter" )
    TMatchType ret = ENoMatch;
    aMatchPos = KErrNotFound;
    if(!aString.Length() || !aToken.Length())
        return ret;
    aMatchPos = aString.Find(aToken);
    
    if(aMatchPos == KErrNotFound)
        {
        TUint8 lastChar = aString[aString.Length()-1];
        TPtrC8 tempToken;
        tempToken.Set(aToken);
        
        TInt tokenPos = tempToken.LocateReverse(lastChar);
        while (tokenPos != KErrNotFound)
            {
            TPtrC8 headPartofToken = tempToken.Left(tokenPos+1);
            TPtrC8 tailPartofString = aString.Right(tokenPos+1);
             if(!headPartofToken.Compare(tailPartofString))
                 break;            
             tempToken.Set(tempToken.Left(tokenPos)); //if 0
             tokenPos = tempToken.LocateReverse(lastChar);
            }
        if(tokenPos != KErrNotFound)
            {
            ret = EPartialMatch;
            aMatchPos = aString.Length() - (tokenPos+1);
            }
        return ret;
        }    
    MSRPLOG( "CMSRPMsgParser::FindToken exit" )
    return EFullMatch;    
    }

//test cases
//shyamprasad, prasad, xyz, add, dan, shyamprasad
//pr, prasad

#if 0
// -----------------------------------------------------------------------------
// CMSRPMsgParser::ExecuteStateL
// -----------------------------------------------------------------------------
//
CMSRPMsgParser::TDataCheck CMSRPMsgParser::ExecuteStateL()
    {         
    TDataCheck ret = EWaitForDataPost; 
    
    switch(iState)
        {
        case ETitleLine:
            ret = HandleTitleLineL();
            break;
        
        case EToPath:
        case EFromPath:    
        case EMandatoryHeader:
            ret = HandleMandatoryHeaderL();            
            break;
            
            
        case EOptionalHeaders:
            //ret = HandleOptionalHeaderL();
            break;
            
            
        case EBody:
            
            break;
            
        default:
            User::LeaveIfError(KErrUnknown);
            break;        
        }    
      return ret;     
    }

// -----------------------------------------------------------------------------
// CMSRPMsgParser::HandleTitleLineL
// -----------------------------------------------------------------------------
//
CMSRPMsgParser::TDataCheck CMSRPMsgParser::HandleTitleLineL()
    {
    TDataCheck ret;
    
    VARS(lex, transitioned, base_offset, lex_bytesdone);
    SET_LEX(lex);
        
    TOKEN(first_token);            
    ret = GetNextToken(lex, transitioned, first_token, base_offset, lex_bytesdone);
    RET_CHECK(ret,lex,MSRP_SPACE);//if transitioned, set mode local, being done above based on local buf length
    LEX_INC_AVAILABLE(lex, lex_bytesdone, transitioned);
    
    
    if(first_token.iLength == 4)
        {        
        FETCH_TOKENSTRING(first_string, first_token, transitioned);
        if (first_string.Compare(KMSRP()))
            {
            User::LeaveIfError(KErrCorrupt);
            }        
        }        
    else
        {
        User::LeaveIfError(KErrCorrupt);
        }
    
    TOKEN(trans_id);
    ret =  GetNextToken(lex, transitioned, trans_id, base_offset, lex_bytesdone);
    RET_CHECK(ret,lex,MSRP_SPACE);
    LEX_INC_AVAILABLE(lex, lex_bytesdone, transitioned);
    
    if (trans_id.iLength < KMSRPIDLenMin || trans_id.iLength > KMSRPIDLenMax)
        {
        User::LeaveIfError(KErrCorrupt);
        }
    
    TOKEN(method);
    ret =  GetNextToken(lex, transitioned, method, base_offset, lex_bytesdone);
    FETCH_TOKENSTRING(method_string, method, transitioned);
      
    if (!method_string.Compare(KMSRPSend()))
        {
        RET_CHECK(ret,lex,MSRP_CARRIAGE);
        LEX_INC_AVAILABLE(lex, lex_bytesdone, transitioned);
        //create msg with correct type
        }
    else if (!method_string.Compare(KMSRPReport))
        {
        RET_CHECK(ret,lex,MSRP_CARRIAGE);
        LEX_INC_AVAILABLE(lex, lex_bytesdone, transitioned);
        //create msg with correct type
        }
    else //if 3 digit string
        {        
        RET_CHECK_LENIENT(ret,lex,MSRP_SPACE,val);
        if (val)
            {
            LEX_INC_AVAILABLE(lex, lex_bytesdone, transitioned);
            TOKEN(reason);
            ret =  GetNextToken(lex, transitioned, reason, base_offset, lex_bytesdone);
            }
        RET_CHECK(ret,lex,MSRP_CARRIAGE);
        LEX_INC_AVAILABLE(lex, lex_bytesdone, transitioned);
        //create msg with correct type
        //set status code == method_string
        }
    //else extn method, TODO
    
    //set trans_id
        
    //EOL data sufficiency and increment
    ret = IsDataSufficient(lex, transitioned, base_offset, lex_bytesdone);
    RET_CHECK(ret,lex,MSRP_NEWLINE);
    LEX_INC_AVAILABLE(lex, lex_bytesdone, transitioned);
             
    STATE_COMPLETE(EToPath, lex_bytesdone);    
    return ret;    
    }


// -----------------------------------------------------------------------------
// CMSRPMsgParser::HandleMandatoryHeaderL
// -----------------------------------------------------------------------------
//
CMSRPMsgParser::TDataCheck CMSRPMsgParser::HandleMandatoryHeaderL()
    {
    TDataCheck ret;    
    VARS(lex, transitioned, base_offset, lex_bytesdone);
    SET_LEX(lex);
        
    TOKEN(header);            
    ret = GetNextToken(lex, transitioned, header, base_offset, lex_bytesdone);
    RET_CHECK(ret,lex,MSRP_SPACE);
    LEX_INC_AVAILABLE(lex, lex_bytesdone, transitioned);
    
    TOKEN(header_val);
    ret =  GetNextToken(lex, transitioned, header_val, base_offset, lex_bytesdone);
    RET_CHECK(ret,lex,MSRP_CARRIAGE);
    LEX_INC_AVAILABLE(lex, lex_bytesdone, transitioned);

    //EOL data sufficiency and increment
    ret = IsDataSufficient(lex, transitioned, base_offset, lex_bytesdone);
    RET_CHECK(ret,lex,MSRP_NEWLINE);
    LEX_INC_AVAILABLE(lex, lex_bytesdone, transitioned);
    
    FETCH_TOKENSTRING(header_string, header, transitioned);
    FETCH_TOKENSTRING(val_string, header_val, transitioned);
    
    //check last char is colon and reduce string len by 1
    //HEADER_CHECK(header_string);
            
    switch (iState)
        {
        case EToPath:
            if (header_string.Compare(KMSRPToPath()))
                {
                User::LeaveIfError(KErrCorrupt);
                }
            //AddHeader callback
            STATE_COMPLETE(EFromPath, lex_bytesdone);            
            break;
            
        case EFromPath:
            if (header_string.Compare(KMSRPFromPath()))
                {
                User::LeaveIfError(KErrCorrupt);
                }               
            //AddHeader callback
            STATE_COMPLETE(EMandatoryHeader, lex_bytesdone); 
            break;
            
        case EMandatoryHeader:
            //Findheaderid and give appropriate callback
            STATE_COMPLETE(EOptionalHeaders, lex_bytesdone);
            break;
            
        default:
            User::LeaveIfError(KErrUnknown);
            break;
        }       
    return ret; 
    }

// -----------------------------------------------------------------------------
// CMSRPMsgParser::HandleOptionalHeaderL
// -----------------------------------------------------------------------------
//
#if 0
CMSRPMsgParser::TDataCheck CMSRPMsgParser::HandleOptionalHeaderL()
    {
    //get first token compare against endline
    //if yes msg done and handover bodiless msg to conn
    //if not, get next token and give header callback
    
    TDataCheck ret;    
    VARS(lex, transitioned, base_offset, lex_bytesdone);
    SET_LEX(lex);
        
    TOKEN(header);            
    ret = GetNextToken(lex, transitioned, header, base_offset, lex_bytesdone);
    RET_CHECK(ret,lex,MSRP_SPACE);
    LEX_INC_AVAILABLE(lex, lex_bytesdone, transitioned);
    
    TOKEN(header_val);
    ret =  GetNextToken(lex, transitioned, header_val, base_offset, lex_bytesdone);
    RET_CHECK(ret,lex,MSRP_CARRIAGE);
    LEX_INC_AVAILABLE(lex, lex_bytesdone, transitioned);

    //EOL data sufficiency and increment
    ret = IsDataSufficient(lex, transitioned, base_offset, lex_bytesdone);
    RET_CHECK(ret,lex,MSRP_NEWLINE);
    LEX_INC_AVAILABLE(lex, lex_bytesdone, transitioned);
    
    FETCH_TOKENSTRING(header_string, header, transitioned);
    FETCH_TOKENSTRING(val_string, header_val, transitioned);
    
    //check last char is colon and reduce string len by 1
    HEADER_CHECK(header_string);
            
    switch (iState)
        {
        case EToPath:
            if (header_string.Compare(KMSRPToPath()))
                {
                User::LeaveIfError(KErrCorrupt);
                }
            //AddHeader callback
            STATE_COMPLETE(EFromPath, lex_bytesdone);            
            break;
            
        case EFromPath:
            if (header_string.Compare(KMSRPFromPath()))
                {
                User::LeaveIfError(KErrCorrupt);
                }               
            //AddHeader callback
            STATE_COMPLETE(EMandatoryHeader, lex_bytesdone); 
            break;
            
        case EMandatoryHeader:
            //Findheaderid and give appropriate callback
            STATE_COMPLETE(EOptionalHeaders, lex_bytesdone);
            break;
            
        default:
            User::LeaveIfError(KErrUnknown);
            break;
        }       
    return ret; 
    }

#endif

    
// -----------------------------------------------------------------------------
// CMSRPMsgParser::GetNextToken
// -----------------------------------------------------------------------------
//
//TBD: convert to macros
CMSRPMsgParser::TDataCheck CMSRPMsgParser::GetNextToken(TLex8& aLex, TBool& aTransitioned, TToken& aToken, TInt& aBaseOffset, TInt& aLexBytesDone)
    {
    TDataCheck ret = EContinueNormal;
    
    aLex.Mark();
    aToken.iOffset += aBaseOffset + aLex.MarkedOffset();
    do
        {
        aLex.SkipCharacters();        
        if (aLex.TokenLength() > 0)
            {    
            aLexBytesDone += aLex.TokenLength();
            aToken.iLength += aLex.TokenLength();
            TPtrC8 tokenString = aLex.MarkedToken();
            if(aTransitioned)
                {
                //TODO: leave or parse error if append not possible 
                iLocalBuf->Ptr().Append(tokenString);
                }                        
            }            
        ret = IsDataSufficient(aLex, aTransitioned, aBaseOffset, aLexBytesDone); 
        //if(ret == EContinueTransitioned) aBytesParsed = 0;        
        } while (ret == EContinueTransitioned);
        
    return ret;
    }


// -----------------------------------------------------------------------------
// CMSRPMsgParser::IsDataSufficient
// not called if in Body, only header data sufficiency check
// -----------------------------------------------------------------------------
//TBD: convert to macro
CMSRPMsgParser::TDataCheck CMSRPMsgParser::IsDataSufficient(TLex8& aLex, TBool& aTransitioned, TInt& aBaseOffset, TInt& aLexBytesDone)
    {
    if(aLex.Peek() == 0)
        {  
        /*iParseBuffers[0] being parsed*/
        aBaseOffset += iParseBuffers[0]->Ptr().Length();
        aLexBytesDone = 0;
        if(aTransitioned)
            {
            delete iParseBuffers[0];
            }
        else
            {
            if (iMode == EQueueBuf)
                {
                /*non collated buffer existence means spanning across buf pools*/  
                if (iParseBuffers.Count() < 2)
                    return EWaitForDataPost; //aLexBytesDone =0, nothing copied to local buf
                else if (iSthgParsed)
                    return EContinueInNextParse; //aLexBytesDone =0
                else
                    iLocalBuf->Ptr().Append(*iParseBuffers[0]);
                }    
            /*in case of localbuf, we are about to begin parsing first buf and copy tokens*/
            aTransitioned = TRUE;
            }
        
#if 0        
        /*for queuebuf avoid copying till bufpool spanning of a parse element can be confirmed*/
        if (iMode == EQueueBuf)
            {
            if (!aTransitioned)
                {
                /*non collated buffer existence means spanning across buf pools*/  
                if (iParseBuffers.Count() < 2)
                    return EWaitForDataPost; //aLexBytesDone =0, nothing copied to local buf
                else if (iSthgParsed)
                    return EContinueInNextParse; //aLexBytesDone =0
                else
                    {                    
                    //copy to local
                    aTransitioned = TRUE;
                    iLocalBuf->Ptr().Append(*iParseBuffers[0]);
                    aBaseOffset += iParseBuffers[0]->Ptr().Length();
                    aLexBytesDone = 0;
                    delete iParseBuffers[0];                    
                    }
                }
            else //once transitioned tokens are being copied,iParseBuffers[0] being parsed, hence no existence check            
                {
                //only free
                aBaseOffset += iParseBuffers[0]->Ptr().Length();
                aLexBytesDone = 0;
                delete iParseBuffers[0];                    
                }            
            }
        else //if (iMode == ELocalBuf)
            {
            if (!aTransitioned)
               {
               /*sthg parsed is always false for local buf*/
               /*also if not transitioned in this pass, means count of buffers in queue is atleast 1*/               
               /*if (iParseBuffers.Count() < 1)
                   return EWaitForDataPost;
               else if (iSthgParsed)
                   return EContinueInNextParse;*/
               //no action  
               aTransitioned = TRUE;
               aBaseOffset += iParseBuffers[0]->Ptr().Length();
               aLexBytesDone = 0;
               }
           else //once transitioned tokens are being copied,iParseBuffers[0] being parsed, hence no existence check
               {
               //only free  
               aBaseOffset += iParseBuffers[0]->Ptr().Length();
               aLexBytesDone = 0;
               //delete iParseBuffers[0];
               }                    
            }
#endif                       
        if(!iParseBuffers.Count())
            return EWaitForDataPost; //aLexBytesDone = 0 //no rbuf to delete
                                                        
        //reassign lex to zeroth
        aLex.Assign(*iParseBuffers[0]);
        
        return EContinueTransitioned;
        }
    return EContinueNormal;
    }
    
#endif
