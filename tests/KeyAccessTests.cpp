#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <cstdio>

#include "../src/KeyAccess.h"

TEST_CASE("Can set and get a key", "[Key]") {
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

	auto val1 = keys.get(keyName, 0, layoutName);
	REQUIRE(val1);
	REQUIRE((*val1)[0].value == "Cato Auestad");

	slotValues[0].value = "Cato";
	REQUIRE(1 == keys.put(keyName, slotValues, layoutName));

	auto val2 = keys.get(keyName, 1, layoutName);
	REQUIRE(val2);
	REQUIRE((*val2)[0].value == "Cato");

	slotValues[0].value = "Auestad";
	REQUIRE(2 == keys.put(keyName, slotValues, layoutName));

	auto val3 = keys.get(keyName, 2, layoutName);
	REQUIRE(val3);
	REQUIRE((*val3)[0].value == "Auestad");

	auto allKeys = keys.getAllVersions(keyName, layoutName);
	REQUIRE(allKeys);
	auto allKeyVersions = *allKeys;

	for(auto i = 0; i < allKeyVersions.size(); i++) {
		auto name = allKeyVersions[i][0].value; 
		switch(i) {
		case 0:
			REQUIRE(name == "Cato Auestad");
			break;
		case 1:
			REQUIRE(name == "Cato");
			break;
		case 2:
			REQUIRE(name == "Auestad");
			break;
		default:
			REQUIRE(false);
		}
	}

	auto allKVs = keys.getAllKeys("Customer");
	// allKV[keyName][version][slot]
	REQUIRE(allKVs->at("CatoAuestad")[0][0].value == "Cato Auestad");
	REQUIRE(allKVs->at("CatoAuestad")[1][0].value == "Cato");
	REQUIRE(allKVs->at("CatoAuestad")[2][0].value == "Auestad");

	system(delcmd.c_str());
}
