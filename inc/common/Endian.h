#ifndef FND_ENDIAN_H_
#define FND_ENDIAN_H_

/*
 * Autodetect system endianess
 */

#ifndef __BYTE_ORDER
  #ifdef __linux__
    #include <endian.h>
  #elif defined (__OpenBSD__) || defined (__FreeBSD__) || \
        defined (__NetBSD__) || defined (__APPLE__)

    #include <machine/endian.h>
    #define __BYTE_ORDER     BYTE_ORDER
    #define __LITTLE_ENDIAN  LITTLE_ENDIAN
    #define __BIG_ENDIAN     BIG_ENDIAN

  #else

    #ifndef __LITTLE_ENDIAN
      #define __LITTLE_ENDIAN   1234
    #endif

    #ifndef __BIG_ENDIAN
      #define __BIG_ENDIAN      4321
    #endif

    #ifdef __LITTLE_ENDIAN__
      #define __BYTE_ORDER __LITTLE_ENDIAN
    #endif /* __LITTLE_ENDIAN */

    #if defined (i386) || defined (__i386__)
      #define __BYTE_ORDER __LITTLE_ENDIAN
    #endif /* defined i386 */

    #if defined (_WIN32)
      #define __BYTE_ORDER __LITTLE_ENDIAN
    #endif /* defined _WIN32 */

    #if defined (sun) && defined (unix) && defined (sparc)
      #define __BYTE_ORDER __BIG_ENDIAN
    #endif /* sun unix sparc */

  #endif /* linux */

#endif /* __BYTE_ORDER */

#ifndef __BYTE_ORDER
  #error Need to know endianess
#endif

#endif /*FND_ENDIAN_H_*/
