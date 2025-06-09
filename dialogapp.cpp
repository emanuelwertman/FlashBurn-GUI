#include <QApplication>
#include <QDialog>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QProcess>
#include <QMessageBox>
#include <QStringList>
#include <QFileDialog>
#include <QString>
#include <QDebug>
#include <QComboBox>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QDialog dialog;
    dialog.setWindowTitle("Command Runner");
    dialog.resize(450, 600);

    // Create widgets first
    QTextEdit *outputDisplay = new QTextEdit(&dialog);
    QPushButton *fileButton = new QPushButton("Select ISO File", &dialog);
    QPushButton *burnButton = new QPushButton("START", &dialog);
    QComboBox *burnTypeBox = new QComboBox;
    QComboBox *usbTypeBox = new QComboBox;

    // Configure widgets
    outputDisplay->setReadOnly(true);
    outputDisplay->setFontFamily("Courier");
    outputDisplay->setMinimumHeight(300);
    fileButton->setMinimumHeight(30);


    burnTypeBox->addItem("GPT");
    burnTypeBox->addItem("UEFI");
    burnTypeBox->addItem("MBR");
    burnTypeBox->addItem("Legacy");

    // Create and set layout
    QGridLayout *layout = new QGridLayout(&dialog);
    
    // Add widgets to layout with row, column, rowspan, colspan
    layout->addWidget(outputDisplay, 0, 0, 1, 2);  
    layout->addWidget(usbTypeBox, 1, 0);              
    layout->addWidget(fileButton, 1, 1);            
    layout->addWidget(burnTypeBox, 2, 0, 1, 2);
    layout->addWidget(burnButton, 3, 0, 1, 2);
    
    
    // Add stretch to push everything up
    layout->setRowStretch(2, 1);

    // DISPLAYS THE OUTPUT OF THE COMMAND
    QString fullCommand = QStringLiteral("fdisk -l | grep -E '^Disk /dev/[a-z]+|^Disk /dev/nvme[0-9]+'");
 
    if (fullCommand.isEmpty()) {
        QMessageBox::warning(nullptr, "Error", "Please enter a command!");
    }

    // Split command into program + arguments
    QStringList parts = fullCommand.split(' ', Qt::SkipEmptyParts);
    QString program = parts.takeFirst();
    QStringList arguments = parts;

    QProcess process;
    process.start("bash", QStringList() << "-c" << fullCommand);
    process.waitForFinished();

    QString output = process.readAllStandardOutput();
    QString error = process.readAllStandardError();

    QString result;
    if (!output.isEmpty()) result += output;
    if (!error.isEmpty()) result += "Errors:\n" + error;
    if (output.isEmpty() && error.isEmpty()) result += "(No output)";

    outputDisplay->setText(result);

    QList<QString> usbDiskPlacement;
    QString prefix = "/dev";
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);

    for (int i = 0; i < lines.size(); i++) {
        QString &line = lines[i];  
        int devIndex = line.indexOf(prefix);
        if (devIndex != -1) {  
            QString diskInfo = line.mid(devIndex).trimmed();
            QString diskName = diskInfo.split(',').first().trimmed();
            usbTypeBox->addItem(diskName);
        }
    }

    QString fileName;

    QObject::connect(fileButton, &QPushButton::clicked, [&fileName, outputDisplay]() {
        fileName = QFileDialog::getOpenFileName(nullptr,
        "Open File",
        "",
        "Iso Files(*.iso)");
        if (!fileName.isEmpty()) {
            qDebug() << "Selected file:" << fileName;
        }
    });

    QObject::connect(burnButton, &QPushButton::clicked, [&fileName, usbTypeBox, burnTypeBox, outputDisplay]() {
        QString fullText = usbTypeBox->currentText();
        QString devicePath = fullText.split(":").first().trimmed();
       
        if (devicePath.isEmpty()) {
            QMessageBox::warning(nullptr, "Error", "Please enter a USB device!");
            return;
        }

        // Display in QTextEdit
        outputDisplay->append("USB Device: " + devicePath);

        QString burnType;

        int selectedIndex = burnTypeBox->currentIndex();

        if (selectedIndex == 0) {
            burnType = "GPT";
        } 
        else if (selectedIndex == 1) {
            burnType = "UEFI";
        }
        else if (selectedIndex == 2) {
            burnType = "MBR";
        }
        else if (selectedIndex == 3) {
            burnType = "Legacy";
        }
        else { 
            QMessageBox::warning(nullptr, "Error", "Please choose the burn type!");
            return;
        }
        outputDisplay->append("Burn Type: " + burnType);
        outputDisplay->append("Selected File: " + fileName);
    });


    dialog.show();
    return app.exec();
}