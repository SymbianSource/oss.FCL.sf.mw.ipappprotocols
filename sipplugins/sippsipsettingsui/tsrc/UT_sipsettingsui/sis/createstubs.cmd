@echo off
rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:
rem
@echo on
@echo off
makesis -s sipsettingsui_stubs.pkg 
copy sipsettingsui_stubs.sis \epoc32\data\Z\system\install
echo copy sipsettingsui_stubs.iby epoc32_rom_include (copy temporarily disabled as iby location changed in SF, copy to correct place)

echo Stub creation is completed!
echo Remember to add the following line into the s60.iby file:
echo " #include <sipsettingsui_stubs.iby> "