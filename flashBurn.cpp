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

#include <iostream>
#include <cstdlib>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

void uefiBoot(std::string isoFileLocation, std::string usb);

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QDialog dialog;
    dialog.setWindowTitle("FlashBurn");
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


    burnTypeBox->addItem("Legacy");
    burnTypeBox->addItem("UEFI");
    burnTypeBox->addItem("Partition to MBR");
    burnTypeBox->addItem("Partition to GPT");

    // Create and set layout
    QGridLayout *layout = new QGridLayout(&dialog);
    
    // Add widgets to layout (plz dont change its really annoying to do)
    layout->addWidget(outputDisplay, 0, 0, 1, 2);  
    layout->addWidget(usbTypeBox, 1, 0);              
    layout->addWidget(fileButton, 1, 1);            
    layout->addWidget(burnTypeBox, 2, 0, 1, 2);
    layout->addWidget(burnButton, 3, 0, 1, 2);
    
    
    // Add stretch to push everything up
    layout->setRowStretch(2, 1);

    
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

    outputDisplay->setText("LOG:");

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
        
        outputDisplay->append("Selected File: " + fileName);

    });

    QObject::connect(burnButton, &QPushButton::clicked, [&fileName, usbTypeBox, burnTypeBox, outputDisplay]() {
        QString fullText = usbTypeBox->currentText();
        QString devicePath = fullText.split(":").first().trimmed();

        std::string devicePathStd = devicePath.toStdString();
        std::string fileNameStd = fileName.toStdString();

        if (devicePath.isEmpty()) {
            QMessageBox::warning(nullptr, "Error", "Please enter a USB device!");
            return;
        }

        outputDisplay->append("USB Device: " + devicePath);

        QString burnType;

        int selectedIndex = burnTypeBox->currentIndex();

        if (selectedIndex == 0) {
            burnType = "Legacy";
        } 
        else if (selectedIndex == 1) {
            burnType = "UEFI";
        }
        else if (selectedIndex == 2) {
            burnType = "MBR";
        }
        else if (selectedIndex == 3) {
            burnType = "GPT";
        }
        else { 
            QMessageBox::warning(nullptr, "Error", "Please choose the burn type!");
            return;
        }
        outputDisplay->append("Burn Type: " + burnType);
        std::string cont;

        switch( QMessageBox::warning( 
            nullptr, 
            "WARNING", 
            "This WILL overide everything on your disk, is that fine (MAKE 100% SURE YOU HAVE NOT SELECTED YOUR MAIN PARTITION THIS WILL BRICK YOUR SYSTEM)?", 

            QMessageBox::Yes | 
            QMessageBox::Cancel, 

            QMessageBox::Cancel ) )
                {
                case QMessageBox::Yes:
                    cont = "yes";
                    break;
                default:
                    break;
                }
        // the rest of the logic to check if everything is fine and call correct functions and stuff
        if (cont == "yes") {
            if (burnType == "UEFI") {
                outputDisplay->append("this disk is formatting, it might take a minute :) sit tight");
                uefiBoot(fileNameStd, devicePathStd);
                    
            }
        }
    });


    dialog.show();
    return app.exec();
}


// all this is from Neil's og program so i lowk dont know how it works
void uefiBoot(std::string isoFileLocation, std::string usb) {
    std::filesystem::path isoFileName = std::filesystem::path(isoFileLocation).filename().string();
    //start gpt reformatting
    std::string gptWipe = ("echo -e \"o\\ny\\nw\\ny\\n\" | sudo gdisk " + usb);
    system(gptWipe.c_str()); //conv to c string
    //construct fat32
    std::cout << "\033[34mFormatting to GPT successful. Constructing file system... \033[0m\n";
    std::string mkfsFat32("sudo mkfs.fat -F 32 " + usb);
    system(mkfsFat32.c_str());
    //dd install files
    std::cout << "\033[34mDrive successfully formatted to FAT32, DD'ing install files...\033[0m\n";
    std::string ddIsoTousb = "sudo dd if=" + isoFileLocation + " of=" + usb + " bs=8M status=progress oflag=direct";
    system(ddIsoTousb.c_str());
}