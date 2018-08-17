#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue> 
#include <functional>

#include <chrono>


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

	//test time
	chrono::steady_clock::time_point startTime;
	//
	HuffmanEncoder(char *sourFile, char* desFile){
		sourceFile = string(sourFile);
		destFile = string(desFile);

		ifstream myfile(sourceFile, ios::binary);

		//myfile.seekg(0, ios::beg);

		//test time
		startTime = chrono::steady_clock::now(); 

		int blockSize = 19999900;
		unsigned char* rreadBuffer = new unsigned char[blockSize];
		while(myfile.read((char*)(&rreadBuffer[0]), blockSize)){

			for(int i = 0; i < blockSize; ++i)
				mmp[rreadBuffer[i]] += 1;
				//mmp[rreadBuffer[i] - char(0)] += 1;
		}

		for(int i = 0; i < myfile.gcount(); ++i)
			mmp[rreadBuffer[i]] += 1;
			//mmp[rreadBuffer[i] - char(0)] += 1;

		myfile.close();
		delete[] rreadBuffer;

		chrono::steady_clock::time_point end1 = chrono::steady_clock::now();
		chrono::duration<double> time_span = chrono::duration_cast<std::chrono::duration<double>>(end1 - startTime);
		cout << "It took to count word frequency: " << time_span.count() << " second\n";
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

		chrono::steady_clock::time_point end1 = chrono::steady_clock::now();
		chrono::duration<double> time_span = chrono::duration_cast<std::chrono::duration<double>>(end1 - startTime);
		cout << "It took to build tree: " << time_span.count() << " second\n";
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
		unsigned char ch(bits.to_ulong());
		return ch;
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


		unsigned char* headBuffer = new unsigned char[1024];
		int headCount = 0;

		int headBytesCnt = rebuildVector.size();




		for(auto& i: rebuildVector){
			headBuffer[headCount] = i;
			++headCount;
			//toFile << i;
		}

		for(; headCount < 1024; ++headCount){
			headBuffer[headCount] = ' ';
		}

		// for(int cc = 0; cc < (1024 - headBytesCnt); ++cc)
		// 	toFile << ' ';

		toFile.write((char*)(&headBuffer[0]), 1024);


		chrono::steady_clock::time_point end1 = chrono::steady_clock::now();
		chrono::duration<double> time_span = chrono::duration_cast<std::chrono::duration<double>>(end1 - startTime);
		cout << "It took to write header: " << time_span.count() << " second\n";


		string nowCode = "";
		string remain = "";


		int blockSize = 10000000;
		int stringSize = 4096000;
		int writeBlockSize = stringSize;
		//int stringSize = 5000000;

		unsigned char* writeBuffer = new unsigned char[writeBlockSize];

		int writeIndex = 0;

		unsigned char ch(0);
		int bitCount = 0;


		int ccccount = 0;

		unsigned char* readBuffer = new unsigned char[blockSize];
		while(fromFile.read((char*)(&readBuffer[0]), blockSize)){
			ccccount++;
			cout << ccccount << endl;
			for(int i = 0; i < blockSize; ++i){
				unsigned char currChar = readBuffer[i];
				string currCode = codeTable[currChar];

				for(auto& c: currCode){
					ch <<= 1;

					if(c == '1')
						ch = ch | 1;

					bitCount += 1;

					if(bitCount == 8){
						writeBuffer[writeIndex] = ch;
						writeIndex++;
						if(writeIndex == writeBlockSize){
							toFile.write((char*)(&writeBuffer[0]), writeBlockSize);
							writeIndex = 0;
						}

						bitCount = 0;
						ch = 0;
					}

				}
			}
		}

		cout << ccccount << endl;

		chrono::steady_clock::time_point end2 = chrono::steady_clock::now();
		chrono::duration<double> time_span2 = chrono::duration_cast<std::chrono::duration<double>>(end2 - startTime);
		cout << "It took to finish part 1: " << time_span2.count() << " second\n";


		int remainNb = fromFile.gcount();

		for(int i = 0; i < remainNb; ++i){
			unsigned char currChar = readBuffer[i];
			string currCode = codeTable[currChar];

			for(auto& c: currCode){
				ch <<= 1;

				if(c == '1')
					ch = ch | 1;

				bitCount += 1;

				if(bitCount == 8){
					writeBuffer[writeIndex] = ch;
					writeIndex++;
					if(writeIndex == writeBlockSize){
						toFile.write((char*)(&writeBuffer[0]), writeBlockSize);
						writeIndex = 0;
					}

					bitCount = 0;
					ch = 0;
				}
			}

		}



		toFile.write((char*)(&writeBuffer[0]), writeIndex);


		int compliNb = 0;

		// if(nowCode.size() < 8)
		// 	compliNb = 8 - nowCode.size();


		// while(nowCode.size() < 8)
		// 	nowCode += "0";

		if(bitCount < 8){
			compliNb = 8 - bitCount;
		}

		while(bitCount < 8){
			ch = ch << 1;
			bitCount++;
		}





		//toFile << stringTochar(nowCode);
		toFile << ch;
		toFile.seekp(headBytesCnt, ios::beg);
		toFile << char(compliNb);


		delete[] readBuffer;
		delete[] writeBuffer;
		delete[] headBuffer;
		// delete[] newWriteBuffer;
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



		int writeBlockSize = 4096000;
		//int stringSize = 5000000;

		unsigned char* writeBuffer = new unsigned char[writeBlockSize];

		int writeIndex = 0;


		while(fromFile.read((char*)(&readBuffer[0]), blockSize)){

			auto currPos = fromFile.tellg();

			// last,  need to deal with last byte
			if(currPos == end){
				endFlag = true;
				for(int i = 0; i < blockSize - 1; ++i){
					unsigned char currChar = readBuffer[i];

					int bitCount = 0;

					while(bitCount < 8){

						if(((currChar >> (7 - bitCount)) & 1) == 1){
							currNode = currNode->right;
						}
						else{
							currNode = currNode->left;
						}

						if(currNode->left == nullptr && currNode->right == nullptr){

							writeBuffer[writeIndex] = currNode->data;
							++writeIndex;

							if(writeIndex == writeBlockSize){
								toFile.write((char*)(&writeBuffer[0]), writeBlockSize);
								writeIndex = 0;
							}

							//toFile << currNode->data;
							currNode = root;
						}

						++bitCount;
					}

				}

				unsigned char lastByte = readBuffer[blockSize - 1];

				int bitCount = 0;

				while(bitCount < (8 - compliNb)){

					if(((lastByte >> (7 - bitCount)) & 1) == 1){
						currNode = currNode->right;
					}
					else{
						currNode = currNode->left;
					}


					if(currNode->left == nullptr && currNode->right == nullptr){

						writeBuffer[writeIndex] = currNode->data;
						++writeIndex;

						if(writeIndex == writeBlockSize){
							toFile.write((char*)(&writeBuffer[0]), writeBlockSize);
							writeIndex = 0;
						}

						//toFile << currNode->data;
						currNode = root;
					}

					++bitCount;

				}

				toFile.write((char*)(&writeBuffer[0]), writeIndex);


			}

			else{


				for(int i = 0; i < blockSize; ++i){
					unsigned char currChar = readBuffer[i];



					int bitCount = 0;

					while(bitCount < 8){

						if(((currChar >> (7 - bitCount)) & 1) == 1){
							currNode = currNode->right;
						}
						else{
							currNode = currNode->left;
						}

						if(currNode->left == nullptr && currNode->right == nullptr){

							writeBuffer[writeIndex] = currNode->data;
							++writeIndex;

							if(writeIndex == writeBlockSize){
								toFile.write((char*)(&writeBuffer[0]), writeBlockSize);
								writeIndex = 0;
							}

							//toFile << currNode->data;
							currNode = root;
						}

						++bitCount;
					}

				}
			}
		}


		if(endFlag == false){
			int remainNb = fromFile.gcount();

			for(int i = 0; i < (remainNb - 1); ++i){

				unsigned char currChar = readBuffer[i];

				int bitCount = 0;

				while(bitCount < 8){

					if(((currChar >> (7 - bitCount)) & 1) == 1){
						currNode = currNode->right;
					}
					else{
						currNode = currNode->left;
					}

					if(currNode->left == nullptr && currNode->right == nullptr){

						writeBuffer[writeIndex] = currNode->data;
						++writeIndex;

						if(writeIndex == writeBlockSize){
							toFile.write((char*)(&writeBuffer[0]), writeBlockSize);
							writeIndex = 0;
						}

						//toFile << currNode->data;
						currNode = root;
					}

					++bitCount;
				}



			}

			unsigned char lastByte2 = readBuffer[remainNb - 1];


			int bitCount = 0;
			while(bitCount < (8 - compliNb)){

				if(((lastByte2 >> (7 - bitCount)) & 1) == 1){
					currNode = currNode->right;
				}
				else{
					currNode = currNode->left;
				}


				if(currNode->left == nullptr && currNode->right == nullptr){

					writeBuffer[writeIndex] = currNode->data;
					++writeIndex;

					if(writeIndex == writeBlockSize){
						toFile.write((char*)(&writeBuffer[0]), writeBlockSize);
						writeIndex = 0;
					}

					//toFile << currNode->data;
					currNode = root;
				}

				++bitCount;

			}

			toFile.write((char*)(&writeBuffer[0]), writeIndex);


		}

		delete[] readBuffer;
		delete[] writeBuffer;
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

					int bitCount = 0;

					while(bitCount < 8){

						if(((currChar >> (7 - bitCount)) & 1) == 1){
							currNode = currNode->right;
						}
						else{
							currNode = currNode->left;
						}

						if(currNode->left == nullptr && currNode->right == nullptr){

							nowAllS.push_back(currNode->data);
							currNode = root;
						}

						++bitCount;
					}


				}

				unsigned char lastByte = readBuffer[blockSize - 1];

				int bitCount = 0;

				while(bitCount < (8 - compliNb)){

					if(((lastByte >> (7 - bitCount)) & 1) == 1){
						currNode = currNode->right;
					}
					else{
						currNode = currNode->left;
					}


					if(currNode->left == nullptr && currNode->right == nullptr){


						nowAllS.push_back(currNode->data);
						currNode = root;
					}

					++bitCount;

				}



				searchInfoNode* node = kmpSearch(nowAllS, Pattern, T);

				res += node->total;
			}

			else{

				for(int i = 0; i < blockSize; ++i){
					unsigned char currChar = readBuffer[i];

					int bitCount = 0;

					while(bitCount < 8){

						if(((currChar >> (7 - bitCount)) & 1) == 1){
							currNode = currNode->right;
						}
						else{
							currNode = currNode->left;
						}

						if(currNode->left == nullptr && currNode->right == nullptr){

							nowAllS.push_back(currNode->data);
							currNode = root;
						}

						++bitCount;
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

				int bitCount = 0;

				while(bitCount < 8){

					if(((currChar >> (7 - bitCount)) & 1) == 1){
						currNode = currNode->right;
					}
					else{
						currNode = currNode->left;
					}

					if(currNode->left == nullptr && currNode->right == nullptr){

						nowAllS.push_back(currNode->data);
						currNode = root;
					}

					++bitCount;
				}


			}

			unsigned char lastByte2 = readBuffer[remainNb - 1];

			int bitCount = 0;
			while(bitCount < (8 - compliNb)){

				if(((lastByte2 >> (7 - bitCount)) & 1) == 1){
					currNode = currNode->right;
				}
				else{
					currNode = currNode->left;
				}


				if(currNode->left == nullptr && currNode->right == nullptr){

					nowAllS.push_back(currNode->data);
					currNode = root;
				}

				++bitCount;

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