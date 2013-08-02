include ../../config.mk

COBJS	= $(THREADSOBJS)

all: $(COBJS)
	cp $(COBJS) $(BASEDIR)/

.c.o: force_look
	$(ECHO) compiling threads directory: $*.c $(CFLAGS)
	$(CC) -c $*.c $(CFLAGS)

clean:
	$(ECHO) cleaning up in threads directory
	$(RM) -f $(COBJS)
	
cleanSilent:
	$(RM) -f $(COBJS)

force_look:
	true

