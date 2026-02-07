// (1, "Alice", "Math")
// encode to this -> [1]["1"][5]["Alice"][4]["Math"]

#include "tuple_encoder.h"
#include <cstring>
#include <cstdint>


std::vector<char>encode_row(const std::vector<std::string>& values){
  std::vector<char>buff;

  for(const auto &v : values){
    uint16_t len = v.size();
    
    //store length 
    buff.insert(buff.end(),reinterpret_cast<char*>(&len),reinterpret_cast<char*>(&len) + sizeof(u_int16_t));
    
    //store bytes
    buff.insert(buff.end(), v.begin() , v.end());

  }

  return buff;
}


// Decoder 

// Read 2 bytes to find out how long the next string is.
// Read N bytes to get the actual text.

std::vector<std::string> decode_row(const std::vector<char>& tuple) {
    std::vector<std::string> values;

    size_t pos = 0;

    while (pos < tuple.size()) {
        uint16_t len;
        std::memcpy(&len, tuple.data() + pos, sizeof(uint16_t));
        pos += sizeof(uint16_t);

        std::string value(tuple.data() + pos, len);
        pos += len;

        values.push_back(value);
    }

    return values;
}





