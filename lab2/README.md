# Lab 2

## Task

>Написать на языке C или C++ библиотеку, которая содержит функцию для запуска программ в фоновом режиме, с возможностью подождать завершения работы дочерней программы и получить код ответа. Написать тестовую утилиту для проверки библиотеки. И библиотека и утилита должны быть кроссплатформенными (собираться и работать на ОС семейств Windows и UNIX (POSIX)
>
>На проверку: исходные коды библиотеки и программы, демонстрация работы библиотеки (если необходимо)

## Setup

**Host Machine**

- OS: Linux Mint 21.3
- Text Editor: NeoVim (Custom Configuration)

**Virtual Machine (Oracle VirtualBox)**

- OS: Windows 10 Pro
- Text Editor: NeoVim (LazyVim)

## Compilation

### UNIX Systems - *Bash*

1. Clone the repository:

   ```bash
   git clone https://github.com/AJMC2002/fefu-os.git
   cd fefu-os/lab2/
   ```

2. Invoke Cmake to generate the Makefile and build the test binary:

   ```bash
   cmake -S . -B build
   cmake --build build
   ```

### Windows Systems - *Powershell*

1. Clone the repository:

   ```powershell
   git clone https://github.com/AJMC2002/fefu-os.git
   cd .\fefu-os\lab2\
   ```

2. Invoke Cmake to generate the Makefile and build the test binary:

   ```powershell
   cmake -S . -B build
   cmake --build build
   ```

## Usage

The library `run_processes` contains the function of the same name which takes in the number of processes (`np`), the name of the program to run (`program`) and a vector containing the args of each process as a vector of strings (`argvv`). The test evaluate different possibilities by using common utilities on Windows and Unix systems.

The test binary can be found in `build/tests/` with the name `test`.

**UNIX Systems**

```bash
./build/tests/test 
```

**Windows Systems**

```powershell
.\build\tests\test.exe
```

## Project Structure

```
./
├── lib/
│   ├── run_processes.cpp
│   └── run_processes.h
├── tests/
│   ├── CMakeLists.txt
│   └── main.cpp
└── CMakeLists.txt
```
