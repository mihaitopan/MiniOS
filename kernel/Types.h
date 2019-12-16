#ifndef __TYPES_H__
#define __TYPES_H__
//
//  Author(s) : Mihai TOPAN
//

typedef          void      VOID,   *PVOID;
typedef          char 	   CHAR,   *PCHAR;
typedef unsigned char 	   BYTE,   *PBYTE;
typedef unsigned char 	   BOOL,   *PBOOL;
typedef          short 	   INT16,  *PINT16;
typedef unsigned short 	   UINT16, *PUINT16;
typedef          int 	   INT32,  *PINT32;
typedef unsigned int 	   UINT32, *PUINT32;
typedef          long long INT64,  *PINT64;
typedef unsigned long long UINT64, *PUINT64;

#define TRUE  1
#define FALSE 0

// #define NULL (PVOID)0
#define UINT64_MAX 18446744073709551615
#define PAGE_SIZE         4096

#endif // __TYPES_H__