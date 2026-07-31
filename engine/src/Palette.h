#ifndef PALETTE_H
#define PALETTE_H

#include <cstdint>

class Palette{
private:
    uint16_t palette[256];

public:
    Palette();
    
    void SetColor(unsigned char index, uint16_t color);
    
    uint16_t GetColor(unsigned char index) {return palette[index];}
};

#endif
