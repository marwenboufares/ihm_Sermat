#include <QApplication>
#include <QWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QMessageBox>
#include <QSerialPortInfo>
#include <QSerialPort>
#include <QFile>
#include <QTextStream>

class UARTConfigForm : public QWidget {
public:
    UARTConfigForm(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("Configuration UART");

        // Création des champs de saisie
        portComboBox = new QComboBox(this);
        detectCOMPorts();

        baudRateComboBox = new QComboBox(this);
        baudRateComboBox->addItems({"1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200"});
        customBaudRateLineEdit = new QLineEdit(this);

        signalTypeComboBox = new QComboBox(this);
        signalTypeComboBox->addItems({"Sinusoidale", "Dent de scie", "Carré"});

        parityComboBox = new QComboBox(this);
        parityComboBox->addItems({"None", "Even", "Odd", "Mark", "Space"});

        dataBitsComboBox = new QComboBox(this);
        dataBitsComboBox->addItems({"5", "6", "7", "8"});

        stopBitsComboBox = new QComboBox(this);
        stopBitsComboBox->addItems({"1", "1.5", "2"});

        // Bouton de validation
        submitButton = new QPushButton("Valider", this);
        connect(submitButton, &QPushButton::clicked, this, &UARTConfigForm::submitForm);

        // Création du layout
        QFormLayout *formLayout = new QFormLayout;
        formLayout->addRow("Port COM :", portComboBox);
        formLayout->addRow("Vitesse standard :", baudRateComboBox);
        formLayout->addRow("Vitesse personnalisée :", customBaudRateLineEdit);
        formLayout->addRow("Type de signal :", signalTypeComboBox);
        formLayout->addRow("Parité :", parityComboBox);
        formLayout->addRow("Bits de données :", dataBitsComboBox);
        formLayout->addRow("Bits de stop :", stopBitsComboBox);
        formLayout->addRow(submitButton);

        setLayout(formLayout);
    }

private slots:
    void detectCOMPorts() {
        QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
        for (const QSerialPortInfo &info : ports) {
            portComboBox->addItem(info.portName());
        }
    }

    void submitForm() {
        QString port = portComboBox->currentText();
        QString baudRate = customBaudRateLineEdit->text().isEmpty() ? baudRateComboBox->currentText() : customBaudRateLineEdit->text();
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
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Appliquer la feuille de style
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
