#include "sqlite3store.hpp"
#include <iostream>
#include <sstream>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <libklio/sensorfactory.hpp>


using namespace klio;

// database statements
const std::string createSensorTableStmt(
    "CREATE TABLE sensors(uuid VARCHAR(16) PRIMARY KEY, name VARCHAR(100), unit vARCHAR(20), timezone INTEGER);");
//TODO: Change this to real prepared statement
const std::string insertSensorStmt(
    "INSERT INTO sensors (uuid, name, unit, timezone) VALUES (?1,?2,?3,?4)"
    );
//TODO: Change this to real prepared statement
const std::string selectSensorStmt(
    "SELECT uuid,name,unit,timezone FROM sensors where (uuid=?1)"
    );
const std::string selectAllSensorUUIDsStmt(
    "SELECT uuid FROM sensors"
    );

// Opens a Database file.
void SQLite3Store::open () {
  int rc = sqlite3_open(_filename.c_str(), &db);
  if( rc ){
    std::ostringstream oss;
    oss << "Can't open database: " << sqlite3_errmsg(db);
    sqlite3_close(db);
    throw StoreException(oss.str());
  }
}


void SQLite3Store::initialize() {
  int rc;
  sqlite3_stmt* stmt;
  const char* pzTail ;

  if (has_table("sensors")) {
    LOG("Sensors table already exists, skipping creation.");
    return;
  }

  rc=sqlite3_prepare(
      db,            /* Database handle */
      createSensorTableStmt.c_str(),       /* SQL statement, UTF-8 encoded */
      -1,              /* Maximum length of zSql in bytes - read complete string. */
      &stmt,  /* OUT: Statement handle */
      &pzTail     /* OUT: Pointer to unused portion of zSql */
    );
  if( rc!=SQLITE_OK ){
    std::ostringstream oss;
    oss << "Can't create sensors creation command: " << sqlite3_errmsg(db) << ", error code " << rc;
    sqlite3_finalize(stmt);
    throw StoreException(oss.str());
  }

  rc=sqlite3_step(stmt);
  if( rc!=SQLITE_DONE ) {  // sqlite3_step has finished, no further result lines available
    std::ostringstream oss;
    oss << "Can't create sensors table: " << sqlite3_errmsg(db) << ", error code " << rc;
    sqlite3_finalize(stmt);
    throw StoreException(oss.str());
  }

  sqlite3_finalize(stmt);
}

void SQLite3Store::close () {
  sqlite3_close(db);
}


bool SQLite3Store::has_table(std::string name) {
  int rc;
  sqlite3_stmt* stmt;
  const char* pzTail ;
  const std::string hasSensorTableStmt("select * from sqlite_master where name='sensors';");


  rc=sqlite3_prepare(
      db,                               /* Database handle */
      hasSensorTableStmt.c_str(),       /* SQL statement, UTF-8 encoded */
      -1,                               /* Maximum length of zSql in bytes - read complete string. */
      &stmt,                            /* OUT: Statement handle */
      &pzTail                           /* OUT: Pointer to unused portion of zSql */
      );
  if( rc!=SQLITE_OK ){
    std::ostringstream oss;
    oss << "Can't create sensors query command: " << sqlite3_errmsg(db) << ", error code " << rc;
    sqlite3_finalize(stmt);
    throw StoreException(oss.str());
  }

  rc=sqlite3_step(stmt);
  if( rc!=SQLITE_ROW ) {  // sqlite3_step has finished, there was no result row.
    sqlite3_finalize(stmt);
    return false;
  } else { // there was a result row - the table exists.
    sqlite3_finalize(stmt);
    return true;
  }
}

const std::string SQLite3Store::str() { 
  std::ostringstream oss;
  oss << "SQLite3 database, stored in file " << _filename; 
  return oss.str();
};


void SQLite3Store::addSensor(klio::Sensor::Ptr sensor) {
  int rc;
  sqlite3_stmt* stmt;
  const char* pzTail ;

  LOG("Adding to store: " << sensor);

  checkSensorTable();

  rc=sqlite3_prepare_v2(
      db,            /* Database handle */
      insertSensorStmt.c_str(),       /* SQL statement, UTF-8 encoded */
      -1,              /* Maximum length of zSql in bytes - read complete string. */
      &stmt,  /* OUT: Statement handle */
      &pzTail     /* OUT: Pointer to unused portion of zSql */
    );
  if( rc!=SQLITE_OK ){
    std::ostringstream oss;
    oss << "Can't prepare sensor insertion statement: " << sqlite3_errmsg(db) << ", error code " << rc;
    sqlite3_finalize(stmt);
    throw StoreException(oss.str());
  }

  sqlite3_bind_text(stmt, 1, sensor->uuid_string().c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, sensor->name().c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, sensor->unit().c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 4, sensor->timezone());

  rc=sqlite3_step(stmt);
  if( rc!=SQLITE_DONE ) {  // sqlite3_step has finished, no further result lines available
    std::ostringstream oss;
    oss << "Can't execute sensor insertion statement: " << sqlite3_errmsg(db) << ", error code " << rc;
    sqlite3_finalize(stmt);
    throw StoreException(oss.str());
  }
  sqlite3_clear_bindings(stmt);
  sqlite3_reset(stmt);
  sqlite3_finalize(stmt);

}

bool SQLite3Store::checkSensorTable() {
  if (!has_table("sensors")) {
    std::ostringstream oss;
    oss << "table sensors is missing in " << str();
    throw StoreException(oss.str());
  }
}

std::vector<klio::Sensor::uuid_t> SQLite3Store::getSensorUUIDs() {
  int rc;
  sqlite3_stmt* stmt;
  const char* pzTail ;
  std::vector<klio::Sensor::uuid_t> uuids;

  LOG("Retrieving UUIDs from store." );
  checkSensorTable();
  rc=sqlite3_prepare_v2(
      db,            /* Database handle */
      selectAllSensorUUIDsStmt.c_str(),       /* SQL statement, UTF-8 encoded */
      -1,              /* Maximum length of zSql in bytes - read complete string. */
      &stmt,  /* OUT: Statement handle */
      &pzTail     /* OUT: Pointer to unused portion of zSql */
    );
  if( rc!=SQLITE_OK ){
    std::ostringstream oss;
    oss << "Can't prepare sensor select statement: " << sqlite3_errmsg(db) << ", error code " << rc;
    sqlite3_finalize(stmt);
    throw StoreException(oss.str());
  }

  //sqlite3_bind_text(stmt, 1, to_string(uuid).c_str(), -1, SQLITE_TRANSIENT);

  while (SQLITE_ROW == sqlite3_step(stmt)) {
    const unsigned char* select_uuid = sqlite3_column_text(stmt, 0);
    std::cout << " -> " << select_uuid  << std::endl;
     // type conversion: uuid_string to real uuid type
    boost::uuids::uuid u;
    std::stringstream ss;
    ss << select_uuid;
    ss >> u;
    uuids.push_back(u);
  }
  return uuids;
}

klio::Sensor::Ptr SQLite3Store::getSensor(const klio::Sensor::uuid_t& uuid) {
  int rc;
  sqlite3_stmt* stmt;
  const char* pzTail ;
  klio::SensorFactory::Ptr sensor_factory(new klio::SensorFactory());

  LOG("Attempting to load sensor " << uuid);
  checkSensorTable();


  rc=sqlite3_prepare_v2(
      db,            /* Database handle */
      selectSensorStmt.c_str(),       /* SQL statement, UTF-8 encoded */
      -1,              /* Maximum length of zSql in bytes - read complete string. */
      &stmt,  /* OUT: Statement handle */
      &pzTail     /* OUT: Pointer to unused portion of zSql */
      );
  if( rc!=SQLITE_OK ){
    std::ostringstream oss;
    oss << "Can't prepare sensor select statement: " << sqlite3_errmsg(db) << ", error code " << rc;
    sqlite3_finalize(stmt);
    throw StoreException(oss.str());
  }

  sqlite3_bind_text(stmt, 1, to_string(uuid).c_str(), -1, SQLITE_TRANSIENT);

  rc=sqlite3_step(stmt);
  if( rc!=SQLITE_ROW ) {  // there MUST be only one result - otherwise, ve would have several primary IDs stored.
    std::ostringstream oss;
    oss << "Can't execute sensor select statement: " << sqlite3_errmsg(db) << ", error code " << rc;
    sqlite3_finalize(stmt);
    throw StoreException(oss.str());
  }
  const unsigned char* select_uuid = sqlite3_column_text(stmt, 0);
  const unsigned char* select_name = sqlite3_column_text(stmt, 1);
  const unsigned char* select_unit = sqlite3_column_text(stmt, 2);
  int select_timezone = sqlite3_column_int(stmt, 3);
  std::cout << " -> " << select_uuid << " . " << select_name << " . " << select_timezone << std::endl;

  klio::Sensor::Ptr retval(sensor_factory->createSensor(
        std::string((char*)select_uuid), 
        std::string((char*)select_name), 
        std::string((char*)select_unit), 
        select_timezone)); 
  sqlite3_clear_bindings(stmt);
  sqlite3_reset(stmt);
  sqlite3_finalize(stmt);
  return retval;
}