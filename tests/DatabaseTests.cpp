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
	const string& path = string("/tmp/immutdb_tests_") + string(random_string(20));
	const string& delcmd = string("rm -r ") + path;

	dbExists = (access(path.c_str(), F_OK) != -1);
	REQUIRE(dbExists == false);

	Database db(path);

	db.open();

	dbExists = (access(path.c_str(), F_OK) != -1);
	REQUIRE(dbExists == true);

	system(delcmd.c_str());
}

TEST_CASE("Database can get and put file", "[Database]") {
	const string& path = string("/tmp/immutdb_tests_") + string(random_string(20));
	const string& delcmd = string("rm -r ") + path;

	Database db(path);
	db.open();
	
	db.put("akey", "avalue");
	unique_ptr<string> value = db.get("akey");

	REQUIRE(*value == string("avalue"));

	system(delcmd.c_str());
}
