#pragma once

#include <cstdint>

#include <vector>
#include <string>

class CUTF8Handle
{
    enum 
    {
        STATE_ACCEPT = 0,
        STATE_REJECT = 12,
    };

public:
    static void Encode(const std::vector<uint32_t>& stCode, std::string& strStr);

    static void Decode(const std::string& strStr, std::vector<uint32_t>& stCode);

private:
	static void DecodeState(uint32_t* uiState, uint32_t* uiCode, uint8_t ucByte);

private:
    static const uint8_t UTF8_DECODE_TABLE[];
};
