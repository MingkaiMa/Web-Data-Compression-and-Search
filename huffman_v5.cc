#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue> 
#include <functional>



using namespace std;

struct HuffmanTreeNode {
	unsigned char data;
	int freq;
	HuffmanTreeNode* left;
	HuffmanTreeNode* right;
	HuffmanTreeNode(unsigned char d, int f): data(d), freq(f), left(nullptr), right(nullptr) {}
	HuffmanTreeNode(): data('0'), freq(0), left(nullptr), right(nullptr) {}
};


class CompareNode2{
public:
	bool operator()(HuffmanTreeNode* p1, HuffmanTreeNode* p2){
		return p1->freq > p2->freq;
	}
};

class HuffmanEncoder{
public:
	string sourceFile;
	string destFile;

	int mmp[256] = {0};


	unordered_map<unsigned char, string> codeTable;
	HuffmanTreeNode* root;
	vector<unsigned char> rebuildVector;

	HuffmanEncoder(char *sourFile, char* desFile){
		sourceFile = string(sourFile);
		destFile = string(desFile);

		ifstream myfile(sourceFile, ios::binary);

		myfile.seekg(0, ios::beg);

		int blockSize = 10000000;
		unsigned char* rreadBuffer = new unsigned char[blockSize];
		while(myfile.read((char*)(&rreadBuffer[0]), blockSize)){

			for(int i = 0; i < blockSize; ++i)
				mmp[rreadBuffer[i] - char(0)] += 1;
		}

		for(int i = 0; i < myfile.gcount(); ++i)
			mmp[rreadBuffer[i] - char(0)] += 1;

		myfile.close();
		delete[] rreadBuffer;
	}

	void buildTree(){

		priority_queue<HuffmanTreeNode*, vector<HuffmanTreeNode*>, CompareNode2> pq;

		for(int i = 0; i < 256; ++i){
			if(mmp[i] == 0)
				continue;

			unsigned char ch(i);
			HuffmanTreeNode* newNode = new HuffmanTreeNode(ch, mmp[i]);
			pq.push(newNode);
		}


		while(pq.size() > 1){

			HuffmanTreeNode* nodeZ = new HuffmanTreeNode();
			HuffmanTreeNode* leftNode = pq.top();

			pq.pop();
			HuffmanTreeNode* rightNode = pq.top();

			pq.pop();

			nodeZ->left = leftNode;
			nodeZ->right = rightNode;
			nodeZ->freq = leftNode->freq + rightNode->freq;

			pq.push(nodeZ);
		}

		root = pq.top();
	}

	void encoderCore(HuffmanTreeNode* root, string path){
		if(root->left == nullptr && root->right == nullptr){
			codeTable[root->data] = path;
			return;
		}

		if(root->left)
			encoderCore(root->left, path + "0");
		if(root->right)
			encoderCore(root->right, path + "1");
	}

	char stringTochar(string& code){
		bitset<8> bits(code);
		return char(bits.to_ulong());
	}


	void rebuildSaveTree(HuffmanTreeNode* root){
		if(root == nullptr)
			return;
		if(root->left == nullptr && root->right == nullptr){
			rebuildVector.push_back(char(1));
			rebuildVector.push_back(root->data);
		}
		else{
			rebuildVector.push_back(char(0));
			rebuildSaveTree(root->left);
			rebuildSaveTree(root->right);
		}
	}

	void encoder(){

		ifstream fromFile(sourceFile, ios::binary);
		ofstream toFile(destFile, ios::out | ios::binary);

		string path = "";
		encoderCore(root, path);

		rebuildSaveTree(root);

		int headBytesCnt = rebuildVector.size();


		for(auto& i: rebuildVector){
			toFile << i;
		}
		for(int cc = 0; cc < (1024 - headBytesCnt); ++cc)
			toFile << ' ';

		string nowCode = "";
		string remain = "";


		int blockSize = 10000000;
		int stringSize = 409600;

		unsigned char* readBuffer = new unsigned char[blockSize];
		while(fromFile.read((char*)(&readBuffer[0]), blockSize)){
			// cout << "while\n";
			for(int i = 0; i < blockSize; ++i){
				unsigned char currChar = readBuffer[i];
				string currCode = codeTable[currChar];

				nowCode += currCode;
				if(nowCode.size() > stringSize){
					int index = 0;
					while(index + 8 <= stringSize){
						string code = nowCode.substr(index, 8);
						index += 8;
						bitset<8> bits(code);
						unsigned char ch(bits.to_ulong());
						toFile << ch;
					}
					nowCode = nowCode.substr(index, nowCode.size() - index);
				}
			}
		}



		int remainNb = fromFile.gcount();

		for(int i = 0; i < remainNb; ++i){
			unsigned char currChar = readBuffer[i];
			string currCode = codeTable[currChar];

			nowCode += currCode;
			if(nowCode.size() > stringSize){
				int index = 0;
				while(index + 8 <= stringSize){
					string code = nowCode.substr(index, 8);
					index += 8;
					bitset<8> bits(code);
					unsigned char ch(bits.to_ulong());
					toFile << ch;
				}
				nowCode = nowCode.substr(index, nowCode.size() - index);
			}
		}

		int index = 0;

		while(index + 8 <= nowCode.size()){
			string code = nowCode.substr(index, 8);
			index += 8;
			bitset<8> bits(code);
			unsigned char ch(bits.to_ulong());
			toFile << ch;
		}

		nowCode = nowCode.substr(index, nowCode.size() - index);



		int compliNb = 0;

		if(nowCode.size() < 8)
			compliNb = 8 - nowCode.size();

		while(nowCode.size() < 8)
			nowCode += "0";

		toFile << stringTochar(nowCode);
		toFile.seekp(headBytesCnt, ios::beg);
		toFile << char(compliNb);


		delete[] readBuffer;
		fromFile.close();
		toFile.close();
	}

};


struct rebuildHuffmanTreeNode {
	unsigned char data;
	rebuildHuffmanTreeNode* left;
	rebuildHuffmanTreeNode* right;
	rebuildHuffmanTreeNode(unsigned char d): data(d), left(nullptr), right(nullptr) {}
	rebuildHuffmanTreeNode(unsigned char d, rebuildHuffmanTreeNode* l, rebuildHuffmanTreeNode* r): data(d), left(l), right(r) {}
	rebuildHuffmanTreeNode(): data('0'), left(nullptr), right(nullptr) {}
};


class HuffmanDecoder{
public:
	string sourceFile;
	string destFile;
	rebuildHuffmanTreeNode* root;
	ifstream fromFile;
	ofstream toFile;
	int compliNb;

	HuffmanDecoder(char *sourFile, char* desFile){
		sourceFile = string(sourFile);
		destFile = string(desFile);
		fromFile.open(sourceFile, ios::binary);
		toFile.open(destFile, ios::out | ios::binary);
	}


	rebuildHuffmanTreeNode* rebuildCore(){
		unsigned char ch;
		ch = fromFile.get();

		bitset<8> b(ch);

		if(ch == char(1)){
			ch = fromFile.get();
			return (new rebuildHuffmanTreeNode(ch));
		}
		else{
			rebuildHuffmanTreeNode* left = rebuildCore();
			rebuildHuffmanTreeNode* right = rebuildCore();
			return (new rebuildHuffmanTreeNode('0', left, right));
		}
	}


	void encoderCore(rebuildHuffmanTreeNode* root, string path){
		if(root->left == nullptr && root->right == nullptr){
			return;
		}

		if(root->left)
			encoderCore(root->left, path + "0");
		if(root->right)
			encoderCore(root->right, path + "1");
	}



	void rebuildHuffmanTree(){

		root = rebuildCore();

		string path = "";

		unsigned char ch;
		ch = fromFile.get();

		char ch1111 = char(0);

		int chNow = ch - ch1111;

		compliNb = chNow;
	}

	void decoder(){


		bool endFlag = false;

		// get end
		fromFile.seekg(0, ios::end);
		streampos end = fromFile.tellg();


		fromFile.seekg(1024, ios::beg);

		rebuildHuffmanTreeNode* currNode = root;


		int blockSize = 10000000;

		unsigned char* readBuffer = new unsigned char[blockSize];

		while(fromFile.read((char*)(&readBuffer[0]), blockSize)){

			auto currPos = fromFile.tellg();

			// last,  need to deal with last byte
			if(currPos == end){
				endFlag = true;
				for(int i = 0; i < blockSize - 1; ++i){
					unsigned char currChar = readBuffer[i];
					bitset<8> nowBits(currChar);
					string nowBitString = nowBits.to_string();
					for(auto& c: nowBitString){
						if(c == '0')
							currNode = currNode->left;

						else if(c == '1')
							currNode = currNode->right;

						if(currNode->left == nullptr && currNode->right == nullptr){
							toFile << currNode->data;
							currNode = root;
						}
					}

				}

				unsigned char lastByte = readBuffer[blockSize - 1];
				bitset<8> nowBits(lastByte);
				string llastByte = nowBits.to_string();
				for(int i = 0; i < (8 - compliNb); ++i){
					char ch = llastByte[i];

					if(ch == '0')
						currNode = currNode->left;

					else if(ch == '1')
						currNode = currNode->right;

					if(currNode->left == nullptr && currNode->right == nullptr){
						toFile << currNode->data;
						currNode = root;
					}
				}
			}

			else{


				for(int i = 0; i < blockSize; ++i){
					unsigned char currChar = readBuffer[i];
					bitset<8> nowBits(currChar);
					string nowBitString = nowBits.to_string();
					for(auto& c: nowBitString){
						if(c == '0')
							currNode = currNode->left;

						else if(c == '1')
							currNode = currNode->right;

						if(currNode->left == nullptr && currNode->right == nullptr){
							toFile << currNode->data;
							currNode = root;
						}
					}
				}
			}
		}


		if(endFlag == false){
			int remainNb = fromFile.gcount();

			for(int i = 0; i < (remainNb - 1); ++i){

				unsigned char currChar = readBuffer[i];
				bitset<8> nowBits(currChar);
				string nowBitString = nowBits.to_string();
				for(auto& c: nowBitString){
					if(c == '0')
						currNode = currNode->left;

					else if(c == '1')
						currNode = currNode->right;

					if(currNode->left == nullptr && currNode->right == nullptr){
						toFile << currNode->data;
						currNode = root;
					}
				}
			}

			unsigned char lastByte2 = readBuffer[remainNb - 1];
			bitset<8> nowBits(lastByte2);
			string lastByte = nowBits.to_string();
			for(int i = 0; i < (8 - compliNb); ++i){
				char ch = lastByte[i];

				if(ch == '0')
					currNode = currNode->left;

				else if(ch == '1')
					currNode = currNode->right;

				if(currNode->left == nullptr && currNode->right == nullptr){
					toFile << currNode->data;
					currNode = root;
				}
			}

		}

		delete[] readBuffer;
		toFile.close();
		fromFile.close();
	}
};



struct searchInfoNode{
	int total;
	int p;
	searchInfoNode(int t, int pp): total(t), p(pp) {}
};


class Search{
public:
	string target;
	string sourceFile;
	ifstream fromFile;
	unordered_map<unsigned char, string> codeTable;

	rebuildHuffmanTreeNode* root;
	int compliNb;

	Search(string toFind, char *sourFile){
		target = toFind;
		sourceFile = string(sourFile);
		fromFile.open(sourceFile, ios::binary);
	}


	void kmpTable(string Pattern, vector<int>& T){
		int i = 1;
		int j = 0;

		while(i < Pattern.size()){
			if(Pattern[i] == Pattern[j]){
				T[i] = T[j];
				++i;
				++j;
			}
			else{
				T[i] = j;
				j = T[j];

				while(j >= 0 && Pattern[i] != Pattern[j])
					j = T[j];

				++i;
				++j;
			}
		}
		T[i] = j;
	}

	searchInfoNode* kmpSearch(string S, string P, vector<int>& T){

		int s = 0;
		int p = 0;
		int cnt = 0;

		while(s < S.size()){
			if(P[p] == S[s]){
				++s;
				++p;
				if(p == P.size()){
					++cnt;
					p = T[p];
				}
			}
			else{
				p = T[p];
				if(p < 0){
					++p;
					++s;
				}
			}
		}

		searchInfoNode* node = new searchInfoNode(cnt, p);
		return node;
	}



	rebuildHuffmanTreeNode* rebuildCore(){
		unsigned char ch;
		ch = fromFile.get();

		bitset<8> b(ch);

		if(ch == char(1)){
			ch = fromFile.get();
			return (new rebuildHuffmanTreeNode(ch));
		}
		else{
			rebuildHuffmanTreeNode* left = rebuildCore();
			rebuildHuffmanTreeNode* right = rebuildCore();
			return (new rebuildHuffmanTreeNode('0', left, right));
		}
	}


	void encoderCore(rebuildHuffmanTreeNode* root, string path){
		if(root->left == nullptr && root->right == nullptr){
			codeTable[root->data] = path;

			return;
		}

		if(root->left)
			encoderCore(root->left, path + "0");
		if(root->right)
			encoderCore(root->right, path + "1");
	}


	void rebuildHuffmanTree(){

		root = rebuildCore();

		string path = "";
		encoderCore(root, path);

		unsigned char ch;
		ch = fromFile.get();

		char ch1111 = char(0);

		int chNow = ch - ch1111;

		compliNb = chNow;
	}

	int searchProcess(){


		int res = 0;

		string Pattern = target;

		for(auto& c: target){
			if(codeTable.count(c) == 0)
				return 0;

		}

		vector<int> T(Pattern.size() + 1, -1);

		kmpTable(Pattern, T);

		bool endFlag = false;

		// get end
		fromFile.seekg(0, ios::end);
		streampos end = fromFile.tellg();


		fromFile.seekg(1024, ios::beg);

		rebuildHuffmanTreeNode* currNode = root;


		string nowAllS = "";


		int blockSize = 10000000;

		unsigned char* readBuffer = new unsigned char[blockSize];

		while(fromFile.read((char*)(&readBuffer[0]), blockSize)){

			//cout << "now here?\n";
			auto currPos = fromFile.tellg();

			// last,  need to deal with last byte
			if(currPos == end){
				endFlag = true;
				for(int i = 0; i < blockSize - 1; ++i){
					unsigned char currChar = readBuffer[i];
					bitset<8> nowBits(currChar);
					string nowBitString = nowBits.to_string();
					for(auto& c: nowBitString){
						if(c == '0')
							currNode = currNode->left;

						else if(c == '1')
							currNode = currNode->right;

						if(currNode->left == nullptr && currNode->right == nullptr){

							nowAllS.push_back(currNode->data);
							currNode = root;
						}
					}

				}

				unsigned char lastByte = readBuffer[blockSize - 1];
				bitset<8> nowBits(lastByte);
				string llastByte = nowBits.to_string();
				for(int i = 0; i < (8 - compliNb); ++i){
					char ch = llastByte[i];

					if(ch == '0')
						currNode = currNode->left;

					else if(ch == '1')
						currNode = currNode->right;

					if(currNode->left == nullptr && currNode->right == nullptr){

						nowAllS.push_back(currNode->data);
						currNode = root;
					}
				}


				searchInfoNode* node = kmpSearch(nowAllS, Pattern, T);

				res += node->total;
			}

			else{

				for(int i = 0; i < blockSize; ++i){
					unsigned char currChar = readBuffer[i];
					bitset<8> nowBits(currChar);
					string nowBitString = nowBits.to_string();
					for(auto& c: nowBitString){
						if(c == '0')
							currNode = currNode->left;

						else if(c == '1')
							currNode = currNode->right;

						if(currNode->left == nullptr && currNode->right == nullptr){
							nowAllS.push_back(currNode->data);
							currNode = root;
						}
					}

					searchInfoNode* node = kmpSearch(nowAllS, Pattern, T);

					res += node->total;

					nowAllS = nowAllS.substr(nowAllS.size() - min((int)nowAllS.size(), node->p));

				}
			}
		}


		if(endFlag == false){

			int remainNb = fromFile.gcount();

			for(int i = 0; i < (remainNb - 1); ++i){

				unsigned char currChar = readBuffer[i];
				bitset<8> nowBits(currChar);

				string nowBitString = nowBits.to_string();
				for(auto& c: nowBitString){
					if(c == '0')
						currNode = currNode->left;

					else if(c == '1')
						currNode = currNode->right;

					if(currNode->left == nullptr && currNode->right == nullptr){
						nowAllS.push_back(currNode->data);
						currNode = root;
					}
				}

			}

			unsigned char lastByte2 = readBuffer[remainNb - 1];
			bitset<8> nowBits(lastByte2);
			string lastByte = nowBits.to_string();
			for(int i = 0; i < (8 - compliNb); ++i){
				char ch = lastByte[i];

				if(ch == '0')
					currNode = currNode->left;

				else if(ch == '1')
					currNode = currNode->right;

				if(currNode->left == nullptr && currNode->right == nullptr){
					//toFile << currNode->data;
					nowAllS.push_back(currNode->data);
					currNode = root;
				}
			}


			searchInfoNode* node = kmpSearch(nowAllS, Pattern, T);

			res += node->total;

		}

		delete[] readBuffer;

		fromFile.close();
		return res;
	}
};



int main(int argc, char *argv[]){
	// HuffmanTreeNode* node = new HuffmanTreeNode('a', 12);
	// cout << node->data << "  "  << node->freq << endl;

	string option = string(argv[1]);
	char *sourFile = argv[2];
	char *desFile = argv[3];


	// cout << sourFile << endl;
	// cout << desFile << endl;

	if(option == "-e"){
		//cout << "encode\n";
		HuffmanEncoder en(sourFile, desFile);
		//cout << "No??\n";
		en.buildTree();
		//cout << "start to encode----------\n";
		en.encoder();



	}


	// HuffmanEncoder en(sourFile, desFile);
	// //cout << "mp size is: " << en.mp.size() << endl;
	// //cout << en.mp['\x89'] << endl;
	// en.buildTree();
	// cout << "start to encode----------\n";
	// en.encoder();

	// // // // //cout << en.mp['j'] << endl;

	// cout << "===========\n";
	// for(auto it = en.codeTable.begin(); it != en.codeTable.end(); ++it){
	// 	cout << it->first << " code is:  " << it->second << endl;
	// }

	else if(option == "-d"){
		//cout << "nothere\n";
		HuffmanDecoder de(sourFile, desFile);
		de.rebuildHuffmanTree();
		de.decoder();
	}

	else if(option == "-s"){

		string tar = string(sourFile);
		Search se(tar, desFile);
		se.rebuildHuffmanTree();
		cout << se.searchProcess() << endl;

	}



}