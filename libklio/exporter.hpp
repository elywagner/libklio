/**
 * This file is part of libklio.
 *
 * (c) Fraunhofer ITWM - Mathias Dalheimer <dalheimer@itwm.fhg.de>, 2012
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
 *
 */


#ifndef LIBKLIO_EXPORTER_HPP
#define LIBKLIO_EXPORTER_HPP 1

#include <iostream>
#include "store.hpp"

namespace klio {
  class Exporter {
    public:
      typedef std::tr1::shared_ptr<Exporter> Ptr;
      Exporter (std::ostream& out) :
        _out(out) {};
      virtual void process(klio::readings_t_Ptr readings,
          const std::string& name, const std::string& description) = 0;
      virtual ~Exporter() {};

    protected:
      std::ostream& _out;

    private:
      Exporter (const Exporter& original);
      Exporter& operator= (const Exporter& rhs);
      
  };
  
};


#endif /* LIBKLIO_EXPORTER_HPP */
