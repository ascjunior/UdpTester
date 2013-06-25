########################################################################
# \file Makefile                                                       #
#                                                                      #
# \author Alberto Carvalho <ascjunior.07@gmail.com>                    #
# \date Tue Jun 25 17:02:27 BRT 2013                                   #
#                                                                      #
########################################################################

include ../config.mk

COBJS	= $(MAINOBJS)

all: $(COBJS)
	cp $(COBJS) $(BASEDIR)/

.c.o: force_look
	$(ECHO) compiling source directory: $*.c $(CFLAGS)
	$(CC) -c $*.c $(CFLAGS)

clean:
	$(ECHO) cleaning up in source directory
	$(RM) -f $(COBJS)
	
cleanSilent:
	$(RM) -f $(COBJS)

force_look:
	true
