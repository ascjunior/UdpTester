DIR_INCLUDE = $(BASEDIR)/include
CC=gcc
CXX=g++
STRIP=strip
ECHO=echo
RM=rm
MFLAGS= -s

DIR_MAIN = $(BASEDIR)/source
DIR_TOOLS = $(DIR_MAIN)/tools
DIR_THREADS = $(DIR_MAIN)/threads
DIR_CTRL = $(DIR_MAIN)/ctrl
DIR_LIST= $(DIR_MAIN) $(DIR_TOOLS) $(DIR_THREADS) $(DIR_CTRL)
	
CFLAGS:=-Wall -Werror \
	-I$(DIR_INCLUDE)

CFLAGS+= -DDEBUG_LEVEL
CFLAGS+= -DDEBUG_LEVEL_DEFAULT=0
	
MAINOBJS=main.o
TOOLSOBJS=parser.o
CTRLOBJS=ctrl.o
THREADSOBJS=recvctrl_thread.o sendctrl_thread.o recvtest_thread.o sendtest_thread.o

COBJS=$(MAINOBJS) $(TOOLSOBJS) $(CTRLOBJS) $(THREADSOBJS)

DEPS=objMain objTools objThreads objCtrl

LIBS:=-lpthread
	
.SILENT:
