# 📁 Filesystem Forensics Tool (`fs_tool`)

`fs_tool` is a command-line utility for analyzing and recovering data from ext4 filesystems. It provides functionality to list directory contents with metadata, scan devices for ext4 signatures, identify deleted files, and recover deleted files using inode numbers.


- 📋 **Directory Listing**: Displays metadata (timestamps, immutability) for directory contents.
- 🔍 **Device Scanning**: Detects ext4 signatures on block devices.
- 🗑️ **Deleted File Detection**: Identifies deleted files via inode table scanning.
- 🔄 **File Recovery**: Reconstructs deleted files using inode numbers.

---

## 📑 Table of Contents

- [🌟 Overview](#-overview)
- [🛠️ Prerequisites](#️-prerequisites)
- [📂 Project Structure](#-project-structure)
- [🚀 Setup Instructions](#-setup-instructions)
- [🧪 Test Cases and Expected Outputs](#-test-cases-and-expected-outputs)
- [🧹 Cleanup](#-cleanup)
- [🔧 Troubleshooting](#-troubleshooting)
- [📝 Notes](#-notes)
- [📬 Contributing](#-contributing)

---

## 🌟 Overview

`fs_tool` is a forensics tool for ext4 filesystems, enabling users to inspect directory metadata, scan devices, detect deleted files, and recover them. Built with C and low-level system calls (e.g., `statx`, `getdents64`, `pread`), it’s ideal for learning filesystem internals and performing data recovery.


---

## 🛠️ Prerequisites

Ensure the following before starting:

- **Operating System**: Linux (e.g., Ubuntu 20.04+ on WSL2 or native Linux).
- **Tools and Libraries**:
  - `gcc`: C compiler.
  - `make`: Build automation.
  - `e2fsprogs`: For `mkfs.ext4`, `dumpe2fs`, `debugfs`.
  - `libext2fs-dev`: Ext4 development headers.
- **Permissions**: Root access (required for loop devices and mounting).
- **Disk Space**: ~100MB for `test.img`.
- **WSL (if applicable)**: WSL2 recommended for loop device support. Check:

  ```bash
  wsl --version
  ```

  Update if needed:

  ```bash
  wsl --update
  ```

**Install Dependencies**:

```bash
sudo apt update
sudo apt install gcc make e2fsprogs libext2fs-dev -y
```

**Verify**:

```bash
gcc --version
make --version
mkfs.ext4 --version
```

**Expected Output** (example):

```plaintext
gcc (Ubuntu 9.4.0-1ubuntu1~20.04) 9.4.0
make 4.2.1
mke2fs 1.46.5 (30-Dec-2021)
```

---

## 📂 Project Structure

The `Filesystem_Forensics` repository, located at `~/fs_forensics/fs_forensics`, includes:

| File            | Description                                                                 |
|-----------------|-----------------------------------------------------------------------------|
| `main.c`        | Entry point, handles command-line arguments and orchestrates functions.     |
| `metadata.c`    | Retrieves file metadata (timestamps, immutability).                         |
| `scanner.c`     | Scans devices, detects deleted files, and reconstructs files.               |
| `recovery.c`    | Manages file recovery.                                                     |
| `utils.c`       | Utility functions (e.g., filesystem integrity stub).                        |
| `metadata.h`    | Declares metadata functions.                                               |
| `scanner.h`     | Declares scanning and reconstruction functions.                             |
| `recovery.h`    | Declares recovery function.                                                |
| `utils.h`       | Declares utility functions.                                                |
| `Makefile`      | Build rules for compilation and cleaning.                                  |

**Generated Files**:
- `fs_tool`: Executable.
- `*.o`: Object files.
- `test.img`: Test filesystem image.
- `recovered_file.txt`: Recovery output.

---

## 🚀 Setup Instructions

Follow these steps to set up and test `fs_tool`. 
### 1. Clone the Repository

Clone the GitHub repository (if not already local):

```bash
git clone https://github.com/Jb-rown/Filesystem-Forensics.git
cd Filesystem_ Forensics
```

> **VS Code**: Open the repository with `code .`.

### 2. Verify Source Files

Check for all files:

```bash
ls
```

**Expected Output**:

```plaintext
main.c  metadata.c  metadata.h  scanner.c  scanner.h  recovery.c  recovery.h  utils.c  utils.h  Makefile
```

If files are missing, ensure the repository is correctly cloned or create them in VS Code (`File > New File`).

### 3. Compile the Tool

Build the executable:

```bash
make
```
This generates the `fs_tool` executable.

**Expected Output**:

```plaintext
gcc -Wall -g -c -o main.o main.c
gcc -Wall -g -c -o metadata.o metadata.c
gcc -Wall -g -c -o scanner.o scanner.c
gcc -Wall -g -c -o recovery.o recovery.c
gcc -Wall -g -c -o utils.o utils.c
gcc -Wall -g -o fs_tool main.o metadata.o scanner.o recovery.o utils.o
```

Verify:

```bash
ls fs_tool
```

**Output**:

```plaintext
fs_tool
```

### 4. Create a Test Filesystem

 Create a 100MB ext4 filesystem image for testing:


```bash
dd if=/dev/zero of=test.img bs=1M count=100
mkfs.ext4 test.img
```

**Expected Output** (for `dd`):

```plaintext
100+0 records in
100+0 records out
104857600 bytes (105 MB, 100 MiB) copied, 0.123456 s, 849 MB/s
```

**Expected Output** (for `mkfs.ext4`):

```plaintext
mke2fs 1.46.5 (30-Dec-2021)
Creating filesystem with 102400 1k blocks and 25688 inodes
...
Writing superblocks and filesystem accounting information: done
```

Verify:

```bash
file test.img
```

**Output**:

```plaintext
test.img: Linux rev 1.0 ext4 filesystem data, UUID=...
```

### 5. Set Up a Loop Device

Attach `test.img` to a loop device (e.g., `/dev/loop2`):

```bash
losetup -fP test.img
losetup -a
```

**Expected Output**:

```plaintext
/dev/loop2: [2080]:38287 (/root/fs_forensics/fs_forensics/test.img)
/dev/loop0: [2112]:14 (/mnt/docker-desktop-disk/isocache/...)
/dev/loop1: [2112]:17 (/mnt/docker-desktop-disk/isocache/...)
```

Use `/dev/loop2` (or the assigned device) in tests.

### 6. Prepare a Deleted File

Mount the filesystem, Create, delete, and unmount a test file:

```bash
mkdir -p /mnt/testfs
mount /dev/loop2 /mnt/testfs
echo "Hello, this is a test file!" > /mnt/testfs/testfile.txt
sync
rm /mnt/testfs/testfile.txt
sync
umount /mnt/testfs
```

**Expected Output**: No output unless errors occur.

---

## 🧪 Test Cases and Expected Outputs

Run these tests as `root` in `Filesystem_ Forensics`. 

### Test 1: Directory Listing and Metadata

**Command**:

```bash
./fs_tool .
```

**Expected Output**:

```plaintext
File: .
Access Time : 1742581200
Modify Time : 1742581200
Change Time : 1742581200
Birth Time  : 1742580000
File is NOT immutable.
Found: .
Found: ..
Found: main.c
Found: metadata.c
Found: metadata.h
Found: scanner.c
Found: scanner.h
Found: recovery.c
Found: recovery.h
Found: utils.c
Found: utils.h
Found: Makefile
Found: fs_tool
Found: test.img
```

- **Description**: Displays timestamps (Unix epoch seconds, e.g., `1742581200` ≈ April 19, 2025), immutability status (`NOT immutable`), and all files in the directory.
- **Notes**: Timestamps vary based on system time and file activity.


### Test 2: Scan Device for ext4 Signatures
Scan the loop device for ext4 filesystem signatures.

**Command**:

```bash
./fs_tool --scan /dev/loop2
```

**Expected Output**:

```plaintext
[+] Found ext4 signature at offset: 1024 + 56 = 1080
```

- **Description**: Detects the ext4 superblock magic number (`0xEF53`) at offset 1080 (superblock start at 1024 + 56 bytes).
- **Notes**: Confirms `test.img` is a valid ext4 filesystem.


### Test 3: List Deleted Files
Identify deleted files by scanning the inode table.
**Command**:

```bash
./fs_tool --deleted /dev/loop2
```

**Expected Output**:

```plaintext
[Deleted] Inode #12 | Deleted At: Sat Apr 19 14:30:00 2025
```

- **Description**: Lists inodes with non-zero deletion times (`i_dtime`). Inode `12` is typical for the first user file (`testfile.txt`).
- **Notes**: Inode number and deletion time depend on filesystem allocation and when `rm` was executed.

### Test 4: Recover a Deleted File
Recover the deleted file using its inode number (e.g., `12` from Test 3).

**Command**:

```bash
./fs_tool --recover /dev/loop2 12 recovered_file.txt
```

**Expected Output**:

```plaintext
[+] Recovering deleted file (Inode: 12) to: recovered_file.txt
[+] Reconstructed 27 bytes to recovered_file.txt
```

**Verify**:

```bash
cat recovered_file.txt
```

**Output**:

```plaintext
Hello, this is a test file!
```

- **Description**: Reconstructs the file from inode `12`, writing 27 bytes (26 characters + newline) to `recovered_file.txt`.
- **Notes**: Recovery assumes direct blocks; larger files with indirect blocks may require code updates.

---

## 🧹 Cleanup

After testing, remove temporary files and detach the loop device:

```bash
losetup -d /dev/loop2
rm test.img
rm -f recovered_file.txt
rmdir /mnt/testfs
make clean
```

**Expected Output** (for `make clean`):

```plaintext
rm -f *.o fs_tool
```

---

## 🔧 Troubleshooting

| Issue                              | Solution                                                                 |
|------------------------------------|--------------------------------------------------------------------------|
| **Loop Device Error** (`losetup: /dev/loop2: detach failed`) | Verify: `losetup -a`. Reattach: `losetup -fP test.img`. Clear: `losetup -D`. |
| **No Deleted Inodes**             | Unmount immediately: `sync; umount /mnt/testfs`. Check: `debugfs /dev/loop2; lsdel`. |
| **Invalid Inode Number**          | Use inode from `--deleted`. Verify: `dumpe2fs test.img \| grep "Inode table"`. |
| **WSL Issues**                    | Update WSL: `wsl --update`. Ensure WSL2. Use native Linux if unstable.   |
| **Compilation Errors**            | Install: `apt install libext2fs-dev`. Check `Makefile` dependencies.     |

**Debugging**:
- Add `printf` in `scanner.c` (e.g., for `pread`).
- Use VS Code debugger:
  - Create `launch.json` for `gdb`.
  - Run: `gdb ./fs_tool`, then `run --deleted /dev/loop2`.

---

## 📝 Notes

- **Inode Numbers**: Vary by filesystem (typically 11–15 for user files).
- **Timestamps**: Reflect system time (e.g., April 19, 2025).
- **Ext4 Assumptions**: The tool assumes 256-byte inodes and 4096-byte blocks, standard for modern ext4.
- **Limitations**: Recovery limited to direct blocks (48KB). Indirect blocks not supported.

**VS Code Tips**:
- Preview Markdown: `Ctrl+Shift+V`.
- Install “Prettier” for formatting: Enable “Format On Save” in Settings.
- Use Git integration: Stage/commit via Source Control (`Ctrl+Shift+G`).

---

## 📬 Contributing

Contributions to `Filesystem-Forensics` are welcome! To contribute:

1. Fork the repository: [github.com/your-username/Filesystem-Forensics](https://github.com/Jb-rown/Filesystem-Forensics).
2. Create a branch: `git checkout -b feature/your-feature`.
3. Commit changes: `git commit -m "Add your feature"`.
4. Push: `git push origin feature/your-feature`.
5. Open a pull request.

For issues, open a GitHub issue or check `scanner.c` (for `--scan`, `--deleted`) and `recovery.c` (for `--recover`).
