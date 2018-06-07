//
// Created by m2 on 18/1/20.
//

int main(void)
{
    int x = 78965;
    long y = 0;
    int i = 0;
    for (;;) {
        if (x > 0) {
            char *y1 = (char *)&y;
            char *x1 = (char *)&x;
            *y1 |= *x1 << i;
            if (*x1 & 0x80) {
                *y1 |= 0x80;
                ((char *)&y)[1] = x1 >> 7-i;
            } else {
                break;
            }
        } else {
            break;
        }
    }
}