#include <thread>
#include <sstream>
#include <msgpack.hpp>
#include "LayoutAccess.h"

using namespace std;

#define LA_CURRENT_LAYOUT -1

LayoutAccess::LayoutAccess(Database& db) : _db(db) {
	assert(db.isOpen());
}

bool 
LayoutAccess::lock(const string& layoutName) {
	auto tid = this_thread::get_id();
	ostringstream ss;
	auto lock_key = string("s_") + layoutName + string("_lock");
	auto lock_val = _db.get(lock_key, layoutName);

	ss << tid;

	if(!lock_val || (lock_val && lock_val->empty())) {
		_db.put(lock_key, ss.str(), layoutName);
	} else {
		return false;
	}

	// Read back to ensure another thread didn't overwrite it
	lock_val = _db.get(lock_key, layoutName);
	_locks[layoutName] = lock_val && *lock_val == ss.str();
	return _locks[layoutName];	
}

void
LayoutAccess::release(const string& layoutName) {
	auto tid = this_thread::get_id();
	ostringstream ss;
	auto lock_key = string("s_") + layoutName + string("_lock");
	auto lock_val = _db.get(lock_key, layoutName);

	ss << tid;

	if(lock_val && *lock_val == ss.str()) {
		_db.del(lock_key, layoutName);
		_locks[layoutName] = false;
	} 
}

void 
LayoutAccess::createLayout(const string& name, Layout& layout) {
	const auto& layoutKey = "s_" + name + ":0";
	auto key = _db.get(layoutKey, name);
	
	if(key)	return; 

	_createLayout(name, layout, 0);
}

void
LayoutAccess::migrateLayout(const string& name, Layout& layout) {
	auto currentLayout = getLayout(name);
	if(!currentLayout) return;

	layout.version = currentLayout->version + 1;

	_createLayout(name, layout, layout.version);
}

void
LayoutAccess::_createLayout(const string& name, Layout& layout, int version) {
	map<string, int> slots;
	stringstream buffer;
	const auto& layoutKey = "s_" + name + ":" + to_string(version);
	const auto& currentLayoutKey = "s_" + name + ":c";
	auto key = _db.get(layoutKey, name);
	
	if(key)	return; // Key exists

	layout.version = version;
	for(auto slot : layout.slots) {
		slots.insert(make_pair(slot.name, slot.type));
	}
	
	msgpack::type::tuple<int, map<string, int>> src(layout.version, slots);
	msgpack::pack(buffer, src);
	_db.put(layoutKey, buffer.str(), name); 
	_db.put(currentLayoutKey, buffer.str(), name); 
}

unique_ptr<Layout>
LayoutAccess::getLayout(const string& name) {
	return getLayout(name, LA_CURRENT_LAYOUT);
}

unique_ptr<Layout>
LayoutAccess::getLayout(const string& name, int version) {
	const auto& layoutKey = "s_" + name + ":" + (version < 0 ? "c" : to_string(version));
	auto rawLayout = _db.get(layoutKey, name);

	if(!rawLayout) return nullptr; 
	
	auto str = *rawLayout;
	auto oh = msgpack::unpack(str.data(), str.size());
	auto deserialized = oh.get();

	msgpack::type::tuple<int, map<string, int>> stored;
	deserialized.convert(stored);

	auto layout = make_unique<Layout>();
	layout->version = stored.get<0>();

	for(auto& kv : stored.get<1>()) {
		layout->slots.push_back(Slot { kv.first, static_cast<SlotType>(kv.second) });
	}

	return layout;
}

void
LayoutAccess::deleteLayout(const string& name) {
	_db.deleteKeyspace(name);	
}

bool
LayoutAccess::hasLock(const string& layoutName) {
	bool locked;

	try {
		locked =_locks.at(layoutName);		
		return locked;
	} catch (out_of_range& ex) {
		return false;
	}		
}
