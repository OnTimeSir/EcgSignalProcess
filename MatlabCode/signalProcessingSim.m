fid  = fopen('..\SignalProcessingTest\rawECG.dat', 'rb');
raw = fread(fid, 'int32');
fclose(fid);
fid  = fopen('..\SignalProcessingTest\firCoeff1.dat', 'rb');
firCoeff1 = fread(fid, 'int16');
fclose(fid);
fid  = fopen('..\SignalProcessingTest\firCoeff2.dat', 'rb');
firCoeff2 = fread(fid, 'int16');
fclose(fid);

dataLen = 50000;
rawECG = int32(raw(1:dataLen));
% Fs = 250;
% Fp = 40;
% A = 1;
% rawECG = A.*sin(2*pi*Fp*(0:49999)./Fs+pi/2)';
% qpath = quantizer('fixed','round','saturate',[24,15]);
% rawECG = int32(quantize(qpath,rawECG).*2^15);

F_delay = 250;
B_delay = 5;
rrs_shift = 12;
rrs_amplitude = single(0.0004);

firCoeff1 = int32([firCoeff1; flipud(firCoeff1)]);
firCoeff2 = int32([firCoeff2(1:end-1); flipud(firCoeff2)]);

rrsOut = zeros(dataLen,1,'int32');
filteredSig = zeros(dataLen,1,'int32');
forwardBuf = zeros(bitshift(F_delay,1),1,'int32');
backBuf = zeros(bitshift(B_delay,1),1,'int32');
firBuf1 = zeros(20,1,'int32');
firBuf2 = zeros(62,1,'int32');
for i=1:dataLen
   xSum = rawECG(i) - bitshift(forwardBuf(end-F_delay+1),1) + forwardBuf(end-bitshift(F_delay,1)+1);
   ySum = bitshift(backBuf(end-B_delay+1),1) - backBuf(end-bitshift(B_delay,1)+1);
   tmpOut = xSum + ySum;
%    rrsOut(i) = forwardBuf(end-F_delay+B_delay+1) - bitshift(tmpOut,-rrs_shift);
   rrsOut(i) = forwardBuf(end-F_delay+B_delay+1) - int32(single(tmpOut)*rrs_amplitude);
   forwardBuf = [forwardBuf(2:end);rawECG(i)];
   backBuf = [backBuf(2:end);tmpOut]; 
   
   tmpOut = int32(bitshift(int64(sum(firCoeff1.*firBuf1(end:-1:1))),-15));
   filteredSig(i) = int32(bitshift(int64(sum(firCoeff2.*firBuf2(end:-2:1))),-17));
   firBuf1 = [firBuf1(2:end);rrsOut(i)]; 
   firBuf2 = [firBuf2(2:end);tmpOut];  

end
outPut = single(filteredSig)./2^10;
hold on;plot(outPut);

