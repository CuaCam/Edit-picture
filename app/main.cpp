#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

// Cấu hình bắt buộc để sử dụng thư viện stb
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// Nhúng các thư viện Template tự chế của nhóm bạn
#include "../lib/Queue.hpp"
#include "../lib/PriorityQueue.hpp"
#include "../lib/HashTable.hpp"
#include "../lib/LinkedList.hpp"
#include "../lib/Algorithms.hpp"

using namespace std;

// Struct quản lý dữ liệu của từng tấm ảnh
struct ImageFrame
{
    string fileName;
    unsigned char *pixelData; // Mảng 1 chiều chứa toàn bộ pixel (R, G, B liên tiếp)
    int width;
    int height;
    int channels;     // Thường là 3 (RGB) hoặc 4 (RGBA)
    int qualityScore; // Điểm dùng cho Priority Queue
};

ImageFrame loadImage(const string &filePath)
{
    ImageFrame frame;
    frame.fileName = filePath;
    // Hàm của stb để bóc tách ảnh thành mảng dữ liệu thô
    frame.pixelData = stbi_load(filePath.c_str(), &frame.width, &frame.height, &frame.channels, 0);

    if (!frame.pixelData)
    {
        cout << "Loi: Khong the doc file anh tai " << filePath << endl;
        frame.width = 0;
    }

    // Giả lập tính điểm chất lượng (ví dụ dựa trên dung lượng hoặc mặc định)
    frame.qualityScore = 100;
    return frame;
}

void processMedianStack(vector<ImageFrame>& frames, const string& outputName) {
    if (frames.empty()) return;

    int w = frames[0].width;
    int h = frames[0].height;
    int c = frames[0].channels;

    // 1. KHIÊN BẢO VỆ: Quét kiểm tra kích thước tất cả các ảnh trước khi chạy
    for (size_t i = 1; i < frames.size(); ++i) {
        if (frames[i].width != w || frames[i].height != h || frames[i].channels != c) {
            cout << "\n[LOI] Cac anh dau vao khong cung kich thuoc!" << endl;
            cout << "- Anh 1: " << w << "x" << h << endl;
            cout << "- Anh " << i+1 << ": " << frames[i].width << "x" << frames[i].height << endl;
            cout << "=> Vui long dung cac buc anh co cung do phan giai (chup lien tiep)." << endl;
            
            // Giải phóng RAM các ảnh đã nạp lỡ trước đó để chống rò rỉ
            for (size_t j = 0; j < frames.size(); ++j) {
                stbi_image_free(frames[j].pixelData);
            }
            return; // Hủy lệnh stack, quay về menu chính thay vì để crash app
        }
    }

    int totalElements = w * h * c; 
    unsigned char* resultPixels = new unsigned char[totalElements];
    int numFrames = frames.size();

    cout << "[ISP Loi] Dang quet va sap xep " << totalElements << " diem mau..." << endl;

    // 2. TỐI ƯU HIỆU NĂNG: Đưa việc cấp phát mảng tạm ra NGOÀI vòng lặp
    unsigned char* tempArray = new unsigned char[numFrames];

    for (int i = 0; i < totalElements; ++i) {
        for (int j = 0; j < numFrames; ++j) {
            tempArray[j] = frames[j].pixelData[i];
        }

        quickSort(tempArray, 0, numFrames - 1);
        resultPixels[i] = tempArray[numFrames / 2];
    }
    
    delete[] tempArray; // Chỉ delete đúng 1 lần sau khi xong 14.7 triệu vòng lặp

    // Ghi mảng kết quả sạch nhiễu thành file ảnh thực tế
    stbi_write_jpg(outputName.c_str(), w, h, c, resultPixels, 100);
    cout << "==> Thanh cong! Da xuat anh net tai: " << outputName << endl;

    // Giải phóng bộ nhớ RAM
    delete[] resultPixels;
    for (int j = 0; j < numFrames; ++j) {
        stbi_image_free(frames[j].pixelData);
    }
}

int main()
{
    // Khởi tạo các cấu trúc dữ liệu từ thư viện của nhóm
    Queue<ImageFrame> imgQueue;                 // Bộ đệm nạp luồng ảnh
    HashTable<string, string> imgMetadata(100); // Tra cứu thông số ảnh nhanh, đã cấp phát 100 bucket
    LinkedList<string> pipelineLog;             // Lưu lịch sử các bước xử lý

    cout << "====================================================\n";
    cout << "   MULTI-FRAME ISP - HE THONG XU LY ANH TRUONG GIA \n";
    cout << "====================================================\n";
    cout << " Cac lenh ho tro:\n";
    cout << "1. Load.\n";
    cout << "2. Info.\n";
    cout << "3. Run.\n";
    cout << "4. History.\n";
    cout << "5. Exit.\n";


    int command;
    vector<ImageFrame> activeFrames;

    while (true)
    {
        cout << "\nLenh:";
        cin >> command;

        if (command == 1)
        {
            int n;
            cout << "Nhap so luong anh muon nap: ";
            cin >> n;
            for (int i = 0; i < n; ++i)
            {
                string path;
                cout << "Nhap duong dan anh thu " << i + 1 << " (vd: raw1.jpg): ";
                cin >> path;

                ImageFrame f = loadImage(path);
                if (f.width > 0)
                {
                    imgQueue.enqueue(f);
                    // Lưu thông số vào bảng băm để tra cứu nhanh
                    string meta = "Resolution: " + to_string(f.width) + "x" + to_string(f.height) + ", Channels: " + to_string(f.channels);
                    imgMetadata.insert(path, meta);
                }
            }
            pipelineLog.insertBack("Da nap " + to_string(n) + " anh vao Queue.");
        }
        else if (command == 2)
        {
            string path;
            cout << "Nhap ten file anh can tra cuu thong so: ";
            cin >> path;
            // Tra cứu thần tốc bằng HashTable
            if (imgMetadata.contains(path))
            {
                cout << "[Metadata] " << path << " -> " << imgMetadata.find(path) << endl;
            }
            else
            {
                cout << "Khong tim thay du lieu anh nay trong cache!" << endl;
            }
        }
        else if (command == 3)
        {
            // Rut anh tu Queue ra vector de xu ly
            while (!imgQueue.empty())
            {
                activeFrames.push_back(imgQueue.getFront()); // Bước 1: Lấy ảnh ở đầu hàng đợi ra
                imgQueue.dequeue();                          // Bước 2: Xóa ảnh đó khỏi hàng đợi
            }

            if (activeFrames.empty())
            {
                cout << "Hang doi trong! Vui long 'load' anh truoc." << endl;
                continue;
            }

            string outName;
            cout << "Nhap ten anh muon xuat ra (vd: cleared.jpg): ";
            cin >> outName;
            string fullpath = "../output_images/" + outName;
            cout << "\n[ISP Engine] Dang thuc hien chong anh..." << endl;
            cout << "=> Anh thanh pham se duoc luu tai: app/output_images/" << outName << endl;

            processMedianStack(activeFrames, outName);
            pipelineLog.insertBack("Da thuc hien chao anh va xuat file " + outName);
            activeFrames.clear(); // Xóa danh sách quản lý sau khi đã giải phóng pixels
        }
        else if (command == 4)
        {
            cout << "--- LICH SU PIPELINE XU LY ---" << endl;
            // Duyệt danh sách liên kết từ đầu đến cuối để in log
            pipelineLog.print();
        }
        else if (command == 5)
        {
            cout << "Dang dong he thong..." << endl;
            break;
        }
        else
        {
            cout << "Lenh khong hop le!" << endl;
        }
    }
    return 0;
}