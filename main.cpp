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
#include <QScreen> // Pour accéder aux informations sur l'écran
#include <QHBoxLayout> // Pour utiliser QHBoxLayout
#include <QLabel> // Pour utiliser QLabel

class UARTConfigForm : public QWidget {
public:
    UARTConfigForm(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("Configuration UART");

        // Définir la taille de l'application
        resize(800, 400); // Remplacez ces valeurs par les dimensions souhaitées

        // Positionner l'application au centre de l'écran
        QRect screenGeometry = QGuiApplication::screens().first()->geometry();
        move((screenGeometry.width() - width()) / 2, (screenGeometry.height() - height()) / 2);

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
        submitButton->setFixedWidth(150);
        connect(submitButton, &QPushButton::clicked, this, &UARTConfigForm::submitForm);

        // TopLayout pour les champs de saisie, etc.
        QFormLayout *topLayout = new QFormLayout;
        topLayout->addRow("Port COM :", portComboBox);
        topLayout->addRow("Vitesse standard :", baudRateComboBox);
        topLayout->addRow("Vitesse personnalisée :", customBaudRateLineEdit);
        topLayout->addRow("Type de signal :", signalTypeComboBox);
        topLayout->addRow("Parité :", parityComboBox);
        topLayout->addRow("Bits de données :", dataBitsComboBox);
        topLayout->addRow("Bits de stop :", stopBitsComboBox);

        // BasLayout horizontal pour le bas de la fenêtre
        QHBoxLayout *bottomLayout = new QHBoxLayout;
        bottomLayout->addStretch(); // Ajout d'espacement à gauche
        bottomLayout->addWidget(submitButton); // Ajout du bouton "Valider" à droite

        // Layout principal de la fenêtre
        QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->addLayout(topLayout); // Ajout du layout des champs de saisie
        mainLayout->addLayout(bottomLayout); // Ajout du layout pour le bouton "Valider"
        setLayout(mainLayout); // Définition du layout principal
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
    QHBoxLayout *createFormItem(const QString &labelText, QWidget *widget) {
        QLabel *label = new QLabel(labelText);
        QHBoxLayout *layout = new QHBoxLayout;
        layout->addWidget(label);
        layout->addWidget(widget);
        return layout;
    }

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
