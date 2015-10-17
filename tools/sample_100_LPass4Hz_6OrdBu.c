/**************************************************************
WinFilter version 0.8
http://www.winfilter.20m.com
akundert@hotmail.com

Filter type: Low Pass
Filter model: Butterworth
Filter order: 6
Sampling Frequency: 100 Hz
Cut Frequency: 4.000000 Hz
Coefficents Quantization: float

Z domain Zeros
z = -1.000000 + j 0.000000
z = -1.000000 + j 0.000000
z = -1.000000 + j 0.000000
z = -1.000000 + j 0.000000
z = -1.000000 + j 0.000000
z = -1.000000 + j 0.000000

Z domain Poles
z = 0.780979 + j -0.051899
z = 0.780979 + j 0.051899
z = 0.823730 + j -0.149552
z = 0.823730 + j 0.149552
z = 0.910010 + j -0.225689
z = 0.910010 + j 0.225689
***************************************************************/
#define NCoef 6
float iir(float NewSample) {
    float ACoef[NCoef+1] = {
        0.00000227167980216140,
        0.00001363007881296841,
        0.00003407519703242103,
        0.00004543359604322805,
        0.00003407519703242103,
        0.00001363007881296841,
        0.00000227167980216140
    };

    float BCoef[NCoef+1] = {
        1.00000000000000000000,
        -5.02943835142160900000,
        10.60704218377968500000,
        -11.99931581621669400000,
        7.67547454820019940000,
        -2.63105512847394830000,
        0.37745238637408868000
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
