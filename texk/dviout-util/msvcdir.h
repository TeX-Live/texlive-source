/*/msvcdir.h */

#if !defined(MSVCDIR_H)
#define MSVCDIR_H
#include <stdlib.h>  /*_splitpath */
#include <direct.h>
#include <assert.h> /*assert */

struct ftime{
	unsigned ft_tsec:  5;
	unsigned ft_min:   6;
	unsigned ft_hour:  5;
	unsigned ft_day:   5;
	unsigned ft_month: 4;
	unsigned ft_year:  7;
};
#define MAXPATH _MAX_PATH
#define MAXFILE _MAX_FNAME
#define MAXDIR _MAX_DIR
#define MAXDRIVE _MAX_DRIVE
#define MAXEXT _MAX_EXT

#define fnsplit _splitpath

void getftime(int handle, struct ftime *ft);

#if	defined(MSVC) && !defined(_WARNING_)
#define	_WARNING_
#pragma warning( disable : 4244 4018 4102 4761 4146 4047 4305 4245 4113)
#endif

#endif /* MSVCDIR_H */
