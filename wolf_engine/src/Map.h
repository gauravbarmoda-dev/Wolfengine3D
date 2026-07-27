#ifndef MAP_H
#define MAP_H

#include <vector>

class Map{
private:
    int mapSize;
    int mapShift;
    int mapMask;
    std::vector <unsigned char> mapData;

public:
    Map(int requestedSize);
    ~Map();

    void SetTile(int x, int y, char tileID);

    int GetWidth()  const {return mapSize;}
    
    int GetHeight() const {return mapSize;}

    const unsigned char* GetRawData() const {return mapData.data();}

    int GetMapShift() {return mapShift;}

    int GetMapMask()  {return mapMask;}

    inline char GetTile(int x, int y) const{
        return mapData[(y << mapShift) + x];
    }

    inline char GetWorldTile(float x, float y) const{
        int Gx = static_cast<int>(x);
        int Gy = static_cast<int>(y);
        return GetTile(Gx, Gy);
    }
};

#endif
