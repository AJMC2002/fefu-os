#pragma once

#include <pqxx/pqxx>
#include <string>

class Database {
   public:
	Database(const std::string& connection_string);
	~Database();

	void insert_temperature(const std::string& timestamp, double temperature);

	void upsert_hourly_average(const std::string& hour, double average);

	void upsert_daily_average(const std::string& day, double average);

	double calculate_hourly_average(const std::string& hour);

	double calculate_daily_average(const std::string& day);

	pqxx::connection& connection();

   private:
	pqxx::connection connection_;

	void init_tables();
};