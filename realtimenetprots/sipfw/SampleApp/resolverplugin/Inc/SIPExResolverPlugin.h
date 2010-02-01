/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



#ifndef __CSIPEXRESOLVERPLUGIN_H__
#define __CSIPEXRESOLVERPLUGIN_H__

// INCLUDES
#include <apgcli.h>
#include <sipresolvedclient.h>


const TInt32 KSIPExResolverPluginUID = 0xA00001EC;

// CLASS DEFINITION

class CSIPExResolverPlugin : public CSIPResolvedClient
	{
	public:	// Constructors and destructor

		static CSIPExResolverPlugin* NewL();
		
		/// Destructor
		~CSIPExResolverPlugin();

	public: // from CSIPResolvedClient
	
	    TUid ChannelL( RStringF aMethod,
                       const TDesC8& aRequestUri,
                       const RPointerArray<CSIPHeaderBase>& aHeaders,
                       const TDesC8& aContent,
                       const CSIPContentTypeHeader* aContentType=0);

		void ConnectL( TUid aUid );
		
		const TDesC8& Capabilities();

	private: // Constructors

		inline CSIPExResolverPlugin() {}
		
		// Second phase constructor
		void ConstructL(); 
	
	private: // New functions

		// Needed for cleanup of a RImplInfoPtrArray:
		static void ResetAndDestroy( TAny* anArray );

		// Name
		RApaLsSession iApaSession;
		TApaAppInfo iAppInfo;
		
		TUid iApplicationUID;
	};

#endif // __CSIPEXRESOLVERPLUGIN_H__
