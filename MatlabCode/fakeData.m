clear
filename1 = ['mit_bihdb/','105','.mat'];
filename2 = ['mit_nstdb/','bw','.mat'];
filename3 = ['mit_nstdb/','ma','.mat'];
load(filename1);
tStart = 0; %起始时间
tLast = 200;  %秒
xs = (Fs*tStart+1:Fs*(tStart+tLast))';
rawECG = signal(xs,1);%肢体II导联
% figure;plot(rawECG);
load(filename2);
basLine = signal(xs,1);
load(filename3);
emgNoise = signal(xs,1);

noiseECG = rawECG+basLine;
%重采样
Fn = 250;
xq = (Fn*tStart+1:Fn*(tStart+tLast))';
noiseECG = interp1(xs, noiseECG, xq);
% 
Fp = 50;%工频
powerLine = 0.5.*sin(2*pi*Fp*xq./Fn+pi/2).*cos(2*pi*0.03*xq./Fn);
noiseECG = noiseECG + powerLine;

%量化
qpath = quantizer('fixed','round','saturate',[24,15]);
fix_noiseECG = int32(quantize(qpath,noiseECG).*2^15);
fid = fopen('..\SignalProcessingTest\rawECG.dat','wb');
fwrite(fid,fix_noiseECG,'int32');
fclose(fid);

Fpass = 37;          % Passband Frequency
Fstop = 49;          % Stopband Frequency
Dpass = 0.005;       % Passband Ripple
Dstop = 0.002;       % Stopband Attenuation
L     = 2;           % Interpolation Factor
optim = 'Simple';  % Optimization Level

% Calculate the coefficients using the IFIR function.
[h, g] = ifir(L, 'low', [Fpass Fstop]/(Fn/2), [Dpass Dstop], optim);
H = dsp.FIRFilter('Numerator', h);
G = dsp.FIRFilter('Numerator', g);
Hcascade = cascade(G,H);
hfv = fvtool(Hcascade);
hfv.Fs = Fn;

q = 50;%45
k = 5;
r = 250;%225
b = [-1 zeros(1,r-k-1) q^2 zeros(1,k-1) 2*(1-q^2) zeros(1,k-1) q^2 zeros(1,r-k-1) -1];
a = (q^2).*[1 zeros(1,k-1) -2 zeros(1,k-1) 1];

K = dsp.IIRFilter( ...
    'Structure', 'Direct form II', ...
    'Numerator', b, ...
    'Denominator', a);

hfv = fvtool(cascade(G,H,K));
hfv.Fs = Fn;

qpath = quantizer('fixed','round','saturate',[16,15]);
fir_coeff1 = int16(quantize(qpath,g).*2^15);
fir_coeff2 = int16(quantize(qpath,h).*2^15);
fid = fopen('..\SignalProcessingTest\firCoeff1.dat','wb');
fwrite(fid,fir_coeff1(1:ceil(size(fir_coeff1,2)/2)),'int16');
fclose(fid);
fid = fopen('..\SignalProcessingTest\firCoeff2.dat','wb');
fwrite(fid,fir_coeff2(1:L:ceil(size(fir_coeff2,2)/2)),'int16');
fclose(fid);