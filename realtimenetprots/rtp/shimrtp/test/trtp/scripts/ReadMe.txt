FOr the two test cases of "OpenSessionRconnect" in the rtp_unittest.script file , one needs to follow this procedure .

Procedure :

The test cases comprising "OPenSessionRConnect" need to be having ethernet setting set up .i.e epoc needs to have an IP extracted from DNS for these two cases .
 For setting up Ethernet follow these steps :

1. Install WinPCap 3.0 (please note that do not install any other version of WinPCap !)
2. Make sure that you have configchange.pl and netcards in ur \epoc32\tools directory.
3.You need to get fake MAC registered with ISSupport. Send a mail with subject "fake MAC registration" to ISSupport.
4. Run the following depending upon whether u have eka1 or eka2
	for EKA1 wins:
		configchange.pl --config ethernetWithCommDB --kernel EKA1 --target wins --variant udeb
	for EKA1 winscw
		configchange.pl --config ethernetWithCommDB --kernel EKA1 --target winscw --variant udeb
	for EKA2 winscw
		configchange.pl --config ethernetWithCommDB --kernel EKA2 --target winscw --variant udeb

5.Do a "ced -i c:\etherentced.xml" on the epoc emulator 

This should change your settings and you will get an epoc IP . To check try pinging www.google.com from your emulator.
 

Hardware

In case of hardware also the procedure more or less remains same .YOu need to follow these two steps :

1. You need to get fake MAC registered (for the hardware you are testing) with ISSupport. Send a mail with subject "fake MAC registration" to ISSupport.
2. On the hardware Emulator run "ced -i c:\etherentced.xml".

This should get you an epoc IP for Hardware.

                                             