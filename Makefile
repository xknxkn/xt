# $Id: Makefile,v 1.1 2001/02/09 06:50:31 querbach Exp $

# Makefile                       Copyright (C) 2001, Real-Time Systems Inc. 
#------------------------------------------ www.realtime.bc.ca ------------
#
#	Makefile
#
#	Compile, install, uninstall and clean "xt" program
# 
#	This program is free software; you can redistribute it and/or
#	modify it under the terms of the GNU General Public License
#	as published by the Free Software Foundation; either version 2
#	of the License, or (at your option) any later version.
#
#	This program is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#---------------------------------------------------------------------------


# Prefix of directory to store executable and man page.

prefix := /usr/local
 

# Rules

all: xt

xt : xt.c Makefile
	cc -O2 -o $@ $<

install : all
	cp xt $(prefix)/bin/
	cp xt.1 $(prefix)/man/man1/

uninstall : clean
	rm $(prefix)/bin/xt
	rm $(prefix)/man/man1/xt.1

clean :
	rm xt
