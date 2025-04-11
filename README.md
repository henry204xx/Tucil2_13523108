
# Tucil2_13523108
> Tugas Kecil 2 IF2211 Strategi Algoritma
Kompresi Gambar Dengan Metode Quadtree
Semester II Tahun 2024/2025

Program kompresi gambar dengan metode Quadtree

## Requirement
C++17 Compiler (GCC 9+/Clang 10+/MSVC 2019+)

## Usage

Cara untuk menjalankan program

1. Buka terminal atau command prompt.
2. Arahkan ke folder tempat file disimpan dengan perintah berikut:
   - Untuk pengguna **Linux** atau **Windows**:
     ```bash
     cd /path/to/folder
     ```
3. Untuk mengkompilasi program (opsional), jalankan perintah berikut.
    ```bash
    g++ src/QuadTree.cpp src/QuadTreeNode.cpp src/Image.cpp src/main.cpp -o bin/main
    ```

3. Untuk menjalankan program, jalankan perintah berikut.  
    ```bash
    ./bin/main
    ```
4. Jika berhasil, akan muncul tampilan program.
    ```bash
    =========================================
       QuadTree Image Compression
    =========================================
    ```
5. Untuk input dan output gambar, pastikan menggunakan alamat absolut sesuai dengan jenis terminal yang digunakan: 
    ```bash
    absolute-path/to/image
    ```
     ### Contoh:
    
        C:/Users/Nice/Downloads/test.jpg

6. Untuk persentase kompresi yang lebih tinggi, gunakan nilai threshold yang besar atau minimum block size yang besar. Khusus metode SSIM, nilai threshold yang lebih rendah menghasilkan kompresi yang lebih tinggi.

7. Untuk mendapat persentase kompresi yang diinginkan, masukkan nilai compression target (0-1). Masukkan nilai 0 jika ingin memasukkan threshold dan minimum block size secara manual.
   

## Features

Program ini dapat
1. Mengkompresi gambar dengan tipe JPG, JPEG, PNG, BMP.
2. Mengkompresi gambar dengan persentase kompresi yang diinginkan.
3. Meyimpan gambar hasil kompresi pada alamat yang ditentukan.



## Created by

Henry Filberto Shenelo (13523108)
