# Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
# * @file
# * creates sample rtp packets for unittesting
# * @internalComponent
# 
#

use File::Copy;

my $plat = shift @ARGV;
my $cfg = shift @ARGV;

my $epocroot;
sub Check_Epocroot 
	{
	$epocroot = $ENV{EPOCROOT};
	die "ERROR: Must set the EPOCROOT environment variable\n" if (!defined($epocroot));
	$epocroot =~ s-/-\\-go;	# for those working with UNIX shells
	die "ERROR: EPOCROOT must not include a drive letter\n" if ($epocroot =~ /^.:/);
	die "ERROR: EPOCROOT must be an absolute path without a drive letter\n" if ($epocroot !~ /^\\/);
	die "ERROR: EPOCROOT must not be a UNC path\n" if ($epocroot =~ /^\\\\/);
	die "ERROR: EPOCROOT must end with a backslash\n" if ($epocroot !~ /\\$/);
	die "ERROR: EPOCROOT must specify an existing directory\n" if (!-d $epocroot);

	$epocroot=~ s-\\$--;		# chop trailing \\
	}

Check_Epocroot();

$found = 0;
$dontmakethis = 0;

my $tmpfile = "$epocroot\\epoc32\\build\\rtp.test.tmp";

sub makeit {
	if ($dontmakethis) {
		return;
	}
	open TEMPFILE, ">$tmpfile" or die "Error opening output file $tmpfile";
	binmode TEMPFILE, ":raw";
	$packet = pack("lNN", $flags, $timestamp, $ssrc);
	if ($csrc) {
		foreach $one_csrc (split /,/, $csrc) {
			$packet .= pack ("N", $one_csrc);
		}
	}
	if ($header) {
		($head, $length, $rest) = split /,/, $header, 3;
		$packet .= pack ("nn", $head, $length);
		foreach $one_header_word (split /,/, $rest) {
			$packet .= pack ("l", $one_header_word);
		}
	}
	$packet .= $payload;

#	if padding bit set then...
	if ($flags & 32 && $pad == 0) {
		# Need some padding. 1 Byte will do
		$packet .= pack("c", 1);
	}
	elsif ($flags & 32) {
		# Pad the specified ammount
		for ($i= 0; $i < $pad-1; $i++) {
		# zero is the 'correct' padding
		$packet .= pack("c", 0);
		}
		#length as last octet
		$packet .= pack("c", $pad);
	}

	print TEMPFILE "$packet";
	close TEMPFILE;

	if ($filename =~ m/^z:/) {
		$realfn = $filename;
		if ($plat =~ m/wins|winscw|WINS|WINSCW/)
		{
			$realfn =~ s!z:!$epocroot\\epoc32\\release\\$plat\\$cfg\\z!;
			print "Creating packet file $realfn\n";
			copy("$tmpfile", "$realfn") or die "Error copying";
		}
		$realfn = $filename;
		$realfn =~ s!z:!$epocroot\\epoc32\\data\\z!;
		print "Creating packet file $realfn\n";
		copy("$tmpfile", "$realfn") or die "Error copying";
		
	}
	$found = 0;
	$dontmakethis = 0;
#nothing set in flags but version (V=2) 0x8000 0000 in big endian
	$flags=128;
	$timestamp=0;
	$ssrc=0;
	$header=0;
	$csrc="";
	$payload="";
}

while (<>) {
	if (m/^\[/ && $found) {makeit;}
	if (m/^PacketFile\s*=\s*(\S*)/)  {
		$found = 1;
		$filename = $1;
	}
	if (m/^DontMakeThis/)  {
		$dontmakethis = 1;
	}
	if (m/^Flags\s*=\s*(\S*)/) {
		$flags = $1;
	}
	if (m/^Timestamp\s*=\s*(\S*)/) {
		$timestamp = $1;
	}
	if (m/^SSRC\s*=\s*(\S*)/) {
		$ssrc = $1;
	}
	if (m/^HeaderExtension\s*=\s*(\S*)/) {
		$header = $1;
	}
	if (m/^CSRC\s*=\s*(\S*)/) {
		$csrc = $1;
	}
	if (m/^Payld\s*=\s*(.*)/) {
		$payload = $1;
	}
	if (m/^Pad\s*=\s*(.*)/) {
		$pad = $1;
	}
	
}

if ($found) { makeit;}
