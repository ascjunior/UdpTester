########################################################################
# \file Makefile                                                       #
#                                                                      #
# \author Alberto Carvalho <ascjunior.07@gmail.com>                    #
# \date Tue Jun 25 17:00:55 BRT 2013                                   #
#                                                                      #
########################################################################

BASEDIR = $(shell pwd)
export BASEDIR

TARGET=UdpTester

DOCS_DIR = docs
DOXY_LOG= doxy_log
DOXYFILE=$(TARGET)_doxy

include config.mk

DEPS=objMain objTools

all: $(TARGET)

$(TARGET): $(DEPS)
	$(ECHO) compiling $(TARGET) : $(CC) -o $(TARGET) $(CFLAGS) $(COBJS) $(LIBS)
	$(CC) -o $(TARGET) $(CFLAGS) $(COBJS) $(LIBS)
	$(STRIP) $(TARGET)
	$(RM) -f *.o
	for d in $(DIR_LIST); do (cd $$d; $(MAKE) cleanSilent ); done

objMain: force_look
	$(ECHO) looking into $(DIR_MAIN) : $(MAKE) $(MFLAGS)
	cd $(DIR_MAIN); $(MAKE) $(MFLAGS)

objTools: force_look
	$(ECHO) looking into $(DIR_TOOLS) : $(MAKE) $(MFLAGS)
	cd $(DIR_TOOLS); $(MAKE) $(MFLAGS)

clean:
	$(ECHO) cleaning up in main directory
	$(RM) -rf *.o $(TARGET) $(DOCS_DIR) $(DOXY_LOG) $(DOXYFILE).orig
	for d in $(DIR_LIST); do (cd $$d; $(MAKE) clean ); done

force_look:
	true
