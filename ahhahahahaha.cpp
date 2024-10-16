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
void CreateMap(unordered_map<string,char>& Haff, HufTree* root, string str) {
    ;
    if (!root)
        return;

    if (root->data != '$') {
        Haff[str] = root->data;
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
void HafmanCE(map<char, double>& chastMap, unordered_map<char, string>& Haff) {
    HufTree* l, * r, * top;

    priority_queue<HufTree*, vector<HufTree*>, myComparator> minHeap;
    for (auto it : chastMap) {
        minHeap.push(new HufTree(it.first, it.second));
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
    //for (auto it : Haff) {
    //    cout << it.first << " " << it.second << "\n";
    //}


}
void HafmanCE(map<char, double>& chastMap, unordered_map<string,char>& Haff) {
    HufTree* l, * r, * top;

    priority_queue<HufTree*, vector<HufTree*>, myComparator> minHeap;
    for (auto it : chastMap) {
        minHeap.push(new HufTree(it.first, it.second));
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
    //for (auto it : Haff) {
    //    cout << it.first << " " << it.second << "\n";
    //}


}

void Chast(const string& filename, map<char, double>& chastMap) {
    ifstream in(filename);
    char ch;
    
    while (in.get(ch)) {
        symbols++;
        if (chastMap.find(ch) == chastMap.end()) {
            chastMap[ch] = 1;
        }
        else {
            chastMap[ch]++;
        }
    }
    for (auto &it : chastMap) {
        it.second = it.second * 100 / symbols;
        it.second = round(it.second * 100.0) / 100.0;
        //cout <<it.first << "  " << it.second << endl;
    }

    in.close();
}
void encode(unordered_map<char, string>& Haff,map<char, double>& chastMap, const string& inputfile, const string& encodefile) {
    ofstream encode(encodefile, ios_base::binary);

    unsigned char buffer = 0;
    unsigned int sizer = chastMap.size();
    encode.write((char*)&sizer, 1);
    for (auto it : chastMap) {
        encode.write((char*)&it.first, 1);

        int exponent = static_cast<int>(it.second);
        int mantissa = static_cast<int>((it.second - exponent) * 100);

        encode.write((char*)&exponent, 1);
        encode.write((char*)&mantissa, 1);
    }
    

    //cout << "size = " << Haff.size() << endl;
    ifstream in(inputfile);
    int size = 0;
    encode.write((char*)&symbols, sizeof(int));
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
void decode(const string& encodefile, const string& outputfile) {
    ifstream encode(encodefile, ios_base::binary);
    ofstream output(outputfile);
    map<char, double> chastMap;
    unsigned int sizer = 0;
    int out_1 = 0;
    int out_2 = 0;
    char temp;
    encode.read((char*)&sizer, 1);
    while (sizer > 0) {
        encode.read((char*)&temp, 1);
        encode.read((char*)&out_1, 1);
        encode.read((char*)&out_2, 1);
        double Mantissa = static_cast<double>(out_2) / 100;
        chastMap[temp] = out_1 + Mantissa;
        sizer--;
    }
    for (auto it : chastMap) {
    cout << it.first << " " << it.second << "\n";
}
    unordered_map<string, char> re_Haff;
    HafmanCE(chastMap, re_Haff);
    for (auto it : re_Haff) {
        cout << it.first << " " << it.second << endl;
    }
    int size;
    encode.read((char*)&size, sizeof(int));

    unsigned char buffer = 0;
    string code;
    int  i = 0;
    while (encode.read((char*)&buffer, 1)) {
        for (int i = 7; i >= 0; i--) {
            code += ((buffer >> i) & 1) ? '1' : '0';

            if (re_Haff.count(code) && size != 0) {
                size--;
                output << re_Haff[code];
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
    unordered_map<char, string> Haff;
    map<char, double> chastMap;
    Chast("input.txt", chastMap);
    HafmanCE(chastMap, Haff);
    encode(Haff,chastMap, "input.txt", "encode.bin");
    decode("encode.bin", "output.txt");

    return 0;
}