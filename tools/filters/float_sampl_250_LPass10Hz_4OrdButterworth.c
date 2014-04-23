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
float iir(float NewSample) {
    float ACoef[NCoef+1] = {
        0.00018539265053394222,
        0.00074157060213576886,
        0.00111235590320365330,
        0.00074157060213576886,
        0.00018539265053394222
    };

    float BCoef[NCoef+1] = {
        1.00000000000000000000,
        -3.34406783771187310000,
        4.23886395088406330000,
        -2.40934285658631710000,
        0.51747819978804011000
    };

    static float y[NCoef+1]; //output samples
    static float x[NCoef+1]; //input samples
    int n;

    //shift the old samples
    for(n=NCoef; n>0; n--) {
       x[n] = x[n-1];
       y[n] = y[n-1];
    }

    //Calculate the new output
    x[0] = NewSample;
    y[0] = ACoef[0] * x[0];
    for(n=1; n<=NCoef; n++)
        y[0] += ACoef[n] * x[n] - BCoef[n] * y[n];
    
    return y[0];
}
