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
float iir(float NewSample) {
    float ACoef[NCoef+1] = {
        0.02675265688403948700,
        0.10701062753615795000,
        0.16051594130423694000,
        0.10701062753615795000,
        0.02675265688403948700
    };

    float BCoef[NCoef+1] = {
        1.00000000000000000000,
        -1.28555068801461640000,
        1.01289917136114170000,
        -0.35368582226312562000,
        0.05438080336347629600
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
