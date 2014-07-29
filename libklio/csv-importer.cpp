#include<fstream>
#include <stdlib.h>
#include <boost/tokenizer.hpp>
#include <libklio/time.hpp>
#include "csv-importer.hpp"


using namespace klio;

readings_t_Ptr CSVImporter::process() {

    readings_t_Ptr readings(new readings_t());
    const TimeConverter::Ptr time_converter(new TimeConverter());
    typedef boost::tokenizer< boost::escaped_list_separator<char> > Tokenizer;

    std::vector<std::string> record;
    std::string line;
    int header_lines = 1;

    std::cout << "Importing readings" << std::endl;

    while (getline(_in, line)) {

        if (header_lines-- < 1 && !line.empty()) {

            Tokenizer tokenizer(line);
            record.assign(tokenizer.begin(), tokenizer.end());

            if (!record.empty() && !record.at(0).empty() && !record.at(1).empty()) {

                timestamp_t timestamp = time_converter->convert_from_epoch(atoi(record.at(0).c_str()));

                if (timestamp > 0) {

                    double reading = atof(record.at(1).c_str());
                    std::cout << timestamp << ": " << reading << std::endl;

                    readings->insert(std::pair<timestamp_t, double>(timestamp, reading));
                }
            }
        }
    }
    return readings;
}
