#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <cstdio>

#include "../src/LayoutAccess.h"

TEST_CASE("Set layout lock", "[Layout}") {
	const string& path = string("/tmp/immutdb_tests_layout_lock");
	const string& delcmd = string("rm -r ") + path;
	const string& keyspace = "Customer";

	Database db(path);
	db.open();

	LayoutAccess layout(db, keyspace);	

	layout.lock();
	REQUIRE(layout.hasLock());

	layout.release();
	REQUIRE(!layout.hasLock());

	system(delcmd.c_str());
}
