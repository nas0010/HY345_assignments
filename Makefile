#############################################
#
#Makefile
#@author:Nasia Boubouraki
#csd4692@csd.uoc.gr
#
#############################################

all: cs345sh.c
	gcc -lreadline shell cs345sh.c

clean: shell
	rm shell 

