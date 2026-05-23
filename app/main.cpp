#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring> 

// Cấu hình bắt buộc để sử dụng thư viện stb
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// Nhúng các thư viện Template tự chế của nhóm
#include "../lib/Queue.hpp"
#include "../lib/PriorityQueue.hpp"
#include "../lib/HashTable.hpp"
#include "../lib/LinkedList.hpp"
#include "../lib/Algorithms.hpp"

using namespace std;

// Struct quản lý dữ liệu ảnh
struct ImageFrame {
    string fileName;
    unsigned char *pixelData; 
    int width;
    int height;
    int channels;     
    int qualityScore; 
};

// =====================================================================
// KHU VỰC 1: CÁC HÀM XỬ LÝ ẢNH LÕI (CORE ISP ENGINE)
// =====================================================================

ImageFrame loadImage(const string &filePath) {
    ImageFrame frame;
    frame.fileName = filePath;
    frame.pixelData = stbi_load(filePath.c_str(), &frame.width, &frame.height, &frame.channels, 0);

    if (!frame.pixelData) {
        cerr << "[Loi] Khong the doc file anh tai " << filePath << endl;
        frame.width = 0;
    }
    frame.qualityScore = 100;
    return frame;
}

void processMedianStack(vector<ImageFrame>& frames, const string& outputName) {
    if (frames.empty()) return;

    int w = frames[0].width;
    int h = frames[0].height;
    int c = frames[0].channels;

    for (size_t i = 1; i < frames.size(); ++i) {
        if (frames[i].width != w || frames[i].height != h || frames[i].channels != c) {
            cerr << "\n[Loi] Cac anh dau vao khong cung kich thuoc!" << endl;
            for (size_t j = 0; j < frames.size(); ++j) {
                stbi_image_free(frames[j].pixelData);
            }
            return;
        }
    }

    int totalElements = w * h * c; 
    unsigned char* resultPixels = new unsigned char[totalElements];
    int numFrames = frames.size();

    cout << "[ISP Loi] Dang quet va sap xep " << totalElements << " diem mau...\n";

    unsigned char* tempArray = new unsigned char[numFrames];
    for (int i = 0; i < totalElements; ++i) {
        for (int j = 0; j < numFrames; ++j) {
            tempArray[j] = frames[j].pixelData[i];
        }
        quickSort(tempArray, 0, numFrames - 1);
        resultPixels[i] = tempArray[numFrames / 2];
    }
    delete[] tempArray; 

    stbi_write_jpg(outputName.c_str(), w, h, c, resultPixels, 100);

    delete[] resultPixels;
    for (int j = 0; j < numFrames; ++j) {
        stbi_image_free(frames[j].pixelData);
    }
}

void applyPixelate(ImageFrame& frame, int blockSize) {
    if (blockSize <= 1 || frame.pixelData == nullptr) return;
    int w = frame.width;
    int h = frame.height;
    int c = frame.channels;

    cout << "[ISP] Dang ap dung Pixelate...\n";
    for (int y = 0; y < h; y += blockSize) {
        for (int x = 0; x < w; x += blockSize) {
            int sumR = 0, sumG = 0, sumB = 0, count = 0;
            for (int by = 0; by < blockSize && (y + by) < h; ++by) {
                for (int bx = 0; bx < blockSize && (x + bx) < w; ++bx) {
                    int index = ((y + by) * w + (x + bx)) * c;
                    sumR += frame.pixelData[index];
                    sumG += frame.pixelData[index + 1];
                    sumB += frame.pixelData[index + 2];
                    count++;
                }
            }
            unsigned char avgR = sumR / count;
            unsigned char avgG = sumG / count;
            unsigned char avgB = sumB / count;
            for (int by = 0; by < blockSize && (y + by) < h; ++by) {
                for (int bx = 0; bx < blockSize && (x + bx) < w; ++bx) {
                    int index = ((y + by) * w + (x + bx)) * c;
                    frame.pixelData[index] = avgR;
                    frame.pixelData[index + 1] = avgG;
                    frame.pixelData[index + 2] = avgB;
                }
            }
        }
    }
}

// KHAI BÁO CÁC MA TRẬN KERNEL
const float KERNEL_SHARPEN[3][3] = {
    { 0, -1,  0},
    {-1,  5, -1},
    { 0, -1,  0}
};

const float KERNEL_SOFT_SHARPEN[3][3] = {
    { 0.0f,  -0.25f,  0.0f},
    {-0.25f,  2.0f,  -0.25f},
    { 0.0f,  -0.25f,  0.0f}
};

const float KERNEL_EDGE_DETECT[3][3] = {
    {-1, -1, -1},
    {-1,  8, -1},
    {-1, -1, -1}
};

const float KERNEL_EDGE_GENTLE[3][3] = {
    {-1, -1, -1},
    {-1,  8, -1},
    {-1, -1, -1}
};

const float KERNEL_BLUR[3][3] = {
    {1.0f/9, 1.0f/9, 1.0f/9},
    {1.0f/9, 1.0f/9, 1.0f/9},
    {1.0f/9, 1.0f/9, 1.0f/9}
};

const float KERNEL_BOX_BLUR_5x5[5][5] = {
    {1.0f/25, 1.0f/25, 1.0f/25, 1.0f/25, 1.0f/25},
    {1.0f/25, 1.0f/25, 1.0f/25, 1.0f/25, 1.0f/25},
    {1.0f/25, 1.0f/25, 1.0f/25, 1.0f/25, 1.0f/25},
    {1.0f/25, 1.0f/25, 1.0f/25, 1.0f/25, 1.0f/25},
    {1.0f/25, 1.0f/25, 1.0f/25, 1.0f/25, 1.0f/25}
};

void applyConvolution(ImageFrame& frame, const float kernel[3][3]) {
    if (frame.pixelData == nullptr) return;
    int w = frame.width;
    int h = frame.height;
    int c = frame.channels;

    unsigned char* newPixels = new unsigned char[w * h * c];

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            for (int ch = 0; ch < 3; ++ch) { 
                float sum = 0.0f;
                for (int ky = -1; ky <= 1; ++ky) {
                    for (int kx = -1; kx <= 1; ++kx) {
                        int ny = std::max(0, std::min(h - 1, y + ky));
                        int nx = std::max(0, std::min(w - 1, x + kx));
                        int index = (ny * w + nx) * c + ch;
                        sum += frame.pixelData[index] * kernel[ky + 1][kx + 1];
                    }
                }
                int finalValue = (int)sum;
                if (finalValue < 0) finalValue = 0;
                if (finalValue > 255) finalValue = 255;
                newPixels[(y * w + x) * c + ch] = (unsigned char)finalValue;
            }
            if (c == 4) newPixels[(y * w + x) * c + 3] = frame.pixelData[(y * w + x) * c + 3];
        }
    }
    memcpy(frame.pixelData, newPixels, w * h * c);
    delete[] newPixels; 
}

void applyBoxBlur5x5(ImageFrame& frame) {
    if (frame.pixelData == nullptr) return;
    int w = frame.width;
    int h = frame.height;
    int c = frame.channels;

    cout << "[ISP] Dang lam min anh (Loc nhiu)...\n";
    unsigned char* newPixels = new unsigned char[w * h * c];

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            for (int ch = 0; ch < 3; ++ch) {
                float sum = 0.0f;
                for (int ky = -2; ky <= 2; ++ky) { 
                    for (int kx = -2; kx <= 2; ++kx) {
                        int ny = std::max(0, std::min(h - 1, y + ky));
                        int nx = std::max(0, std::min(w - 1, x + kx));
                        int index = (ny * w + nx) * c + ch;
                        sum += frame.pixelData[index] * KERNEL_BOX_BLUR_5x5[ky + 2][kx + 2];
                    }
                }
                newPixels[(y * w + x) * c + ch] = (unsigned char)sum;
            }
            if (c == 4) newPixels[(y * w + x) * c + 3] = frame.pixelData[(y * w + x) * c + 3];
        }
    }
    memcpy(frame.pixelData, newPixels, w * h * c);
    delete[] newPixels;
}

void applyGrayscale(ImageFrame& frame) {
    if (frame.pixelData == nullptr) return;
    int w = frame.width, h = frame.height, c = frame.channels;
    if (c < 3) return; 

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int index = (y * w + x) * c;
            unsigned char r = frame.pixelData[index];
            unsigned char g = frame.pixelData[index + 1];
            unsigned char b = frame.pixelData[index + 2];
            unsigned char gray = (unsigned char)(0.299f * r + 0.587f * g + 0.114f * b);
            frame.pixelData[index] = gray;
            frame.pixelData[index + 1] = gray;
            frame.pixelData[index + 2] = gray;
        }
    }
}

void applyThresholdAndInvert(ImageFrame& frame, int threshold) {
    if (frame.pixelData == nullptr) return;
    int w = frame.width, h = frame.height, c = frame.channels;

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int index = (y * w + x) * c;
            int val = frame.pixelData[index];
            if (val < threshold) {
                frame.pixelData[index] = 255;
                frame.pixelData[index + 1] = 255;
                frame.pixelData[index + 2] = 255;
            } else {
                frame.pixelData[index] = 0;
                frame.pixelData[index + 1] = 0;
                frame.pixelData[index + 2] = 0;
            }
        }
    }
}

// =====================================================================
// KHU VỰC 2: QUẢN LÝ GIAO DIỆN VÀ LOGIC ĐIỀU HƯỚNG
// =====================================================================

void showMenu() {
    cout << "\n====================================================\n";
    cout << "   MULTI-FRAME ISP - HE THONG XU LY ANH TRUONG GIA \n";
    cout << "====================================================\n";
    cout << " Cac lenh ho tro:\n";
    cout << "1. Load (Nap anh vao Hang doi)\n";
    cout << "2. Info (Xem Metadata anh)\n";
    cout << "3. Edit (Chong anh / Lam min / Gan hieu ung)\n";
    cout << "4. Remove (Xoa anh khoi Hang doi)\n";
    cout << "5. History (Xem lich su pipeline)\n";
    cout << "0. Exit\n";
}

void handleLoad(Queue<ImageFrame>& imgQueue, HashTable<string, string>& imgMetadata, LinkedList<string>& pipelineLog) {
    int n;
    cout << "Nhap so luong anh muon nap (0 de tro ve): ";
    if (!(cin >> n) || n == 0) {
        cin.clear(); cin.ignore(10000, '\n');
        return;
    }
    
    for (int i = 0; i < n; ++i) {
        string path;
        cout << "Nhap duong dan anh thu " << i + 1 << " (vd: raw1.jpg): ";
        cin >> path;
        ImageFrame f = loadImage(path);
        if (f.width > 0) {
            imgQueue.enqueue(f);
            string meta = "Resolution: " + to_string(f.width) + "x" + to_string(f.height) + ", Channels: " + to_string(f.channels);
            imgMetadata.insert(path, meta);
        }
    }
    pipelineLog.insertBack("Da nap " + to_string(n) + " anh vao Queue.");
}

void handleInfo(HashTable<string, string>& imgMetadata) {
    string path;
    cout << "Nhap ten file anh can tra cuu (0 de tro ve): ";
    cin >> path;
    if (path == "0") return;

    if (imgMetadata.contains(path)) {
        cout << "[Metadata] " << path << " -> " << imgMetadata.find(path) << endl;
    } else {
        cerr << "[Loi] Khong tim thay du lieu anh nay trong cache!" << endl;
    }
}

// TÍNH NĂNG MỚI: XÓA ẢNH KHỎI HÀNG ĐỢI SỬ DỤNG GIẢI THUẬT ĐỆM
void handleRemove(Queue<ImageFrame>& imgQueue, LinkedList<string>& pipelineLog) {
    if (imgQueue.empty()) {
        cerr << "[Loi] Hang doi trong, khong co anh de xoa!\n";
        return;
    }

    string targetPath;
    cout << "Nhap chinh xac duong dan anh muon xoa (0 de tro ve): ";
    cin >> targetPath;
    if (targetPath == "0") return;

    Queue<ImageFrame> tempQueue;
    bool found = false;

    // Duyệt Queue bằng các hàm cơ bản để tìm và loại bỏ phần tử
    while (!imgQueue.empty()) {
        ImageFrame f = imgQueue.getFront();
        imgQueue.dequeue();

        if (f.fileName == targetPath) {
            if (f.pixelData != nullptr) {
                stbi_image_free(f.pixelData); // Giải phóng bộ nhớ RAM để chống rò rỉ
            }
            found = true;
            cout << "=> Da xoa anh: " << targetPath << " khoi Hang doi.\n";
        } else {
            tempQueue.enqueue(f);
        }
    }

    // Đổ ngược dữ liệu lại Queue chính
    while (!tempQueue.empty()) {
        imgQueue.enqueue(tempQueue.getFront());
        tempQueue.dequeue();
    }

    if (!found) {
        cerr << "[Loi] Khong tim thay anh co duong dan nay trong Hang doi!\n";
    } else {
        pipelineLog.insertBack("Da xoa anh khoi Queue: " + targetPath);
    }
}

void handleEdit(Queue<ImageFrame>& imgQueue, LinkedList<string>& pipelineLog) {
    while (true) {
        if (imgQueue.empty()) {
            cerr << "[Loi] Chua co anh nao trong Hang doi! Vui long Load anh truoc.\n";
            return;
        }

        cout << "\n--- MENU CHINH SUA (EDIT) ---\n";
        cout << "0. Tro ve Menu chinh\n";
        cout << "1. Median Stacking (Gop nhieu anh xoa nhiu)\n";
        cout << "2. Pixelate (Hieu ung khoi vuong)\n";
        cout << "3. Sharpen (Lam net gat)\n";
        cout << "4. Soft Sharpen (Lam net diu nhe)\n";
        cout << "5. Smooth (Lam min / Blur 5x5)\n";
        cout << "6. Edge Raw (Do vien tho)\n";
        cout << "7. Sketch (Tranh ve chi)\n";
        cout << "=> Nhap lua chon (0-7): ";

        int editChoice;
        if (!(cin >> editChoice)) {
            cin.clear(); cin.ignore(10000, '\n');
            cerr << "[Loi] Vui long nhap mot so nguyen!\n";
            continue;
        }

        if (editChoice == 0) return;

        if (editChoice < 1 || editChoice > 7) {
            cerr << "[Loi] Lua chon khong hop le! Vui long nhap tu 0 den 7.\n";
            continue;
        }

        string outName;
        cout << "Nhap ten file muon xuat (vd: ketqua.jpg) hoac '0' de chon lai: ";
        cin >> outName;
        if (outName == "0") continue;

        string algoFolder;

        if (editChoice == 1) {
            algoFolder = "median";
            vector<ImageFrame> activeFrames;
            while (!imgQueue.empty()) {
                activeFrames.push_back(imgQueue.getFront());
                imgQueue.dequeue();
            }

            if (activeFrames.size() < 2) {
                cerr << "[Loi] Can it nhat 2 anh de thuc hien Median Stacking!\n";
                return;
            }

            string fullpath = "../output_images/" + algoFolder + "/" + outName;
            cout << "\n[ISP Engine] Dang thuc hien chong anh...\n";
            processMedianStack(activeFrames, fullpath);
            
            cout << "==> Thanh cong! Da luu tai: " << fullpath << "\n";
            pipelineLog.insertBack("Da Edit: " + algoFolder + " -> " + fullpath);
            activeFrames.clear();
            return; 
        }

        ImageFrame& originalFrame = imgQueue.getFront();
        ImageFrame tempFrame = originalFrame;
        int totalBytes = tempFrame.width * tempFrame.height * tempFrame.channels;
        tempFrame.pixelData = new unsigned char[totalBytes];
        memcpy(tempFrame.pixelData, originalFrame.pixelData, totalBytes);

        switch (editChoice) {
            case 2:
                algoFolder = "pixelate";
                applyPixelate(tempFrame, 15);
                break;
            case 3:
                algoFolder = "sharpen";
                applyConvolution(tempFrame, KERNEL_SHARPEN);
                break;
            case 4:
                algoFolder = "soft_sharpen";
                applyConvolution(tempFrame, KERNEL_SOFT_SHARPEN);
                break;
            case 5:
                algoFolder = "smooth";
                applyBoxBlur5x5(tempFrame);
                break;
            case 6:
                algoFolder = "edge_raw";
                applyConvolution(tempFrame, KERNEL_EDGE_DETECT);
                break;
            case 7:
                algoFolder = "sketch";
                cout << "[ISP] Dang chay Pipeline Sketch...\n";
                applyGrayscale(tempFrame);
                applyConvolution(tempFrame, KERNEL_BLUR);
                applyConvolution(tempFrame, KERNEL_EDGE_GENTLE);
                applyThresholdAndInvert(tempFrame, 30);
                break;
        }

        string fullpath = "../output_images/" + algoFolder + "/" + outName;
        stbi_write_jpg(fullpath.c_str(), tempFrame.width, tempFrame.height, tempFrame.channels, tempFrame.pixelData, 100);
        
        cout << "==> Thanh cong! Da xuat file tai: " << fullpath << "\n";
        pipelineLog.insertBack("Da Edit: " + algoFolder + " -> " + fullpath);

        delete[] tempFrame.pixelData;
    }
}

// =====================================================================
// KHU VỰC 3: HÀM MAIN
// =====================================================================

int main() {
    Queue<ImageFrame> imgQueue;                 
    HashTable<string, string> imgMetadata(100); 
    LinkedList<string> pipelineLog;             

    while (true) {
        showMenu();
        int command;
        cout << "\nLenh: ";
        
        if (!(cin >> command)) {
            cin.clear(); cin.ignore(10000, '\n');
            cerr << "[Loi] Lenh khong hop le! Vui long nhap so.\n";
            continue;
        }

        switch (command) {
            case 0:
                cout << "Dang dong he thong...\n";
                return 0;
            case 1:
                handleLoad(imgQueue, imgMetadata, pipelineLog);
                break;
            case 2:
                handleInfo(imgMetadata);
                break;
            case 3:
                handleEdit(imgQueue, pipelineLog);
                break;
            case 4:
                handleRemove(imgQueue, pipelineLog); // Ánh xạ hàm xóa mới
                break;
            case 5:
                cout << "--- LICH SU PIPELINE XU LY ---\n";
                pipelineLog.print();
                break;
            default:
                cerr << "[Loi] Lenh khong hop le! Vui long nhap tu 0 den 5.\n";
                break;
        }
    }
    return 0;
}
