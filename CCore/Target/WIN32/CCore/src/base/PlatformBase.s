# PlatformBase.s
#-----------------------------------------------------------------------------------------
#
#  Project: CCore 4.01
#
#  Tag: Target/WIN32
#
#  License: Boost Software License - Version 1.0 - August 17th, 2003
#
#            see http://www.boost.org/LICENSE_1_0.txt or the local copy
#
#  Copyright (c) 2020 Sergey Strukov. All rights reserved.
#
#-----------------------------------------------------------------------------------------

        .text

        .global __ZN5CCore15OptimizeBarrierEPvj

#-----------------------------------------------------------------------------------------

        .p2align 4,,15

  # CCore::OptimizeBarrier

__ZN5CCore15OptimizeBarrierEPvj:

        pushl   %ebp
        movl    %esp, %ebp

        popl    %ebp
        ret

#-----------------------------------------------------------------------------------------
