#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include <cassert>

#include "src/preprocessing.hpp"
#include "src/query.hpp"
#include "src/query_processing.hpp"
// #include "src/block.hpp"

Query parse_query(std::string matric_no)
{
    int year = (int)(matric_no.at(matric_no.size() - 2)) - 48;
    year = (year < 2) ? (year + 10) : year;
    bool city = ((int)matric_no.at(matric_no.size() - 3) - 48) % 2;

    return Query(year, city);
}

void test_reading(int block_size, int target_pos)
{
    Block<ColumnTypeConstants::year> year_block(block_size);
    Block<ColumnTypeConstants::month> month_block(block_size);
    Block<ColumnTypeConstants::day> day_block(block_size);

    Block<ColumnTypeConstants::time> time_block(block_size);

    Block<ColumnTypeConstants::raw_timestamp> raw_timestamp_block(block_size);

    Block<ColumnTypeConstants::city> city_block(block_size);

    Block<ColumnTypeConstants::temperature> temperature_block(block_size);
    Block<ColumnTypeConstants::humidity> humidity_block(block_size);

    std::ifstream fin;

    for (auto it = FileNameConstants::file_names.begin(); it != FileNameConstants::file_names.end(); ++it)
    {
        fin.open("data/column_store/" + it->second, std::ios::binary);
        // std::cout << it->second << std::endl;
        if (it->second == "year_encoded.dat")
        {
            std::cout << "Year: ";
            year_block.read_data(fin, target_pos, true);
            // year_block.print_data();
        }
        else if (it->second == "month_encoded.dat")
        {
            std::cout << "Month: ";
            month_block.read_data(fin, target_pos, true);
            // month_block.print_data();
        }
        else if (it->second == "day_encoded.dat")
        {
            std::cout << "Day: ";
            day_block.read_data(fin, target_pos, true);
            // day_block.print_data();
        }
        else if (it->second == "time_encoded.dat")
        {
            std::cout << "Time: ";
            time_block.read_data(fin, target_pos, true);
            // time_block.print_data();
        }
        else if (it->second == "raw_timestamp_encoded.dat")
        {
            std::cout << "Raw Timestamp: ";
            raw_timestamp_block.read_data(fin, target_pos, true);
            // raw_timestamp_block.print_data();
        }
        else if (it->second == "city_encoded.dat")
        {
            std::cout << "City: ";
            city_block.read_data(fin, target_pos, true);
            // city_block.print_data();
        }
        else if (it->second == "temperature_encoded.dat")
        {
            std::cout << "Temperature: ";
            temperature_block.read_data(fin, target_pos, true);
            // temperature_block.print_data();
        }
        else if (it->second == "humidity_encoded.dat")
        {
            std::cout << "Humidity: ";
            humidity_block.read_data(fin, target_pos, true);
            // humidity_block.print_data();
        }
        fin.close();
    }
}

int main(int argc, char **argv)
{
    try
    {
        assert(argc == 3);

        std::string matriculation_number = argv[1];
        int block_size = atoi(argv[2]);

        Query query = parse_query(matriculation_number);
        std::cout << "Query: " << query.year1 << " " << query.year2 << " " << query.city << std::endl;
        preprocess_csv();
        createZonemap<ColumnTypeConstants::year>(block_size, FileNameConstants::year);
        std::cout<<"Finished preprocessing"<<std::endl;

        QueryProcessor query_processor(block_size);

        std::vector<std::string> algos = {"Filter", "BinarySearch", "ZoneMap"};

        for (auto algo : algos)
        {
            query_processor.process_query(algo + "_" + matriculation_number, query.year1, query.year2, query.city, algo);
        }

    }

    catch (const std::exception &exc)
    {
        std::cerr << "Exception " << exc.what();
    }
    return 0;
}