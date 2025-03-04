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
	closed = false;
	vector<string> familyNames;

	auto s = DB::ListColumnFamilies(options, _path, &familyNames);
	if(s.code() != Status::kIOError) {
		for (auto name : familyNames) {
			if (name != kDefaultColumnFamilyName)
				cfd.push_back(ColumnFamilyDescriptor(name, ColumnFamilyOptions()));
		}
	}
}

void 
Database::open(void) {
	init();

	auto s = DB::Open(options, _path, cfd, &handles, &db);
	assert(s.ok());
}

void 
Database::openReadOnly(void) {
	init();

	auto s = DB::OpenForReadOnly(options, _path, cfd, &handles, &db);
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

unique_ptr<string>
Database::get(const string& key) {
	return get(key, kDefaultColumnFamilyName);
}

unique_ptr<string>
Database::get(const string& key, const string& keyspace) {
	auto handle = getHandle(keyspace);
	auto value = make_unique<string>();
	auto s = db->Get(ReadOptions(), handle, key, &(*value));	

	if(s.IsNotFound()) {
		return nullptr;
	}
	
	return value;
}

unique_ptr<vector<string>>
Database::getAll(const string& key, const string& keyspace) {
	auto handle = getHandle(keyspace);
	auto * it = db->NewIterator(ReadOptions(), handle);
	vector<string> values;

	for(it->Seek(key); it->Valid() && !it->key().ToString().compare(0, key.size(), key); it->Next()) {
		auto str = it->value().ToString();
		values.push_back(str);
	}

	delete it;
	
	return make_unique<vector<string>>(values);
}

unique_ptr<map<string, string>>
Database::getAllWithKeys(const string& key, const string& keyspace) {
	auto handle = getHandle(keyspace);
	auto * it = db->NewIterator(ReadOptions(), handle);

	auto kvs = make_unique<map<string, string>>();
	for(it->Seek(key); it->Valid() && !it->key().ToString().compare(0, key.size(), key); it->Next()) {
		auto str = it->value().ToString();
		auto k = it->key().ToString();
		kvs->insert(pair<string, string>(k, str));
	}

	delete it;
	
	return kvs;
}

void 
Database::put(const string& key, const string& val) {
	put(key, val, kDefaultColumnFamilyName);
}

void 
Database::put(const string& key, const string& val, const string& keyspace) {
	if(key.empty()) return;	

	auto handle = getHandle(keyspace);
	auto s = db->Put(WriteOptions(), handle, key, val);
	assert(s.ok());
}

void 
Database::del(const string& key) {
	del(key, kDefaultColumnFamilyName);
}

void 
Database::del(const string& key, const string& keyspace) {
	if(key.empty()) return;
		
	auto handle = getHandle(keyspace);
	auto s = db->Delete(WriteOptions(), handle, key);
	assert(s.ok());
}

void
Database::deleteKeyspace(const string& keyspace) {
	auto handle = getHandle(keyspace);
	auto i = -1;
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
		auto s = db->CreateColumnFamily(ColumnFamilyOptions(), keyspace, &handle);
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
