#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern float dot_product(size_t N, const float* A, const float* B);

void main_part_2()
{
    size_t N;
    scanf("%"SCNu64, &N);
    float* A = calloc(N, sizeof(*A));
    float* B = calloc(N, sizeof(*B));
    for (int i = 0; i < N; ++i) {
        scanf("%f", &(A[i]));
    }
    for (int i = 0; i < N; ++i) {
        scanf("%f", &(B[i]));
    }
    float res = dot_product(N, A, B);
    printf("%.9f\n", res);
    free(A);
    free(B);
}

int main()
{
    main_part_2();
}
