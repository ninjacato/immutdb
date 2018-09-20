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
Database::init(void) {
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
}

void 
Database::open(void) {
	Status s;
	init();

	s = DB::Open(options, _path, cfd, &handles, &db);
	assert(s.ok());
}

void 
Database::openReadByPrefix(const string& prefix) {
	Status s;
	init();

	options.prefix_extractor.reset(NewFixedPrefixTransform(prefix.length()));
	s = DB::OpenForReadOnly(options, _path, cfd, &handles, &db);
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

	auto handle = getHandle(keyspace);
	auto value = make_unique<string>();
	s = db->Get(ReadOptions(), handle, key, &(*value));	

	if(s.IsNotFound()) {
		return nullopt;
	}
	
	return value;
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

void
Database::deleteKeyspace(const string& keyspace) {
	auto handle = getHandle(keyspace);
	int i = -1;
	db->DropColumnFamily(handle);

	for(i = 0; i < (int)handles.size(); i++) {
		if(handles[i]->GetName() == keyspace) {
			break;
		}
	}

	if(i > -1) handles.erase(handles.begin() + i);

	for(i = 0; i < (int)cfd.size(); i++) {
		if(cfd[i].name == keyspace) {
			break;
		}
	}
	
	if(i > -1) cfd.erase(cfd.begin() + i);
	delete handle;
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
