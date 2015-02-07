#include <sstream>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <libklio/sensor.hpp>


using namespace klio;
using namespace boost::uuids;

const string Sensor::uuid_string() const {
    return to_string(_uuid);
}

const string Sensor::str() {
    ostringstream oss;
    oss << _name << "(" << to_string(_uuid) << "), unit: "
            << _unit << ", tz: " << _timezone << ", description: " << _description
            << ", device type: " << _device_type->name();
    return oss.str();
}

const string Sensor::uuid_short() const {

    return boost::algorithm::erase_all_copy(uuid_string(), "-");
}

bool Sensor::operator ==(const Sensor& sensor) {
    return _uuid == sensor.uuid() && _name == sensor.name() && _unit == sensor.unit() && _timezone == sensor.timezone() && _device_type == sensor.device_type();
}

bool Sensor::operator !=(const Sensor& sensor) {
    return not operator==(sensor);
}
