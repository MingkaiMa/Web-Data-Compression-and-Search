#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue> 
#include <functional>
#include <bitset>

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


		int blockSize = 10999900;
		unsigned char* rreadBuffer = new unsigned char[blockSize];
		while(myfile.read((char*)(&rreadBuffer[0]), blockSize)){

			for(int i = 0; i < blockSize; ++i)
				mmp[rreadBuffer[i]] += 1;

		}

		for(int i = 0; i < myfile.gcount(); ++i)
			mmp[rreadBuffer[i]] += 1;


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

		if(pq.size() == 0)
			root = nullptr;
		else
			root = pq.top();

	}

	void encoderCore(HuffmanTreeNode* root, string path){
		if(root->left == nullptr && root->right == nullptr){
			if(path.size() == 0){
				codeTable[root->data] = "0";
			}else
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

		if(root == nullptr){
			unsigned char* headBuffer = new unsigned char[1024];


			for(int i = 0; i < 1024; ++i){
				headBuffer[i] = ' ';
			}


			toFile.write((char*)(&headBuffer[0]), 1024);

			toFile.close();
			return;
		}


		string path = "";
		encoderCore(root, path);

		rebuildSaveTree(root);


		unsigned char* headBuffer = new unsigned char[1024];
		int headCount = 0;

		int headBytesCnt = rebuildVector.size();




		for(auto& i: rebuildVector){
			headBuffer[headCount] = i;
			++headCount;
		}

		for(; headCount < 1024; ++headCount){
			headBuffer[headCount] = ' ';
		}



		toFile.write((char*)(&headBuffer[0]), 1024);


		string nowCode = "";
		string remain = "";


		int blockSize = 10000000;
		int writeBlockSize = 409600;

		unsigned char* writeBuffer = new unsigned char[writeBlockSize];

		int writeIndex = 0;

		unsigned char ch(0);
		int bitCount = 0;



		unsigned char* readBuffer = new unsigned char[blockSize];
		while(fromFile.read((char*)(&readBuffer[0]), blockSize)){
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


		int remainNb = fromFile.gcount();

		int tmepCount = 0;

		for(int i = 0; i < remainNb; ++i){
			unsigned char currChar = readBuffer[i];
			string currCode = codeTable[currChar];
			tmepCount++;

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

		if(bitCount != 0){

			if(bitCount < 8){
				compliNb = 8 - bitCount;
			}

			while(bitCount < 8){
				ch = ch << 1;
				bitCount++;
			}
			toFile << ch;
		}	



		toFile.seekp(headBytesCnt, ios::beg);
		toFile << char(compliNb);


		delete[] readBuffer;
		delete[] writeBuffer;
		delete[] headBuffer;

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

		streampos begin, end;
		begin = fromFile.tellg();

		fromFile.seekg(0, ios::end);

		end = fromFile.tellg();
		int length = end - begin;

		fromFile.seekg(0, ios::beg);

		if(length == 1024){

			root = nullptr;
			return;
		}

		root = rebuildCore();

		string path = "";

		unsigned char ch;
		ch = fromFile.get();


		char ch1111 = char(0);

		int chNow = ch - ch1111;


		compliNb = chNow;

	}

	void decoder(){

		if(root == nullptr){
			return;
		}

		bool endFlag = false;

		fromFile.seekg(0, ios::end);
		streampos end = fromFile.tellg();


		fromFile.seekg(1024, ios::beg);

		rebuildHuffmanTreeNode* currNode = root;


		if(root->left == nullptr && root->right == nullptr){

			streampos curr = fromFile.tellg();

			int length = end - curr;

			length = length * 8 - compliNb;

			unsigned char* writeBuffer = new unsigned char[length];

			for(int i = 0; i < length; ++i)
				writeBuffer[i] = root->data;

			toFile.write((char*)(&writeBuffer[0]), length);

			delete[] writeBuffer;
			toFile.close();
			return;
		}



		int blockSize = 10000000;

		unsigned char* readBuffer = new unsigned char[blockSize];



		int writeBlockSize = 409600;

		unsigned char* writeBuffer = new unsigned char[writeBlockSize];

		int writeIndex = 0;


		while(fromFile.read((char*)(&readBuffer[0]), blockSize)){

			auto currPos = fromFile.tellg();

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
        
        int PatternSize = Pattern.size();

		while(i < PatternSize){
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

	searchInfoNode* kmpSearch(unsigned char* S, int SSize, string P, vector<int>& T){

		int s = 0;
		int p = 0;
		int cnt = 0;

        int S_Size = SSize;
        int P_Size = P.size();

		while(s < S_Size){
			if(P[p] == S[s]){
				++s;
				++p;
				if(p == P_Size){
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

		streampos begin, end;
		begin = fromFile.tellg();

		fromFile.seekg(0, ios::end);

		end = fromFile.tellg();
		int length = end - begin;

		fromFile.seekg(0, ios::beg);

		if(length == 1024){

			root = nullptr;
			return;
		}



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


		if(root == nullptr)
			return 0;


		int res = 0;

		string Pattern = target;

		for(auto& c: target){
			if(codeTable.count(c) == 0)
				return 0;

		}

		vector<int> T(Pattern.size() + 1, -1);

		kmpTable(Pattern, T);

		bool endFlag = false;

		fromFile.seekg(0, ios::end);
		streampos end = fromFile.tellg();


		fromFile.seekg(1024, ios::beg);

		rebuildHuffmanTreeNode* currNode = root;



		if(root->left == nullptr && root->right == nullptr){

			for(auto& c: target){
				if(c != root->data)
					return 0;
			}


			streampos curr = fromFile.tellg();

			int length = end - curr;

			length = length * 8 - compliNb;

			fromFile.close();

			int targetSize = target.size();
			if(targetSize > length)
				return 0;


			if(targetSize == length)
				return 1;

			return (length - target.size() + 1);

		}



		string nowAllS = "";




		int blockSize = 10000000;
		int searchSize = 409600;

		unsigned char* readBuffer = new unsigned char[blockSize];

		unsigned char* searchBuffer = new unsigned char[searchSize];
		int searchIndex = 0;


		while(fromFile.read((char*)(&readBuffer[0]), blockSize)){


			auto currPos = fromFile.tellg();

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

							//nowAllS.push_back(currNode->data);
							searchBuffer[searchIndex] = currNode->data;
							++searchIndex;

							if(searchIndex == searchSize){
								searchInfoNode* node = kmpSearch(searchBuffer, searchIndex, Pattern, T);

								res += node->total;

								int newStart = searchIndex - min(searchIndex, node->p);
								int newSearchIndex = 0;

								
								for(int i = newStart; i < searchIndex; ++i){
									searchBuffer[newSearchIndex] = searchBuffer[i];
									++newSearchIndex;
								}

								searchIndex = newSearchIndex;
							}

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

						searchBuffer[searchIndex] = currNode->data;
						++searchIndex;

						if(searchIndex == searchSize){
							searchInfoNode* node = kmpSearch(searchBuffer, searchIndex, Pattern, T);

							res += node->total;

							int newStart = searchIndex - min(searchIndex, node->p);
							int newSearchIndex = 0;

							
							for(int i = newStart; i < searchIndex; ++i){
								searchBuffer[newSearchIndex] = searchBuffer[i];
								++newSearchIndex;
							}

							searchIndex = newSearchIndex;
						}

						currNode = root;
					}

					++bitCount;

				}



				searchInfoNode* node = kmpSearch(searchBuffer, searchIndex, Pattern, T);

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

							searchBuffer[searchIndex] = currNode->data;
							++searchIndex;

							if(searchIndex == searchSize){
								searchInfoNode* node = kmpSearch(searchBuffer, searchIndex, Pattern, T);

								res += node->total;

								int newStart = searchIndex - min(searchIndex, node->p);
								int newSearchIndex = 0;

								
								for(int i = newStart; i < searchIndex; ++i){
									searchBuffer[newSearchIndex] = searchBuffer[i];
									++newSearchIndex;
								}

								searchIndex = newSearchIndex;
							}


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

						searchBuffer[searchIndex] = currNode->data;
						++searchIndex;

						if(searchIndex == searchSize){
							searchInfoNode* node = kmpSearch(searchBuffer, searchIndex, Pattern, T);

							res += node->total;

							int newStart = searchIndex - min(searchIndex, node->p);
							int newSearchIndex = 0;

							
							for(int i = newStart; i < searchIndex; ++i){
								searchBuffer[newSearchIndex] = searchBuffer[i];
								++newSearchIndex;
							}

							searchIndex = newSearchIndex;
						}


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

					searchBuffer[searchIndex] = currNode->data;
					++searchIndex;

					if(searchIndex == searchSize){
						searchInfoNode* node = kmpSearch(searchBuffer, searchIndex, Pattern, T);

						res += node->total;

						int newStart = searchIndex - min(searchIndex, node->p);
						int newSearchIndex = 0;

						
						for(int i = newStart; i < searchIndex; ++i){
							searchBuffer[newSearchIndex] = searchBuffer[i];
							++newSearchIndex;
						}

						searchIndex = newSearchIndex;
					}

					currNode = root;
				}

				++bitCount;

			}



			searchInfoNode* node = kmpSearch(searchBuffer, searchIndex, Pattern, T);

			res += node->total;

		}

		delete[] readBuffer;
		delete[] searchBuffer;
		fromFile.close();
		return res;
	}
};



int main(int argc, char *argv[]){


	string option = string(argv[1]);
	char *sourFile = argv[2];
	char *desFile = argv[3];



	if(option == "-e"){

		HuffmanEncoder en(sourFile, desFile);
		en.buildTree();
		en.encoder();
	}


	else if(option == "-d"){

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
