#include "Database.h"
#include <memory>

using namespace std;

Database::Database(const string& path) : _path(path) {
	options.IncreaseParallelism();
	options.OptimizeLevelStyleCompaction();
	options.create_if_missing = true;
}

void Database::open(void) {
	Status s;

	s = DB::Open(options, _path, &_db);
	assert(s.ok());
}

void Database::close(void) {
	delete _db;
}

unique_ptr<string> Database::get(const string& key) {
	Status s;
	string value;

	s = _db->Get(ReadOptions(), key, &value);	
	assert(s.ok());
	return make_unique<string>(value);
}

void Database::put(const string& key, const string& val) {
	Status s;
	
	s = _db->Put(WriteOptions(), key, val);
	assert(s.ok());
}
