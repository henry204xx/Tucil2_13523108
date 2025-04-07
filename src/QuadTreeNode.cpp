#include "QuadTreeNode.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <cstring>
#include <limits>

QuadTreeNode::QuadTreeNode(int x, int y, int width, int height) 
    : x(x), y(y), width(width), height(height), isLeaf(false), avgColor{0, 0, 0}, children{nullptr, nullptr, nullptr, nullptr} {
}

QuadTreeNode::~QuadTreeNode() {
    for (int i = 0; i < 4; i++) {
        delete children[i];
    }
}

QuadTreeNode* QuadTreeNode::getChild(int index) const {
    if (index < 0 || index > 3) return nullptr;
    return children[index];
}

int QuadTreeNode::getWidth() const {
    return width;
}

int QuadTreeNode::getHeight() const {
    return height;
}

int QuadTreeNode::countLeafNodes() const {
    if (isLeaf) return 1;
    int count = 0;
    for (int i = 0; i < 4; i++) {
        if (children[i]) count += children[i]->countLeafNodes();
    }
    return count;
}

int QuadTreeNode::countTotalNodes() const {
    int count = 1; // Count this node
    for (int i = 0; i < 4; i++) {
        if (children[i]) count += children[i]->countTotalNodes();
    }
    return count;
}

int QuadTreeNode::depth() const {
    if (isLeaf) return 0;
    int maxDepth = 0;
    for (int i = 0; i < 4; i++) {
        if (children[i]) {
            int childDepth = children[i]->depth();
            if (childDepth > maxDepth) {
                maxDepth = childDepth;
            }
        }
    }
    return maxDepth + 1;
}

void QuadTreeNode::compress(const Image& img, const int method, double threshold, int minBlockSize) {

    if (method == 5) {
        compressWithSSIM(img, threshold, minBlockSize);
        return;
    }
    
    if (width <= minBlockSize || height <= minBlockSize) {
        calculateAverageColor(img);
        isLeaf = true;
        return;
    }

    if (width / 2 < minBlockSize || height / 2 < minBlockSize) {
        calculateAverageColor(img);
        isLeaf = true;
        return;
    }

    double error = 0.0;
    if (method == 1) {
        error = calculateVariance(img);
    } else if (method == 2) {
        error = calculateMAD(img);
    } else if (method == 3) {
        error = calculateMaxDifference(img);
    } else if (method == 4) {
        error = calculateEntropy(img);
    }

    if (error <= threshold) {
        calculateAverageColor(img);
        isLeaf = true;
    } else {
        int halfWidth = width / 2;
        int halfHeight = height / 2;
        int remainingWidth = width - halfWidth;
        int remainingHeight = height - halfHeight;
        
        children[0] = new QuadTreeNode(x, y, halfWidth, halfHeight);
        children[1] = new QuadTreeNode(x + halfWidth, y, remainingWidth, halfHeight);
        children[2] = new QuadTreeNode(x, y + halfHeight, halfWidth, remainingHeight);
        children[3] = new QuadTreeNode(x + halfWidth, y + halfHeight, remainingWidth, remainingHeight);

        for (int i = 0; i < 4; i++) {
            if (children[i]) {
                children[i]->compress(img, method, threshold, minBlockSize);
            }
        }
    }
}

double QuadTreeNode::calculateVariance(const Image& img) const {
    double mean[3] = {0};
    double variance[3] = {0};
    int pixelCount = 0;
    int imgWidth = img.getWidth();
    int imgHeight = img.getHeight();

    // Calculate mean
    for (int y_pos = y; y_pos < y + height && y_pos < imgHeight; y_pos++) {
        for (int x_pos = x; x_pos < x + width && x_pos < imgWidth; x_pos++) {
            mean[0] += img.getPixel(x_pos, y_pos, 0);
            mean[1] += img.getPixel(x_pos, y_pos, 1);
            mean[2] += img.getPixel(x_pos, y_pos, 2);
            pixelCount++;
        }
    }

    if (pixelCount == 0) return 0.0;

    for (int k = 0; k < 3; k++) {
        mean[k] /= pixelCount;
    }

    // Calculate variance ()
    for (int y_pos = y; y_pos < y + height && y_pos < imgHeight; y_pos++) {
        for (int x_pos = x; x_pos < x + width && x_pos < imgWidth; x_pos++) {
            variance[0] += pow(img.getPixel(x_pos, y_pos, 0) - mean[0], 2);
            variance[1] += pow(img.getPixel(x_pos, y_pos, 1) - mean[1], 2);
            variance[2] += pow(img.getPixel(x_pos, y_pos, 2) - mean[2], 2);
        }
    }

    return (variance[0] + variance[1] + variance[2]) / (3 * pixelCount);
}

double QuadTreeNode::calculateMAD(const Image& img) const {
    double mean[3] = {0};
    double mad[3] = {0};
    int pixelCount = 0;
    int imgWidth = img.getWidth();
    int imgHeight = img.getHeight();

    // Calculate mean
    for (int y_pos = y; y_pos < y + height && y_pos < imgHeight; y_pos++) {
        for (int x_pos = x; x_pos < x + width && x_pos < imgWidth; x_pos++) {
            mean[0] += img.getPixel(x_pos, y_pos, 0);
            mean[1] += img.getPixel(x_pos, y_pos, 1);
            mean[2] += img.getPixel(x_pos, y_pos, 2);
            pixelCount++;
        }
    }

    if (pixelCount == 0) return 0.0;

    for (int k = 0; k < 3; k++) {
        mean[k] /= pixelCount;
    }

    // absolute of (pixel - mean)
    for (int y_pos = y; y_pos < y + height && y_pos < imgHeight; y_pos++) {
        for (int x_pos = x; x_pos < x + width && x_pos < imgWidth; x_pos++) {
            mad[0] += abs(img.getPixel(x_pos, y_pos, 0) - mean[0]);
            mad[1] += abs(img.getPixel(x_pos, y_pos, 1) - mean[1]);
            mad[2] += abs(img.getPixel(x_pos, y_pos, 2) - mean[2]);
        }
    }

    return (mad[0] + mad[1] + mad[2]) / (3 * pixelCount);
}

double QuadTreeNode::calculateMaxDifference(const Image& img) const {
    int minVal[3] = {numeric_limits<int>::max(), 
                    numeric_limits<int>::max(), 
                    numeric_limits<int>::max()};
    int maxVal[3] = {numeric_limits<int>::min(), 
                    numeric_limits<int>::min(), 
                    numeric_limits<int>::min()};
    int imgWidth = img.getWidth();
    int imgHeight = img.getHeight();

    // Calculate min and max values for each channel
    for (int y_pos = y; y_pos < y + height && y_pos < imgHeight; y_pos++) {
        for (int x_pos = x; x_pos < x + width && x_pos < imgWidth; x_pos++) {
            for (int c = 0; c < 3; c++) {
                int val = img.getPixel(x_pos, y_pos, c);
                minVal[c] = min(minVal[c], val);
                maxVal[c] = max(maxVal[c], val);
            }
        }
    }
    
    //calculate max difference (max - min) for each channel and divide by 3
    double maxDiff = 0;
    for (int c = 0; c < 3; c++) {
        maxDiff += (maxVal[c] - minVal[c]);
    }

    return maxDiff / 3.0;
}

double QuadTreeNode::calculateEntropy(const Image& img) const {
    int occ[3][256] = {0};
    int pixelCount = 0;
    int imgWidth = img.getWidth();
    int imgHeight = img.getHeight();

    // calculate occurences of each pixel value for each channel
    for (int y_pos = y; y_pos < y + height && y_pos < imgHeight; y_pos++) {
        for (int x_pos = x; x_pos < x + width && x_pos < imgWidth; x_pos++) {
            for (int c = 0; c < 3; c++) {
                int val = img.getPixel(x_pos, y_pos, c);
                occ[c][val]++;
            }
            pixelCount++;
        }
    }

    if (pixelCount == 0) return 0.0;

    // calculate entropy for each channel
    double entropy[3] = {0};
    for (int c = 0; c < 3; c++) {
        for (int val = 0; val < 256; val++) {
            if (occ[c][val] > 0) {
                double p = static_cast<double>(occ[c][val]) / pixelCount;
                entropy[c] -= p * log2(p);
            }
        }
    }

    return (entropy[0] + entropy[1] + entropy[2]) / 3.0;
}


// Constants for SSIM calculation


double QuadTreeNode::calculateSSIM(const Image& img1, const Image& img2, int x1, int y1, int x2, int y2, int width, int height) {
    const double C1 = 6.5025;  // (0.01*255)^2
    const double C2 = 58.5225; // (0.03*255)^2
    const double C3 = C2 / 2;
    double sumX = 0.0, sumY = 0.0;
    double sumX2 = 0.0, sumY2 = 0.0;
    double sumXY = 0.0;
    int pixelCount = 0;
    
    int img1Width = img1.getWidth();
    int img1Height = img1.getHeight();
    int img2Width = img2.getWidth();
    int img2Height = img2.getHeight();
    
    for (int dy = 0; dy < height; dy++) {
        for (int dx = 0; dx < width; dx++) {
            int px1 = x1 + dx;
            int py1 = y1 + dy;
            int px2 = x2 + dx;
            int py2 = y2 + dy;
            
           
            if (px1 >= img1Width || py1 >= img1Height || px2 >= img2Width || py2 >= img2Height) {
                continue;
            }
            
            // Convert to luminance
            double l1 = 0.299 * img1.getPixel(px1, py1, 0) + 
                        0.587 * img1.getPixel(px1, py1, 1) + 
                        0.114 * img1.getPixel(px1, py1, 2);
            
            double l2 = 0.299 * img2.getPixel(px2, py2, 0) + 
                        0.587 * img2.getPixel(px2, py2, 1) + 
                        0.114 * img2.getPixel(px2, py2, 2);
            
            sumX += l1;
            sumY += l2;
            sumX2 += l1 * l1;
            sumY2 += l2 * l2;
            sumXY += l1 * l2;
            pixelCount++;
        }
    }
    
    if (pixelCount == 0) return 0.0;
    
    // Calculate means
    double muX = sumX / pixelCount;
    double muY = sumY / pixelCount;
    
    // Calculate variance and covariance
    double sigmaX2 = (sumX2 / pixelCount) - (muX * muX);
    double sigmaY2 = (sumY2 / pixelCount) - (muY * muY);
    double sigmaXY = (sumXY / pixelCount) - (muX * muY);
    
    // Calculate SSIM components
    double luminance = (2 * muX * muY + C1) / (muX * muX + muY * muY + C1);
    double contrast = (2 * sqrt(sigmaX2) * sqrt(sigmaY2) + C2) / (sigmaX2 + sigmaY2 + C2);
    double structure = (sigmaXY + C3) / (sqrt(sigmaX2) * sqrt(sigmaY2) + C3);
    
    return luminance * contrast * structure;
}



void QuadTreeNode::compressWithSSIM(const Image& img, double threshold, int minBlockSize) {
    if (width <= minBlockSize || height <= minBlockSize) {
        calculateAverageColor(img);
        isLeaf = true;
        return;
    }

    if (width / 2 < minBlockSize || height / 2 < minBlockSize) {
        calculateAverageColor(img);
        isLeaf = true;
        return;
    }
    
    // Calculate the average color for the current node's region
    calculateAverageColor(img);

    // Create a temporary image filled with the average color
    Image temp(width, height);
    for (int y_pos = 0; y_pos < height; y_pos++) {
        for (int x_pos = 0; x_pos < width; x_pos++) {
            temp.setPixel(x_pos, y_pos, 0, static_cast<int>(avgColor[0]));
            temp.setPixel(x_pos, y_pos, 1, static_cast<int>(avgColor[1]));
            temp.setPixel(x_pos, y_pos, 2, static_cast<int>(avgColor[2]));
        }
    }

    // Calculate SSIM between the original region and the temp image
    double ssim = calculateSSIM(img, temp, x, y, 0, 0, width, height);
    
    if (ssim >= threshold) {
        isLeaf = true;
    } else {
        // Split into children and compress them
        int halfWidth = width / 2;
        int halfHeight = height / 2;
        int remainingWidth = width - halfWidth;
        int remainingHeight = height - halfHeight;
        
        children[0] = new QuadTreeNode(x, y, halfWidth, halfHeight);
        children[1] = new QuadTreeNode(x + halfWidth, y, remainingWidth, halfHeight);
        children[2] = new QuadTreeNode(x, y + halfHeight, halfWidth, remainingHeight);
        children[3] = new QuadTreeNode(x + halfWidth, y + halfHeight, remainingWidth, remainingHeight);

        for (int i = 0; i < 4; i++) {
            if (children[i]) {
                children[i]->compressWithSSIM(img, threshold, minBlockSize);
            }
        }
    }
}

void QuadTreeNode::calculateAverageColor(const Image& img) {
    avgColor = {0, 0, 0};
    int pixelCount = 0;
    int imgWidth = img.getWidth();
    int imgHeight = img.getHeight();

    for (int y_pos = y; y_pos < y + height && y_pos < imgHeight; y_pos++) {
        for (int x_pos = x; x_pos < x + width && x_pos < imgWidth; x_pos++) {
            avgColor[0] += img.getPixel(x_pos, y_pos, 0);
            avgColor[1] += img.getPixel(x_pos, y_pos, 1);
            avgColor[2] += img.getPixel(x_pos, y_pos, 2);
            pixelCount++;
        }
    }

    if (pixelCount > 0) {
        for (int k = 0; k < 3; k++) {
            avgColor[k] /= pixelCount;
        }
    }
}

void QuadTreeNode::fillImage(Image& img) const {
    if (isLeaf) {
        int imgWidth = img.getWidth();
        int imgHeight = img.getHeight();
        for (int y_pos = y; y_pos < y + height && y_pos < imgHeight; y_pos++) {
            for (int x_pos = x; x_pos < x + width && x_pos < imgWidth; x_pos++) {
                img.setPixel(x_pos, y_pos, 0, avgColor[0]);
                img.setPixel(x_pos, y_pos, 1, avgColor[1]);
                img.setPixel(x_pos, y_pos, 2, avgColor[2]);
            }
        }
    } else {
        for (int i = 0; i < 4; i++) {
            if (children[i]) {
                children[i]->fillImage(img);
            }
        }
    }
}