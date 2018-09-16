#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <cstdio>

#include "../src/Schema.h"

TEST_CASE("Set schema lock", "[Schema}") {
	const string& path = string("/tmp/immutdb_tests_schema_lock");
	const string& delcmd = string("rm -r ") + path;
	const string& keyspace = "Customer";

	Database db(path);
	db.open();

	Schema schema(db, keyspace);	

	schema.lock();
	REQUIRE(schema.hasLock());

	schema.release();
	REQUIRE(!schema.hasLock());

	system(delcmd.c_str());
}
