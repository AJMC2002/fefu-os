#pragma once

#include <httplib.h>

#include <string>

#include "database.h"
#include "log_parser.h"

class Server {
   public:
	Server(Database& db, LogParser& temp_parser, LogParser& hourly_parser,
		   LogParser& daily_parser);
	void start(int port);

   private:
	void setup_routes();

	Database& db_;
	LogParser& temp_parser_;
	LogParser& hourly_parser_;
	LogParser& daily_parser_;
	httplib::Server server_;
};