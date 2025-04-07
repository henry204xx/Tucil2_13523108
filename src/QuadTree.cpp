#include "QuadTree.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>

QuadTree::QuadTree(const Image& img, int method, double threshold, int minSize)
    : root(nullptr), errorMethod(method), threshold(threshold), minBlockSize(minSize),
      originalWidth(img.getWidth()), originalHeight(img.getHeight()) {
    if (minSize < 1) {
        throw invalid_argument("Minimum block size must be at least 1");
    }
    if (threshold < 0) {
        throw invalid_argument("Threshold must be non-negative");
    }

    root = new QuadTreeNode(0, 0, originalWidth, originalHeight);
    compressImage(img);
}

QuadTree::~QuadTree() {
    deleteTree(root);
}

void QuadTree::deleteTree(QuadTreeNode* node) {
    if (node) {
        for (int i = 0; i < 4; i++) {
            deleteTree(node->getChild(i));
        }
        delete node;
    }
}

void QuadTree::compressImage(const Image& img) {
    if (root) {
        root->compress(img, errorMethod, threshold, minBlockSize);
    }
}

void QuadTree::decompressImage(Image& img) const {
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
    // Get the size of the original image
    size_t originalSize = 0;
    size_t compressedSize = 0;

    try {
        originalSize = filesystem::file_size(originalFile);
        compressedSize = filesystem::file_size(compressedFile);
    } catch (const filesystem::filesystem_error& e) {
        cerr << "Error: Could not determine file sizes. " << e.what() << endl;
        return 1.0; // Default to no compression if error occurs
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
