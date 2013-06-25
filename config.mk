DIR_INCLUDE = $(BASEDIR)/include
CC=gcc
CXX=g++
STRIP=strip
ECHO=echo
RM=rm
MFLAGS= -s

DIR_MAIN = $(BASEDIR)/source
DIR_LIST= $(DIR_MAIN) 
	
CFLAGS:=-Wall -Werror \
	-I$(DIR_INCLUDE)

#CFLAGS+= -DBBBBB
	
MAINOBJS=main.o

COBJS=$(MAINOBJS)

LIBS:=
	
.SILENT:
