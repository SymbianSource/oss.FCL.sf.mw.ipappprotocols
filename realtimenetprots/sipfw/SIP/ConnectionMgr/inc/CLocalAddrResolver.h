/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CLocalAddrResolver.h
* Part of       : ConnectionMgr
* Version       : SIP/5.0 
*
*/




/**
 @internalComponent
*/

#ifndef CLOCALADDRRESOLVER_H
#define CLOCALADDRRESOLVER_H

#include <in_sock.h>


class CLocalAddrResolver : public CBase
	{ 
public:

	static CLocalAddrResolver* NewL(
	    RSocketServ& aServer,
	    RConnection& aConnection);
	
	static CLocalAddrResolver* NewLC(
	    RSocketServ& aServer,
	    RConnection& aConnection);
	
	~CLocalAddrResolver();
	
	const TInetAddr& IPv6Addr() const;
	
	TBool HasIPv4Addr()	const;
	
	const TInetAddr& IPv4Addr() const;	
	
	TBool IsLocalAddr(const TInetAddr& aAddr) const;
	
	void RefreshLocalAddressesL();

private:

	void ConstructL();

	CLocalAddrResolver(
	    RSocketServ& aServer,
	    RConnection& aConnection);

    TBool Match(const TSoInetInterfaceInfo& aInfo,
                const TSoInetIfQuery& aQuery) const;
    
    void SetAddress(const TInetAddr& aAddr);
    
private: // data

    TUint32 iIapId;
	RSocketServ& iServer;
	RConnection& iConnection;
	TInetAddr iIPv6Addr;
	TInetAddr iIPv4Addr;
	
private: //For testing purposes
#ifdef CPPUNIT_TEST
    friend class CLocalAddrResolverTest;
    friend class CSipConnectionTest;	
#endif
	};
	
#endif // CLOCALADDRRESOLVER_H

// End of File
