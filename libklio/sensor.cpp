/**
 * This file is part of libklio.
 *
 * (c) Fraunhofer ITWM - Mathias Dalheimer <dalheimer@itwm.fhg.de>, 2010
 *
 * libklio is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libklio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with libklio. If not, see <http://www.gnu.org/licenses/>.
 */

#include <sstream>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string/erase.hpp>
#include "sensor.hpp"


using namespace klio;

const std::string Sensor::uuid_string() const {
    return to_string(_uuid);
}

const std::string Sensor::str() {
    std::ostringstream oss;
    oss << _name << "(" << to_string(_uuid) << "), unit: "
            << _unit << ", tz: " << _timezone << ", description: " << _description;
    return oss.str();
}

const std::string Sensor::uuid_short() const {

    return boost::algorithm::erase_all_copy(uuid_string(), "-");
}

bool Sensor::operator ==(const Sensor& rhs) {
    return _name == rhs.name() && _uuid == rhs.uuid() && _unit == rhs.unit() && _timezone == rhs.timezone();
}

bool Sensor::operator !=(const Sensor& rhs) {
    return not operator==(rhs);
}
