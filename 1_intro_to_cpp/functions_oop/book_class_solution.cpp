#include <iostream>
#include <string>

class Book {
private:
    std::string title;
    std::string author;
    int pageCount;

public:
    // Default Constructor
    Book() {
        title = "Unknown";
        author = "Unknown";
        pageCount = 0;
    }

    // Constructor with parameters
    Book(std::string t, std::string a, int p) {
        title = t;
        author = a;
        pageCount = p;
    }

    ~Book() {
        std::cout << "Destroying book: " << title << std::endl;
    }

    std::string getTitle() const { 
        return title; 
    }
    std::string getAuthor() const { 
        return author; 
    }
    int getPageCount() const { 
        return pageCount; 
    }
};

int main() {
    Book defaultBook;
    Book b1("The Pragmatic Programmer", "Andy Hunt", 352);
    Book b2("Clean Code", "Robert C. Martin", 464);

    std::cout << "Default Book:\n";
    std::cout << "Title: " << defaultBook.getTitle() << "\n";
    std::cout << "Author: " << defaultBook.getAuthor() << "\n";
    std::cout << "Pages: " << defaultBook.getPageCount() << "\n\n";

    std::cout << "Book 1:\n";
    std::cout << "Title: " << b1.getTitle() << "\n";
    std::cout << "Author: " << b1.getAuthor() << "\n";
    std::cout << "Pages: " << b1.getPageCount() << "\n";

    std::cout << "Book 2:\n";
    std::cout << "Title: " << b2.getTitle() << "\n";
    std::cout << "Author: " << b2.getAuthor() << "\n";
    std::cout << "Pages: " << b2.getPageCount() << "\n";

    return 0;
}