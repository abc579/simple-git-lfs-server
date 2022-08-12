#include "base64.h"

std::string base64_decode(const std::string& in) {

    std::string out;

    std::vector<int> base_table (256, -1);
    
    for (int i = 0; i < 64; i++) {
	base_table["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;
    }
    
    unsigned int val = 0;
    int valb = -8;
    
    for (const auto c : in) {
        if (base_table[c] == -1) {
	    break;
	}
	
        val = (val << 6) + base_table[c];
        valb += 6;
	
        if (valb >= 0) {
            out.push_back(char((val>>valb)&0xFF));
            valb -= 8;
        }
    }
    
    return out;
}
