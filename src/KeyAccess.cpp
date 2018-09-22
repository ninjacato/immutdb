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

		for(auto slot : (*layout)->slots) {
			if(value.name == slot.name && value.type == slot.type) {
				value.layoutVersion = (*layout)->version;
				validPut = true;
			}	
		}

		if(!validPut) return false;
	}		

	auto prefix = string("k_") + lname + string(":") + name + string(":");
	auto version = 0;
	auto cursor = 0;

	auto cursorName = string("c") + prefix;
	auto currentCursor = _db.get(cursorName, lname);
	if(currentCursor) {
		cursor = stoi(**currentCursor);
		cursor++;
		version = cursor;
	}
	
	auto keyName = prefix + to_string(version);

	msgpack::pack(buffer, values);
	_db.put(keyName, buffer.str(), lname); 
	_db.put(cursorName, to_string(cursor), lname); 

	return version;
}

optional<unique_ptr<vector<SlotValue>>>
KeyAccess::get(const string& name, int version, const string& lname) {
	auto layout = _la.getLayout(lname);	
	if(!layout) return nullopt;
	
	auto prefix = string("k_") + lname + string(":") + name + string(":");
	auto slotName = prefix + to_string(version);
	auto slot = _db.get(slotName, lname);
	if(!slot) return nullopt;

	auto str = **slot;
	msgpack::object_handle oh = msgpack::unpack(str.data(), str.size());
	msgpack::object deserialized = oh.get();

	vector<SlotValue> slots;
	deserialized.convert(slots);

	return make_unique<vector<SlotValue>>(slots);
}

optional<unique_ptr<vector<vector<SlotValue>>>>
KeyAccess::getAllVersions(const string& name, const string& lname) {
	auto layout = _la.getLayout(lname);	
	if(!layout) return nullopt;

	vector<vector<SlotValue>> versions;
	auto prefix = string("k_") + lname + string(":") + name + string(":");
	auto cursorName = string("c") + prefix;
	auto cursorOpt = _db.get(cursorName, lname);
	if(!cursorOpt) return nullopt;

	auto cursor = **cursorOpt;
	auto allValues = _db.getAll(prefix + "0", lname, stoi(cursor));

	if(!allValues) return nullopt;

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
