//
// Created by M2 on 2019-01-26.
//

void bench_fcase(void);
void bench_flabel(void);
void bench_flineno(void);
void bench_acase(void);
void bench_ccase(void);

int main(void)
{
    int N = 1;
    for (int i = 0; i < N; i++) {
        bench_flineno();
        bench_fcase();
        bench_flabel();
        bench_acase();
        bench_ccase();
    }
}