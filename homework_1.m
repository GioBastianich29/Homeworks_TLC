clc 

load('./FilteredData_Subject_8_task_CW.mat');

%ESERCIZIO 1
signal = ECG(30000:32500, 1);

figure; plot(signal); xlim([0 2500]); title("Segnale"); hold on;

valore_medio = mean(signal);
yline(valore_medio, "--m", "Mean Value", "LineWidth",2);
varianza = var(signal);
energia = sum(abs(signal.^2));

annotation("textbox", [.15 .6 .3 .3], "String", strcat("Energia: ", num2str(energia)), 'FitBoxToText','on', "BackgroundColor",[1 1 1]);
annotation("textbox", [.35 .6 .3 .3], "String", strcat("Varianza: ", num2str(varianza)), 'FitBoxToText','on', "BackgroundColor",[1 1 1]);

%FINE ESERCIZIO 1

%ESERCIZIO 2
signalLength = numel(signal(:,1));
disp(signalLength)
p = round(signalLength*15/100);

segnaleDegradato = signal(:,1);
segnaleDegradato(signalLength-p:signalLength, 1)=0;

figure; plot(segnaleDegradato); title("Segnale degradato"); hold on;

valore_medioDegradato = mean(segnaleDegradato);
energiaDegradato = sum(abs(segnaleDegradato.^2));

yline(valore_medioDegradato, "--m", "Mean Value", "LineWidth",2);
annotation("textbox", [.15 .6 .3 .3], "String", strcat("Energia: ", num2str(energiaDegradato)), 'FitBoxToText','on', "BackgroundColor",[1 1 1]);

%FINE ESERCIZIO 2


