#ifndef PATH_TO_HEADER_H
#define PATH_TO_HEADER_H

//#include <assert.h>

typedef struct T        *T;
typedef struct TMethods *TMethods;
typedef union  TX1       TX1;
typedef union  TX2       TX2;

// T       T_New    (...);       //impl
// int     T_Init   (T, ...);    //impl
// int     T_Destroy(T *);       //T_Gc(T *)
// TX1     T_X1     (T);
// TX2     T_X2     (T);
// F1      T_F1     (T, ...);

struct T {
  TMethods  methods;
  void     *data;
};

union TX1 {
  T1 T1;
  T2 T2;
};
union TX2 {
  T1 T1;
  T2 T2;
};

struct TMethods {
//T    (*T_New)   (...);       //in implement
//int  (*T_Init)  (T, ...);    //in implement

  int  (*Destroy) (T *);
  TX1  (*X1)      (T);
  TX2  (*X2)      (T);
  F1   (*F1)      (T, ...);
};

//////
//
//static int T_Destroy(T *o)
//{
//  free(*o);
//  *o = NULL;
//}
//
//static TX1 T_X1(T o)
//{
//  assert(o);
//  return o->x1;
//}
//
//static F1 T_F1(T o, ...)
//{
//  return o->methods->F1(o, ...);
//}
//
#endif // PATH_TO_HEADER_H