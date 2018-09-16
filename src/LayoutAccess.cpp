#include <thread>
#include <sstream>
#include "LayoutAccess.h"

using namespace std;

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

	if(!lock_val || (lock_val && (**lock_val).empty())) {
		_db.put(lock_key, ss.str(), layoutName);
	} else {
		return false;
	}

	// Read back to ensure another thread didn't overwrite it
	lock_val = _db.get(lock_key, layoutName);
	_locks[layoutName] = lock_val && **lock_val == ss.str();
	return _locks[layoutName];	
}

void
LayoutAccess::release(const string& layoutName) {
	auto tid = this_thread::get_id();
	ostringstream ss;
	auto lock_key = string("s_") + layoutName + string("_lock");
	auto lock_val = _db.get(lock_key, layoutName);

	ss << tid;

	if(lock_val && **lock_val == ss.str()) {
		_db.del(lock_key, layoutName);
		_locks[layoutName] = false;
	} 
};

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
