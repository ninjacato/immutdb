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

	auto cursorName = prefix + string("c");
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
