/**************************************************************
WinFilter version 0.8
http://www.winfilter.20m.com
akundert@hotmail.com

Filter type: Low Pass
Filter model: Butterworth
Filter order: 4
Sampling Frequency: 250 Hz
Cut Frequency: 1.000000 Hz
Coefficents Quantization: float

Z domain Zeros
z = -1.000000 + j 0.000000
z = -1.000000 + j 0.000000
z = -1.000000 + j 0.000000
z = -1.000000 + j 0.000000

Z domain Poles
z = 0.976995 + j -0.009396
z = 0.976995 + j 0.009396
z = 0.990168 + j -0.023001
z = 0.990168 + j 0.023001
***************************************************************/
#define NCoef 4
float iir(float NewSample) {
    float ACoef[NCoef+1] = {
        0.00000002238270019847,
        0.00000008953080079387,
        0.00000013429620119081,
        0.00000008953080079387,
        0.00000002238270019847
    };

    float BCoef[NCoef+1] = {
        1.00000000000000000000,
        -3.93432582079873730000,
        5.80512542105514040000,
        -3.80723245722885120000,
        0.93643324315201870000
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
