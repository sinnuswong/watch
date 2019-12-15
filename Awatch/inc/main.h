#if !defined(_$(appNameUpper)_H_)
#define _$(appNameUpper)_H_

#include <app.h>
#include <dlog.h>

#if !defined(PACKAGE)
#define PACKAGE "$(packageName)"
#endif

#ifdef  LOG_TAG
#undef  LOG_TAG
#endif
#define LOG_TAG "$(appName)"

#endif
