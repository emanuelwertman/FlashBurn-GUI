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
#include <QList>
#include <typeinfo>
#include <iostream>  
#include <string>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);


    QDialog dialog;
    dialog.setWindowTitle("Command Runner");
    dialog.resize(450, 700);

    QGridLayout *layout = new QGridLayout();
    centralWidget->setLayout (layout);

    layout->addWidget (outputDisplay, 0, 0, -1, 1);
    layout->addWidget (usbInput, 0, 1, 1, 1);
    layout->addWidget (fileButton, 1, 1, 1, 1);


    QLineEdit *usbInput = new QLineEdit(&dialog);
    QPushButton *fileButton = new QPushButton("Select ISO File", &dialog);
    QTextEdit *outputDisplay = new QTextEdit(&dialog);

    outputDisplay->setReadOnly(true);
    outputDisplay->setFontFamily("Courier");
    outputDisplay->setMaximumHeight(200);
    
    fileButton->setMaximumHeight(200);

    usbInput->setPlaceholderText("/dev/nvme0n1");

    // layout->addItem(outputDisplay);
    // layout->addItem("Enter usb:", usbInput);
    // layout->addItem(fileButton);  
    

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
    // Sort through output
    // Look for aything beginning with /dev
    // put that into a list (just thename of the disk)
    QString result;
    if (!output.isEmpty()) result += output;
    if (!error.isEmpty()) result += "Errors:\n" + error;
    if (output.isEmpty() && error.isEmpty()) result += "(No output)";

    outputDisplay->setText(result);

    QObject::connect(fileButton, &QPushButton::clicked, [usbInput, outputDisplay]() {
        QString usbText = usbInput->text().trimmed();
        
        if (usbText.isEmpty()) {
            QMessageBox::warning(nullptr, "Error", "Please enter a USB device!");
            return;
        }

        std::string usbStdString = usbText.toStdString();

        // Display in QTextEdit
        outputDisplay->append("USB Device: " + usbText);

        QString fileName = QFileDialog::getOpenFileName(nullptr,
        "Open File",
        "",
        "Iso Files(*.iso)");
        if (!fileName.isEmpty()) {
            qDebug() << "Selected file:" << fileName;
            outputDisplay->append("Selected File: " + fileName);
        }
    });

    dialog.show();
    return app.exec();
}