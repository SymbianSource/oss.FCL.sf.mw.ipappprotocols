/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : TTCPMsgState.h
* Part of     : ConnectionMgr
* Version     : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef __TTCPMSGSTATE_H__
#define __TTCPMSGSTATE_H__

// INCLUDES
#include <e32base.h>


// FORWARD DECLARATIONS
class MMsgAssemblerContext;

/*
 * Base class for all TCP message states.
 */
class TTCPMsgState 
	{
public:
	virtual ~TTCPMsgState() {};

protected:

	TTCPMsgState(MMsgAssemblerContext& aMsgAssembler);

public:

	virtual void DataReceivedL( TPtr8 aData, TUint& aNextLength ) = 0;

protected:

   /**
    * find first sip message start position from the parameter data,
    * 
    * @param aData TCP messages data received.
    * @return sip message header start postion if it could be found,
    *         otherwise KErrNotFound. 
    */
    TInt FindHeaderStartPosition( TDesC8& aData ); 
    
   /**
    * find first sip message end position from the parameter data,
    * 
    * @param aData TCP messages data received.
    * @return sip message header end postion if it could be found,
    *         otherwise KErrNotFound. 
    */
    TInt FindHeaderEndPosition( TDesC8& aData );  
    
   /** get header end position by compare it with the new header end
	* position currentlly found.
	*/
	void GetHeaderPartEndPosition ( TInt  aEndMarkPosition, 
		                            TInt  aEndMarkLength,  
		                            TInt& aHeaderEndPosition );         
    
protected:
    MMsgAssemblerContext& iMsgAssembler;
    
#ifdef CPPUNIT_TEST
    friend class CTCPMsgStatesTest;
#endif
	};

#endif // end of __TTCPMSGSTATE_H__

// end of file
