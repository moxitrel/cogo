// a_type.h
#ifndef PATH_TO_HEADER_H
#define PATH_TO_HEADER_H

#include "interface.h"
//#include <assert.h>

T        AT_New     (...);
int      AT_Init    (T, ...);
int      AT_Destroy (T *);
TX1      AT_X1      (T);
TX2      AT_X2      (T);
F1       AT_F1      (T, ...);

const struct Methods A_T_METHODS = {
  .Destroy  = AT_Destroy,
  .X1       = AT_X1,
  .X2       = At_X2,
  .F1       = AT_F1,
};

#define A_T(...) (&(struct Type){...})

#endif // PATH_TO_HEADER_H

/*

typedef struct Methods  *Methods;

typedef struct Type {
  Methods methods;
  union {
    Type1 Type1;
    Type2 Type2;
  };
} *Type;


#define OBJECT(METHODS, FIELDS) (&(struct Object){.methods = (METHODS), .fields = (FIELDS)})

*/