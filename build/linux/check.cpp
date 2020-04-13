#include <stdio.h>
#include "../../src/fks/fks_common.h"

int main(void)
{
    printf("sizeof(long) = %d\tFKS_LONG_BIT=%d\n", (int)sizeof(long), FKS_LONG_BIT);
    printf("sizeof(long long) = %d\tFKS_LLONG_BIT=%d\n", (int)sizeof(long long), FKS_LLONG_BIT);
    printf("sizeof(long double) = %d\tFKS_LDOUBLE_BIT=%d\n", (int)sizeof(long double), FKS_LDOUBLE_BIT);
    printf("sizeof(void*) = %d\tFKS_PTR_BIT=%d\n", (int)sizeof(void*), FKS_PTR_BIT);
    return 0;
}
