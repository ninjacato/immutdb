#include <thread>
#include <sstream>
#include "Schema.h"

using namespace std;

Schema::Schema(Database& db, const string& keyspace) : _db(db), _ks(keyspace) {
	assert(db.isOpen());
	assert(!keyspace.empty());
}

bool 
Schema::lock(void) {
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

	lock_val = _db.get(lock_key, _ks);
	_hasLock = lock_val && **lock_val == ss.str();
	return _hasLock;	
}

bool
Schema::hasLock(void) {
	return _hasLock;
}
