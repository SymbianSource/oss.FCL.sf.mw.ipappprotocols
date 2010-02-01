# we do not really  know what files get genersted...
# controled by the ini file

do_nothing:
	@rem do nothing
#
# The targets invoked by bld...
#

MAKMAKE : do_nothing

BLD : do_nothing

SAVESPACE : do_nothing

CLEAN : do_nothing

FREEZE : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE : do_nothing

FINAL :
	perl ..\scripts\makepackets.pl $(PLATFORM) $(CFG) ..\scripts\rtp_unittest.ini

RELEASABLES : do_nothing
