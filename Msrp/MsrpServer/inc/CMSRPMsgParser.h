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

#ifndef CMSRPMSGPARSER_H_
#define CMSRPMSGPARSER_H_

#ifdef UT_TEST
#include "../../tsrc/conn_parser/inc/CMSRPMessageHandler.h"
#else
#include "CMSRPMessageHandler.h"
#endif

#include "MMSRPParserObserver.h"
#include "MSRPBuffer.h"

#define MSRP_COLON ':'
#define MSRP_SPACE ' '
#define MSRP_CARRIAGE '\r'
#define MSRP_TERMINATE  '#'
#define MSRP_CONTINUE  '+'
#define MSRP_END  '$'

#define KMSRPIDLenMax 32
#define KMSRPIDLenMin 4
#define KMSRPEndTokenMax KMSRPIDLenMax+8+2 //init-crlf

class CMSRPMsgParser: public CBase
    {
    friend class CMSRPParser;    
public:
    enum TParseState 
        {
        EIdle = 0,
        ETitleLine,
        EToPath,
        EFromPath,
        //EMandatoryHeader,
        EOptionalHeaders,
        EBody,
        EExtraCRLF,        
        EEndofEndLine
        };
        
    enum TMatchType
        {
        ENoMatch,
        EFullMatch,        
        EPartialMatch   
        };
    
    static CMSRPMsgParser* NewL(MMSRPParserObserver& aConnection);
    virtual ~CMSRPMsgParser();
    
    TBool ParseL();

private:
    CMSRPMsgParser(MMSRPParserObserver& aConnection);
    void ConstructL();
    
    TBool HandleFullMatchL(TPtrC8& aToken, TInt aMatchPos, TBool aCopyToLocal = FALSE);
    TBool HandlePartialMatchL(TPtrC8& aToken, TInt aCurBufMatchPos);
    TBool HandleNoMatchL(TPtrC8& aToken, TInt aCurBufMatchPos);
    
    void HandleStateL(const TDesC8& aString, TInt aMatchPos);
    void HandleTitleLineL(const TDesC8& aString, TInt aMatchPos);
    void HandleHeaderL(const TDesC8& aString, TInt aMatchPos);
    void HandleOptionalHeaderL(const TDesC8& aString, TInt aMatchPos);
    void HandleBodyL(const TDesC8& aString, TInt aMatchPos);
    void HandleXtraCrlfL(const TDesC8& aString, TInt aMatchPos);
    void HandleEndofEndLineL(const TDesC8& aString, TInt aMatchPos);
        
    TMatchType FindToken(const TDesC8& aString, const TDesC8& aToken, TInt& aMatchPos);
        

private:
    MMSRPParserObserver& iConnection;
    RPointerArray<RMsrpBuf> iParseBuffers;
    TParseState iState;
    RMsrpBuf* iLocalBuf;//keep it above pool
    CMSRPMessageHandler* iMessage;
    CMSRPBufPool iBufPool;
    HBufC8* iEndToken;
    };
#endif /* CMSRPMSGPARSER_H_ */
