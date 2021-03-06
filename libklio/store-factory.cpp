#include <sstream>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/algorithm/string/erase.hpp>
#include "store-factory.hpp"


using namespace boost::algorithm;
using namespace klio;

boost::uuids::random_generator StoreFactory::_gen_uuid;

SQLite3Store::Ptr StoreFactory::create_sqlite3_store(const bfs::path& path) {

    return create_sqlite3_store(path, true);
}

SQLite3Store::Ptr StoreFactory::create_sqlite3_store(const bfs::path& path, const bool prepare) {

    return create_sqlite3_store(path, prepare, true, true, 600, SQLite3Store::OS_SYNC_FULL);
}

SQLite3Store::Ptr StoreFactory::create_sqlite3_store(
        const bfs::path& path,
        const bool prepare,
        const bool auto_commit,
        const bool auto_flush,
        const timestamp_t flush_timeout,
        const std::string& synchronous) {

    SQLite3Store::Ptr store = SQLite3Store::Ptr(new SQLite3Store(path, auto_commit, auto_flush, flush_timeout, synchronous));
    store->open();
    store->initialize();
    if (prepare) {
        store->prepare();
    }
    return store;
}

SQLite3Store::Ptr StoreFactory::open_sqlite3_store(const bfs::path& path) {

    return open_sqlite3_store(path, true, true, 600, SQLite3Store::OS_SYNC_FULL);
}

SQLite3Store::Ptr StoreFactory::open_sqlite3_store(
        const bfs::path& path,
        const bool auto_commit,
        const bool auto_flush,
        const timestamp_t flush_timeout,
        const std::string& synchronous) {

    SQLite3Store::Ptr store = SQLite3Store::Ptr(new SQLite3Store(path, auto_commit, auto_flush, flush_timeout, synchronous));
    store->open();
    store->check_integrity();
    store->prepare();
    return store;
}

#ifdef ENABLE_MSG

MSGStore::Ptr StoreFactory::create_msg_store() {

    return create_msg_store(
            boost::uuids::to_string(_gen_uuid()),
            boost::uuids::to_string(_gen_uuid()));
}

MSGStore::Ptr StoreFactory::create_msg_store(const std::string& id, const std::string& key) {

    return create_msg_store(
            MSGStore::DEFAULT_MSG_URL,
            id,
            key,
            MSGStore::DEFAULT_MSG_DESCRIPTION,
            MSGStore::DEFAULT_MSG_TYPE);
}

MSGStore::Ptr StoreFactory::create_msg_store(
        const std::string& url,
        const std::string& id,
        const std::string& key,
        const std::string& description,
        const std::string& type) {

    MSGStore::Ptr store = MSGStore::Ptr(new MSGStore(url,
            erase_all_copy(id, "-"),
            erase_all_copy(key, "-"),
            description,
            type));
    store->open();
    store->initialize();
    store->prepare();
    return store;
}

#endif /* ENABLE_MSG */

#ifdef ENABLE_ROCKSDB

RocksDBStore::Ptr StoreFactory::create_rocksdb_store(const bfs::path& path) {

    std::map<const std::string, const std::string> db_options;
    std::map<const std::string, const std::string> read_options;
    std::map<const std::string, const std::string> write_options;

    return create_rocksdb_store(path, db_options, read_options, write_options);
}

RocksDBStore::Ptr StoreFactory::create_rocksdb_store(const bfs::path& path,
        const std::map<const std::string, const std::string>& db_options,
        const std::map<const std::string, const std::string>& read_options,
        const std::map<const std::string, const std::string>& write_options) {

    RocksDBStore::Ptr store = RocksDBStore::Ptr(new RocksDBStore(path, db_options, read_options, write_options));
    store->open();
    store->initialize();
    return store;
}

RocksDBStore::Ptr StoreFactory::open_rocksdb_store(const bfs::path& path) {

    std::map<const std::string, const std::string> db_options;
    std::map<const std::string, const std::string> read_options;
    std::map<const std::string, const std::string> write_options;

    return open_rocksdb_store(path, db_options, read_options, write_options);
}

RocksDBStore::Ptr StoreFactory::open_rocksdb_store(const bfs::path& path,
        const std::map<const std::string, const std::string>& db_options,
        const std::map<const std::string, const std::string>& read_options,
        const std::map<const std::string, const std::string>& write_options) {

    RocksDBStore::Ptr store = RocksDBStore::Ptr(new RocksDBStore(path, db_options, read_options, write_options));
    store->open();
    store->check_integrity();
    store->prepare();
    return store;
}

#endif /* ENABLE_ROCKSDB */