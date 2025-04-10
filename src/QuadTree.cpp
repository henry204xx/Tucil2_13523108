#include "QuadTree.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>

QuadTree::QuadTree(const Image& img, int method, double threshold, int minSize, bool targetOn)
    : root(nullptr), errorMethod(method), threshold(threshold), minBlockSize(minSize),
      originalWidth(img.getWidth()), originalHeight(img.getHeight()), targetOn(targetOn) {
    if (minSize < 1) {
        throw invalid_argument("Minimum block size must be at least 1");
    }
    if (threshold < 0) {
        throw invalid_argument("Threshold must be non-negative");
    }

    root = new QuadTreeNode(0, 0, originalWidth, originalHeight);
 
    compressImage(img);
}

void QuadTree::compressImage(const Image& img) {
    if (root) {
        root->compress(img, errorMethod, threshold, minBlockSize, targetOn);
    }
}

void QuadTree::decompressImage(Image& img) const { // Fill the image with the average color of each node
    if (root) {
        root->fillImage(img);
    }
}

bool QuadTree::saveImage(const string& filename) const {
    if (!root) return false;
    
    Image decompressedImage(originalWidth, originalHeight);
    decompressImage(decompressedImage);
    
    return decompressedImage.save(filename);
}

double QuadTree::getCompressionRatio(const string& originalFile, const string& compressedFile) const {
    // Get the size of the original image and the compressed image
    size_t originalSize = 0;
    size_t compressedSize = 0;

    try {
        originalSize = filesystem::file_size(originalFile);
        compressedSize = filesystem::file_size(compressedFile);
    } catch (const filesystem::filesystem_error& e) {
        cerr << e.what() << endl;
        return 1.0; 
    }

    if (compressedSize == 0) {
        cerr << "Error: Compressed file size is zero. Invalid compression result." << endl;
        return 1.0;
    }

    cout << "Original size: " << originalSize << " bytes" << endl;
    cout << "Compressed size: " << compressedSize << " bytes" << endl;
    cout << "Compression percentage: " << (1.0 - static_cast<double>(compressedSize)/originalSize) * 100.0 << "%" << endl;

    return (1.0 - static_cast<double>(compressedSize)/originalSize) * 100.0;
}

double QuadTree::getBestThreshold(const string& inputFilename, int method, double targetRatio) {
    if (targetRatio < 0 || targetRatio > 1) {
        throw invalid_argument("Target ratio must be between 0 and 1");
    }

    double low = 0.0;
    double high = getMaxThresholdForMethod(method);
    double bestThreshold = 0.0;
    double bestError = numeric_limits<double>::max();
    const double tolerance = 0.01; 
    const int maxIterations = 15;
    int iteration = 0;


    while (iteration < maxIterations && (high - low) > tolerance) {
        try {
            Image img(inputFilename);
            size_t originalSize = filesystem::file_size(inputFilename);
            double mid = (low + high) / 2.0;
            string tempOutput = "temp_" + filesystem::path(inputFilename).filename().string();

            QuadTree quadTree(img, method, mid, 1, targetOn);
            quadTree.compressImage(img); // Compress the image with the current threshold
       
            if (!quadTree.saveImage(tempOutput)) {
                throw runtime_error("Failed to save temporary image.");
            }

            size_t compressedSize = filesystem::file_size(tempOutput);
            double currentRatio = static_cast<double>(compressedSize) / originalSize;
            double currentError = abs(currentRatio - targetRatio);

            if (currentError < bestError) {
                bestError = currentError;
                bestThreshold = mid;
            }

            if (method == 5) { // SSIM case
                if (currentRatio < targetRatio) {
                    low = mid;  
                } else {
                    high = mid;
                }
            } else { // Other methods
                if (currentRatio < targetRatio) {
                    high = mid;
                } else {
                    low = mid;
                }
            }

            iteration++;
            filesystem::remove(tempOutput); // Clean up temporary file
        } 
        catch (const exception& e) {
            cerr << "Error in iteration " << iteration << ": " << e.what() << std::endl;
        }
     }
    return bestThreshold;
}

double QuadTree::getMaxThresholdForMethod(int method) const {
    switch(method) {
        case 1: return 16256.25;   // Variance
        case 2: return 127.5;    // MAD
        case 3: return 255.0;    // Max Difference
        case 4: return 8.0;      // Entropy
        case 5: return 1.0;      // SSIM
        default: throw invalid_argument("Invalid method for threshold calculation");
    }
}
