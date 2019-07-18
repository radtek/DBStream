#include<stdio.h>
#include <stdint.h>
#include <string.h>
static uint16_t MAT[] =
{ 0x3030, 0x3130, 0x3230, 0x3330, 0x3430, 0x3530, 0x3630, 0x3730, 0x3830,
        0x3930, 0x3031, 0x3131, 0x3231, 0x3331, 0x3431, 0x3531, 0x3631, 0x3731,
        0x3831, 0x3931, 0x3032, 0x3132, 0x3232, 0x3332, 0x3432, 0x3532, 0x3632,
        0x3732, 0x3832, 0x3932, 0x3033, 0x3133, 0x3233, 0x3333, 0x3433, 0x3533,
        0x3633, 0x3733, 0x3833, 0x3933, 0x3034, 0x3134, 0x3234, 0x3334, 0x3434,
        0x3534, 0x3634, 0x3734, 0x3834, 0x3934, 0x3035, 0x3135, 0x3235, 0x3335,
        0x3435, 0x3535, 0x3635, 0x3735, 0x3835, 0x3935, 0x3036, 0x3136, 0x3236,
        0x3336, 0x3436, 0x3536, 0x3636, 0x3736, 0x3836, 0x3936, 0x3037, 0x3137,
        0x3237, 0x3337, 0x3437, 0x3537, 0x3637, 0x3737, 0x3837, 0x3937, 0x3038,
        0x3138, 0x3238, 0x3338, 0x3438, 0x3538, 0x3638, 0x3738, 0x3838, 0x3938,
        0x3039, 0x3139, 0x3239, 0x3339, 0x3439, 0x3539, 0x3639, 0x3739, 0x3839,
        0x3939 };
int p(unsigned long i,char * parsedData)
{
    int parsedDataSize=0;
    uint32_t A = i / 100000000;
    uint32_t YYYY = A/100;
    if(YYYY>1000&&YYYY<9999)//YYYY
    {
        uint16_t B = ((uint16_t) YYYY) / 100;
        *(uint16_t *) parsedData = MAT[B];
        *(uint16_t *) (parsedData + 2) = MAT[YYYY - B * 100];
        parsedDataSize = 4;
    }
    else
    {
    }
    parsedData[parsedDataSize]='-';
    uint32_t _A = i - A * 100000000;
    char *p = parsedData + parsedDataSize+1;
    *(uint16_t *) p = MAT[A-YYYY*100]; //MM
    p[2]='-';
    p+=3;
    register uint16_t C;
    uint16_t H = _A / 10000, L = _A - 10000 * H;
    C = H / 100;
    *(uint16_t *) p = MAT[C];//DD
    p[2]=' ';
    *(uint16_t *) (p + 2+1) = MAT[H - 100 * C];//HH
    p[5]=':';
    C = L / 100;
    *(uint16_t *) (p + 6) = MAT[C];//mm
    p[8]=':';
    *(uint16_t *) (p + 9) = MAT[L - 100 * C];//SS
    return parsedDataSize + 15;
}
int main()
{
    char s[23] = {0};
    int len = p(20140523123465,s);
    printf("%s,%d,%d\n",s,len,strlen(s));
}