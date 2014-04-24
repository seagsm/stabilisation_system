
#include "api_filters.h"


/**************************************************************
WinFilter version 0.8
http://www.winfilter.20m.com
akundert@hotmail.com

Filter type: Low Pass
Filter model: Butterworth
Filter order: 4
Sampling Frequency: 250 Hz
Cut Frequency: 10.000000 Hz
Coefficents Quantization: float

Z domain Zeros
z = -1.000000 + j 0.000000
z = -1.000000 + j 0.000000
z = -1.000000 + j 0.000000
z = -1.000000 + j 0.000000

Z domain Poles
z = 0.787620 + j -0.077389
z = 0.787620 + j 0.077389
z = 0.884414 + j -0.209794
z = 0.884414 + j 0.209794
***************************************************************/
#define NCoef 4
float float_api_filters_iir_acc_x(float float_new_sample)
{
    float ACoef[NCoef+1] = {
        0.00018539265053394222f,
        0.00074157060213576886f,
        0.00111235590320365330f,
        0.00074157060213576886f,
        0.00018539265053394222f
    };

    float BCoef[NCoef+1] = {
        1.00000000000000000000f,
        -3.34406783771187310000f,
        4.23886395088406330000f,
        -2.40934285658631710000f,
        0.51747819978804011000f
    };
    
    /* Output samples. */
    static float y[NCoef+1]; 
    
    /* Input samples.  */
    static float x[NCoef+1]; 
    int n;

    /* shift the old samples */
    for(n = NCoef; n > 0; n--)
    {
       x[n] = x[n-1];
       y[n] = y[n-1];
    }

    /* Calculate the new output */
    x[0] = float_new_sample;
    y[0] = ACoef[0] * x[0];
    for(n = 1; n <= NCoef; n++)
    {
        y[0] += ACoef[n] * x[n] - BCoef[n] * y[n];
    }
    return y[0];
}

float float_api_filters_iir_acc_y(float float_new_sample)
{
    float ACoef[NCoef+1] = {
        0.00018539265053394222f,
        0.00074157060213576886f,
        0.00111235590320365330f,
        0.00074157060213576886f,
        0.00018539265053394222f
    };

    float BCoef[NCoef+1] = {
        1.00000000000000000000f,
        -3.34406783771187310000f,
        4.23886395088406330000f,
        -2.40934285658631710000f,
        0.51747819978804011000f
    };
    
    /* Output samples. */
    static float y[NCoef+1]; 
    
    /* Input samples.  */
    static float x[NCoef+1]; 
    int n;

    /* shift the old samples */
    for(n = NCoef; n > 0; n--)
    {
       x[n] = x[n-1];
       y[n] = y[n-1];
    }

    /* Calculate the new output */
    x[0] = float_new_sample;
    y[0] = ACoef[0] * x[0];
    for(n = 1; n <= NCoef; n++)
    {
        y[0] += ACoef[n] * x[n] - BCoef[n] * y[n];
    }
    return y[0];
}

float float_api_filters_iir_acc_z(float float_new_sample)
{
    float ACoef[NCoef+1] = {
        0.00018539265053394222f,
        0.00074157060213576886f,
        0.00111235590320365330f,
        0.00074157060213576886f,
        0.00018539265053394222f
    };

    float BCoef[NCoef+1] = {
        1.00000000000000000000f,
        -3.34406783771187310000f,
        4.23886395088406330000f,
        -2.40934285658631710000f,
        0.51747819978804011000f
    };
    
    /* Output samples. */
    static float y[NCoef+1]; 
    
    /* Input samples.  */
    static float x[NCoef+1]; 
    int n;

    /* shift the old samples */
    for(n = NCoef; n > 0; n--)
    {
       x[n] = x[n-1];
       y[n] = y[n-1];
    }

    /* Calculate the new output */
    x[0] = float_new_sample;
    y[0] = ACoef[0] * x[0];
    for(n = 1; n <= NCoef; n++)
    {
        y[0] += ACoef[n] * x[n] - BCoef[n] * y[n];
    }
    return y[0];
}


/**************************************************************
WinFilter version 0.8
http://www.winfilter.20m.com
akundert@hotmail.com

Filter type: Low Pass
Filter model: Butterworth
Filter order: 4
Sampling Frequency: 250 Hz
Cut Frequency: 42.000000 Hz
Coefficents Quantization: float

Z domain Zeros
z = -1.000000 + j 0.000000
z = -1.000000 + j 0.000000
z = -1.000000 + j 0.000000
z = -1.000000 + j 0.000000

Z domain Poles
z = 0.273139 + j -0.184598
z = 0.273139 + j 0.184598
z = 0.369637 + j -0.603107
z = 0.369637 + j 0.603107
***************************************************************/
#define NCoef 4
float float_api_filters_iir_gyro_x(float float_new_sample)
{
    float ACoef[NCoef+1] = {
        0.02675265688403948700f,
        0.10701062753615795000f,
        0.16051594130423694000f,
        0.10701062753615795000f,
        0.02675265688403948700f
    };

    float BCoef[NCoef+1] = {
        1.00000000000000000000f,
        -1.28555068801461640000f,
        1.01289917136114170000f,
        -0.35368582226312562000f,
        0.05438080336347629600f
    };
    
    /* output samples*/
    static float y[NCoef+1]; 
    /* input samples */
    static float x[NCoef+1]; 
    int n;

    /* shift the old samples */
    for(n = NCoef; n>0; n--)
    {
       x[n] = x[n-1];
       y[n] = y[n-1];
    }

    /* Calculate the new output */
    x[0] = float_new_sample;
    y[0] = ACoef[0] * x[0];
    for(n = 1; n <= NCoef; n++)
    {
        y[0] += ACoef[n] * x[n] - BCoef[n] * y[n];
    }
    return y[0];
}

float float_api_filters_iir_gyro_y(float float_new_sample)
{
    float ACoef[NCoef+1] = {
        0.02675265688403948700f,
        0.10701062753615795000f,
        0.16051594130423694000f,
        0.10701062753615795000f,
        0.02675265688403948700f
    };

    float BCoef[NCoef+1] = {
        1.00000000000000000000f,
        -1.28555068801461640000f,
        1.01289917136114170000f,
        -0.35368582226312562000f,
        0.05438080336347629600f
    };
    
    /* output samples*/
    static float y[NCoef+1]; 
    /* input samples */
    static float x[NCoef+1]; 
    int n;

    /* shift the old samples */
    for(n = NCoef; n>0; n--)
    {
       x[n] = x[n-1];
       y[n] = y[n-1];
    }

    /* Calculate the new output */
    x[0] = float_new_sample;
    y[0] = ACoef[0] * x[0];
    for(n = 1; n <= NCoef; n++)
    {
        y[0] += ACoef[n] * x[n] - BCoef[n] * y[n];
    }
    return y[0];
}

float float_api_filters_iir_gyro_z(float float_new_sample)
{
    float ACoef[NCoef+1] = {
        0.02675265688403948700f,
        0.10701062753615795000f,
        0.16051594130423694000f,
        0.10701062753615795000f,
        0.02675265688403948700f
    };

    float BCoef[NCoef+1] = {
        1.00000000000000000000f,
        -1.28555068801461640000f,
        1.01289917136114170000f,
        -0.35368582226312562000f,
        0.05438080336347629600f
    };
    
    /* output samples*/
    static float y[NCoef+1]; 
    /* input samples */
    static float x[NCoef+1]; 
    int n;

    /* shift the old samples */
    for(n = NCoef; n>0; n--)
    {
       x[n] = x[n-1];
       y[n] = y[n-1];
    }

    /* Calculate the new output */
    x[0] = float_new_sample;
    y[0] = ACoef[0] * x[0];
    for(n = 1; n <= NCoef; n++)
    {
        y[0] += ACoef[n] * x[n] - BCoef[n] * y[n];
    }
    return y[0];
}












