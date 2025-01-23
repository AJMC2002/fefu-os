#include "serial_port.h"

#include <iostream>
#include <stdexcept>

SerialPort::SerialPort(const std::string& port_name, unsigned int baud_rate) {
#ifdef _WIN32
	handle_ = CreateFile(port_name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0,
						 OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (handle_ == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("Failed to open COM port");
	}

	DCB dcb = {0};
	dcb.DCBlength = sizeof(DCB);
	if (!GetCommState(handle_, &dcb)) {
		CloseHandle(handle_);
		throw std::runtime_error("Failed to get COM state");
	}

	dcb.BaudRate = baud_rate;
	dcb.ByteSize = 8;
	dcb.StopBits = ONESTOPBIT;
	dcb.Parity = NOPARITY;

	if (!SetCommState(handle_, &dcb)) {
		CloseHandle(handle_);
		throw std::runtime_error("Failed to set COM state");
	}
#else
	fd_ = open(port_name.c_str(), O_RDWR);
	if (fd_ < 0) {
		throw std::runtime_error("Failed to open serial port");
	}

	struct termios tty;
	if (tcgetattr(fd_, &tty) != 0) {
		close(fd_);
		throw std::runtime_error("Failed to get serial port attributes");
	}

	cfsetospeed(&tty, baud_rate);
	cfsetispeed(&tty, baud_rate);

	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
	tty.c_iflag &= ~IGNBRK;
	tty.c_lflag = 0;
	tty.c_oflag = 0;
	tty.c_cc[VMIN] = 1;
	tty.c_cc[VTIME] = 5;

	tty.c_iflag &= ~(IXON | IXOFF | IXANY);
	tty.c_cflag |= (CLOCAL | CREAD);
	tty.c_cflag &= ~(PARENB | PARODD);
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CRTSCTS;

	if (tcsetattr(fd_, TCSANOW, &tty) != 0) {
		close(fd_);
		throw std::runtime_error("Failed to set serial port attributes");
	}
#endif
}

SerialPort::~SerialPort() {
#ifdef _WIN32
	CloseHandle(handle_);
#else
	close(fd_);
#endif
}

void SerialPort::write(const std::string& data) {
#ifdef _WIN32
	DWORD bytes_written;
	if (!WriteFile(handle_, data.c_str(), data.size(), &bytes_written, NULL)) {
		throw std::runtime_error("Failed to write to serial port");
	}
#else
	ssize_t bytes_written = ::write(fd_, data.c_str(), data.size());
	if (bytes_written < 0) {
		throw std::runtime_error("Failed to write to serial port");
	}
#endif
}

std::string SerialPort::read_line() {
	std::string line;
	char buffer[1];

#ifdef _WIN32
	DWORD bytes_read;
	while (true) {
		if (!ReadFile(handle_, buffer, 1, &bytes_read, NULL) ||
			bytes_read == 0) {
			throw std::runtime_error("Failed to read from serial port");
		}
		if (buffer[0] == '\n') {
			break;
		}
		line += buffer[0];
	}
#else
	while (true) {
		ssize_t bytes_read = ::read(fd_, buffer, 1);
		if (bytes_read < 0) {
			throw std::runtime_error("Failed to read from serial port");
		}
		if (buffer[0] == '\n') {
			break;
		}
		line += buffer[0];
	}
#endif

	return line;
}