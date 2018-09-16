#include <thread>
#include <sstream>
#include "LayoutAccess.h"

using namespace std;

LayoutAccess::LayoutAccess(Database& db, const string& keyspace) : _db(db), _ks(keyspace) {
	assert(db.isOpen());
	assert(!keyspace.empty());
}

bool 
LayoutAccess::lock(void) {
	auto tid = this_thread::get_id();
	ostringstream ss;
	auto lock_key = string("s_") + _ks + string("_lock");
	auto lock_val = _db.get(lock_key, _ks);

	ss << tid;

	if(!lock_val || (lock_val && (**lock_val).empty())) {
		_db.put(lock_key, ss.str(), _ks);
	} else {
		return false;
	}

	// Read back to ensure another thread didn't overwrite it
	lock_val = _db.get(lock_key, _ks);
	_hasLock = lock_val && **lock_val == ss.str();
	return _hasLock;	
}

void
LayoutAccess::release(void) {
	auto tid = this_thread::get_id();
	ostringstream ss;
	auto lock_key = string("s_") + _ks + string("_lock");
	auto lock_val = _db.get(lock_key, _ks);

	ss << tid;
	if(!_hasLock) return;

	if(lock_val && **lock_val == ss.str()) {
		_db.del(lock_key, _ks);
		_hasLock = false;
	} 
};

bool
LayoutAccess::hasLock(void) {
	return _hasLock;
}
