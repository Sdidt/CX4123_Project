#ifndef PRE_PROCESS_H
#define PRE_PROCESS_H

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdio>
#include <unordered_map>
#include <iomanip>
#include <locale>
#include "block.hpp"
#include "zonemap/Zone.hpp"
#include "vector"

#include "constants.hpp"

void preprocess_csv();
void readZonemap(int block_size);

template <typename T>
void createZonemap(int block_size, std::string filename)
{
    std::ifstream yearDataStream ("data/column_store/" + filename, std::ios::binary);
    int num_records_per_block= block_size / sizeof(T);
    std::cout << "Num Records per block: " << num_records_per_block <<'\n';
    Block<T> year_read_block = Block<T>(block_size);
    std::ofstream zoneOutStream ("data/zone_maps/zones_" + filename, std::ios::out | std::ios::binary);
    Block<Zone<T>> write_block = Block<Zone<T>>(block_size);

    int block_index = 0;
    int blk_ptr = 0;

    for (int pos = 0; pos < ProgramConstants::num_rows; pos += num_records_per_block)
    {
        year_read_block.read_data(yearDataStream, pos, false);
        //Since the dates are sorted, we can just check the first and last element of the block
        //to determine the range of the block
        T year1 = year_read_block.block_data.front();
        T year2 = year_read_block.block_data.back();
        Zone<T> zone = Zone(block_index, year1, year2);
        write_block.push_data(zone, blk_ptr);
        blk_ptr ++;
        if (write_block.is_full(blk_ptr))
        {
            write_block.write_data(zoneOutStream, blk_ptr);
            blk_ptr = 0;
            write_block.clear();
        }

        block_index++;
        
    }
    if (blk_ptr != 0)
    {
        write_block.write_data(zoneOutStream, blk_ptr);
    }
    yearDataStream.close();
    zoneOutStream.close();
}


#endif