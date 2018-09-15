#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "../src/Database.h"

TEST_CASE("Database create file if not already existent", "[Database]") {
	bool dbExists;
	const string& path = "/tmp/test_db";

	dbExists = (access(path.c_str(), F_OK) != -1);
	REQUIRE(dbExists == false);
}
