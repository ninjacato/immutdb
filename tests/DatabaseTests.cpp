#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <cstdio>

#include "../src/Database.h"

std::string random_string(size_t length)
{
	auto randchar = []() -> char
	{
		const char charset[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";
		const size_t max_index = (sizeof(charset) - 1);
		return charset[ rand() % max_index ];
	};

	std::string str(length,0);
	std::generate_n( str.begin(), length, randchar );
	return str;
}

TEST_CASE("Database create file if not already existent", "[Database]") {
	bool dbExists;
	const string& path = string("/tmp/immutdb_tests_") + string(random_string(19));
	const string& delcmd = string("rm -r ") + path;

	dbExists = (access(path.c_str(), F_OK) != -1);
	REQUIRE(dbExists == false);

	Database db(path);

	db.open();

	dbExists = (access(path.c_str(), F_OK) != -1);
	REQUIRE(dbExists == true);

	system(delcmd.c_str());
}

TEST_CASE("Database can get and put key", "[Database]") {
	const string& path = string("/tmp/immutdb_tests_") + string(random_string(20));
	const string& delcmd = string("rm -r ") + path;

	Database db(path);
	db.open();
	
	db.put("akey", "avalue");
	unique_ptr<string> value = db.get("akey");

	REQUIRE(*value == string("avalue"));

	system(delcmd.c_str());
}

TEST_CASE("Database can put keys and get all with keys", "[Database]") {
	const string& path = string("/tmp/immutdb_tests_") + string(random_string(21));
	const string& delcmd = string("rm -r ") + path;

	Database db(path);
	db.open();
	
	db.put("k_Customer:CatoAuestad:0", "avalue0", "Customer");
	db.put("k_Customer:CatoAuestad:1", "avalue1", "Customer");
	db.put("k_Customer:CatoAuestad:2", "avalue2", "Customer");
	db.put("k_Customer:Cato:0", "bvalue0", "Customer");
	db.put("k_Customer:Cato:1", "bvalue1", "Customer");

	auto allKVs = db.getAllWithKeys("k_Customer:", "Customer");
	REQUIRE(allKVs->at("k_Customer:CatoAuestad:0") == "avalue0");

	system(delcmd.c_str());
}

TEST_CASE("Database can delete a key", "[Database]") {
	const string& path = string("/tmp/immutdb_tests_") + string(random_string(22));
	const string& delcmd = string("rm -r ") + path;

	Database db(path);
	db.open();
	
	db.put("akey", "avalue");
	db.del("akey");

	unique_ptr<string> value = db.get("akey");
	REQUIRE(!value);

	system(delcmd.c_str());
}

TEST_CASE("Database can get a custom keyspace", "[Database]") {
	const string& path = string("/tmp/immutdb_tests_") + string(random_string(23));
	const string& delcmd = string("rm -r ") + path;

	Database cdb(path);
	cdb.open();
	
	cdb.put("akey", "avalue", "Customer");
	unique_ptr<string> value = cdb.get("akey", "Customer");
	unique_ptr<string> novalue = cdb.get("akey");

	REQUIRE(*value == string("avalue"));
	REQUIRE(!novalue);

	system(delcmd.c_str());
}

TEST_CASE("Database can check status", "[Database]") {
	const string& path = string("/tmp/immutdb_tests_") + string(random_string(24));
	const string& delcmd = string("rm -r ") + path;

	Database cdb(path);

	cdb.open();
	REQUIRE(cdb.isOpen());

	cdb.close();
	REQUIRE(!cdb.isOpen());

	system(delcmd.c_str());
}

TEST_CASE("Database can open read only DB", "[Database]") {
	const string& path = string("/tmp/immutdb_tests_") + string(random_string(25));
	const string& delcmd = string("rm -r ") + path;

	Database db(path);
	db.open();
	
	db.put("akey", "avalue");
	unique_ptr<string> value = db.get("akey");

	REQUIRE(*value == string("avalue"));

	Database db1(path);
	db1.openReadOnly();
	unique_ptr<string> value1 = db1.get("akey");

	Database db2(path);
	db2.openReadOnly();
	unique_ptr<string> value2 = db2.get("akey");

	Database db3(path);
	db3.openReadOnly();
	unique_ptr<string> value3 = db3.get("akey");

	REQUIRE(*value1 == string("avalue"));
	REQUIRE(*value2 == string("avalue"));
	REQUIRE(*value3 == string("avalue"));

	system(delcmd.c_str());
}
