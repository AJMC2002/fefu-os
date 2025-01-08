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

2. Invoke Cmake to generate the Makefile and build the binary:

   ```bash
   cmake -B build
   cmake --build build
   ./build/main.out -n <number> -np <processes>
   ```

### Windows Systems - *Powershell*

1. Clone the repository:

   ```powershell
   git clone https://github.com/AJMC2002/fefu-os.git
   cd .\fefu-os\lab2\
   ```

2. Invoke Cmake to generate the Makefile and build the binary:

   ```powershell
   cmake -B build
   cmake --build build
   .\build\main.exe -n <number> -np <processes>
   ```

## Usage

The program generates the number of sub-processes indicated by `np` and simulates some processing on each child process. It's ouput is whether the number `n` is divisible by the number of processes `np`.

The way the program simulates its processing on each child is the following:

- Calculate the number of elements that will go in each child process (aka `bucket_size`) without overflow by using `(int)std::ceil((double)n / (double)np)`.

- Distribute the numbers from 1 to `n` as lists of at most `bucket_size` length for each child.

- Wait 100 ms for each element in the list received on each child and error if the number of elements isn't equal to `bucket_size`.

- Gather the exit codes on the parent child and check if any of the processes errored. If not, then `n` is divisible by `np`.

*Example 1 (n = 27, np = 4)*

```stdout
Process 3: Not Equal - [ 22 23 24 25 26 27 ]
Process 0:     Equal - [ 1 2 3 4 5 6 7 ]
Process 1:     Equal - [ 8 9 10 11 12 13 14 ]
Process 2:     Equal - [ 15 16 17 18 19 20 21 ]
--------------------------------------------------
Process 0: Success
Process 1: Success
Process 2: Success
Process 3: Error
RESULT: 27 is not divisible by 4.
```

*Example 2 (n = 10, np = 5)*

```stdout
Process 0:     Equal - [ 1 2 ]
Process 1:     Equal - [ 3 4 ]
Process 2:     Equal - [ 5 6 ]
Process 3:     Equal - [ 7 8 ]
Process 4:     Equal - [ 9 10 ]
--------------------------------------------------
Process 0: Success
Process 1: Success
Process 2: Success
Process 3: Success
Process 4: Success
RESULT: 10 is  divisible by 5.
```

## Project Structure

```
./
├── CMakeLists.txt
└── main.cpp
```
