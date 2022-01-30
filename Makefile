# Makefile
#----------------------------------------------------------------------------------------
#
#  Project: vmake 1.00
#
#  License: Boost Software License - Version 1.0 - August 17th, 2003
#
#            see http://www.boost.org/LICENSE_1_0.txt or the local copy
#
#  Copyright (c) 2022 Sergey Strukov. All rights reserved.
#
#----------------------------------------------------------------------------------------

OPT = -j8

all:
	make -C CCore all
	make -C vmake all

clean:
	make -C CCore clean
	make -C vmake clean

list:
	make -C CCore list
	make -C vmake list


