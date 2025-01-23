#include <time.h>

#include <iostream>
#include <thread>

#include "serial_port.h"

void simulate_temperature(SerialPort& port) {
	std::srand(std::time(nullptr));
	while (true) {
		int temperature = std::rand() % 51 - 20;  // temp between -20°C and 30°C
		std::string temp_str = std::to_string(temperature) + "\n";
		port.write(temp_str);

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

int main() {
	try {
		std::string port_name;
#ifdef _WIN32
		port_name = "COM3";	 // On Windows use `com0com` to set.this.
#else
		port_name =
			"/dev/pts/6";  // On UNIX apt-get install `socat` and run
						   // `socat -d -d pty,raw,echo=0 pty,raw,echo=0`
#endif

		unsigned int baud_rate = 9600;
		SerialPort port(port_name, baud_rate);
		simulate_temperature(port);
	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}