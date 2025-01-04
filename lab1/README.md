# Lab 1

## Task

> Установить на компьютер Oracle VM VirtualBox, на нем настроить виртуальную машину с ОС:
>
> - Ubuntu 20.04.6 LTS, если на основном компьютере установлена ОС Windows
> - Windows 10, если на основном комьютере установлены ОС семейств Linux или macOS.
>
> На основной компьютер и виртуальную машину установить компилятор gcc (mingw на windows) и систему сборки CMake
>
> На основной компьютер и виртуальную машину установить git и создать git-репозиторий для проекта "Hello world!" на C++.
>
> Создать скрипты cmd (для Windows) и sh (для Linux), автоматизирующие обновление исходных кодов проекта с GIT-репозитория, сборку и компиляцию проекта.
>
> Настроить любимый редактор кода (Notepad++, Geany, MS Visual Studio Code, Eclipse, Qt Creator...) для компиляции и отладки своего проекта с использованием отладчика gdb.
>
> На проверку: скрипты сборки, cmake-скрипты, файлы настройки редактора кода (если нужны).

## Setup

**Host Machine**

- OS: Linux Mint 21.3
- Text Editor: NeoVim (Custom Configuration)

**Virtual Machine (Oracle VirtualBox)**

- OS: Windows 10 Pro
- Text Editor: NeoVim (LazyVim)

## Instructions

### UNIX Systems - *Bash*

1. Clone the repository:

   ```bash
   git clone https://github.com/AJMC2002/fefu-os.git
   cd fefu-os/lab1/
   ```

2. Run the repo update, build and run script:

   ```bash
   ./linux-mint/build.sh
   ```

3. Clean the build files:

   ```bash
   ./linux-mint/clean.sh
   ```

> Note: The binary `HelloWorld.out` will be located in the `build` directory after run.

### Windows Systems - *Powershell*

1. Clone the repository:

   ```powershell
   git clone https://github.com/AJMC2002/fefu-os.git
   cd .\fefu-os\lab1\
   ```

2. Run the repo update, build and run script:

   ```powershell
   .\windows\build.cmd
   ```

3. Clean the build files:

   ```powershell
   .\windows\clean.cmd
   ```

> Note: The binary `HelloWorld.exe` will be located in the `build` directory after run.

## Project Structure

```
./
├── linux-mint/
│   ├── build.sh*
│   └── clean.sh*
├── windows/
│   ├── build.cmd
│   └── clean.cmd
├── CMakeLists.txt
└── main.cpp
```
