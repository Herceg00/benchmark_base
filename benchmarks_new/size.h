#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var "=" VALUE(var)

//#pragma message VAR_NAME_VALUE(SIZE_PREDEF)

// #define BLOCK_SIZE_PREDEF 4 // why is it here?
#define SHIFT_PREDEF 6

//#include "size.h.1"
#include "size.h.30"
//#include "size.h.tiny"
//#include "size.h.less_tiny"
