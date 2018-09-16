#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <cstdio>

#include "../src/LayoutAccess.h"

TEST_CASE("Set layout lock", "[Layout}") {
	const string& path = string("/tmp/immutdb_tests_layout_lock");
	const string& delcmd = string("rm -r ") + path;
	const string& layoutName = "Customer";

	Database db(path);
	db.open();

	LayoutAccess layout(db);	

	REQUIRE(layout.lock(layoutName));
	REQUIRE(layout.hasLock(layoutName));

	layout.release(layoutName);
	REQUIRE(!layout.hasLock(layoutName));

	REQUIRE(!layout.hasLock("Nonsense"));

	system(delcmd.c_str());
}
