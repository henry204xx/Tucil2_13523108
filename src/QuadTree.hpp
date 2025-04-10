#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include "Image.hpp"
#include "QuadTreeNode.hpp"

class QuadTree {
public:
    QuadTree(const Image& img, const int method, double threshold, int minSize, bool targetOn);

    void compressImage(const Image& img);
    void decompressImage(Image& img) const;
    bool saveImage(const string& filename) const;
    double getCompressionRatio(const string& inputFilename, const string& outputFilename) const;
    QuadTreeNode* getRoot() const { return root; }
    double getBestThreshold(const string& inputFilename, int method, double targetRatio);
    double getMaxThresholdForMethod(int method) const;

private:
    QuadTreeNode* root;
    int errorMethod;
    double threshold;
    int minBlockSize;
    int originalWidth;
    int originalHeight;
    bool targetOn;
    bool compressNow;
};

#endif // QUADTREE_HPP