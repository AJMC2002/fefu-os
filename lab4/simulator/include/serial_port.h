#pragma once

#include <string>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#endif

class SerialPort {
public:
    SerialPort(const std::string& port_name, unsigned int baud_rate);
    ~SerialPort();

    void write(const std::string& data);
    std::string read_line(); // New method to read a line from the serial port

private:
#ifdef _WIN32
    HANDLE handle_;
#else
    int fd_;
#endif
};