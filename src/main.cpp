#include <iostream>
#include <string>
#include <chrono>
#include "Image.hpp"
#include "QuadTree.hpp"

using namespace std;
using namespace chrono;

int main() {

    cout << endl;
    cout << "============================" << endl;
    cout << "QuadTree Image Compression" << endl;
    cout << "============================\n" << endl;
    try {
        string filename;
        double threshold;
        int method; 
        int minBlockSize;

        cout << "Enter the absolute path of the image: ";
        cin >> filename;

        do {
            cout << "Select the error calculation method:" << endl;
            cout << "1. Variance" << endl;
            cout << "2. MAD (Mean Absolute Deviation)" << endl;
            cout << "3. Max Difference" << endl;
            cout << "4. Entropy" << endl;
            cout << "5. SSIM (Structural Similarity Index)" << endl;
            cout << "Enter your choice (1-5): ";
            cin >> method;

            if (cin.fail() || method < 1 || method > 5) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number between 1 and 5.\n";
            } else {
            break;
            }
        } while (true);

        bool validThreshold = false;
        do {
            cout << "Enter the compression threshold: ";
            cin >> threshold;

            if (cin.fail()) {
                cin.clear(); 
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter a numeric value.\n";
                continue;
            }

            switch (method) {
                case 1: // Variance
                case 2: // MAD
                
                    if (threshold >= 0) validThreshold = true;
                    else cout << "Threshold must be >= 0.\n";
                    break;

                case 3: // Max Difference
                    if (threshold >= 0 && threshold <= 255) validThreshold = true;
                    else cout << "Max Difference threshold must be between 0 and 255.\n";
                    break;
                case 4: // Entropy
                    if (threshold >= 0 && threshold <= 8) validThreshold = true;
                    else cout << "Entropy threshold must be between 0 and 8.\n";
                    break;

                case 5: // SSIM
                    if (threshold >= 0 && threshold <= 1) validThreshold = true;
                    else cout << "SSIM threshold must be between 0 and 1.\n";
                    break;
            }

        } while (!validThreshold);

        do {
            cout << "Enter the minimum block size: ";
            cin >> minBlockSize;

            if (cin.fail() || minBlockSize < 1) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter block size larger or equal to 1.\n";
            } else {
            break;
            }
        } while (true);
        cout << endl;

        // Start measuring execution time
        auto start = high_resolution_clock::now();

        // Load the image
        Image img(filename);

        // Create and compress the image using QuadTree
        QuadTree quadTree(img, method, threshold, minBlockSize);

        // Save the compressed image
        string outputFilename = "test/compressed_" + filename.substr(filename.find_last_of("/\\") + 1);
        if (quadTree.saveImage(outputFilename)) {
            cout << "Compressed image saved as: " << outputFilename << endl;
        } else {
            cerr << "Failed to save the compressed image." << endl;
        }

        // Display compression ratio
        quadTree.getCompressionRatio(filename, outputFilename);

        cout << "Total nodes: " << quadTree.getRoot()->countTotalNodes() << endl;
        cout << "Depth of the QuadTree: " << quadTree.getRoot()->depth() << endl;

        // Stop measuring execution time
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);
        cout << "Execution time: " << duration.count() << " ms" << endl;

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}