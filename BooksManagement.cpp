#include <iostream>
#include <vector>
#include <fstream>
#include <exception>
#include <algorithm>
#include <memory>
#include "nlohmann/json.hpp"
using namespace std;

struct BookData {
    string author;
    string country;
    string imageLink;
    string language;
    string link;
    int pages;
    string title;
    int year;
};

class Person {
protected:
    string name;

public:
    Person() : name("") {}
    Person(const string& name) : name(name) {}

    string getName() const {
        return name;
    }

    void setName(const string& name) {
        this->name = name;
    }
};

class Author : public Person {
public:
    Author() : Person() {}
    Author(const string& name) : Person(name) {}

    Author(const nlohmann::json& jsonData) : Person(jsonData["author"]) {}
};

class Book {
private:
    string title;
    int publicationYear;
    Author author;

public:
    Book() : title(""), publicationYear(0), author() {}
    Book(const string& title, int publicationYear, const Author& author)
        : title(title), publicationYear(publicationYear), author(author) {}

    bool operator==(const Book& other) const {
        return title == other.title;
    }

    bool operator<(const Book& other) const {
        return publicationYear < other.publicationYear;
    }

    string getTitle() const {
        return title;
    }

    int getPublicationYear() const {
        return publicationYear;
    }

    Author getAuthor() const {
        return author;
    }

    Book(const nlohmann::json& jsonData)
        : title(jsonData["title"]), publicationYear(jsonData["year"]), author(jsonData) {}
};

template <typename T>
class Library {
private:
    std::vector<T> items;

public:
    void addItem(const T& item) {
        items.push_back(item);
    }

    int searchItem(const string& title) {
        for (size_t i = 0; i < items.size(); ++i) {
            if (items[i].getTitle() == title) {
                return i;
            }
        }
        return -1;
    }

    void sortItems() {
        sort(items.begin(), items.end());
    }

    const T& getItem(size_t index) const {
        return items[index];
    }

    size_t getSize() const {
        return items.size();
    }
};

vector<BookData> parseJsonData(const nlohmann::json& jsonData) {
    vector<BookData> booksData;

    for (const auto& bookData : jsonData) {
        BookData data;
        data.author = bookData["author"];
        data.country = bookData["country"];
        data.imageLink = bookData["imageLink"];
        data.language = bookData["language"];
        data.link = bookData["link"];
        data.pages = bookData["pages"];
        data.title = bookData["title"];
        data.year = bookData["year"];

        booksData.push_back(data);
    }

    return booksData;
}

json loadJsonFile(const string& filename) {
    ifstream inFile(filename);

    if (!inFile) {
        throw runtime_error("Error opening file: " + filename);
    }

    json jsonData;
    inFile >> jsonData;
    inFile.close();
    return jsonData;
}

const string DATA_FILE_PATH = "TestData/";
int main() {
    Library<Book> library;

    try {
        json jsonData = loadJsonFile(DATA_FILE_PATH + "books.json");

        for (const auto& bookData : jsonData) {
            library.addItem(Book(bookData));
        }
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }

    // Console I/O example: Search for a book
    string titleToSearch;
    cout << "Enter the title of the book you want to search for: ";
    getline(cin, titleToSearch);

    int index = library.searchItem(titleToSearch);
    if (index != -1) {
        cout << "Found " << titleToSearch << " at index " << index << endl;
    }
    else {
        cout << "Book not found." << endl;
    }
}