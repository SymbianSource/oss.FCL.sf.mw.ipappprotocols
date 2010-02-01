@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description:
@rem Backup of rtp logs - start
@rem

md c:\logs\
md c:\logs\rtp
md c:\logs\tcpip6

debugport 3
copy z:\commsdbg.ini c:\logs\
del c:\logs\testexecute\*
del c:\logs\rtp\*
del c:\logs\log.txt
del c:\logs\tcpip6\*

testexecute z:\testdata\scripts\rtp\rtpcore_unittest.script -d

copy c:\logs\rtp\rtpcorelog.txt c:\logs\rtp\rtpcorelog_t1.txt
del c:\logs\rtp\rtpcorelog.txt
copy c:\logs\rtp\rtplog.txt c:\logs\rtp\rtplog_t1.txt
del c:\logs\rtp\rtplog.txt

testexecute z:\testdata\scripts\rtp\rtp_unittest.script -d

md f:\ak_logs\
del f:\ak_logs\*
copy c:\logs\log.txt f:\ak_logs\
copy c:\logs\testexecute\* f:\ak_logs\
copy c:\logs\rtp\* f:\ak_logs\
copy c:\logs\tcpip6\* f:\ak_logs\

md e:\ak_logs\
del e:\ak_logs\*
copy c:\logs\log.txt e:\ak_logs\
copy c:\logs\testexecute\* e:\ak_logs\
copy c:\logs\rtp\* e:\ak_logs\
copy c:\logs\tcpip6\* e:\ak_logs\
