#include "database.h"

#include <stdexcept>

Database::Database(const std::string& connection_string)
	: connection_(connection_string) {
	if (!connection_.is_open()) {
		throw std::runtime_error("Failed to connect to the database");
	}
	init_tables();
}

Database::~Database() {}

void Database::init_tables() {
	pqxx::work txn(connection_);
	txn.exec(R"(
        CREATE TABLE IF NOT EXISTS temperature_data (
            id SERIAL PRIMARY KEY,
            timestamp TIMESTAMP NOT NULL UNIQUE,
            temperature DOUBLE PRECISION NOT NULL
);
    )");
	txn.exec(R"(
        CREATE TABLE IF NOT EXISTS hourly_averages (
            id SERIAL PRIMARY KEY,
            hour TIMESTAMP NOT NULL UNIQUE,
            average_temperature DOUBLE PRECISION NOT NULL
);
    )");
	txn.exec(R"(
        CREATE TABLE IF NOT EXISTS daily_averages (
            id SERIAL PRIMARY KEY,
            day DATE NOT NULL UNIQUE,
            average_temperature DOUBLE PRECISION NOT NULL
);
    )");
	txn.commit();
}

void Database::insert_temperature(const std::string& timestamp,
								  double temperature) {
	pqxx::work txn(connection_);
	txn.exec("INSERT INTO temperature_data (timestamp, temperature) VALUES (" +
			 txn.quote(timestamp) + ", " + txn.quote(temperature) +
			 ") "
			 "ON CONFLICT (timestamp) DO NOTHING");
	txn.commit();
}

void Database::upsert_hourly_average(const std::string& hour, double average) {
	pqxx::work txn(connection_);
	txn.exec(
		"INSERT INTO hourly_averages (hour, average_temperature) VALUES (" +
		txn.quote(hour) + ", " + txn.quote(average) +
		") "
		"ON CONFLICT (hour) DO UPDATE SET average_temperature = "
		"EXCLUDED.average_temperature");
	txn.commit();
}

void Database::upsert_daily_average(const std::string& day, double average) {
	pqxx::work txn(connection_);
	txn.exec("INSERT INTO daily_averages (day, average_temperature) VALUES (" +
			 txn.quote(day) + ", " + txn.quote(average) +
			 ") "
			 "ON CONFLICT (day) DO UPDATE SET average_temperature = "
			 "EXCLUDED.average_temperature");
	txn.commit();
}

double Database::calculate_hourly_average(const std::string& hour) {
	pqxx::work txn(connection_);
	pqxx::result result = txn.exec(
		"SELECT AVG(temperature) FROM temperature_data "
		"WHERE date_trunc('hour', timestamp) = " +
		txn.quote(hour));
	txn.commit();
	return result[0][0].as<double>();
}

double Database::calculate_daily_average(const std::string& day) {
	pqxx::work txn(connection_);
	pqxx::result result = txn.exec(
		"SELECT AVG(temperature) FROM temperature_data "
		"WHERE date_trunc('day', timestamp) = " +
		txn.quote(day));
	txn.commit();
	return result[0][0].as<double>();
}

pqxx::connection& Database::connection() { return connection_; }
