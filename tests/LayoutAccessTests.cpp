#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <cstdio>

#include "../src/LayoutAccess.h"
#include "../src/KeyAccess.h"

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

	Database db(path);
	db.open();

	LayoutAccess layoutAccess(db);
	REQUIRE(layoutAccess.lock(layoutName));

	layoutAccess.createLayout(layoutName, layout);

	auto created_layout = layoutAccess.getLayout(layoutName);
	REQUIRE(created_layout);

	REQUIRE(created_layout->version == 0);
	REQUIRE((*created_layout).slots.size() == 2);
	REQUIRE((*created_layout).slots[0].name == slots[1].name);
	REQUIRE((*created_layout).slots[0].type == slots[1].type);
	REQUIRE((*created_layout).slots[1].name == slots[0].name);
	REQUIRE((*created_layout).slots[1].type == slots[0].type);

	layoutAccess.release(layoutName);
	REQUIRE(!layoutAccess.hasLock(layoutName));

	system(delcmd.c_str());
}

TEST_CASE("Can migrate a layout to new layout structure", "[Layout]") {
	const string& path = string("/tmp/immutdb_tests_layout_migrate");
	const string& delcmd = string("rm -r ") + path;
	const string& layoutName = "Customer";

	vector<Slot> slots {
		{ "Name", SlotType::STRING },
		{ "Age", SlotType::INT }
	};

	Layout layout { 0, slots };

	Database db(path);
	db.open();

	// Set up first layout
	LayoutAccess layoutAccess(db);
	layoutAccess.lock(layoutName);

	layoutAccess.createLayout(layoutName, layout);

	auto created_layout = layoutAccess.getLayout(layoutName);
	REQUIRE(created_layout);

	REQUIRE(created_layout->version == layout.version);
	layoutAccess.release(layoutName);

	// Migrate to a new layout

	slots.push_back(Slot { "Gender", SlotType::STRING });
	Layout newLayout { 1, slots };

	layoutAccess.migrateLayout(layoutName, newLayout);

	unique_ptr<Layout> new_layout;
	new_layout = layoutAccess.getLayout(layoutName, 1);
	auto n_layout = *new_layout;

	unique_ptr<Layout> old_layout;
	old_layout = layoutAccess.getLayout(layoutName, 0);
	auto o_layout = *old_layout;

	unique_ptr<Layout> current_layout;
	current_layout = layoutAccess.getLayout(layoutName);
	auto c_layout = *new_layout;

	REQUIRE(n_layout.version == 1);
	REQUIRE(n_layout.slots.size() == 3);

	REQUIRE(o_layout.version == 0);
	REQUIRE(o_layout.slots.size() == 2);

	REQUIRE(c_layout.version == 1);
	REQUIRE(c_layout.slots.size() == 3);

	system(delcmd.c_str());
}

TEST_CASE("Can delete a layout", "[Layout]") {
	const string& path = string("/tmp/immutdb_tests_layout_delete");
	const string& delcmd = string("rm -r ") + path;
	const string& layoutName = "Customer";

	vector<Slot> slots {
		{ "Name", SlotType::STRING },
		{ "Age", SlotType::INT }
	};

	Layout layout { 1, slots };

	unique_ptr<Layout> created_layout;

	Database db(path);
	db.open();

	LayoutAccess layoutAccess(db);
	layoutAccess.createLayout(layoutName, layout);

	created_layout = layoutAccess.getLayout(layoutName);
	REQUIRE(created_layout);
	layoutAccess.release(layoutName);

	layoutAccess.deleteLayout(layoutName);
	created_layout = layoutAccess.getLayout(layoutName);
	REQUIRE(!created_layout);

	system(delcmd.c_str());
}

TEST_CASE("Can set and get record count", "[Layout]") {
	const string& path = string("/tmp/immutdb_tests_layout_delete");
	const string& delcmd = string("rm -r ") + path;
	const string& layoutName = "Customer";
	const string& keyName = "CatoAuestad";
	
	vector<Slot> slots {
		{ "Name", SlotType::STRING },
		{ "Age", SlotType::INT }
	};

	vector<SlotValue> slotValues {
		{ "Name", SlotType::STRING, "Cato Auestad" },
		{ "Age", SlotType::INT, "30" }
	};

	Layout layout { 1, slots };

	Database db(path);
	db.open();

	LayoutAccess layoutAccess(db);	
	layoutAccess.createLayout(layoutName, layout);
	
	KeyAccess keys(db, layoutAccess);

	REQUIRE(0 == keys.put(keyName, slotValues, layoutName));
	slotValues[0].value = "Cato";
	REQUIRE(1 == keys.put(keyName, slotValues, layoutName));

	auto records = layoutAccess.countRecords(layoutName);
	REQUIRE(2 == records);

	system(delcmd.c_str());
}
