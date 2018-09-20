#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <cstdio>

#include "../src/KeyAccess.h"

TEST_CASE("Can set a key", "[Key]") {
	const string& path = string("/tmp/immutdb_tests_key_create");
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
	REQUIRE(1 == keys.put(keyName, slotValues, layoutName));
	REQUIRE(2 == keys.put(keyName, slotValues, layoutName));

	system(delcmd.c_str());
}

