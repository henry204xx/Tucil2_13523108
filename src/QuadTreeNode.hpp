#ifndef QUADTREENODE_HPP
#define QUADTREENODE_HPP


#include "Image.hpp"

class Image;

class QuadTreeNode {
public:
    QuadTreeNode(int x, int y, int width, int height);
    ~QuadTreeNode();

    QuadTreeNode* getChild(int index) const;
    int getWidth() const;
    int getHeight() const;
    bool isLeafNode() const { return isLeaf; }

    int countLeafNodes() const;
    int countTotalNodes() const;
    int depth() const;

    void compress(const Image& img, const int method, double threshold, int minBlockSize);
    void fillImage(Image& img) const;

private:
    int x, y, width, height;
    bool isLeaf;
    std::vector<int> avgColor;
    QuadTreeNode* children[4];

    void calculateAverageColor(const Image& img);
    double calculateVariance(const Image& img) const;
    double calculateMAD(const Image& img) const;
    double calculateMaxDifference(const Image& img) const;
    double calculateEntropy(const Image& img) const;
    double calculateSSIM(const Image& img1, const Image& img2, int x1, int y1, int x2, int y2, int width, int height);
    void compressWithSSIM(const Image& img, double threshold, int minBlockSize);

};

#endif // QUADTREENODE_HPP