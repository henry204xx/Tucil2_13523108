#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <vector>
#include <string>
#include <stdexcept>
using namespace std;

class Image {
public:
    Image(const string& filename);
    Image(int width, int height);  
    ~Image();

    bool save(const string& filename) const;
    int getPixel(int x, int y, int channel) const;
    void setPixel(int x, int y, int channel, int value);
    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    int width, height;
    vector<unsigned char> pixels;
};

#endif // IMAGE_HPP