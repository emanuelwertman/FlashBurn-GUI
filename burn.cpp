#include <iostream>
#include <cstdlib> // For system()
#include <string>
#include <filesystem>


using namespace std;
namespace fs = std::filesystem;

string usb;
string isoFileLocation;  
string isoFileName;
uintmax_t isoFileSize;

bool fileLimit;

//uefi boot
void uefiBoot() {
    string buffer;
    char check;
    isoFileName = filesystem::path(isoFileLocation).filename().string();
    isoFileSize = filesystem::file_size(isoFileLocation);
    fileLimit = isoFileSize > (4ULL * 1024 * 1024 * 1024);

    //start gpt reformatting
    string gptWipe = ("echo -e \"o\\ny\\nw\\ny\\n\" | sudo gdisk " + usb);
    system(gptWipe.c_str()); //conv to c string
    //construct fat32
    cout << "\033[34mFormatting to GPT successful. Constructing file system... \033[0m\n";
    string mkfsFat32("sudo mkfs.fat -F 32 " + usb);
    system(mkfsFat32.c_str());
    //dd install files
    cout << "\033[34mDrive successfully formatted to FAT32, DD'ing install files...\033[0m\n";
    string ddIsoTousb = "sudo dd if=" + isoFileLocation + " of=" + usb + " bs=8M status=progress oflag=direct";
    system(ddIsoTousb.c_str());
}


int main() {
    uefiBoot();
}