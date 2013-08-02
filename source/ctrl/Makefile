include ../../config.mk

COBJS	= $(CTRLOBJS)

all: $(COBJS)
	cp $(COBJS) $(BASEDIR)/

.c.o: force_look
	$(ECHO) compiling ctrl directory: $*.c $(CFLAGS)
	$(CC) -c $*.c $(CFLAGS)

clean:
	$(ECHO) cleaning up in ctrl directory
	$(RM) -f $(COBJS)
	
cleanSilent:
	$(RM) -f $(COBJS)

force_look:
	true

