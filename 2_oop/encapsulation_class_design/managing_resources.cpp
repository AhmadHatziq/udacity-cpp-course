#include <iostream>
#include <stdexcept>
#include <cstring>
#include <cstdio>   // for FILE*, fopen, fclose, fputs

using namespace std;

struct FileStats {
    size_t bytesWritten;
    int writeCount;
};

class FileWriter {
private:
    FILE* file;         // C-style file handle
    FileStats* info;    // Track stats dynamically
    char* filename;     // Store filename dynamically

public:
    // Constructor acquires resources
    FileWriter(const char* name) : file(nullptr), info(nullptr), filename(nullptr) {
        if (!name) throw invalid_argument("Invalid filename: null pointer");

        size_t length = strlen(name);
        if (length == 0) throw invalid_argument("Invalid filename: empty string");

        filename = new char[length + 1];
        strcpy(filename, name);

        file = fopen(filename, "w");
        if (!file) {
            delete[] filename;
            throw runtime_error("Failed to open file");
        }

        info = new FileStats{0, 0};
        cout << "FileWriter constructed for " << filename << "\n";
    }

    // Destructor releases resources
    ~FileWriter() {
        if (file) {
            fclose(file);
            file = nullptr;
        }
        delete info;
        delete[] filename;
        cout << "FileWriter destroyed\n";
    }

    // Write a message to the file
    void write(const char* message) {
        if (!file) throw runtime_error("File not open");
        fputs(message, file);
        fputs("\n", file);
        info->bytesWritten += strlen(message);
        info->writeCount++;
    }

    // Print file statistics
    void outputStats() const {
        cout << "Bytes written: " << info->bytesWritten
             << ", Write count: " << info->writeCount << "\n";
    }
};

int main() {
    try {
        FileWriter writer("demo.txt");
        writer.write("Hello, world!");
        writer.write("RAII makes resource management easy.");
        writer.outputStats();
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
    }
    return 0;
}
