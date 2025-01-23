#include <stdexcept>
#include <string>

#include "serial_port.h"

double read_temperature_from_serial() {
	static SerialPort serial_port(
#if defined(_WIN32)
		"COM4",
#elif defined(__unix__)
		"/dev/pts/7",
#endif
		9600);

	std::string line = serial_port.read_line();
	try {
		return std::stod(line);
	} catch (const std::invalid_argument& e) {
		throw std::runtime_error("Invalid temperature data: " + line);
	}
}