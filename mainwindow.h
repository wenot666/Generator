#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSpinBox>
#include <QPushButton>
#include <QTextEdit>
#include <QTimer>
#include "qcustomplot.h"  // Подключаем библиотеку для графиков QCustomPlot

// Класс для генерации синусоидальных значений
class SignalGenerator {
public:
    SignalGenerator(double amplitude, double frequency, double phase);  // Конструктор
    double generateValue(double time);  // Метод для генерации значения синусоиды на заданное время

    void setAmplitude(double amplitude);  // Динамическое изменение амплитуды
    void setFrequency(double frequency);  // Динамическое изменение частоты
    void setPhase(double phase);          // Динамическое изменение фазы

private:
    double amplitude;  // Амплитуда сигнала
    double frequency;  // Частота сигнала
    double phase;      // Фаза сигнала
};

// Основной класс приложения
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();  // Деструктор

private slots:
    void onGenerateClicked();  // Слот для обработки нажатия на кнопку "Старт"
    void onStopClicked();      // Слот для обработки нажатия на кнопку "Стоп"
    void updateData();         // Слот для обновления данных и графика
    void onAmplitudeChanged(int value);  // Слот для изменения амплитуды
    void onFrequencyChanged(int value);  // Слот для изменения частоты
    void onPhaseChanged(int value);      // Слот для изменения фазы
    void onIntervalChanged(int value);   // Слот для изменения интервала

private:
    QSpinBox *amplitudeInput;
    QSpinBox *frequencyInput;
    QSpinBox *phaseInput;
    QSpinBox *intervalInput;
    QPushButton *generateButton;
    QPushButton *stopButton;
    QTextEdit *logTextEdit;

    QTimer *timer;
    SignalGenerator *generator;
    double time;
    QCustomPlot *customPlot;

    void setupPlot();
    void updatePlot(double value);
};

#endif // MAINWINDOW_H
