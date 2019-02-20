% Autor: Jakub Sadilek
% Login: xsadil07
% 1)
[y, Fs] = audioread('xsadil07.wav')	% Fs = 16000
N = length(y)		% délka ve vzorcích = 32000
time = N/Fs		% èasová délka = 2s
bits = N/16		% poèet binárních symbolù = 2000
fprintf('Vzorkovaci frekvence je %dHz.\n', Fs)
fprintf('Delka signalu ve vzorcich je %d.\n', N)
fprintf('Casova delka je %ds.\n', time)
fprintf('Pocet reprezentovanych binarnich symbolu je %d.\n', bits)

% 2)
s = y(8:16:end)		% Každý osmý prvek ze segmentu 16ti prvkù
s = s > 0		% Pøevedeno na binární podobu
lin = linspace(0.00047619047, 0.01952380953, 20)	% Rozložení na xové ose
x = 0:1/Fs:0.02
figure
hold on
plot(x(1:320),y(1:320))		% Vykreslíme prvních 20ms audia
stem(lin, s(1:20))		% Vykreslíme binární signál
xlabel('t[s]')			% Popíšeme osy
ylabel('s[n]')
title('2. pøíklad')
hold off

% 3)
% fdatool
fprintf('Filtr je stabilní. Viz graf.\n', time)	% body lezi uvnitr jed. kruznice.

% 4)
b = [0.0192 -0.0185 -0.0185 0.0192];
a = [1.0000 -2.8870 2.7997 -0.9113];
h = impz(b,a);		% získání impulzní charakteristiky
amplit_frekv_char = abs(fft(h,Fs));	% získání amplitudové frekv.charakteristiky
figure
hold on
plot(amplit_frekv_char(1:Fs/2))		% zobrazení ampl.frekv. char od 1 do Fs/2
xlabel('Hz')
ylabel('Modul')
title('4. pøíklad')
hold off
fprintf('Dolní propust. Vysoke frekvence potlacuje.\n', time)
fprintf('Mezní frekvence je 488Hz.\n')

% 5)
ss = filter(b, a, y);		% samotná filtrace
figure 
grid on
hold on				% pomocná møížka
plot(x(1:320),y(1:320))		% zobrazíme s[n]
plot(x(1:320),ss(1:320))	% zobrazíme filtrované s[n]
xlabel('t[s]')
ylabel('s[n], ss[n]')
title('5. pøíklad')
hold off
fprintf('Signal ss[n] je posunuty o 16 vzorkù doleva (predbehnuti).\n')

% 6)
binshifted = ss(16:335)		% uložíme si posunutý signál
binshifted = binshifted > 0	% pøevedeme na binár
binshifted = binshifted(8:16:end)	% vezmeme vždy 8. vzorek ze segmentu 16. vzorkù
figure
hold on
plot(x(1:320),y(1:320)) 	% zobrazení s[n]
plot(x(1:320),ss(1:320))	% zobrazení ss[n]
plot(x(1:320),ss(16:335)) 	% zobrazeni ss[n] shifted - posun o 16 vzorkù
stem(lin, binshifted)		% Zobrazení dekodováného ss[n] shifted
xlabel('t[s]')
ylabel('s[n], ss[n], ssshifted[n]')
title('6. pøíklad')
hold off

% 7)
ssdecoded = ss(16:end)		% posunuty signál
ssdecoded = ssdecoded(8:16:end)	% dekódování
ssdecoded = ssdecoded > 0
amount = xor(ssdecoded, s(1:1999))	% Spoèítáme rozdílné bity
amount = sum(amount)
ErrRate = amount/length(ssdecoded)*100	% Spoèítáme chybovost
fprintf('Chybovost je %f procent.\n', ErrRate)

% 8)
sfourier = fft(y)	% Fourierova transformace signálù
ssfourier = fft(ss)
sfourier = sfourier(1:Fs/2)	% Pouze do poloviny Fs
ssfourier = ssfourier(1:Fs/2)
sfourier = abs(sfourier)	% Abs
ssfourier = abs(ssfourier)
figure
hold on
plot(sfourier)
plot(ssfourier)
xlabel('Hz')
ylabel('Modul')
title('8. pøíklad')
hold off

% 9)
px = hist(y, 50)/N	% Funkce hustoty pravdìpodobnosti
figure
hold on
plot(px)
title('9. pøíklad')
hold off
integral1 = sum(px)		% Ovìøení integrálu, vyšlo 1
fprintf('Integral ma hodnotu %f\n', integral1)

% 10)
kor = xcorr(y, 50, 'biased')
x2 = linspace(-50, 50, 101)
figure
hold on
plot(x2, kor)
xlabel('k')
ylabel('R[k]')
title('10. pøíklad')
hold off

% 11)
R0 = kor(51)	% celkem 101 koeficientù od -50 do 50 vèetnì 0 a indexujeme zleva doprava
R1 = kor(52)
R16 = kor(67)
fprintf('R[0] = %f\n', R0)
fprintf('R[1] = %f\n', R1)
fprintf('R[16] = %f\n', R16)

% 12)
x3 = linspace(min(y), max(y), 50)	% Rozlozeni
matrix = zeros(50, 50)			% Vynulovana matice
[~, samp] = min(abs(repmat(y(:)', 50, 1) - repmat(x3(:), 1, N)))
samp2 = samp(2:N)		% Posunute vzorky o 1
for i = 1:N - 1,		% Rozrazovaci funkce
	indx = samp(i);
	indy = samp2(i);
	matrix(indx, indy) = matrix(indx, indy) + 1;
end
surf = (x3(2)-x3(1))^2
img = matrix/(N-1)/surf		% v matici je N-1 prvkù, protože jsme posunutí o 1
figure
hold on
imagesc(x3, x3, img)
axis xy
colorbar
title('12. pøíklad')
hold off

% 13)
integral2 = sum(sum(img)) * surf	% Ovìøení integrálu
fprintf('Hodnota integralu je %f\n', integral2)

% 14)
iR1 = sum(sum(x3.*x3.*img)) * surf
fprintf('Hodnota R[1] je %f\n', iR1)
