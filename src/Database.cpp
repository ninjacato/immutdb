#include "Database.h"
#include <memory>

using namespace std;

Database::Database(const string& path) : _path(path) {
	options.IncreaseParallelism();
	options.OptimizeLevelStyleCompaction();
	options.create_if_missing = true;
	options.create_missing_column_families = true;

	cfd.push_back(ColumnFamilyDescriptor(kDefaultColumnFamilyName, ColumnFamilyOptions()));
}

Database::~Database(void) {
	close();
}

void 
Database::open(void) {
	Status s;
	closed = false;
	vector<string> familyNames;

	s = DB::ListColumnFamilies(options, _path, &familyNames);
	if(s.code() != Status::kIOError) {
		for (auto name : familyNames) {
			if (name != kDefaultColumnFamilyName)
				cfd.push_back(ColumnFamilyDescriptor(name, ColumnFamilyOptions()));
		}
	}

	s = DB::Open(options, _path, cfd, &handles, &db);
	assert(s.ok());
}

void 
Database::close(void) {
	// Since this can be closed outside of a destructor
	// we need to check that it havent already been closed
	// so we dont sigsegv on the destructor
	if(!closed) {
		for (auto handle : handles) {
			delete handle;
		}

		delete db;
	}

	closed = true;
}

optional<unique_ptr<string>> 
Database::get(const string& key) {
	return get(key, kDefaultColumnFamilyName);
}

optional<unique_ptr<string>> 
Database::get(const string& key, const string& keyspace) {
	Status s;
	string value;
	ColumnFamilyHandle * handle;

	handle = getHandle(keyspace);
	s = db->Get(ReadOptions(), handle, key, &value);	

	if(s.IsNotFound()) {
		return nullopt;
	}
	
	return make_unique<string>(value);
}

void 
Database::put(const string& key, const string& val) {
	put(key, val, kDefaultColumnFamilyName);
}

void 
Database::put(const string& key, const string& val, const string& keyspace) {
	Status s;
	ColumnFamilyHandle * handle;
	if(key.empty()) return;	

	handle = getHandle(keyspace);
	s = db->Put(WriteOptions(), handle, key, val);
	assert(s.ok());
}

void 
Database::del(const string& key) {
	del(key, kDefaultColumnFamilyName);
}

void 
Database::del(const string& key, const string& keyspace) {
	Status s;
	ColumnFamilyHandle * handle;
	if(key.empty()) return;
		
	handle = getHandle(keyspace);
	s = db->Delete(WriteOptions(), handle, key);
	assert(s.ok());
}

ColumnFamilyHandle *
Database::getHandle(const string& keyspace) {
	Status s;
	ColumnFamilyDescriptor descriptor;
	ColumnFamilyHandle * handle;

	handle = NULL;
	for(auto h : handles) {
		if(h->GetName() == keyspace) {
			handle = h;
			break;
		}
	}

	if(!handle) {
		s = db->CreateColumnFamily(ColumnFamilyOptions(), keyspace, &handle);
		assert(s.ok());
		handles.push_back(handle);
		cfd.push_back(ColumnFamilyDescriptor(keyspace, ColumnFamilyOptions()));	
	}
	
	return handle;
}

bool
Database::isOpen(void) {
	return !closed;
};
