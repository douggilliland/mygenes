/* 
 * File:   gedCompare.h
 * Author: doug
 *
 * Created on September 3, 2010, 10:42 PM
 */

#ifndef _GEDCOMPARE_H
#define	_GEDCOMPARE_H

#define DEBUG 1
#undef  DEBUG

#define DEBUG2 1

#define NOERR 0

#define ERRORLOADINGGED1    1
#define ERRORLOADINGGED2    2
#define MATCH1STERR         3
#define OPENBRANCHFOUND     4
#define NOHEADER1           5
#define NOHEADER2           6
#define EOFREACHED          -1

#define STARTSTATE          0
#define LOOKINGFORINDNUM    1
#define LOOKINGFORNAME      2
#define LOOKINGFORBDATE     3
#define LOOKINGFORBDVAL     4
#define LOOKINGFORFAMILY    5
#define LOOKINGFORFAMINXT   6

#endif	/* _GEDCOMPARE_H */

