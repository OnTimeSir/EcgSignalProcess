fid  = fopen('..\SignalProcessingTest\filteredECG.dat', 'rb');
rstSig = single(fread(fid, 'int16'));
rstSig = rstSig./2^12;
fclose(fid);
figure;plot(rstSig);