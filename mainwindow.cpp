#include "mainwindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QtMath>
#include <QTimer>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <cmath>
#include "qcustomplot.h"

// Конструктор класса MainWindow
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), generator(nullptr), time(0.0) {

    setWindowTitle("Генератор");

    // Создаем элементы интерфейса
    amplitudeInput = new QSpinBox(this);
    amplitudeInput->setRange(0, 100); // Устанавливаем диапазон значений для амплитуды

    frequencyInput = new QSpinBox(this);
    frequencyInput->setRange(1, 1000); // Устанавливаем диапазон частоты от 1 до 1000 Гц

    phaseInput = new QSpinBox(this);
    phaseInput->setRange(0, 360); // Устанавливаем диапазон фазы от 0 до 360 градусов

    intervalInput = new QSpinBox(this);
    intervalInput->setRange(100, 10000); // Интервал от 100 мкс до 10 мс
    intervalInput->setValue(100);        // Значение по умолчанию 100 мкс

    generateButton = new QPushButton("Старт", this); // Кнопка "Старт"
    stopButton = new QPushButton("Стоп", this);                // Кнопка "Стоп"
    logTextEdit = new QTextEdit(this);
    logTextEdit->setReadOnly(true); // Делаем текстовое поле только для чтения

    customPlot = new QCustomPlot(this); // Создаем график
    customPlot->setFixedSize(400,300);

    // Создаем вертикальную компоновку интерфейса
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(new QLabel("Амплитуда"));
    layout->addWidget(amplitudeInput);
    layout->addWidget(new QLabel("Частота"));
    layout->addWidget(frequencyInput);
    layout->addWidget(new QLabel("Фаза"));
    layout->addWidget(phaseInput);
    layout->addWidget(new QLabel("Интервал (мкс)"));
    layout->addWidget(intervalInput);
    layout->addWidget(generateButton);
    layout->addWidget(stopButton);      // Добавляем кнопку "Стоп"
    layout->addWidget(customPlot);      // Добавляем график
    layout->addWidget(logTextEdit);     // Добавляем поле для логов

    QWidget *widget = new QWidget();
    widget->setLayout(layout);
    setCentralWidget(widget);

    // Подключаем изменение параметров к слотам
    connect(amplitudeInput, SIGNAL(valueChanged(int)), this, SLOT(onAmplitudeChanged(int)));
    connect(frequencyInput, SIGNAL(valueChanged(int)), this, SLOT(onFrequencyChanged(int)));
    connect(phaseInput, SIGNAL(valueChanged(int)), this, SLOT(onPhaseChanged(int)));
    connect(intervalInput, SIGNAL(valueChanged(int)), this, SLOT(onIntervalChanged(int)));

    // Подключаем кнопки "Старт" и "Стоп"
    connect(generateButton, &QPushButton::clicked, this, &MainWindow::onGenerateClicked);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::onStopClicked);

    // Создаем таймер
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateData);

    setupPlot();
}

// Реализация деструктора класса MainWindow
MainWindow::~MainWindow() {
    // Освобождаем ресурсы
    if (generator) {
        delete generator;  // Удаляем объект генератора
    }
}

// Настройка графика
void MainWindow::setupPlot() {
    customPlot->addGraph();                          // Добавляем график
    customPlot->xAxis->setLabel("Время (с)");        // Подпись оси X
    customPlot->yAxis->setLabel("Значение");         // Подпись оси Y

    // Устанавливаем диапазоны осей
    customPlot->xAxis->setRange(0, 10);              // Диапазон времени (X)
    customPlot->yAxis->setRange(-100, 100);          // Диапазон значений (Y)
}

// Слот для запуска генерации значений
void MainWindow::onGenerateClicked() {
    // Получаем параметры из GUI
    double amplitude = amplitudeInput->value();
    double frequency = frequencyInput->value();
    double phase = qDegreesToRadians(static_cast<double>(phaseInput->value()));

    // Если генератор уже создан, освобождаем его
    if (generator) {
        delete generator;
    }

    // Создаем новый объект генератора
    generator = new SignalGenerator(amplitude, frequency, phase);

    // Сбрасываем время
    time = 0.0;

    // Запускаем таймер с интервалом, заданным в GUI
    int interval = intervalInput->value();
    timer->start(interval);  // Запуск таймера
}

// Слот для остановки генерации
void MainWindow::onStopClicked() {
    timer->stop();  // Остановка таймера
}

// Слот для изменения амплитуды
void MainWindow::onAmplitudeChanged(int value) {
    if (generator) {
        generator->setAmplitude(value);  // Динамическое изменение амплитуды
    }
}

// Слот для изменения частоты
void MainWindow::onFrequencyChanged(int value) {
    if (generator) {
        generator->setFrequency(value);  // Динамическое изменение частоты
    }
}

// Слот для изменения фазы
void MainWindow::onPhaseChanged(int value) {
    if (generator) {
        generator->setPhase(qDegreesToRadians(static_cast<double>(value)));  // Динамическое изменение фазы
    }
}

// Слот для изменения интервала
void MainWindow::onIntervalChanged(int value) {
    timer->setInterval(value);  // Динамическое изменение интервала таймера
}

// Обновление данных и графика
void MainWindow::updateData() {
    if (generator) {
        // Генерация нового значения
        double value = generator->generateValue(time);

        // Логирование значения
        logTextEdit->append("Текущее время: " + QString::number(time) + " | Значение: " + QString::number(value));

        // Обновление графика
        updatePlot(value);

        // Увеличиваем время
        time += 0.0001;  // Увеличение времени на 100 мкс
    }
}

// Обновление графика
void MainWindow::updatePlot(double value) {
    static QVector<double> xData, yData;
    xData.append(time);   // Добавляем текущее время
    yData.append(value);  // Добавляем сгенерированное значение

    customPlot->graph(0)->setData(xData, yData);  // Обновляем данные графика
    customPlot->replot();                         // Перерисовываем график

    // Ограничиваем количество точек
    if (xData.size() > 1000) {
        xData.remove(0);
        yData.remove(0);
    }

    // Обновляем диапазон по оси X
    customPlot->xAxis->setRange(xData.front(), xData.back());
}

// Реализация класса SignalGenerator
SignalGenerator::SignalGenerator(double amplitude, double frequency, double phase)
    : amplitude(amplitude), frequency(frequency), phase(phase) {}

// Метод для генерации значений синусоиды
double SignalGenerator::generateValue(double time) {
    // Формула синусоиды: y(t) = A * sin(2 * M_PI * f * t + φ)
    return amplitude * sin(2 * M_PI * frequency * time + phase);
}

void SignalGenerator::setAmplitude(double amplitude) {
    this->amplitude = amplitude;
}

void SignalGenerator::setFrequency(double frequency) {
    this->frequency = frequency;
}

void SignalGenerator::setPhase(double phase) {
    this->phase = phase;
}
