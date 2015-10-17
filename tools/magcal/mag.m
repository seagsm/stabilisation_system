% Octave script. 
% Script read magnetometer data Mx,My,Mz from file
% and plat 3D and 2D graph 
M = dlmread("D:\\tmp\\magcal\\m.txt ",' \t');
size(M,1);
size(M,2);
x = M(:,1);
y = M(:,2);
z = M(:,3); 
% calculate hard iron, linear shift od data to canter of ordinat
% show min and max value from massive of data
max(x)
min(x)
max(y)
min(y)
max(z)
min(z)
%d = figure();
%plot (x,y,x,z,y,z);
%calculate shift
sx = (max(x) + min(x))/2
sy = (max(y) + min(y))/2
sz = (max(z) + min(z))/2
% shift all data
x = x-sx;
y = y-sy;
z = z-sz;
% Calculate unlinearity
% We can compare it later with unlinearity after correction.
SigmaXY = (max(x)-min(x))/(max(y)-min(y))
SigmaXZ = (max(x)-min(x))/(max(z)-min(z))
SigmaYZ = (max(y)-min(y))/(max(z)-min(z))
d = figure();
plot (x,y,x,z,y,z);

% Here we use correction matrix calculated by MagCal
% for correction of elipsoide
A = [  	
    6.1223,   -0.0505,   -0.0721;         
   -0.0505,    6.1163,   -0.1209;         
   -0.0721,   -0.1209,    5.6381;
    ]
% carrect data using formula H = A^-1 * h	
% there h is input magn vector after hard iron correction.
tmp = [x,y,z];
tmp = transpose(tmp);
M = inv(A)*tmp;
M = transpose(M);	
x = M(:,1);
y = M(:,2);
z = M(:,3); 
% Calculate unlinearity again 
% to see difference after elipsoide correction.
SigmaXY = (max(x)-min(x))/(max(y)-min(y))
SigmaXZ = (max(x)-min(x))/(max(z)-min(z))
SigmaYZ = (max(y)-min(y))/(max(z)-min(z))


%create new graph windows "h"
%h = figure();
%plot3(x,y,z);
%create new graph windows "d"
e = figure();
plot (x,y,x,z,y,z);
%pause(5);
%delete(d);
%scatter3 (x, y, z);