// type.h

#ifndef PATH_TO_HEADER_H
#define PATH_TO_HEADER_H

typedef struct T   *T;

//inline static
T        T_New     (...);
int      T_Init    (T, ...);
int      T_Destroy (T *);       //T_Gc(T *)
X1       T_X1      (T);
F1       T_F1      (T, ...);

//////
#include <assert.h>

struct T {
  ...;
};

#define T(...) (&(struct T){...})

#endif // PATH_TO_HEADER_H
