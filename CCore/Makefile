# Makefile
#----------------------------------------------------------------------------------------
#
#  Project: CCore 4.01
#
#  License: Boost Software License - Version 1.0 - August 17th, 2003 
#
#            see http://www.boost.org/LICENSE_1_0.txt or the local copy
#
#  Copyright (c) 2020 Sergey Strukov. All rights reserved.
#
#----------------------------------------------------------------------------------------

include Makefile.host

OPT = -j8

all:
	make $(OPT) -C Target/$(CCORE_TARGET)

clean:
	make -C Target/$(CCORE_TARGET)       clean

list:
	make -C Target/WIN32                 list


