clc 
clear all
load('./FilteredData_Subject_8_task_CW.mat');

%ESERCIZIO 1
signal = ECG(30000:32500, 1);
signal_duration = 0:2500;
figure;
subplot(2,2,1);
plot(signal); grid on; ylim([-1 1]); xlim([0 2500]); title("Original Signal"); xlabel("Time"); ylabel("Amplitude"); hold on;

valore_medio = mean(signal);
yline(valore_medio, "--m", "Mean Value", "LineWidth",2);
varianza = var(signal);
energia = sum(abs(signal.^2));

annotation("textbox", [.15 .6 .3 .3], "String", strcat("Energia: ", num2str(energia)), 'FitBoxToText','on', "BackgroundColor",[1 1 1]);
annotation("textbox", [.32 .6 .3 .3], "String", strcat("Varianza: ", num2str(varianza)), 'FitBoxToText','on', "BackgroundColor",[1 1 1]);

%FINE ESERCIZIO 1

%ESERCIZIO 2
signalLength = numel(signal(:,1));
p = round(signalLength*15/100);

segnaleDegradato = signal(:,1);
segnaleDegradato(signalLength-p:signalLength, 1)=0;

valore_medioDegradato = mean(segnaleDegradato);
energiaDegradato = sum(abs(segnaleDegradato.^2));

subplot(2,2,2);
plot(segnaleDegradato); grid on; title("Degraded Signal"); xlabel("Time"); ylabel("Amplitude"); ylim([-1 1]); xlim([0 2500]); hold on;
yline(valore_medioDegradato, "--m", "Mean Value", "LineWidth",2);
annotation("textbox", [.6 .6 .3 .3], "String", strcat("Energia: ", num2str(energiaDegradato)), 'FitBoxToText','on', "BackgroundColor",[1 1 1]);
%FINE ESERCIZIO 2

%ESERCIZIO 3
subplot(2,2,3);
h = rect(250, 1050, signal_duration);
plot(h); grid on; xlim([0 2500]); ylim([-0.5 1.5]); title("Input (Rect)", "FontSize",12); ylabel("Amplitude"); xlabel("Time");
subplot(2,2,4);
segnaleFiltrato = conv(segnaleDegradato, h, "full");
plot(segnaleFiltrato); grid on; ylim([-10 10]);xlim([0 numel(segnaleFiltrato)]); title("Filtered Signal", "FontSize",12); ylabel("Amplitude"); xlabel("Time");

%FINE ESERCIZIO 3
function y = rect(start, last, x)
    y = zeros(numel(x), 1);
    y(x>=start & x<=last) = 1;
end