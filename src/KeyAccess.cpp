#include <sstream>

#include "KeyAccess.h"

using namespace std;

KeyAccess::KeyAccess(Database& db, LayoutAccess& la) : _db(db), _la(la) {
	assert(_db.isOpen());
}

int
KeyAccess::put(const string& name, vector<SlotValue> values, const string& lname) {
	auto layout = _la.getLayout(lname);	
	if(!layout) return -1;

	bool validPut = false;
	stringstream buffer;
	for(auto value : values) {
		validPut = false;

		for(auto slot : layout->slots) {
			if(value.name == slot.name && value.type == slot.type) {
				value.layoutVersion = layout->version;
				validPut = true;
			}	
		}

		if(!validPut) return -1;
	}		

	auto prefix = "k_" + lname + ":" + name + ":";
	auto version = 0;
	auto cursor = 0;

	auto cursorName = "c" + prefix;
	auto currentCursor = _db.get(cursorName, lname);
	if(currentCursor) {
		cursor = stoi(*currentCursor);
		cursor++;
		version = cursor;
	}
	
	auto keyName = prefix + to_string(version);

	msgpack::pack(buffer, values);
	_db.put(keyName, buffer.str(), lname); 
	_db.put(cursorName, to_string(cursor), lname); 
	_la.incrementRecordCount(lname);

	return version;
}

unique_ptr<vector<SlotValue>>
KeyAccess::get(const string& name, int version, const string& lname) {
	auto prefix = "k_" + lname + ":" + name + ":";
	auto slotName = prefix + to_string(version);
	auto slot = _db.get(slotName, lname);
	if(!slot) return nullptr;

	auto str = *slot;
	auto oh = msgpack::unpack(str.data(), str.size());
	auto deserialized = oh.get();

	vector<SlotValue> slots;
	deserialized.convert(slots);

	return make_unique<vector<SlotValue>>(slots);
}

unique_ptr<vector<vector<SlotValue>>>
KeyAccess::getAllVersions(const string& name, const string& lname) {
	vector<vector<SlotValue>> versions;
	auto prefix = "k_" + lname + ":" + name + ":";
	auto allValues = _db.getAll(prefix, lname);

	if(!allValues) return nullptr;

	auto values = *allValues;
	for(auto str : values) {
		msgpack::object_handle oh = msgpack::unpack(str.data(), str.size());
		msgpack::object deserialized = oh.get();

		vector<SlotValue> slots;
		deserialized.convert(slots);

		versions.push_back(slots);
	}

	return make_unique<vector<vector<SlotValue>>>(versions);
}

unique_ptr<map<string, vector<vector<SlotValue>>>>
KeyAccess::getAllKeys(const string& lname) {
	auto prefix = "k_" + lname + ":";
	auto allKeys = _db.getAllWithKeys(prefix, lname);
	if(!allKeys) return nullptr;

	map<string, vector<vector<SlotValue>>> result;
	for(auto const& [key, val] : *allKeys) {
		auto keyName = key.substr(prefix.length()); // Strip prefix
		keyName = keyName.substr(0, keyName.find(":")); // Strip version no

		auto oh = msgpack::unpack(val.data(), val.size());
		msgpack::object deserialized = oh.get();
		vector<SlotValue> slots;
		deserialized.convert(slots);
		
		result[keyName].push_back(slots);
	}
	
	return make_unique<map<string, vector<vector<SlotValue>>>>(result);
}
