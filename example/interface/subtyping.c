//structural subtyping: T < S
typedef struct {
  ...
} S;

typedef struct {
  S S;
  ...
} T;
