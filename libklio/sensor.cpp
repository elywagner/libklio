#include "sensor.hpp"
#include <sstream>
#include <boost/uuid/uuid_io.hpp>

using namespace klio;

const std::string Sensor::uuid_string() const { 
  return to_string(_uuid); 
};

const std::string Sensor::str() {
  std::ostringstream oss;
  oss << "Sensor " << _name << "(" << to_string(_uuid) << "), unit " 
    << _unit << ", tz=" << (int)_timezone;
  return oss.str();
}


bool Sensor::operator == (const Sensor& rhs) {
  //std::cout << "DOOOO" << std::endl;
  //LOG("name: " << _name << " == " << rhs.name());
  //LOG("uuid: " << _uuid << " == " << rhs.uuid());
  //LOG("unit: " << _unit << " == " << rhs.unit());
  //LOG("timezone: " << (int)_timezone << " == " << (int)rhs.timezone());
  if (_name==rhs.name() && _uuid==rhs.uuid() &&
      _unit==rhs.unit() && _timezone==rhs.timezone())
    return true;
  else
    return false;
}

bool Sensor::operator != (const Sensor& rhs) {
  return not operator==(rhs);
}