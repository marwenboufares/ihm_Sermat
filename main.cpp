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
#include <QFrame>
#include <QTextEdit>
#include <QTabWidget>
#include <QVBoxLayout>

class TerminalTab : public QWidget {
public:
    TerminalTab(QWidget *parent = nullptr) : QWidget(parent) {
        QVBoxLayout *layout = new QVBoxLayout(this);

        // Création de QTextEdit pour afficher le contenu
        textEdit = new QTextEdit(this);
        textEdit->setReadOnly(true); // Rendre le QTextEdit en lecture seule
        layout->addWidget(textEdit);

        // Création du QSerialPort pour lire les données de l'UART
        serialPort = new QSerialPort(this);
        connect(serialPort, &QSerialPort::readyRead, this, &TerminalTab::readData);
    }

    // Méthode pour ouvrir le port série avec le nom spécifié
    void openSerialPort(const QString &portName) {
        if (!serialPort->isOpen()) {
            serialPort->setPortName(portName);
            if (serialPort->open(QIODevice::ReadOnly)) {
                // Succès de l'ouverture du port série
            } else {
                // Échec de l'ouverture du port série
            }
        }
    }

private slots:
    void readData() {
        QByteArray data = serialPort->readAll(); // Lire les données de l'UART
        textEdit->append(QString::fromUtf8(data)); // Afficher les données dans le QTextEdit
    }

private:
    QTextEdit *textEdit;
    QSerialPort *serialPort;
};

class UARTConfigForm : public QWidget {
public:
    UARTConfigForm(QWidget *parent = nullptr) : QWidget(parent) {
        setWindowTitle("S-Actium config");

        // Définir la taille de l'application
        resize(800, 400);

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
        bottomLayout->addWidget(statusLabel); // Ajout du label de statut (LED)

        // Ligne de séparation
        line = new QFrame();
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        // ONGLET CONFIGBANC
        configBancTab = new QWidget(this);
        configBancTab->setStyleSheet("background-color: #D3D3D3;");

        // ONGLET ACTIONNEUR 1
        actionneur1Tab = new QWidget(this);
        actionneur1Tab->setStyleSheet("background-color: #D3D3D3;");

        // ONGLET ACTIONNEUR 2
        actionneur2Tab = new QWidget(this);
        actionneur2Tab->setStyleSheet("background-color: #D3D3D3;");

        // ONGLET SIMULTANE
        simultaneTab = new QWidget(this);
        simultaneTab->setStyleSheet("background-color: #D3D3D3;");

        // ONGLET TERMINAL
        terminalTab = new TerminalTab(this);
        terminalTab->setStyleSheet("background-color: #D3D3D3;");

        // WIDGET A ONGLETS
        tabWidget = new QTabWidget(this);
        tabWidget->addTab(configBancTab, "Configuration banc");
        tabWidget->addTab(actionneur1Tab, "Actionneur 1");
        tabWidget->addTab(actionneur2Tab, "Actionneur 2");
        tabWidget->addTab(simultaneTab, "Simultané");
        tabWidget->addTab(terminalTab, "Terminal");

        // Layout principal de la fenêtre
        QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->addLayout(topLayout);
        mainLayout->addLayout(bottomLayout);
        mainLayout->addWidget(line);
        mainLayout->addWidget(tabWidget); // Ajout du widget à onglets
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
            disableTabs(false); //A inverser juste pour test
        } else {
            QMessageBox::critical(this, "Erreur", "Échec de l'ouverture du port série !");
            updateStatusLabel(false);
            disableTabs(true); //A inverser juste pour test
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

    void disableTabs(bool isConnected) {
        if (isConnected) {
            configBancTab->setStyleSheet("");
            actionneur1Tab->setStyleSheet("");
            actionneur2Tab->setStyleSheet("");
            simultaneTab->setStyleSheet("");
            terminalTab->setStyleSheet("");
        } else {
            configBancTab->setStyleSheet("background-color: #4D4D4D;");
            actionneur1Tab->setStyleSheet("background-color: #4D4D4D;");
            actionneur2Tab->setStyleSheet("background-color: #4D4D4D;");
            simultaneTab->setStyleSheet("background-color: #4D4D4D;");
            terminalTab->setStyleSheet("background-color: #4D4D4D;");
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
    QCheckBox *customBaudRateCheckBox;
    QSerialPort *serialPort;
    QLabel *statusLabel;

    // Widget pour les nouveaux champs et le bouton "Envoyer"
    QWidget *configBancTab;     // ONGLET CONFIGBANC
    QWidget *actionneur1Tab;    // ONGLET ACTIONNEUR1
    QWidget *actionneur2Tab;    // ONGLET ACTIONNEUR2
    QWidget *simultaneTab;      // ONGLET SIMULTANE
    TerminalTab *terminalTab;   // ONGLET TERMINAL
    QTabWidget *tabWidget;

    QFrame *line;
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
