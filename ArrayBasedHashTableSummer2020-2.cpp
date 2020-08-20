//Copyright 2020, Bradley Peterson, Weber State University, all rights reserved. (6/20)

#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <memory>
#include <cstdio>


using std::cout;
using std::endl;
using std::map;
using std::unique_ptr;
using std::make_unique;
using std::string;
using std::stringstream;


//forward declarations
template <typename T> class HashTable;

//************************************************************
//A quick and simple class that simulates a Product object.
//************************************************************
class Product {
public:
    void setCost(int cost);
    void setName(const string&);
    string getName();
    int getCost();
    string getAllInfo();
private:
    string name;
    int cost = 0;
};
void Product::setCost(int cost) {
    this->cost = cost;
}
void Product::setName(const string& name) {
    this->name = name;
}
string Product::getName() {
    return name;
}
int Product::getCost() {
    return cost;
}
string Product::getAllInfo() {
    stringstream ss;
    ss << "Name: " << name << ", Cost: " << cost;
    return ss.str();
}

//************************************************************
//The HashTable class
//************************************************************
template <typename T>
class HashTable
{
public:
    unsigned int getNumBuckets() { return capacity; }
    unsigned int getTotalCount() const;
    unsigned int getWorstClump() const;

    HashTable();

    HashTable<T>& operator=(HashTable<T>&& obj) noexcept;

    HashTable(const HashTable<T>& obj) {
        cout << "Failed homework issue:  You hit the HashTable copy constructor.  That's bad!" << endl;
    }
    HashTable<T>& operator=(const HashTable& obj) {
        cout << "Failed homework issue:  You hit the HashTable copy assignment.  That's bad!" << endl;
        HashTable temp;
        return temp;
    }

    // We won't implement this move constructor for the homework:
    HashTable(HashTable<T>&& obj) = delete;
   
    void create(const string& key, const T& item);// method for L - values
    void create(const string& key, T&& item);// method for R - values
    T retrieve(const string& key); // method(return by value, acts as a read only retrieve)
    T& operator[](const string& key); // method(return by reference which can allow for modification of values)
    bool exists(const string& key);//  method(returns a boolean)
    void remove(const string& key); //method to remove something from the hashtable.

private:

    HashTable(const unsigned int customCapacity);

    unsigned int hash(const string& key) const;

    unsigned int capacity{ 20 };
    unsigned int count{ 0 };
    unique_ptr<int[]> status_arr;
    unique_ptr <string[]> key_arr;
    unique_ptr<T[]> value_arr;

};// end class HashTable

//************************************************************
// Put HashTable consrtuctors and methods below here
//************************************************************
template <typename T>
HashTable<T>::HashTable() {

    this->status_arr = make_unique <int[]>(this->capacity);
    this->key_arr = make_unique <string[]>(this->capacity);
    this->value_arr = make_unique <T[]>(this->capacity);

    for (unsigned int i = 0; i < this->capacity; i++) {
        this->status_arr[i] = 0;
    }
}

template<typename T>
HashTable<T>& HashTable<T>::operator=(HashTable<T>&& obj) noexcept
{
    this->capacity = obj.capacity;
    this->count = obj.count;
    obj.capacity = 0;
    obj.count = 0;
    this->status_arr = std::move(obj.status_arr);
    this->key_arr = std::move(obj.key_arr);
    this->value_arr = std::move(obj.value_arr);
    return *this;
}

template <typename T>
HashTable<T>::HashTable(const unsigned int customCapacity) {
    this->capacity = customCapacity;

    this->status_arr = make_unique <int[]>(this->capacity);
    this->key_arr = make_unique <string[]>(this->capacity);
    this->value_arr = make_unique <T[]>(this->capacity);

    for (int i = 0; i < this->capacity; i++) {
        this->status_arr[i] = 0;
    }
}

template<typename T>
void HashTable<T>::create(const string& key, const T& item)
{
    //if needed rehash
    unsigned short newCount = this->count + 1;
    unsigned short almostFull = this->capacity * .75;
    if (newCount >= almostFull) {
        HashTable<T>newTable(this->capacity * 2); //create new table
        for (int i = 0; i < this->capacity; i++) { //check each element in the hash to see if it has contents
            if (this->status_arr[i] == 1) {//if there's something there then get the key, rehash it, put it in the new table at the new index and move the value to the new table as well. 
                newTable.create(this->key_arr[i], std::move((this->value_arr[i])));
            }
        }
        *this = std::move(newTable);
    }

    unsigned int index = hash(key);
    while (true) {
        if (this->status_arr[index] == 0 || this->status_arr[index] == -1) {
            this->key_arr[index] = key;
            this->value_arr[index] = item;
            this->status_arr[index] = 1;
            this->count++;
            return;
        }
        else {
            if (this->capacity - 1 == index) {
                index = 0;
            }
            else {
                index++;
            }

        }
    }
}

template<typename T>
void HashTable<T>::create(const string& key, T&& item)
{
    //if needed rehash
    unsigned short newCount = this->count + 1;
    unsigned short almostFull = this->capacity * .75;
    if (newCount >= almostFull) {
        HashTable<T>newTable(this->capacity * 2); //create new table
        for (int i = 0; i < this->capacity; i++) { //check each element in the hash to see if it has contents
            if (this->status_arr[i] == 1) {//if there's something there then get the key, rehash it, put it in the new table at the new index and move the value to the new table as well. 
                newTable.create(this->key_arr[i], std::move((this->value_arr[i])));
            }
        }
        *this = std::move(newTable);
    }

    unsigned int index = hash(key);
    while (true) {
        if (this->status_arr[index] == 0 || this->status_arr[index] == -1) {
            this->key_arr[index] = std::move(key);
            this->value_arr[index] = std::move(item);
            this->status_arr[index] = 1;
            this->count++;
            return;
        }
        else {
            if (this->capacity - 1 == index) {
                index = 0;
            }
            else {
                index++;
            }

        }
    }
}

template<typename T>
T HashTable<T>::retrieve(const string& key)
{
    unsigned int tries = 0;
    unsigned int index = hash(key);
    if (this->count == 0) {
        throw 1;
    }
    while (true) {
        if (this->status_arr[index] == 0) {
            throw 1;
        }
        else if (this->status_arr[index] == -1) {
            if (this->capacity - 1 == index) {
                index = 0;
            }
            else {
                index++;
            }
            
        }
        else {
            if (this->key_arr[index] == key) {
                return this->value_arr[index];
            }
            else {
                if (this->capacity - 1 == index) {
                    index = 0;
                }
                else {
                    index++;    
                }
                tries++;
                if (this->count == tries) {
                    throw 1;
                }
            }
        }
    }
}

template<typename T>
T& HashTable<T>::operator[](const string& key)
{
    unsigned int tries = 0;
    unsigned int index = hash(key);
    if (this->count == 0) {
        throw 1;
    }
    while (true) {
        if (this->status_arr[index] == 0) {
            throw 1;
        }
        else if (this->status_arr[index] == -1) {
            if (this->capacity - 1 == index) {
                index = 0;
            }
            else {
                index++;
            }

        }
        else {
            if (this->key_arr[index] == key) {
                return this->value_arr[index];
            }
            else {
                if (this->capacity - 1 == index) {
                    index = 0;
                }
                else {
                    index++;
                }
                tries++;
                if (this->count == tries) {
                    throw 1;
                }
            }
        }
    }
}

template<typename T>
bool HashTable<T>::exists(const string& key)
{
    
    unsigned int tries = 0;
    unsigned int index = hash(key);
    if (this->count == 0) {
        return false;
    }
    while (true) {
        if (this->status_arr[index] == 0) {
            return false;
        }
        else if (this->status_arr[index] == -1) {
            if (this->capacity - 1 == index) {
                index = 0;
            }
            else {
                index++;
            }

        }
        else {
            if (this->key_arr[index] == key) {
                return true;
            }
            else {
                if (this->capacity - 1 == index) {
                    index = 0;
                }
                else {
                    index++;
                }
                tries++;
                if (this->count == tries) {
                    return false;
                }
            }
        }
    }
}

template<typename T>
void HashTable<T>::remove(const string& key)
{
    unsigned int tries = 0;
    unsigned int index = hash(key);
    if (this->count == 0) {
        return;
    }
    while (true) {
        if (this->status_arr[index] == 0) {
            return;
        }
        else if (this->status_arr[index] == -1) {
            if (this->capacity - 1 == index) {
                index = 0;
            }
            else {
                index++;
            }

        }
        else {
            if (this->key_arr[index] == key) {
                this->status_arr[index] = -1;
                this->count--;
                return;
            }
            else {
                if (this->capacity - 1 == index) {
                    index = 0;
                }
                else {
                    index++;
                }
                tries++;
                if (this->count == tries) {
                    return;
                }
            }
        }
    }
}

//************************************************************
// Put HashTable consrtuctors and methods above here
//************************************************************

template <typename T>
unsigned int HashTable<T>::hash(const string& key) const {

    return std::hash<std::string>{}(key) % capacity;

}

template <typename T>
unsigned int HashTable<T>::getTotalCount() const {
    unsigned int count = 0;
    for (unsigned int i = 0; i < capacity; i++) {
        if (status_arr[i] == 1) {
            count++;
        }
    }
    return count;
}

template <typename T>
unsigned int HashTable<T>::getWorstClump() const {
    unsigned int clumpCount = 0;
    unsigned int maxClump = 0;
    for (unsigned int i = 0; i < capacity; i++) {
        if (status_arr[i] == 1) {
            clumpCount++;
        }
        else {
            if (clumpCount > maxClump) {
                maxClump = clumpCount;
                clumpCount = 0;
            }
        }
    }
    if (clumpCount > maxClump) {
        maxClump = clumpCount;
    }
    return maxClump;
}


//This helps with testing, do not modify.
template <typename T>
string NumberToString(T Number)
{
    stringstream ss;
    ss << Number;
    return ss.str();
}

//This helps with testing, do not modify.
bool checkEmpty(string testName, string whatItIs) {

    if (whatItIs != "") {
        cout << "Passed " << testName << ", the data was " << whatItIs << endl;
        return true;
    }
    else {
        cout << "***Failed test " << testName << " *** " << endl << "   No data was found! " << endl;
        return false;
    }
}

//This helps with testing, do not modify.
bool checkTest(string testName, string whatItShouldBe, string whatItIs) {

    if (whatItShouldBe == whatItIs) {
        cout << "Passed " << testName << endl;
        return true;
    }
    else if (whatItShouldBe == "") {
        cout << "****** Failed test " << testName << " ****** " << endl << "     Output was '" << whatItIs << endl << "'     Output should have been blank" << endl;
        return false;

    }
    else {
        cout << "****** Failed test " << testName << " ****** " << endl << "     Output was " << whatItIs << endl << "     Output should have been " << whatItShouldBe << endl;
        return false;
    }
}

//This helps with testing, do not modify.
bool checkTest(string testName, int whatItShouldBe, int whatItIs) {

    if (whatItShouldBe == whatItIs) {
        cout << "Passed " << testName << endl;
        return true;
    }
    else {
        cout << "****** Failed test " << testName << " ****** " << endl << "     Output was " << whatItIs << endl << "     Output should have been " << whatItShouldBe << endl;
        return false;
    }
}

//This helps with testing, do not modify.
bool checkTestMemory(string testName, int whatItShouldBe, int whatItIs) {

    if (whatItShouldBe == whatItIs) {
        cout << "Passed " << testName << endl;
        return true;
    }
    else {
        cout << "***Failed test " << testName << " *** " << endl << "  There are " << whatItIs << " bytes of memory yet to be reclaimed!" << endl;
        return false;
    }
}

//This helps with testing, do not modify.
void testSimpleIntHash() {
    //cout << "|||### You need to comment in testSimpleIntHash() tests one-by-one###|||" << endl;

    HashTable<string> myHash;

    //Test #1, add "Jazz" into our hash with a key of 6.  Try to retrieve it.
    string myString = "Jazz";

    myHash.create("6", myString);
    checkTest("testSimpleIntHash #1", "Jazz", myHash.retrieve("6"));

    //Test #2, attempt to get the Jazz using the operator[] code.
    checkTest("testSimpleIntHash #2", "Jazz", myHash["6"]);

    //Test #3, attempt to change the value at this location:
    myHash["6"] = "Nuggets";

    checkTest("testSimpleIntHash #3", "Nuggets", myHash["6"]);

    //Test #4,
    //See if we can find it
    if (myHash.exists("6")) {
      checkTest("testSimpleIntHash #4", "", "");
    }
    else {
      checkTest("testSimpleIntHash #4", "This test should have found an item with key \"6\"", "This test did NOT find an item with key \"6\"");
    }

    //test #5, see if we can find something that doesn't exist yet.
    if (myHash.exists("1234")) {
      checkTest("testsimpleinthash #5", "this test should not have found an item with key \"1234\".", "this test found an item with key \"1234\"");
    }
    else {
      checkTest("testsimpleinthash #5", "", "");
    }

    //Test #6, removing it from the hash
    myHash.remove("6");
    if (myHash.exists("6")) {
      checkTest("testSimpleIntHash #6", "This test should NOT have found an item with key \"6\".", "This test found an item with key \"6\"");
    }
    else {
      checkTest("testSimpleIntHash #6", "", "");
    }

    //Add more into the hash
    myHash.create("13", "Lakers");
    myHash.create("25", "Bulls");
    myHash.create("101", "Pelicans");
    myHash.create("77", "Bucks");
    myHash.create("12", "Thunder");
    myHash.create("42", "Nets");
    myHash.create("97", "Timberwolves");
    myHash.create("123", "Hornets");
    myHash.create("500", "Mavericks");

    //Attempt to retrieve some
    checkTest("testSimpleIntHash #7", "Thunder", myHash.retrieve("12"));
    checkTest("testSimpleIntHash #8", "Bucks", myHash.retrieve("77"));
    checkTest("testSimpleIntHash #9", "Hornets", myHash.retrieve("123"));

    //Now count up how many are in there
    checkTest("testSimpleIntHash #10", 9, myHash.getTotalCount());

    //Now just verify that they aren't clumping up badly.
    int worst = 0;
    worst = myHash.getWorstClump();
    if (worst > 7) {
     cout << "***Failed testSimpleIntHash #11!  There exists a clump of " << worst << " consecutive items, it shouldn't be worse than 7." << endl;
    }
    else {
      cout << "Passed testSimpleIntHash #11.  Your worst clump was " << worst << " items." << endl;
    }

}


void testHashOfObjects() {
    //cout << "|||### You need to comment in testHashOfObjects() tests one-by-one###|||" << endl;

    //Create a HashTable. We want this to be a hash table with int keys, string object values,
    //And we also supply the hash function we want to use for integers..

    HashTable<Product> myHash;

    //Test #1, add in a studentObject.  Try to retrive it.
    Product tempProduct;
    tempProduct.setCost(5);
    tempProduct.setName("Silly string");
    myHash.create("12341-51231", tempProduct);
    checkTest("testHashOfObjects #1", "Silly string", myHash.retrieve("12341-51231").getName());

    //Test #2, attempt to get the product using its ID code
    checkTest("testHashOfObjects #2", "Silly string", myHash["12341-51231"].getName());

    //Test #3, see what happens if two products have the same ID code.  This should overwrite the former.
    tempProduct.setCost(18);
    tempProduct.setName("Novelty foam hat");
    myHash["12341-51231"] = tempProduct;
    checkTest("testHashOfObjects #3", "Novelty foam hat", myHash["12341-51231"].getName());

    //Test #4,
    //See if we can find it
    if (myHash.exists("12341-51231")) {
      checkTest("testHashOfObjects #4", "", "");
    }
    else {
      checkTest("testHashOfObjects #4", "This test should have found an item with key 12341-51231", "This test did NOT find an item with key 12341-51231");
    }

    //Test #5, see if we can find something that doesn't exist yet.
    if (myHash.exists("56756-75675")) {
      checkTest("testHashOfObjects #5", "This test should NOT have found an item with key 56756-75675.", "This test found an item with key56756-75675");
    }
    else {
      checkTest("testHashOfObjects #5", "", "");
    }

    //Test #6, removing it from the hash
    myHash.remove("12341-51231");
    if (myHash.exists("12341-51231")) {
      checkTest("testHashOfObjects #6", "This test should NOT have found an item with key 12341-51231.", "This test found an item with key 12341-51231");
    }
    else {
      checkTest("testHashOfObjects #6", "", "");
    }
}


//This helps with testing, do not modify.
void testHashofHashes() {
    //cout << "|||### You need to comment in testHashofHashes() tests one-by-one###|||" << endl;

    HashTable< HashTable<int> > studentAssignments;
    studentAssignments.create("Alice", HashTable<int>());

    HashTable<int> tempHash2;
    studentAssignments.create("Bob", HashTable<int>());

    HashTable<int> tempHash3;
    studentAssignments.create("Karl", HashTable<int>());

    //Give alice some assignment scores
    studentAssignments["Alice"].create("1", 73);
    studentAssignments["Alice"].create("2", 65);
    studentAssignments["Alice"].create("4", 91);
    //Ensure it went in
    checkTest("testHashofHashes #1", 65, studentAssignments["Alice"]["2"]);

    //And Bob
    studentAssignments["Bob"].create("1", 90);
    studentAssignments["Bob"].create("3", 84);
    studentAssignments["Bob"].create("4", 99);

    //And Karl
    studentAssignments["Karl"].create("1", 92);
    studentAssignments["Karl"].create("2", 92);
    studentAssignments["Karl"].create("3", 87);
    studentAssignments["Karl"].create("4", 10);

    //Now find the average of assignment 4 scores
    int average = (studentAssignments["Alice"]["4"] + studentAssignments["Bob"]["4"] + studentAssignments["Karl"]["4"]) / 3;
    checkTest("testHashofHashes #2", 66, average);

}

void testRehashing() {
    //cout << "|||### You need to comment in testRehashing() tests one-by-one###|||" << endl;

    HashTable<string> myHash;

    //Throw in more items.
    int key = 0;
    stringstream out;
    for (unsigned int i = 0; i < 10000; i++) {

      //this next part just helps create some variation in generated W#s...
      if (i % 2 == 0) {
        key += 17;
      }
      else if (i % 3 == 0) {
        key += 23;
      }
      else if (i % 5 == 0) {
        key += 51;
      }
      else if (i % 7 == 0) {
        key += 13;
      }
      else {
        key += 71;
      }
      //convert an int to a string via help from the stringstream class
      out.str("");
      out << key;
      string temp = out.str();

      out.str("");
      out << "a-" << i;
      string value = out.str();
      myHash.create(temp, value); //Just add a bunch of letter a's
    }

    //Make sure they all go in there. 
    checkTest("testRehashing #1", 10000, myHash.getTotalCount());

    //Make sure the capacity is large enough
    checkTest("testRehashing #2", 20480, myHash.getNumBuckets());

    //Verify one of the values in the hash table.
    checkTest("testRehashing #3", "a-2345", myHash.retrieve("76154"));

    int worst = myHash.getWorstClump();
    if (worst > 1000) {
      cout << "Failed testRehashing #4!  There exists a clump of " << worst << " consecutive items, it shouldn't be worse than 1000." << endl;
    }
    else {
      cout << "Passed testRehashing #4.  Your worst clump was " << worst << " items." << endl;
    }

    //Remove the key "184275".
    myHash.remove("184275");
    if (myHash.exists("184275")) {
      checkTest("testRehashing #5", "This test should NOT have found an item with key \"2437968\".", "This test found an item with key \"2437968\"");
    }
    else {
      checkTest("testRehashing #5", "", "");
    }
    //There should be one less value now
    checkTest("testRehashing #6", 9999, myHash.getTotalCount());

}

void pressEnterToContinue() {
    cout << "Press any key to continue...";

    cout << endl;
}

int main() {

    testSimpleIntHash();
    //pressEnterToContinue();

    testHashOfObjects();
    //pressEnterToContinue();

    testHashofHashes();
    // pressEnterToContinue();

    testRehashing();
    pressEnterToContinue();
    return 0;
}


