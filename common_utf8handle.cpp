#include "common_utf8handle.h"

const uint8_t CUTF8Handle::UTF8_DECODE_TABLE[] =
{
    // The first part of the table maps bytes to character classes that
    // to reduce the size of the transition table and create bitmasks.
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
   10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,

    // The second part is a transition table that maps a combination
    // of a state of the automaton and a character class to a state.
     0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
    12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
    12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
    12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
    12,36,12,12,12,12,12,12,12,12,12,12,
};

void 
CUTF8Handle::Encode(const std::vector<uint32_t>& stCode, std::string& strStr)
{
    for (auto uiCode : stCode)
    {
        if (uiCode <= 0x7f)
        {
            strStr.push_back((unsigned char)uiCode);
        }
        else if (uiCode <= 0x7ff)
        {
            strStr.push_back((unsigned char) (0xc0 + (uiCode >> 6)));
            strStr.push_back((unsigned char) (0x80 + (uiCode & 0x3f)));
        }
        else if (uiCode <= 0xffff)
        {
            strStr.push_back((unsigned char) (0xe0 + (uiCode >> 12)));
            strStr.push_back((unsigned char) (0x80 + ((uiCode >> 6) & 63)));
            strStr.push_back((unsigned char) (0x80 + (uiCode & 63)));
        }
        else if (uiCode <= 0x1ffff)
        {
            strStr.push_back((unsigned char) (0xf0 + (uiCode >> 18)));
            strStr.push_back((unsigned char) (0x80 + ((uiCode >> 12) & 0x3f)));
            strStr.push_back((unsigned char) (0x80 + ((uiCode >> 6) & 0x3f)));
            strStr.push_back((unsigned char) (0x80 + (uiCode & 0x3f)));
        }
    }
}

void
CUTF8Handle::Decode(const std::string& strStr, std::vector<uint32_t>& stCode)
{
    uint32_t uiIndex = 0, uiCode = 0;
    uint32_t uiState = STATE_ACCEPT;

    while (uiIndex < strStr.size())
    {
        DecodeState(&uiState, &uiCode, strStr[uiIndex++]);
        if (STATE_ACCEPT == uiState)
        {
            stCode.push_back(uiCode);
        }
        else if (STATE_REJECT == uiState)
        {
            uiState = STATE_ACCEPT;
            stCode.push_back(0xFFFD);
        }
    }
}

void CUTF8Handle::DecodeState(uint32_t* uiState, uint32_t* uiCode, uint8_t ucByte)
{
	uint8_t ucType = UTF8_DECODE_TABLE[ucByte];

	*uiCode = (*uiState != STATE_ACCEPT) ? (ucByte & 0x3fu) | (*uiCode << 6) : (0xff >> ucType) & (ucByte);

	*uiState = UTF8_DECODE_TABLE[256 + *uiState + ucType];
}
