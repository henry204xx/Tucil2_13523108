#include "Image.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "include/stb_image_write.h"
#include <iostream>

Image::Image(const string& filename) {
    int channels;
   
    FILE* testFile = fopen(filename.c_str(), "rb");
    if (!testFile) {
        cerr << "Error: File is not found.\n";
    } else {
        fclose(testFile);
    }
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 3);

    try {
        pixels.resize(width * height * 3);
        copy(data, data + width * height * 3, pixels.begin());
    } catch (...) {
        stbi_image_free(data);
        throw;
    }
    
    stbi_image_free(data);
}

Image::Image(int width, int height) : width(width), height(height) {
    pixels.resize(width * height * 3, 0); // Initialize to black
}

Image::~Image() {
    // Automatic cleanup by vector
}

bool Image::save(const string& filename) const {
    try {
        string ext = filename.substr(filename.find_last_of(".") + 1);
        int success = 0;
        
        if (ext == "png") {
            success = stbi_write_png(filename.c_str(), width, height, 3, pixels.data(), width * 3);
        } else if (ext == "jpg" || ext == "jpeg") {
            success = stbi_write_jpg(filename.c_str(), width, height, 3, pixels.data(), 90);
        } else if (ext == "bmp") {
            success = stbi_write_bmp(filename.c_str(), width, height, 3, pixels.data());
        } else {
            throw runtime_error("Please check your path and extension.");
        }
        
        return success != 0;
    } catch (const exception& e) {
        cerr << "Error saving image: " << e.what() << endl;
        return false;
    }
}

int Image::getPixel(int x, int y, int channel) const {
    if (x < 0 || x >= width || y < 0 || y >= height || channel < 0 || channel > 2) {
        throw out_of_range("Pixel coordinates or channel out of range");
    }
    return pixels[(y * width + x) * 3 + channel];
}

void Image::setPixel(int x, int y, int channel, int value) {
    if (x < 0 || x >= width || y < 0 || y >= height || channel < 0 || channel > 2) {
        throw out_of_range("Pixel coordinates or channel out of range");
    }
    value = max(0, min(255, value));  // limit it to 0-255 range
    pixels[(y * width + x) * 3 + channel] = static_cast<unsigned char>(value);
}