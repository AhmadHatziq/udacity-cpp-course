#include <iostream>
#include <string>

class Book {
private:
    std::string title;
    std::string author;
    int pageCount;

public:
    // Setters
    void setTitle(const std::string& t) { 
        title = t; 
    }
    void setAuthor(const std::string& a) { 
        author = a; 
    }
    void setPageCount(int pages) { 
        pageCount = pages; 
    }

    // Getters
    std::string getTitle() { 
        return title; 
    }
    std::string getAuthor() { 
        return author; 
    }
    int getPageCount() { 
        return pageCount; 
    }
};

int main() {
    Book b1, b2;

    b1.setTitle("The Pragmatic Programmer");
    b1.setAuthor("Andy Hunt");
    b1.setPageCount(352);

    b2.setTitle("Clean Code");
    b2.setAuthor("Robert C. Martin");
    b2.setPageCount(464);

    std::cout << "Book 1:\n";
    std::cout << "Title: " << b1.getTitle() << "\n";
    std::cout << "Author: " << b1.getAuthor() << "\n";
    std::cout << "Pages: " << b1.getPageCount() << "\n\n";

    std::cout << "Book 2:\n";
    std::cout << "Title: " << b2.getTitle() << "\n";
    std::cout << "Author: " << b2.getAuthor() << "\n";
    std::cout << "Pages: " << b2.getPageCount() << "\n";

    return 0;
}
