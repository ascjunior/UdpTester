DIR_INCLUDE = $(BASEDIR)/include
CC=gcc
CXX=g++
STRIP=strip
ECHO=echo
RM=rm
MFLAGS= -s

DIR_MAIN = $(BASEDIR)/source
DIR_TOOLS = $(DIR_MAIN)/tools
DIR_LIST= $(DIR_MAIN) $(DIR_TOOLS)
	
CFLAGS:=-Wall -Werror \
	-I$(DIR_INCLUDE)

#CFLAGS+= -DBBBBB
	
MAINOBJS=main.o
TOOLSOBJS=parser.o

COBJS=$(MAINOBJS) $(TOOLSOBJS)

LIBS:=
	
.SILENT:
