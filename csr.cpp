// CMSC 341 - Fall 2023 - Project 1
// Author: Avi Donaty
#include "csr.h"
///////////////CSR Class Implementation///////////////
CSR::CSR(){
    m_values = nullptr;
    m_col_index = nullptr;
    m_row_index = nullptr;
    m_nonzeros = 0;
    m_m = 0;
    m_n = 0;
    m_next = nullptr;
}
CSR::~CSR(){
    delete [] m_values;
    delete [] m_col_index;
    delete [] m_row_index;
}
CSR::CSR(const CSR & rhs){
    if (rhs.empty()) {
        m_values = nullptr;
        m_col_index = nullptr;
        m_row_index = nullptr;
        m_nonzeros = 0;
        m_m = 0;
        m_n = 0;
        m_next = nullptr;
    }
    else {
        m_nonzeros = rhs.m_nonzeros;
        m_m = rhs.m_m;
        m_n = rhs.m_n;
        m_next = rhs.m_next;
        m_values = new int[rhs.m_nonzeros];
        m_col_index = new int[rhs.m_nonzeros];
        m_row_index = new int[rhs.m_m + 1];

        // fills arrays based on arrays of rhs
        for (int i = 0; i < rhs.m_nonzeros; i++) {
            m_values[i] = rhs.m_values[i];
            m_col_index[i] = rhs.m_col_index[i];
        }
        for (int i = 0; i < rhs.m_m + 1; i++) {
            m_row_index[i] = rhs.m_row_index[i];
        }
    }
}
void CSR::clear(){
    if (m_values) {
        delete [] m_values;
    }
    if (m_col_index) {
        delete [] m_col_index;
    }
    if (m_row_index) {
     delete [] m_row_index;
    }

    // initializes new empty object
    m_values = nullptr;
    m_col_index = nullptr;
    m_row_index = nullptr;
    m_nonzeros = 0;
    m_m = 0;
    m_n = 0;
    m_next = nullptr;
}
bool CSR::empty() const{
    if (m_m == 0 && m_n == 0) {
        return true;
    }
    else {
        return false;
    }
}
void CSR::compress(int m, int n, int array[], int arraySize){
    // makes sure values are empty before
    clear();
    
    // error case makes empty object
    if (m < 1 || n < 1 || arraySize < 1) {
        return;
    }

    m_m = m;
    m_n = n;
    int amountNonZeros = 0;
    int rowCounter = 0;

    // initializes m_nonzeros
    for (int i = 0; i < arraySize; i++) {
        if ((array[i]) && (array[i] != 0)) {
            m_nonzeros++;
        }
    }
    m_values = new int[m_nonzeros];
    m_col_index = new int[m_nonzeros];
    m_row_index = new int[m_m + 1];

    for (int i = 0; i < arraySize; i++) {
        // gets current column
        int column = i % n;   

        // before beginning of each column we add amount of non-zeroes
        // to m_row_index
        if (column == 0) {
            m_row_index[rowCounter] = amountNonZeros;
            rowCounter++;
        }

        if (array[i] != 0) {
            m_col_index[amountNonZeros] = column;
            m_values[amountNonZeros] = array[i];
            amountNonZeros++;
        }
    }

    m_row_index[rowCounter] = amountNonZeros;    
}
int CSR::getAt(int row, int  col) const{
    // error case
    if (row < 0 || row > m_m || col < 0 || col > m_n) {
        throw runtime_error("Error: Indices out of Range");
    }
    
    // between these two you have just the row in question
    int row_start = m_row_index[row];
    int row_end = m_row_index[row + 1];

    // iterates through only the row that is asked for and 
    // returns the value that is in the correct column
    for (int i = row_start; i < row_end; i++) {
        if (col == m_col_index[i]) {
            return m_values[i];
        }
    }

    return 0;
} 
bool CSR::operator==(const CSR & rhs) const{
    bool result = true;

    // checks each individual member variable against rhs's version
    result = result && (m_nonzeros == rhs.m_nonzeros);
    result = result && (m_m == rhs.m_m);
    result = result && (m_n == rhs.m_n);

    // checks to make sure arrays are the same
    if (result) {
        for (int i = 0; i < m_nonzeros; i++) {
            result = result && (m_values[i] == rhs.m_values[i]);
            result = result && (m_col_index[i] == rhs.m_col_index[i]);
        }
        for (int i = 0; i < m_n - 1; i++) {
            result = result && (m_row_index[i] == rhs.m_row_index[i]);
        }
    }

    return result;
}
int CSR::sparseRatio(){
    if (empty()) {
        return 0;
    }

    // takes amount of zeros and divides it by amount of elements
    // to get a percentage. then multiples by 100 and returns
    float elements = m_m * m_n;
    float zeros = elements - m_nonzeros;
    float ratio = zeros / elements * 100;

    if (elements == zeros) {
        return 100;
    }

    return ratio;
}
void CSR::dump(){
    cout << endl;
    if (!empty()){
        for (int i=0;i<m_nonzeros;i++)
            cout << m_values[i] << " ";
        cout << endl;
        for (int i=0;i<m_nonzeros;i++)
            cout << m_col_index[i] << " ";
        cout << endl;
        for (int i=0;i<m_m+1;i++)
            cout << m_row_index[i] << " ";
    }
    else
        cout << "The object is empty!";
    cout << endl;
}

//////////////CSRList Class Implementation///////////////
CSRList::CSRList(){
    m_head = nullptr;
    m_size = 0;
}
CSRList::CSRList(const CSRList & rhs){

    // checks if rhs is empty
    if (rhs.empty()) {
        this->m_head = nullptr;
        this->m_size = 0;
    }

    CSR* temp = rhs.m_head;

    for (int i = 0; i < rhs.m_size; i++) {
        temp = rhs.m_head;

        // every time outer loop runs we traverse one less index
        // than last time in order to insert that node at head
        for (int j = 0; j < rhs.m_size - (i + 1); j++) {
            temp = temp->m_next;
        }

        CSR* insert = new CSR;

        // equalizes temp with node to be inserted
        insert->m_nonzeros = temp->m_nonzeros;
        insert->m_m = temp->m_m;
        insert->m_n = temp->m_n;

        insert->m_values = new int[temp->m_nonzeros];
        insert->m_col_index = new int[temp->m_nonzeros];
        for (int x = 0; x < temp->m_nonzeros; x++) {
            insert->m_values[x] = temp->m_values[x];
            insert->m_col_index[x] = temp->m_col_index[x];
        }

        insert->m_row_index = new int[temp->m_m + 1];
        for (int x = 0; x < temp->m_m + 1; x++) {
            insert->m_row_index[x] = temp->m_row_index[x];
        }

        // inserts new node at beginning of new linked list
        if (m_head == nullptr) {
            m_head = insert;
        }
        else {
            insert->m_next = m_head;
            m_head = insert;
        }
    }
    m_size = rhs.m_size;
}
CSRList::~CSRList(){
    CSR *curr = m_head;

    while (!empty()) {
        curr = curr->m_next;
        delete m_head;
        m_head = curr;
    }

    m_size = 0;
    m_head = nullptr;
}
bool CSRList::empty() const{
    if (m_head == nullptr) {
        return true;
    }
    else {
        return false;
    }
}
void CSRList::insertAtHead(const CSR & matrix){
    // creates new node with same characteristics as passed matrix
    CSR* curr = new CSR(matrix);

    // adds if list is empty
    if (empty()) {
        m_head = curr;
        m_size++;
        return;
    }

    // adds in the beginning if list is not empty
    curr->m_next = m_head;
    m_head = curr;
    m_size++;
    
}
void CSRList::clear(){
    // does nothing if already empty
    if (empty()) {
        return;
    }

    CSR *curr = m_head;

    // iterates through each element in list and deletes it
    while (!empty()) {
        curr = curr->m_next;
        m_head->clear();
        delete m_head;
        m_head = curr;
    }

    // resets to empty list
    m_size = 0;
    m_head = nullptr;
}

int CSRList::getAt(int CSRIndex, int row, int col) const{
    // error case
    if (CSRIndex < 0 || CSRIndex > m_size - 1) {
        throw runtime_error("Exception Error: Object is not in the list!");
    }

    CSR* curr = m_head;
    int counter = 0;

    // loops through CSRList and calls getAt function on 
    // selected index's CSR and return result
    while (curr != nullptr) {
        if (counter == CSRIndex) {
            return curr->getAt(row, col);
        }
        curr = curr->m_next;
        counter++;
    }
    return 0;
}
bool CSRList::operator== (const CSRList & rhs) const{
    // checks if both are empty
    if (empty() && rhs.empty()) {
        return true;
    }

    bool result = true;
    CSR* l_node = m_head;
    CSR* r_node = rhs.m_head;

    // returns false if member variables not the same
    if (m_size != rhs.m_size) {
        return false;
    }

    // iterates through both lists and compares each node 
    while (r_node != nullptr) {
        result = result && (*l_node == *r_node);
        l_node = l_node->m_next;
        r_node = r_node->m_next;
    }

    return result;
}
const CSRList& CSRList::operator=(const CSRList & rhs){
    // checks for self-assignment
    if (*this == rhs) {
        return *this;
    }
    CSR* temp = rhs.m_head;
    if (!empty()) {
        clear();
    }

    for (int i = 0; i < rhs.m_size; i++) {
        temp = rhs.m_head;

        // every time outer loop runs we traverse one less index
        // than last time in order to insert that node at head
        for (int j = 0; j < rhs.m_size - (i + 1); j++) {
            temp = temp->m_next;
        }

        // CSR* insert = new CSR(temp);
        CSR* insert = new CSR;

        // equalizes temp with node to be inserted
        insert->m_nonzeros = temp->m_nonzeros;
        insert->m_m = temp->m_m;
        insert->m_n = temp->m_n;

        insert->m_values = new int[temp->m_nonzeros];
        insert->m_col_index = new int[temp->m_nonzeros];
        for (int x = 0; x < temp->m_nonzeros; x++) {
            insert->m_values[x] = temp->m_values[x];
            insert->m_col_index[x] = temp->m_col_index[x];
        }

        insert->m_row_index = new int[temp->m_m + 1];
        for (int x = 0; x < temp->m_m + 1; x++) {
            insert->m_row_index[x] = temp->m_row_index[x];
        }

        // inserts new node at beginning of new linked list
        if (m_head == nullptr) {
            m_head = insert;
        }
        else {
            insert->m_next = m_head;
            m_head = insert;
        }
    }
    m_size = rhs.m_size;

    return *this;
}
int CSRList::averageSparseRatio(){
    // error case
    if (empty()) {
        return 0;
    }

    CSR* curr = m_head;
    int total = 0;

    // traverses through and adds each index's sparse ratio
    // to the total
    while (curr != nullptr) {
        total += curr->sparseRatio();
        curr = curr->m_next;
    }
    // gets the average of all the ratios
    total = total / m_size;

    return total;    
}
void CSRList::dump(){
    if (!empty()){
        CSR* temp = m_head;
        while (temp != nullptr){
            temp->dump();
            temp = temp->m_next;
        }
    }
    else
        cout << "Error: List is empty!" << endl;
}