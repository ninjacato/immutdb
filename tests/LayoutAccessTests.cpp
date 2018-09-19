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
	REQUIRE(!layout.lock(layoutName));
	REQUIRE(layout.hasLock(layoutName));

	layout.release(layoutName);
	REQUIRE(!layout.hasLock(layoutName));

	REQUIRE(!layout.hasLock("Nonsense"));

	system(delcmd.c_str());
}

TEST_CASE("Can create and get layout", "[Layout]") {

	const string& path = string("/tmp/immutdb_tests_layout_create");
	const string& delcmd = string("rm -r ") + path;
	const string& layoutName = "Customer";

	vector<Slot> slots {
		{ "Name", SlotType::STRING },
		{ "Age", SlotType::INT }
	};

	Layout layout { 1, slots };

	optional<unique_ptr<Layout>> created_layout;

	Database db(path);
	db.open();

	LayoutAccess layoutAccess(db);
	REQUIRE(layoutAccess.lock(layoutName));

	layoutAccess.createLayout(layoutName, layout);

	created_layout = layoutAccess.getLayout(layoutName);
	REQUIRE(created_layout);

	REQUIRE((**created_layout).version == layout.version);
	REQUIRE((**created_layout).slots.size() == 2);
	REQUIRE((**created_layout).slots[0].name == slots[1].name);
	REQUIRE((**created_layout).slots[0].type == slots[1].type);
	REQUIRE((**created_layout).slots[1].name == slots[0].name);
	REQUIRE((**created_layout).slots[1].type == slots[0].type);

	layoutAccess.release(layoutName);
	REQUIRE(!layoutAccess.hasLock(layoutName));

	system(delcmd.c_str());
}
