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

        customBaudRateCheckBox = new QCheckBox(this);
        customBaudRateLineEdit->setEnabled(false); // Désactiver l'éditeur de ligne au début
        connect(customBaudRateCheckBox, &QCheckBox::stateChanged, this, &UARTConfigForm::toggleCustomBaudRate);

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

        // Bouton d'envoi de données
        sendButton = new QPushButton("Envoyer", this);
        sendButton->setFixedWidth(150);
        connect(sendButton, &QPushButton::clicked, this, &UARTConfigForm::sendData);

        // Label pour la LED
        statusLabel = new QLabel(this);
        statusLabel->setFixedSize(20, 20); // Taille fixe
        updateStatusLabel(false);

        // TopLayout pour les champs de saisie, etc.
        QFormLayout *topLayout = new QFormLayout;
        topLayout->addRow("Port COM :", portComboBox);
        topLayout->addRow("Vitesse standard :", baudRateComboBox);

        // Créer un layout horizontal pour la checkbox et l'éditeur de ligne
        QHBoxLayout *customBaudRateLayout = new QHBoxLayout;
        customBaudRateLayout->addWidget(customBaudRateCheckBox);
        customBaudRateLayout->addWidget(customBaudRateLineEdit);
        topLayout->addRow("Vitesse personnalisée :", customBaudRateLayout);

        topLayout->addRow("Type de signal :", signalTypeComboBox);
        topLayout->addRow("Parité :", parityComboBox);
        topLayout->addRow("Bits de données :", dataBitsComboBox);
        topLayout->addRow("Bits de stop :", stopBitsComboBox);

        // BasLayout horizontal pour le bas de la fenêtre
        QHBoxLayout *bottomLayout = new QHBoxLayout;
        bottomLayout->addStretch(); // Ajout d'espacement à gauche
        bottomLayout->addWidget(submitButton); // Ajout du bouton "Valider" à droite
        bottomLayout->addWidget(sendButton); // Ajout du bouton "Envoyer" à droite
        bottomLayout->addWidget(statusLabel); // Ajout du label de statut (LED)

        // Layout principal de la fenêtre
        QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->addLayout(topLayout); // Ajout du layout des champs de saisie
        mainLayout->addLayout(bottomLayout); // Ajout du layout pour les boutons
        setLayout(mainLayout); // Définition du layout principal

        // Initialiser le port série
        serialPort = new QSerialPort(this);
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
            baudRateComboBox->setStyleSheet("QComboBox::down-arrow { border-color: red; }");
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

        // Configurer le port série
        serialPort->setPortName(port);
        serialPort->setBaudRate(baudRate.toInt());
        serialPort->setDataBits(static_cast<QSerialPort::DataBits>(dataBits.toInt()));
        serialPort->setParity(static_cast<QSerialPort::Parity>(parityComboBox->currentIndex()));
        serialPort->setStopBits(static_cast<QSerialPort::StopBits>(stopBitsComboBox->currentIndex() + 1));
        serialPort->setFlowControl(QSerialPort::NoFlowControl);

        if (serialPort->open(QIODevice::ReadWrite)) {
            QMessageBox::information(this, "Configuration UART", "Port série ouvert avec succès !");
            updateStatusLabel(true);
        } else {
            QMessageBox::critical(this, "Erreur", "Échec de l'ouverture du port série !");
            updateStatusLabel(false);
        }
    }

    void sendData() {
        if (serialPort->isOpen() && serialPort->isWritable()) {
            // Exemple d'envoi de consigne
            QString consigne = "Exemple de consigne à envoyer au dsPIC";
            serialPort->write(consigne.toUtf8());
            QMessageBox::information(this, "Envoyer", "Consigne envoyée avec succès !");
        } else {
            QMessageBox::critical(this, "Erreur", "Le port série n'est pas ouvert ou non accessible en écriture !");
        }
    }

private:
    QHBoxLayout *createFormItem(const QString &labelText, QWidget *widget) {
        QLabel *label = new QLabel(labelText);
        QHBoxLayout *layout = new QHBoxLayout;
        layout->addWidget(label);
        layout->addWidget(widget);
        return layout;
    }

    void updateStatusLabel(bool isConnected) {
        if (isConnected) {
            statusLabel->setStyleSheet("background-color: green; border-radius: 10px; width: 20px; height: 20px;");
        } else {
            statusLabel->setStyleSheet("background-color: red; border-radius: 10px; width: 20px; height: 20px;");
        }
    }

    QComboBox *portComboBox;
    QComboBox *baudRateComboBox;
    QLineEdit *customBaudRateLineEdit;
    QComboBox *signalTypeComboBox;
    QComboBox *parityComboBox;
    QComboBox *dataBitsComboBox;
    QComboBox *stopBitsComboBox;
    QPushButton *submitButton;
    QPushButton *sendButton;
    QCheckBox *customBaudRateCheckBox;
    QSerialPort *serialPort;
    QLabel *statusLabel; // Label pour la LED
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
