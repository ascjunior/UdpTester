include ../../config.mk

COBJS	= $(TOOLSOBJS)

all: $(COBJS)
	cp $(COBJS) $(BASEDIR)/

.c.o: force_look
	$(ECHO) compiling tools directory: $*.c $(CFLAGS)
	$(CC) -c $*.c $(CFLAGS)

clean:
	$(ECHO) cleaning up in tools directory
	$(RM) -f $(COBJS)
	
cleanSilent:
	$(RM) -f $(COBJS)

force_look:
	true

