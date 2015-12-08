#ifndef __base_def_h__
#define __base_def_h__

#define DISABLE_COPY_AND_ASSIGN(theclass)\
    theclass(const theclass&);\
    void operator=(const theclass&);

#endif