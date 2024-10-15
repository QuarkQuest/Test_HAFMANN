#include <iostream> 
#include <fstream> 
#include <queue>
#include <map>
#include <string>
#include <unordered_map>
using namespace std;

int symbols = 0;
class HufTree {
public:
    char data;
    int chast;
    HufTree* left, * right;
    HufTree(char data, int chast) {
        this->data = data;
        this->chast = chast;
        left = right = NULL;
    };
    ~HufTree() {};
};

class myComparator
{
public:
    int operator() (const HufTree* r, const HufTree* l) {
        return r->chast > l->chast;
    }
};
void CreateMap(unordered_map<char, string>& Haff, HufTree* root, string str) {
    ;
    if (!root)
        return;

    if (root->data != '$') {
        Haff[root->data] = str;
    }

    CreateMap(Haff, root->left, str + "0");
    CreateMap(Haff, root->right, str + "1");
}
void printCodes(HufTree* root, string str)
{

    if (!root)
        return;

    if (root->data != '$')
        cout << root->data << ": " << str << "\n";

    printCodes(root->left, str + "0");
    printCodes(root->right, str + "1");
}
void HafmanCE(char arr[], int chast[], int size, unordered_map<char, string>& Haff) {
    HufTree* l, * r, * top;

    priority_queue<HufTree*, vector<HufTree*>, myComparator> minHeap;
    for (int i = 0; i < size; ++i) {
        minHeap.push(new HufTree(arr[i], chast[i]));
    }
    while (minHeap.size() > 1) {
        l = minHeap.top();
        minHeap.pop();

        r = minHeap.top();
        minHeap.pop();

        top = new HufTree('$', l->chast + r->chast);

        top->left = l;
        top->right = r;

        minHeap.push(top);
    }
    //printCodes(minHeap.top(), "");
    CreateMap(Haff, minHeap.top(), "");
    for (auto it : Haff) {
        cout << it.first << " " << it.second << "\n";
    }


}

void Chast(const string& filename, char* arr, int* chast, int& size) {
    ifstream in(filename);
    char ch;
    map<char, int> chastMap;
    while (in.get(ch)) {
        symbols++;
        if (chastMap.find(ch) == chastMap.end()) {
            chastMap[ch] = 1;
        }
        else {
            chastMap[ch]++;
        }
    }
    int totalChars = 0;
    for (auto it : chastMap) {
        totalChars += it.second;
    }
    size = 0;
    for (auto it : chastMap) {
        arr[size] = it.first;
        chast[size] = it.second * 100 / totalChars;
        size++;
    }


    in.close();
}
void encode(unordered_map<char, string>& Haff, const string& inputfile, const string& encodefile) {
    ofstream encode(encodefile, ios_base::binary);

    unsigned char buffer = 0;
    int size = 0;
    encode.write((char*)&symbols, sizeof(int));
    ifstream in(inputfile);
    char ch;
    while (in.get(ch)) {
        string code = Haff[ch];
        for (char bit : code) {
            buffer |= (bit == '1') << (7 - size);
            size++;

            if (size == 8) {
                encode.write((char*)&buffer, 1);
                buffer = 0;
                size = 0;
            }
        }
    }

    if (size > 0) {
        encode.write((char*)&buffer, 1);
    }

    encode.close();
    in.close();
}
void decode(const string& encodefile, const string& outputfile, unordered_map<char, string>& Haff) {
    ifstream encode(encodefile, ios_base::binary);
    ofstream output(outputfile);
    int size;
    encode.read((char*)&size, sizeof(int));
    unordered_map<string, char> reverse_Haff;
    for (auto& it : Haff) {
        reverse_Haff[it.second] = it.first;
    }
    for (auto it : reverse_Haff) {
        cout << it.first << " " << it.second << "\n";
    }

    unsigned char buffer = 0;
    string code;
    int  i = 0;
    while (encode.read((char*)&buffer, 1)) {
        for (int i = 7; i >= 0; i--) {
            code += ((buffer >> i) & 1) ? '1' : '0';

            if (reverse_Haff.count(code) && size != 0) {
                size--;
                output << reverse_Haff[code];
                code.clear();
            }
        }

    }



    encode.close();
    output.close();
}

int main() {
    system("chcp 1251");
    system("cls");
    char arr[256];
    int   chast[256];
    unordered_map<char, string> Haff;
    int size = 0;

    Chast("input.txt", arr, chast, size);
    HafmanCE(arr, chast, size, Haff);
    encode(Haff, "input.txt", "encode.bin");
    decode("encode.bin", "output.txt", Haff);

    return 0;
}