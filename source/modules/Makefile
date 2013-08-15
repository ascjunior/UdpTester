include ../../config.mk

COBJS	= $(MODULESOBJS)

all: $(COBJS)
	cp $(COBJS) $(BASEDIR)/

.c.o: force_look
	$(ECHO) compiling modules directory: $*.c $(CFLAGS)
	$(CC) -c $*.c $(CFLAGS)

clean:
	$(ECHO) cleaning up in modules directory
	$(RM) -f $(COBJS)
	
cleanSilent:
	$(RM) -f $(COBJS)

force_look:
	true

