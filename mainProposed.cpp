#include <QApplication>
#include <QWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QMessageBox>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QFile>
#include <QTextStream>
#include <QScreen>
#include <QHBoxLayout>
#include <QLabel>

class UARTConfigForm : public QWidget {
public:
    UARTConfigForm(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("Configuration UART");

        resize(800, 400);
        QRect screenGeometry = QGuiApplication::screens().first()->geometry();
        move((screenGeometry.width() - width()) / 2, (screenGeometry.height() - height()) / 2);

        portComboBox = new QComboBox(this);
        detectCOMPorts();

        baudRateComboBox = new QComboBox(this);
        baudRateComboBox->addItems({"1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200"});

        customBaudRateLineEdit = new QLineEdit(this);

        customBaudRateCheckBox = new QCheckBox(this);
        customBaudRateLineEdit->setEnabled(false);
        connect(customBaudRateCheckBox, &QCheckBox::stateChanged, this, &UARTConfigForm::toggleCustomBaudRate);

        signalTypeComboBox = new QComboBox(this);
        signalTypeComboBox->addItems({"Sinusoidale", "Dent de scie", "Carré"});

        parityComboBox = new QComboBox(this);
        parityComboBox->addItems({"None", "Even", "Odd", "Mark", "Space"});

        dataBitsComboBox = new QComboBox(this);
        dataBitsComboBox->addItems({"5", "6", "7", "8"});

        stopBitsComboBox = new QComboBox(this);
        stopBitsComboBox->addItems({"1", "1.5", "2"});

        submitButton = new QPushButton("Valider", this);
        submitButton->setFixedWidth(150);
        connect(submitButton, &QPushButton::clicked, this, &UARTConfigForm::submitForm);

        QFormLayout *topLayout = new QFormLayout;
        topLayout->addRow("Port COM :", portComboBox);
        topLayout->addRow("Vitesse standard :", baudRateComboBox);

        QHBoxLayout *customBaudRateLayout = new QHBoxLayout;
        customBaudRateLayout->addWidget(customBaudRateCheckBox);
        customBaudRateLayout->addWidget(customBaudRateLineEdit);
        topLayout->addRow("Vitesse personnalisée :", customBaudRateLayout);

        topLayout->addRow("Type de signal :", signalTypeComboBox);
        topLayout->addRow("Parité :", parityComboBox);
        topLayout->addRow("Bits de données :", dataBitsComboBox);
        topLayout->addRow("Bits de stop :", stopBitsComboBox);

        QHBoxLayout *bottomLayout = new QHBoxLayout;
        bottomLayout->addStretch();
        bottomLayout->addWidget(submitButton);

        QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->addLayout(topLayout);
        mainLayout->addLayout(bottomLayout);
        setLayout(mainLayout);
    }

private slots:
    void detectCOMPorts() {
        QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
        for (const QSerialPortInfo &info : ports) {
            portComboBox->addItem(info.portName());
        }
    }

    void toggleCustomBaudRate(int state) {
        if (state == Qt::Checked) {
            customBaudRateLineEdit->setEnabled(true);
            baudRateComboBox->setEnabled(false);
            // Appliquer le style avec la flèche rouge
            baudRateComboBox->setStyleSheet("QComboBox::down-arrow { border: 1px solid red; }");
        } else {
            customBaudRateLineEdit->setEnabled(false);
            baudRateComboBox->setEnabled(true);
            // Réinitialiser le style
            baudRateComboBox->setStyleSheet("");
        }
    }

    void submitForm() {
        QString port = portComboBox->currentText();
        QString baudRate;
        if (customBaudRateCheckBox->isChecked()) {
            baudRate = customBaudRateLineEdit->text();
        } else {
            baudRate = baudRateComboBox->currentText();
        }
        QString signalType = signalTypeComboBox->currentText();
        QString parity = parityComboBox->currentText();
        QString dataBits = dataBitsComboBox->currentText();
        QString stopBits = stopBitsComboBox->currentText();

        QMessageBox::information(this, "Configuration UART",
                                 QString("Port COM : %1\nVitesse : %2\nType de signal : %3\nParité : %4\nBits de données : %5\nBits de stop : %6")
                                     .arg(port).arg(baudRate).arg(signalType).arg(parity).arg(dataBits).arg(stopBits));
    }

private:
    QComboBox *portComboBox;
    QComboBox *baudRateComboBox;
    QLineEdit *customBaudRateLineEdit;
    QComboBox *signalTypeComboBox;
    QComboBox *parityComboBox;
    QComboBox *dataBitsComboBox;
    QComboBox *stopBitsComboBox;
    QPushButton *submitButton;
    QCheckBox *customBaudRateCheckBox;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QFile file(":/styles/stylesheet.qss");
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        app.setStyleSheet(stream.readAll());
        file.close();
    } else {
        qWarning("Could not open stylesheet.qss file");
    }

    UARTConfigForm uartConfigForm;
    uartConfigForm.show();

    return app.exec();
}
