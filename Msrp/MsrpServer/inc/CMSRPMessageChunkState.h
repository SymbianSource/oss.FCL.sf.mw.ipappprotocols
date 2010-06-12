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

#ifndef CMSRPMESSAGECHUNKSTATE_H_
#define CMSRPMESSAGECHUNKSTATE_H_

// System Includes
#include <e32base.h>
#include <in_sock.h>

const TInt KMaxLgthOfTransactionIdString = 100;


class CMSRPMessageChunkState : public CBase
    {
    
public: // Constructors and destructor
       IMPORT_C static CMSRPMessageChunkState* NewL( );  
       IMPORT_C ~CMSRPMessageChunkState();
       
       
       IMPORT_C TBool GetResponseReceived( );
       IMPORT_C void SetResponseReceived(TBool aResponseReceived);
       IMPORT_C TDesC8& GetTransactionId( );
       IMPORT_C void SetTransactionId(TDesC8& aTransactionId );
       IMPORT_C void SetStartPos(TInt aStartPos);
       IMPORT_C void SetEndPos(TInt aEndPos);
       IMPORT_C TInt GetStartPos( );
       IMPORT_C TInt GetEndPos( );
       IMPORT_C void SetResponseSent(TBool aResponseSent);
       IMPORT_C TBool GetResponseSent( );
       
private:    
       
       CMSRPMessageChunkState();    
      // void ConstructL();
       
       

   
private:
         TInt iStartPos;
         TInt iEndPos;
         TBuf8< KMaxLgthOfTransactionIdString > iTransactionId;
         TBool iResponseReceived;
         TBool iResponseSent;
    };





#endif /* CMSRPMESSAGECHUNKSTATE_H_ */





