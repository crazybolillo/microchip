/*
* The purpose of this file is to let IDEs index conventional
* headers so that register declarations are available on IDEs 
* other than MPLAB.
*
* During compilation 'conventional' headers are not used and
* <xc.h> is used instead. Using <xc.h> is the recommended way
* to compile projects according to Microchip.
*/
#ifndef __PICONF__
#define __PICONF__

#ifndef __COMPILATION__
#   include <pic.h>
#   include <pic16f887.h>
#   else
#       include <xc.h>
#endif

#endif