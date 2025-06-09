
# FlashBurn

![Qt](https://img.shields.io/badge/Qt-6.0%2B-green)  
![Platform](https://img.shields.io/badge/Platform-Linux-blue)  
![Status](https://img.shields.io/badge/Status-In%20Development-yellow)

A graphical USB flashing tool for Arch Linux, continuing the work of [Neil Kumaran's terminal-based FlashBurn](https://github.com/neilkumaran/FlashBurn).

## Features

- Simple GUI for creating bootable USB drives  
- Multiple burn modes (only UEFI works as of writing this):
  - Legacy BIOS  
  - UEFI  
  - MBR partitioning  
  - GPT partitioning  
- Automatic USB device detection  
- Safety confirmation before overwriting disks  

## Requirements

- Arch Linux (or derivatives)  
- Qt6 development packages  
- Basic system utilities: `fdisk`, `gdisk`, `mkfs.fat`, `dd`  
- sudo/root access for disk operations  

## Installation

### From Source

1. Install dependencies:

   ```bash
   sudo pacman -S qt6-base qt6-tools gptfdisk dosfstools
   ```

2. Clone the repository:

   ```bash
   git clone https://github.com/yourusername/flashburn-gui.git
   cd flashburn-gui
   ```

3. Build and run:

   ```bash
   qmake6 && make
   ./FlashBurn
   ```

---

### Package Installation

*(Coming soon - package will be available in AUR)*

---

## Usage

- Launch FlashBurn from your application menu or terminal  
- Click **"Select ISO File"** to choose your bootable image  
- Select target USB device from dropdown list  
- Choose burn type:
  - **Legacy**: Traditional BIOS boot  
  - **UEFI**: Modern UEFI boot  
  - **MBR**: Partition to Master Boot Record  
  - **GPT**: Partition to GUID Partition Table  
- Click **"START"** and confirm the operation  


## ⚠️ Safety Warning ⚠️

**This tool will completely overwrite the selected disk.**  
Double-check that you've selected the correct USB device to avoid permanent data loss.  
The developers are not responsible for any lost data.  

## Technical Details

The application uses low-level disk operations including:

- `gdisk` for GPT partitioning  
- `mkfs.fat` for filesystem creation  
- `dd` for direct disk writing  

---

## Roadmap

- Add progress bars for operations  
- Implement verification checks  
- Support for persistent storage  
- Additional filesystem options  
- Package for AUR distribution

### Made by Emanuel Wertman, Atharva Usturge, and Neil Kumaran
