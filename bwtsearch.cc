#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <map>
#include <set>
#include <string>

#define LIMIT_1 256
#define LIMIT_2 65536
#define LIMIT_3 16777216
#define bwtFileLimit 10000000

using namespace std;

inline bool file_exist(string& index_file){
	ifstream f(index_file);
	return f.good();
}

void writeBufferFunc(unsigned char* writeBuffer, int& writeIndex, int toWrite, int alignment){

	if(alignment == 1){
		writeBuffer[writeIndex] = char(toWrite);
		++writeIndex;
	}
	else if(alignment == 2){
		writeBuffer[writeIndex] = char(toWrite >> 8);
		++writeIndex;
		writeBuffer[writeIndex] = char(toWrite & 255);
		++writeIndex;
	}
	else if(alignment == 3){
		writeBuffer[writeIndex] = char(toWrite >> 16);
		++writeIndex;
		writeBuffer[writeIndex] = char((toWrite >> 8) & 255);
		++writeIndex;
		writeBuffer[writeIndex] = char(toWrite & 255);
		++writeIndex;
	}
	else if(alignment == 4){
		writeBuffer[writeIndex] = char(toWrite >> 24);
		++writeIndex;
		writeBuffer[writeIndex] = char((toWrite >> 16) & 255);
		++writeIndex;
		writeBuffer[writeIndex] = char((toWrite >> 8) & 255);
		++writeIndex;
		writeBuffer[writeIndex] = char(toWrite & 255);
		++writeIndex;
	}
}

void writeIndexFile(int* count, string index_file, int alignment, int writeSize, int rowNb, int* charList, int charNb){

	unsigned char* writeBuffer = new unsigned char[writeSize];
	int writeIndex = 0;

	writeBufferFunc(writeBuffer, writeIndex, rowNb, alignment);


	for(int i = 0; i < charNb; ++i){
		// writeBuffer[writeIndex] = char(charList[i]);
		// ++writeIndex;
		writeBufferFunc(writeBuffer, writeIndex, count[charList[i]], alignment);

	}

	ofstream toFile(index_file, ios::out | ios::app | ios::binary);
	toFile.write((char*)(&writeBuffer[0]), writeIndex);
	delete[] writeBuffer;
	toFile.close();
}


int getOCC(char c, int position, string index_file, int alignment, int writeSize, char* bwt_encoded_file, int lastIndex, int* getOCC_aid){

	ifstream fromFile(index_file, ios::binary);
	ifstream fromFile2(bwt_encoded_file, ios::binary);
	fromFile2.seekg(0, ios::beg);


	if(c == 0 && position >= lastIndex)
		return 1;

	if(c == 0 && position < lastIndex)
		return 0;


	if(!file_exist(index_file)){

		if((position >= lastIndex && (position - 1) < writeSize - 1)){
			position -= 1;

			int cnt = 0;
			char* readBuffer = new char[position + 1];
			fromFile2.read(readBuffer, position + 1);
			for(int i = 0; i < position + 1; ++i){
				if(readBuffer[i] == c)
					++cnt;
			}
			return cnt;
		}



		if((position < lastIndex && position < writeSize - 1)){

			int cnt = 0;
			char* readBuffer = new char[position + 1];
			fromFile2.read(readBuffer, position + 1);
			for(int i = 0; i < position + 1; ++i){
				if(readBuffer[i] == c)
					++cnt;
			}
			return cnt;
		}

	}



	if((position >= lastIndex && (position - 1) < writeSize - 1)){
		position -= 1;

		int cnt = 0;
		char* readBuffer = new char[position + 1];
		fromFile2.read(readBuffer, position + 1);
		for(int i = 0; i < position + 1; ++i){
			if(readBuffer[i] == c)
				++cnt;
		}
		return cnt;
	}



	if((position < lastIndex && position < writeSize - 1)){


		int cnt = 0;
		char* readBuffer = new char[position + 1];
		fromFile2.read(readBuffer, position + 1);
		for(int i = 0; i < position + 1; ++i){
			if(readBuffer[i] == c)
				++cnt;
		}
		return cnt;
	}

	int readIndex = (position + 1) / writeSize;

	readIndex -= 1;

	readIndex = readIndex * writeSize;

	fromFile.seekg(readIndex, ios::beg);

	unsigned char* getBuffer = new unsigned char[alignment];
	fromFile.read((char*)(&getBuffer[0]), alignment);
	int rowNb = 0;
	for(int i = 0; i < alignment; ++i){

		rowNb = rowNb | getBuffer[i];

		if(i != alignment - 1)
			rowNb = rowNb << 8;
	}	

	int toMoveIndex = getOCC_aid[c - char(0)];

	fromFile.seekg(toMoveIndex * alignment, ios::cur);

	fromFile.read((char*)(&getBuffer[0]), alignment);
	int nowCount = 0;
	for(int i = 0; i < alignment; ++i){

		nowCount = nowCount | getBuffer[i];

		if(i != alignment - 1)
			nowCount = nowCount << 8;
	}





	if(position == rowNb)
		return nowCount;



	if(position >= lastIndex)
		position -= 1;

	if(rowNb >= lastIndex){
		rowNb -= 1;
	}

	int readSize = position - rowNb + 1;
	char* locateBuffer = new char[readSize];

	// cout << "position: " << position << " rowNb: " <<rowNb << "\n";

	// cout << "readSize: " << readSize << "\n";
	fromFile2.seekg(rowNb, ios::beg);

	fromFile2.read(locateBuffer, readSize);

	for(int j = 1; j < readSize; ++j){

		if(locateBuffer[j] == c)
			++nowCount;
	}

	delete[] locateBuffer;
	fromFile2.close();

	// cout << "nowCount: " << nowCount << "\n";
	return nowCount;
}


int getOCC2(char c, int position, string index_file, int alignment, int writeSize, char* bwt_encoded_file, int lastIndex, int* getOCC_aid, unsigned char* bwtBuffer){

	ifstream fromFile(index_file, ios::binary);
	ifstream fromFile2(bwt_encoded_file, ios::binary);
	fromFile2.seekg(0, ios::beg);


	if(c == 0 && position >= lastIndex)
		return 1;

	if(c == 0 && position < lastIndex)
		return 0;


	if(!file_exist(index_file)){

		if((position >= lastIndex && (position - 1) < writeSize - 1)){
			position -= 1;

			int cnt = 0;
			char* readBuffer = new char[position + 1];
			fromFile2.read(readBuffer, position + 1);
			for(int i = 0; i < position + 1; ++i){
				if(readBuffer[i] == c)
					++cnt;
			}
			return cnt;
		}



		if((position < lastIndex && position < writeSize - 1)){

			int cnt = 0;
			char* readBuffer = new char[position + 1];
			fromFile2.read(readBuffer, position + 1);
			for(int i = 0; i < position + 1; ++i){
				if(readBuffer[i] == c)
					++cnt;
			}
			return cnt;
		}

	}



	if((position >= lastIndex && (position - 1) < writeSize - 1)){
		position -= 1;

		int cnt = 0;
		char* readBuffer = new char[position + 1];
		fromFile2.read(readBuffer, position + 1);
		for(int i = 0; i < position + 1; ++i){
			if(readBuffer[i] == c)
				++cnt;
		}
		return cnt;
	}



	if((position < lastIndex && position < writeSize - 1)){


		int cnt = 0;
		char* readBuffer = new char[position + 1];
		fromFile2.read(readBuffer, position + 1);
		for(int i = 0; i < position + 1; ++i){
			if(readBuffer[i] == c)
				++cnt;
		}
		return cnt;
	}




	int readIndex = (position + 1) / writeSize;

	readIndex -= 1;

	readIndex = readIndex * writeSize;

	fromFile.seekg(readIndex, ios::beg);

	unsigned char* getBuffer = new unsigned char[alignment];
	fromFile.read((char*)(&getBuffer[0]), alignment);
	int rowNb = 0;
	for(int i = 0; i < alignment; ++i){

		rowNb = rowNb | getBuffer[i];

		if(i != alignment - 1)
			rowNb = rowNb << 8;
	}	

	int toMoveIndex = getOCC_aid[c - char(0)];

	fromFile.seekg(toMoveIndex * alignment, ios::cur);

	fromFile.read((char*)(&getBuffer[0]), alignment);
	int nowCount = 0;
	for(int i = 0; i < alignment; ++i){

		nowCount = nowCount | getBuffer[i];

		if(i != alignment - 1)
			nowCount = nowCount << 8;
	}

	// cout << "position: " << position << "\n";

	// cout << "writeSize: " << writeSize << "\n";
	// cout << "now rowNb: " << rowNb << "\n";
	// cout << "next rowNb: " << rowNb + writeSize << "\n";





	int nextN;
	int nextCount;




	fromFile.seekg(readIndex + writeSize, ios::beg);

	int nn = fromFile.tellg();

	// cout << "nn is: " << nn << "\n";

	fromFile.seekg(0, ios::end);

	int fileSize = fromFile.tellg();

	// cout << "fileSize: " << fileSize << "\n";

	if(nn >= fileSize){
		nextN = INT_MAX;
		nextCount = 0;
	}
	else{
		fromFile.seekg(readIndex + writeSize, ios::beg);
		fromFile.read((char*)(&getBuffer[0]), alignment);
		nextN = 0;
		for(int i = 0; i < alignment; ++i){

			nextN = nextN | getBuffer[i];

			if(i != alignment - 1)
				nextN = nextN << 8;
		}
		// cout << "nextN: " << nextN << "\n";

		// cout << "position - rowNb: " << position - rowNb << "\n";
		// cout << "nextN - position: " << nextN - position << "\n";
		


		fromFile.seekg(toMoveIndex * alignment, ios::cur);

		fromFile.read((char*)(&getBuffer[0]), alignment);
		nextCount = 0;
		for(int i = 0; i < alignment; ++i){

			nextCount = nextCount << 8;
			nextCount = nextCount | getBuffer[i];			
		}
	}



	if(position == rowNb)
		return nowCount;

	if(position == nextN)
		return nextCount;


	////////
	//cout << "---------return\n";
	//return nowCount;
	////////
	// cout << "writeSize: " << writeSize << "\n";
	// cout << "pre  nowCount: " << nowCount << "\n";

	if(position >= lastIndex)
		position -= 1;

	if(rowNb >= lastIndex){
		rowNb -= 1;
	}

	int readSize = position - rowNb + 1;


	if(position < bwtFileLimit){
		

		if(position - rowNb < nextN - position){

			// cout << "11\n";
			for(int j = rowNb + 1; j <= position; ++j){
				if(bwtBuffer[j] == c)
					++nowCount;
			}

			return nowCount;
		}
		else{

			// cout << "22\n";
			int tempCounter = 0;

			for(int j = position; j < nextN; ++j){
				if(bwtBuffer[j] == c)
					++tempCounter;
			}

			if(bwtBuffer[nextN] == c){
				if(bwtBuffer[position] == c){
					return nextCount - 1 - tempCounter + 1;
				}
				else
					return nextCount - 1 - tempCounter;
			}
			else{
				if(bwtBuffer[position] == c){
					return nextCount - tempCounter + 1;
				}
				else
					return nextCount - tempCounter;
			}

		}
	}

	else{
		// cout << "&&&&&&no " << position << "\n";

		char* locateBuffer = new char[readSize];

		// cout << "position: " << position << " rowNb: " <<rowNb << "\n";

		// cout << "readSize: " << readSize << "\n";
		fromFile2.seekg(rowNb, ios::beg);

		fromFile2.read(locateBuffer, readSize);

		for(int j = 1; j < readSize; ++j){

			if(locateBuffer[j] == c)
				++nowCount;
		}

		delete[] locateBuffer;
		fromFile2.close();

		// cout << "nowCount: " << nowCount << "\n";
		return nowCount;
	}
	
}

int getOCC3(char c, int position, string index_file, int alignment, int writeSize, char* bwt_encoded_file, int lastIndex, int* getOCC_aid, unsigned char* bwtBuffer, ifstream& fromFile, ifstream& fromFile2){

	// ifstream fromFile(index_file, ios::binary);
	// ifstream fromFile2(bwt_encoded_file, ios::binary);
	// fromFile.seekg(0, ios::beg);
	// fromFile2.seekg(0, ios::beg);



	if(c == 0 && position >= lastIndex)
		return 1;

	if(c == 0 && position < lastIndex)
		return 0;


	if(!file_exist(index_file)){
		fromFile2.seekg(0, ios::beg);

		if((position >= lastIndex && (position - 1) < writeSize - 1)){
			position -= 1;

			int cnt = 0;
			char* readBuffer = new char[position + 1];
			fromFile2.read(readBuffer, position + 1);
			for(int i = 0; i < position + 1; ++i){
				if(readBuffer[i] == c)
					++cnt;
			}
			return cnt;
		}



		if((position < lastIndex && position < writeSize - 1)){

			int cnt = 0;
			char* readBuffer = new char[position + 1];
			fromFile2.read(readBuffer, position + 1);
			for(int i = 0; i < position + 1; ++i){
				if(readBuffer[i] == c)
					++cnt;
			}
			return cnt;
		}

	}



	if((position >= lastIndex && (position - 1) < writeSize - 1)){
		fromFile2.seekg(0, ios::beg);
		position -= 1;

		int cnt = 0;
		char* readBuffer = new char[position + 1];
		fromFile2.read(readBuffer, position + 1);
		for(int i = 0; i < position + 1; ++i){
			if(readBuffer[i] == c)
				++cnt;
		}
		return cnt;
	}



	if((position < lastIndex && position < writeSize - 1)){
		fromFile2.seekg(0, ios::beg);


		int cnt = 0;
		char* readBuffer = new char[position + 1];
		fromFile2.seekg(0, ios::beg);
		fromFile2.read(readBuffer, position + 1);
		for(int i = 0; i < position + 1; ++i){
			if(readBuffer[i] == c)
				++cnt;
		}
		// cout << "cnt: " << cnt << "\n";
		return cnt;
	}




	int readIndex = (position + 1) / writeSize;

	readIndex -= 1;

	readIndex = readIndex * writeSize;

	fromFile.seekg(readIndex, ios::beg);

	unsigned char* getBuffer = new unsigned char[alignment];
	fromFile.read((char*)(&getBuffer[0]), alignment);
	int rowNb = 0;
	for(int i = 0; i < alignment; ++i){

		rowNb = rowNb | getBuffer[i];

		if(i != alignment - 1)
			rowNb = rowNb << 8;
	}	

	int toMoveIndex = getOCC_aid[c - char(0)];

	fromFile.seekg(toMoveIndex * alignment, ios::cur);

	fromFile.read((char*)(&getBuffer[0]), alignment);
	int nowCount = 0;
	for(int i = 0; i < alignment; ++i){

		nowCount = nowCount | getBuffer[i];

		if(i != alignment - 1)
			nowCount = nowCount << 8;
	}




	if(position == rowNb)
		return nowCount;


	if(position >= lastIndex)
		position -= 1;

	if(rowNb >= lastIndex){
		rowNb -= 1;
	}

	int readSize = position - rowNb + 1;


	if(position < bwtFileLimit){
		

	// if(position - rowNb < nextN - position){

		// cout << "11\n";
		for(int j = rowNb + 1; j <= position; ++j){
			if(bwtBuffer[j] == c)
				++nowCount;
		}

		return nowCount;
	}
	else{
		// cout << "&&&&&&no " << position << "\n";

		char* locateBuffer = new char[readSize];

		// cout << "position: " << position << " rowNb: " <<rowNb << "\n";

		// cout << "readSize: " << readSize << "\n";
		fromFile2.seekg(rowNb, ios::beg);

		fromFile2.read(locateBuffer, readSize);

		for(int j = 1; j < readSize; ++j){

			if(locateBuffer[j] == c)
				++nowCount;
		}

		delete[] locateBuffer;
		///fromFile2.close();

		// cout << "nowCount: " << nowCount << "\n";
		return nowCount;
	}
	
}



int getNextPosition(string index_file, string position_file, int position, int alignment, int writeSize, int* C, char* bwt_encoded_file, int lastIndex, int* getOCC_aid){

	if(position == lastIndex){
		char c = char(0);
		return C[c - char(0)] + getOCC(c, position, index_file, alignment, writeSize, bwt_encoded_file, lastIndex, getOCC_aid) - 1;
	}

	if(position < lastIndex){


		ifstream fromFile(bwt_encoded_file, ios::binary);

		fromFile.seekg(position, ios::beg);

		char* charBuffer = new char[1];
		fromFile.read(charBuffer, 1);

		char c = charBuffer[0];
		delete[] charBuffer;
		fromFile.close();
		return C[c - char(0)] + getOCC(c, position, index_file, alignment, writeSize, bwt_encoded_file, lastIndex, getOCC_aid) - 1;
	}

	if(position > lastIndex){


		ifstream fromFile(bwt_encoded_file, ios::binary);

		fromFile.seekg(position - 1, ios::beg);
		char* charBuffer = new char[1];
		fromFile.read(charBuffer, 1);

		char c = charBuffer[0];
		delete[] charBuffer;
		fromFile.close();


		//return 8;
		return C[c - char(0)] + getOCC(c, position, index_file, alignment, writeSize, bwt_encoded_file, lastIndex, getOCC_aid) - 1;
	}

	return -1;

}

int getNextPosition2(string index_file, string position_file, char c, int position, int alignment, int writeSize, int* C, char* bwt_encoded_file, int lastIndex, int* getOCC_aid, unsigned char* bwtBuffer){

	if(position == lastIndex){
		char c = char(0);
		return C[c - char(0)] + getOCC2(c, position, index_file, alignment, writeSize, bwt_encoded_file, lastIndex, getOCC_aid, bwtBuffer) - 1;
	}

	// if(position < lastIndex){


	// 	// ifstream fromFile(bwt_encoded_file, ios::binary);

	// 	// fromFile.seekg(position, ios::beg);

	// 	// char* charBuffer = new char[1];
	// 	// fromFile.read(charBuffer, 1);

	// 	// char c = charBuffer[0];
	// 	// delete[] charBuffer;
	// 	// fromFile.close();
	// 	return C[c - char(0)] + getOCC(c, position, index_file, alignment, writeSize, bwt_encoded_file, lastIndex, getOCC_aid) - 1;
	// }

	// if(position > lastIndex){


		// ifstream fromFile(bwt_encoded_file, ios::binary);

		// fromFile.seekg(position - 1, ios::beg);
		// char* charBuffer = new char[1];
		// fromFile.read(charBuffer, 1);

		// char c = charBuffer[0];
		// delete[] charBuffer;
		// fromFile.close();


		//return 8;
	return C[c - char(0)] + getOCC2(c, position, index_file, alignment, writeSize, bwt_encoded_file, lastIndex, getOCC_aid, bwtBuffer) - 1;


	// }

	return -1;

}

int getNextPosition3(string index_file, string position_file, char c, int position, int alignment, int writeSize, int* C, char* bwt_encoded_file, int lastIndex, int* getOCC_aid, unsigned char* bwtBuffer, ifstream& indexFile, ifstream& bwtFile){

	if(position == lastIndex){
		char c = char(0);
		return C[c - char(0)] + getOCC3(c, position, index_file, alignment, writeSize, bwt_encoded_file, lastIndex, getOCC_aid, bwtBuffer, indexFile, bwtFile) - 1;
	}

	return C[c - char(0)] + getOCC3(c, position, index_file, alignment, writeSize, bwt_encoded_file, lastIndex, getOCC_aid, bwtBuffer, indexFile, bwtFile) - 1;
}


int getNextPosition_NA(ifstream& indexFile, ifstream& bwtFile, int position, int alignment, int writeSize, int* C, string index_file, char* bwt_encoded_file, int lastIndex, int* getOCC_aid, unsigned char* bwtBuffer){

	if(position == lastIndex){
		char c = char(0);
		return C[c - char(0)] + getOCC3(c, position, index_file, alignment, writeSize, bwt_encoded_file, lastIndex, getOCC_aid, bwtBuffer, indexFile, bwtFile) - 1;
	}

	if(position < lastIndex){

		char c;
		bwtFile.seekg(position, ios::beg);
		bwtFile.read(&c, 1);

		return C[c - char(0)] + getOCC3(c, position, index_file, alignment, writeSize, bwt_encoded_file, lastIndex, getOCC_aid, bwtBuffer, indexFile, bwtFile) - 1;
	}

	if(position > lastIndex){


		char c; 
		bwtFile.seekg(position - 1, ios::beg);
		bwtFile.read(&c, 1);

		return C[c - char(0)] + getOCC3(c, position, index_file, alignment, writeSize, bwt_encoded_file, lastIndex, getOCC_aid, bwtBuffer, indexFile, bwtFile) - 1;
	}

	return -1;
}

// no sa is stored in positional file
//
int getSa_v1(string index_file, char* bwt_encoded_file, ifstream& indexFile, ifstream& bwtFile, int position, int alignment, int writeSize, int* C, int lastIndex, int* getOCC_aid, unsigned char* bwtBuffer){

	int posCnt = 0;

	while(true){
		if(position == lastIndex)
			return posCnt;
		++posCnt;
		position = getNextPosition_NA(indexFile, bwtFile, position, alignment, writeSize, C, index_file, bwt_encoded_file, lastIndex, getOCC_aid, bwtBuffer);
	}
}


// only one sa is stored
//
int getSa_v2(string index_file, char* bwt_encoded_file, int beginNb, int beginVal, ifstream& indexFile, ifstream& bwtFile, int position, int alignment, int writeSize, int* C, int lastIndex, int* getOCC_aid, unsigned char* bwtBuffer){

	int posCnt = 0;

	while(true){
		if(position == lastIndex)
			return posCnt;
		else if(position == beginNb){
			return posCnt + beginVal;
		}
		++posCnt;
		position = getNextPosition_NA(indexFile, bwtFile, position, alignment, writeSize, C, index_file, bwt_encoded_file, lastIndex, getOCC_aid, bwtBuffer);
	}
}

// last situation
//

int getSa_v3(string index_file, char* bwt_encoded_file, int beginNb, int beginVal, int lastNb, int lastVal, int step, ifstream& posFile, ifstream& indexFile, ifstream& bwtFile, int position, int alignment, int writeSize, int* C, int lastIndex, int* getOCC_aid, unsigned char* bwtBuffer, int deliNum){

	int posCnt = 0;

	while(true){

		if(position == lastIndex)
			return posCnt;

		if(position == beginNb)
			return posCnt + beginVal;

		if(position == lastNb)
			return posCnt + lastVal;


		if(position > beginNb && (position - beginNb) % step == 0 && position <= lastNb){
			// find the suffix array value
			int movebit = (position - beginNb) / step;

			posFile.seekg(2 * alignment + deliNum * (alignment * 2) + (movebit) * (2 * alignment) + alignment , ios::beg);
			unsigned char* delimiterNumBuffer = new unsigned char[alignment];
			posFile.read((char*)(&delimiterNumBuffer[0]), alignment);
			int saValue = 0;
			for(int i = 0; i < alignment; ++i){
				saValue = saValue | delimiterNumBuffer[i];
				if(i != alignment - 1)
					saValue = saValue << 8;
			}
			
			delete[] delimiterNumBuffer;
			return saValue + posCnt;
		}
		++posCnt;

		position = getNextPosition_NA(indexFile, bwtFile, position, alignment, writeSize, C, index_file, bwt_encoded_file, lastIndex, getOCC_aid, bwtBuffer);

	}

}



int getPosition(string position_file, string index_file, int position, int alignment, int writeSize, int* C, char* bwt_encoded_file, int lastIndex, int* getOCC_aid){

	ifstream fromFile(position_file, ios::binary);
	fromFile.seekg(alignment, ios::beg);
	unsigned char* delimiterNumBuffer = new unsigned char[alignment];
	fromFile.read((char*)(&delimiterNumBuffer[0]), alignment);
	int deliNum = 0;
	for(int i = 0; i < alignment; ++i){
		deliNum = deliNum | delimiterNumBuffer[i];
		if(i != alignment - 1)
			deliNum =deliNum << 8;
	}
	delete[] delimiterNumBuffer;


	fromFile.seekg(deliNum * (alignment * 2), ios::cur);


	if(fromFile.peek() == EOF){

		int posCnt = 0;

		while(true){
			if(position == lastIndex){
				return posCnt;
			}
			++posCnt;
			position = getNextPosition(index_file, position_file, position, alignment, writeSize, C, bwt_encoded_file, lastIndex, getOCC_aid);
		}

	}





	delimiterNumBuffer = new unsigned char[alignment];
	fromFile.read((char*)(&delimiterNumBuffer[0]), alignment);
	int beginNb = 0;
	for(int i = 0; i < alignment; ++i){
		beginNb = beginNb | delimiterNumBuffer[i];
		if(i != alignment - 1)
			beginNb = beginNb << 8;
	}


	fromFile.read((char*)(&delimiterNumBuffer[0]), alignment);
	int beginVal = 0;
	for(int i = 0; i < alignment; ++i){
		beginVal = beginVal | delimiterNumBuffer[i];
		if(i != alignment - 1)
			beginVal = beginVal << 8;
	}

	delete[] delimiterNumBuffer;

	if(fromFile.peek() == EOF){
		int posCnt = 0;

		while(true){
			if(position == lastIndex)
				return posCnt;
			else if(position == beginNb){
				return posCnt + beginVal;
			}
			++posCnt;
			position = getNextPosition(index_file, position_file, position, alignment, writeSize, C, bwt_encoded_file, lastIndex, getOCC_aid);

		}
	}

	int beginEndIndex = fromFile.tellg();


	fromFile.seekg(0, ios::end);

	int ssize = fromFile.tellg();

	fromFile.seekg(ssize - 2 * alignment, ios::beg);


	delimiterNumBuffer = new unsigned char[alignment];
	fromFile.read((char*)(&delimiterNumBuffer[0]), alignment);
	int lastNb = 0;
	for(int i = 0; i < alignment; ++i){
		lastNb = lastNb | delimiterNumBuffer[i];
		if(i != alignment - 1)
			lastNb = lastNb << 8;
	}


	fromFile.read((char*)(&delimiterNumBuffer[0]), alignment);
	int lastVal = 0;
	for(int i = 0; i < alignment; ++i){
		lastVal = lastVal | delimiterNumBuffer[i];
		if(i != alignment - 1)
			lastVal = lastVal << 8;
	}
	delete[] delimiterNumBuffer;


	int lastEndIndex = fromFile.tellg();

	int step = (lastNb - beginNb) / ((lastEndIndex - beginEndIndex) / (2 * alignment));

	int posCnt = 0;

	while(true){

		if(position == lastIndex)
			return posCnt;

		if(position == beginNb)
			return posCnt + beginVal;

		if(position == lastNb)
			return posCnt + lastVal;


		if(position > beginNb && (position - beginNb) % step == 0 && position <= lastNb){
			// find the suffix array value
			int movebit = (position - beginNb) / step;

			fromFile.seekg(2 * alignment + deliNum * (alignment * 2) + (movebit) * (2 * alignment) + alignment , ios::beg);
			delimiterNumBuffer = new unsigned char[alignment];
			fromFile.read((char*)(&delimiterNumBuffer[0]), alignment);
			int saValue = 0;
			for(int i = 0; i < alignment; ++i){
				saValue = saValue | delimiterNumBuffer[i];
				if(i != alignment - 1)
					saValue = saValue << 8;
			}
			
			delete[] delimiterNumBuffer;
			return saValue + posCnt;
		}
		++posCnt;

		position = getNextPosition(index_file, position_file, position, alignment, writeSize, C, bwt_encoded_file, lastIndex, getOCC_aid);

	}
	return -1;
}

void searchMode_M(string index_file, int alignment, int writeSize, const char* queryString, int* C, char* bwt_encoded_file, int lastIndex, int* getOCC_aid){


	int queryLength = strlen(queryString);

	int i = queryLength - 1;

	char c = queryString[i];

	int first = C[c - char(0)] + 1 - 1;
	int last = C[c - char(0) + 1] - 1;

	if(first > last){
		cout << 0 << "\n";
		return;
	}


	while(first <= last && i >= 1){
		c = queryString[i - 1];

		first = C[c - char(0)] + getOCC(c, first - 1, index_file, alignment, writeSize, bwt_encoded_file, lastIndex, getOCC_aid);

		last =  C[c - char(0)] + getOCC(c, last,      index_file, alignment, writeSize, bwt_encoded_file, lastIndex, getOCC_aid) - 1;
		
		--i;
	}
	cout << last - first + 1 << "\n";
}

void searchMode_N(string position_file, string index_file, int alignment, int writeSize, const char* queryString, int* C, char* bwt_encoded_file, char* mode, int lastIndex, int* getOCC_aid){

	ifstream bwtFile(bwt_encoded_file, ios::binary);
	bwtFile.seekg(0, ios::end);
	int bwtFileSize = bwtFile.tellg();
	bwtFile.seekg(0, ios::beg);


	int bwtBlock = 10000000;
	unsigned char* bwtBuffer;

	if(bwtFileSize < bwtBlock){

		bwtBuffer = new unsigned char[bwtFileSize];

		bwtFile.read((char*)(&bwtBuffer[0]), bwtFileSize);

	}
	else{

		bwtBuffer = new unsigned char[bwtBlock];
		bwtFile.read((char*)(&bwtBuffer[0]), bwtBlock);
	}

	ifstream indexFile(index_file, ios::binary);




	int queryLength = strlen(queryString);

	int i = queryLength - 1;

	char c = queryString[i];

	int first = C[c - char(0)] + 1 - 1;
	int last = C[c - char(0) + 1] - 1;


	if(first > last){
		cout << 0 << "\n";
		return;
	}


	while(first <= last && i >= 1){
		c = queryString[i - 1];

		first = C[c - char(0)] + getOCC(c, first - 1, index_file, alignment, writeSize, bwt_encoded_file, lastIndex, getOCC_aid);

		last =  C[c - char(0)] + getOCC(c, last,      index_file, alignment, writeSize, bwt_encoded_file, lastIndex, getOCC_aid) - 1;
		
		--i;
	}

	if(first > last){
		cout << 0 << "\n";
		return;
	}




	ifstream fromFile(position_file, ios::binary);
	unsigned char* delimiterNumBuffer = new unsigned char[alignment];
	fromFile.seekg(alignment, ios::beg);
	fromFile.read((char*)(&delimiterNumBuffer[0]), alignment);
	int deliNum = 0;
	for(int i = 0; i < alignment; ++i){
		deliNum = deliNum | delimiterNumBuffer[i];
		if(i != alignment - 1)
			deliNum =deliNum << 8;
	}
	delete[] delimiterNumBuffer;

	if(deliNum == 1){
		cout << 1 << "\n";
		return;
	}


	// fromFile.seekg(deliNum * (alignment * 2), ios::cur);
	unsigned char* deliNb_pos = new unsigned char[deliNum * (alignment * 2)];

	fromFile.read((char*)(&deliNb_pos[0]), deliNum * (alignment * 2));

	int* deliNb_list = new int[deliNum];
	int deliNb_list_index = 0;


	for(int i = 0; i < deliNum * (alignment * 2); i += 2 * alignment){
		int curNb = 0;

		for(int j = i; j < i + alignment; ++j){
			curNb = curNb | deliNb_pos[j];
			if(j != i + alignment - 1)
				curNb = curNb << 8;
		}

		deliNb_list[deliNb_list_index] = curNb;
		++deliNb_list_index;
	}


	if(fromFile.peek() == EOF){

		char* flagBits = new char[deliNum];
		memset(flagBits, '0', deliNum);


		for(int i = first; i <= last; ++i){
			

			//int getSa_v1(ifstream& indexFile, ifstream& bwtFile, int position, int alignment, int writeSize, int* C, int lastIndex, int* getOCC_aid, unsigned char* bwtBuffer){

			int saValue = getSa_v1(index_file, bwt_encoded_file, indexFile, bwtFile, i, alignment, writeSize, C, lastIndex, getOCC_aid, bwtBuffer);
			cout << "saValue: " << saValue << "\n";

			auto low = lower_bound(deliNb_list, deliNb_list + deliNum, saValue);

			flagBits[low - deliNb_list] = '1';

		}

		

		if(strcmp(mode, "-n") == 0){

			int ans = 0;

			for(int i = 0; i < deliNum; ++i){
				if(flagBits[i] == '1'){
					++ans;
				}
			}

			cout << ans << "\n";
			return;
		}

		for(int i = 0; i < deliNum; ++i){
			if(flagBits[i] == '1')
				cout << i + 1 << "\n";
		}
		return;

	}


	delimiterNumBuffer = new unsigned char[alignment];
	fromFile.read((char*)(&delimiterNumBuffer[0]), alignment);
	int beginNb = 0;
	for(int i = 0; i < alignment; ++i){
		beginNb = beginNb << 8;
		beginNb = beginNb | delimiterNumBuffer[i];
	}


	fromFile.read((char*)(&delimiterNumBuffer[0]), alignment);
	int beginVal = 0;
	for(int i = 0; i < alignment; ++i){
		beginVal = beginVal << 8;
		beginVal = beginVal | delimiterNumBuffer[i];
	}

	delete[] delimiterNumBuffer;

	if(fromFile.peek() == EOF){

		char* flagBits = new char[deliNum];
		memset(flagBits, '0', deliNum);


		for(int i = first; i <= last; ++i){
			


			//int getSa_v2(int beginNb, int beginVal, ifstream& indexFile, ifstream& bwtFile, int position, int alignment, int writeSize, int* C, int lastIndex, int* getOCC_aid, unsigned char* bwtBuffer)
			int saValue = getSa_v2(index_file, bwt_encoded_file, beginNb, beginVal, indexFile, bwtFile, i, alignment, writeSize, C, lastIndex, getOCC_aid, bwtBuffer);
			cout << "saValue: " << saValue << "\n";

			auto low = lower_bound(deliNb_list, deliNb_list + deliNum, saValue);

			flagBits[low - deliNb_list] = '1';

		}

		

		if(strcmp(mode, "-n") == 0){

			int ans = 0;

			for(int i = 0; i < deliNum; ++i){
				if(flagBits[i] == '1'){
					++ans;
				}
			}

			cout << ans << "\n";
			return;
		}

		for(int i = 0; i < deliNum; ++i){
			if(flagBits[i] == '1')
				cout << i + 1 << "\n";
		}
		return;
	}


	cout << "hehe\n";
	int beginEndIndex = fromFile.tellg();


	fromFile.seekg(0, ios::end);

	int ssize = fromFile.tellg();

	fromFile.seekg(ssize - 2 * alignment, ios::beg);


	delimiterNumBuffer = new unsigned char[alignment];
	fromFile.read((char*)(&delimiterNumBuffer[0]), alignment);
	int lastNb = 0;
	for(int i = 0; i < alignment; ++i){
		lastNb = lastNb | delimiterNumBuffer[i];
		if(i != alignment - 1)
			lastNb = lastNb << 8;
	}


	fromFile.read((char*)(&delimiterNumBuffer[0]), alignment);
	int lastVal = 0;
	for(int i = 0; i < alignment; ++i){
		lastVal = lastVal | delimiterNumBuffer[i];
		if(i != alignment - 1)
			lastVal = lastVal << 8;
	}
	delete[] delimiterNumBuffer;


	int lastEndIndex = fromFile.tellg();

	int step = (lastNb - beginNb) / ((lastEndIndex - beginEndIndex) / (2 * alignment));


	char* flagBits = new char[deliNum];
	memset(flagBits, '0', deliNum);


	for(int i = first; i <= last; ++i){
		

		int saValue = getSa_v3(index_file,bwt_encoded_file, beginNb, beginVal, lastNb, lastVal, step, fromFile, indexFile,  bwtFile, i, alignment, writeSize, C, lastIndex, getOCC_aid, bwtBuffer, deliNum);
		// cout << "saValue: " << saValue << "\n";

		auto low = lower_bound(deliNb_list, deliNb_list + deliNum, saValue);

		flagBits[low - deliNb_list] = '1';

	}

	

	if(strcmp(mode, "-n") == 0){

		int ans = 0;

		for(int i = 0; i < deliNum; ++i){
			if(flagBits[i] == '1'){
				++ans;
			}
		}

		cout << ans << "\n";
		return;
	}

	for(int i = 0; i < deliNum; ++i){
		if(flagBits[i] == '1')
			cout << i + 1 << "\n";
	}

	return;
}


void recoverRecord(char delimiter, string position_file, string index_file, int* C, int writeSize, int alignment, int position, char* bwt_encoded_file, int lastIndex, int* getOCC_aid, unsigned char* bwtBuffer){

	// cout << "rev: \n";
	// cout << "lastIndex: " << lastIndex << "\n";
	char c;
	// char* charBuffer;
	// string res = "";
	char* res = new char[5000];
	int resIndex = 0;


	ifstream fromFile(bwt_encoded_file, ios::binary);
	ifstream indexFile(index_file, ios::binary);

	int nextPosition = getNextPosition3(index_file, position_file, delimiter,position, alignment, writeSize, C, bwt_encoded_file, lastIndex, getOCC_aid, bwtBuffer, indexFile, fromFile);

	// int nextPosition = getNextPosition2(index_file, position_file, delimiter,position, alignment, writeSize, C, bwt_encoded_file, lastIndex, getOCC_aid, bwtBuffer);

	// cout << "next----: " << nextPosition << "\n";
	if(nextPosition > lastIndex){
		int tempnextPosition = nextPosition - 1;



		// fromFile.seekg(tempnextPosition, ios::beg);

		// charBuffer = new char[1];
		// fromFile.read(charBuffer, 1);

		// c = charBuffer[0];

		if(tempnextPosition < bwtFileLimit){
			c = bwtBuffer[tempnextPosition];
		}
		else{
			fromFile.seekg(tempnextPosition, ios::beg);

			// charBuffer = new char[1];
			fromFile.read(&c, 1);
		}

		// res.push_back(c);
		res[resIndex] = c;
		++resIndex;
		// delete[] charBuffer;
	}
	else if(nextPosition < lastIndex){


		// cout << "here\n";
		// fromFile.seekg(nextPosition, ios::beg);

		// charBuffer = new char[1];
		// fromFile.read(charBuffer, 1);

		// c = charBuffer[0];

		if(nextPosition < bwtFileLimit){
			c = bwtBuffer[nextPosition];
		}
		else{
			fromFile.seekg(nextPosition, ios::beg);

			// charBuffer = new char[1];
			fromFile.read(&c, 1);
		}

		// res.push_back(c);
		res[resIndex] = c;
		++resIndex;
		// delete[] charBuffer;
	}
	else
		c = 0;


	// fromFile.seekg(0, ios::end);
	// int tmpSize = fromFile.tellg();
	// fromFile.seekg(0, ios::beg);
	// char* tmpBuffer = new char[tmpSize];
	// fromFile.read(tmpBuffer, tmpSize);


	int tempnextPosition;
	while(c != delimiter){
		nextPosition = getNextPosition3(index_file, position_file, c, nextPosition, alignment, writeSize, C, bwt_encoded_file, lastIndex, getOCC_aid, bwtBuffer, indexFile, fromFile);
		// nextPosition = getNextPosition2(index_file, position_file, c, nextPosition, alignment, writeSize, C, bwt_encoded_file, lastIndex, getOCC_aid, bwtBuffer);
		// cout << "nextPosition: " << nextPosition << "\n";
		if(nextPosition > lastIndex){
			tempnextPosition = nextPosition - 1;
		}
		else
			tempnextPosition = nextPosition;

		if(nextPosition == lastIndex)
			continue;


		if(tempnextPosition < bwtFileLimit){
			c = bwtBuffer[tempnextPosition];
		}
		else{
			fromFile.seekg(tempnextPosition, ios::beg);

			fromFile.read(&c, 1);

		}


		// fromFile.seekg(tempnextPosition, ios::beg);
		// charBuffer = new char[1];
		// fromFile.read(charBuffer, 1);
		// c = charBuffer[0];
		// c = bwtBuffer[tempnextPosition];

		// c = tmpBuffer[tempnextPosition];

		if(c == delimiter)
			break;
		// res.push_back(c);
		res[resIndex] = c;
		++resIndex;
	}
	
	res[resIndex] = '\0';

	// reverse(res.begin(), res.end());
	reverse(res, res + resIndex);

	cout << res << "\n";
}

void searchMode_I(char delimiter, char* bwt_encoded_file, string position_file, string index_file, int* C, int writeSize, int alignment, int startRec, int endRec, int fileSize, int lastIndex, int* getOCC_aid){


	// cout << "come in\n";
	ifstream bwtFile(bwt_encoded_file, ios::binary);
	bwtFile.seekg(0, ios::end);
	int bwtFileSize = bwtFile.tellg();
	// cout << "bwtFileSize: " << bwtFileSize << "\n";
	bwtFile.seekg(0, ios::beg);

	// int nextRowNb = INT_MAX;

	int bwtBlock = 10000000;
	unsigned char* bwtBuffer;
	//map<int, unsigned char*> Map;

	if(bwtFileSize < bwtBlock){

		bwtBuffer = new unsigned char[bwtFileSize];

		bwtFile.read((char*)(&bwtBuffer[0]), bwtFileSize);

		// while(indexFile.read((char*)(&indexBuffer[0]), writeSize)){
		// 	rowNb = 0;
		// 	for(int i = 0; i < alignment; ++i){
		// 		rowNb = rowNb << 8;
		// 		rowNb = rowNb | indexBuffer[i];
		// 	}

		// 	unsigned char* newStart = indexBuffer + alignment;
		// 	Map[rowNb] = newStart;
		// }
	}
	else{

		bwtBuffer = new unsigned char[bwtBlock];
		bwtFile.read((char*)(&bwtBuffer[0]), bwtBlock);
	}

	// cout << "hghhh\n";

	// ifstream fromFile(position_file, ios::binary);
	// fromFile.seekg(2 * alignment, ios::beg);
	// unsigned char* delimiterNumBuffer;
	// unsigned char* delimiterNumBuffer = new unsigned char[alignment];
	// fromFile.read((char*)(&delimiterNumBuffer[0]), alignment);
	// int deliNum = 0;
	// for(int i = 0; i < alignment; ++i){
	// 	deliNum = deliNum | delimiterNumBuffer[i];
	// 	if(i != alignment - 1)
	// 		deliNum =deliNum << 8;
	// }
	// delete[] delimiterNumBuffer;


	ifstream fromFile(position_file, ios::binary);
	// unsigned char* delimiterNumBuffer = new unsigned char[alignment];
	// fromFile.seekg(alignment, ios::beg);
	// fromFile.read((char*)(&delimiterNumBuffer[0]), alignment);
	// int deliNum = 0;
	// for(int i = 0; i < alignment; ++i){
	// 	deliNum = deliNum | delimiterNumBuffer[i];
	// 	if(i != alignment - 1)
	// 		deliNum =deliNum << 8;
	// }
	// delete[] delimiterNumBuffer;

	fromFile.seekg(2 * alignment * startRec, ios::beg);


	unsigned char* deliNb_pos = new unsigned char[(endRec - startRec + 1) * 2 * alignment];

	fromFile.read((char*)(&deliNb_pos[0]), (endRec - startRec + 1) * 2 * alignment);

	int* deli_pos_list = new int[endRec - startRec + 1];
	int deli_pos_index = 0;


	for(int i = alignment; i < (endRec - startRec + 1) * 2 * alignment; i += 2 * alignment){
		int curNb = 0;

		for(int j = i; j < i + alignment; ++j){
			curNb = curNb << 8;
			curNb = curNb | deliNb_pos[j];
		}

		deli_pos_list[deli_pos_index] = curNb;
		++deli_pos_index;
	}

	// cout << "Dfsdfsfsdf\n";
	// cout << "deli_pos_index: " << deli_pos_index << "\n";
	// for(int i = 0; i < deli_pos_index; ++i)
	// 	cout << deli_pos_list[i] << ", ";
	// cout << "\n";


	// int deli_pos_index = 5000;

	// cout << "aaa\n";
	// int a[5000] = {15623159, 16077559, 16031644, 16031645, 16080931, 16031646, 16031647, 16081203, 16081202, 16081201, 16081200, 16081204, 16081199, 16081205, 15623158, 16081206, 16070148, 15622004, 15621580, 16052915, 16052914, 15619712, 15619502, 15679805, 15679804, 15617876, 15621028, 16049798, 16049797, 15617598, 16032485, 16032484, 15619116, 15620143, 15679819, 15679818, 15618385, 15618890, 16083912, 16083911, 15619270, 16035729, 16035728, 15619718, 16057438, 16057437, 15617392, 15621340, 16065876, 16065875, 15622838, 15619411, 16071131, 16071130, 15617771, 16065868, 16065867, 15622390, 16066227, 16066226, 15620408, 15621789, 16072881, 16072880, 15622675, 16052674, 16052567, 15621855, 16052690, 16052689, 15622309, 16082082, 16082081, 15617944, 15621226, 16057240, 16057239, 15621061, 16052772, 16052771, 15617442, 16035713, 16035712, 15621316, 15620919, 15623086, 15623085, 15616925, 16052497, 16052496, 15617452, 15615901, 15615902, 15622108, 16056972, 16056971, 15622336, 15679815, 15679814, 15618368, 16035770, 16035769, 15618662, 16050830, 16050829, 15622965, 15618795, 16052821, 16052820, 15618335, 16035645, 16035644, 15620449, 16035007, 16035006, 15620819, 15619465, 15620372, 15619852, 15679446, 15679445, 15617462, 15619619, 16082038, 16082037, 15622366, 16081965, 16081964, 15619436, 16084106, 16084105, 15618971, 16050931, 16050930, 15619072, 15620553, 16057376, 16057375, 15622861, 16083796, 16083795, 15620359, 15620757, 16052649, 16052648, 15621489, 16034829, 16034828, 15621218, 16052902, 16052901, 15618308, 16071667, 16071666, 15622939, 15618430, 16071097, 16071096, 15617513, 15619526, 16035035, 16035034, 15618774, 16057133, 16057132, 15619313, 16034849, 16034848, 15617132, 16070503, 16070502, 15620716, 16050810, 16050809, 15622544, 15617636, 15616118, 15616119, 15617217, 15615899, 15615900, 15622098, 15622985, 15622984, 15620360, 16071171, 16071170, 15618049, 16034997, 16034996, 15620413, 16035115, 16035114, 15619536, 16052708, 16052707, 15622254, 15623538, 15623537, 15621567, 16051572, 16051571, 15617541, 16052894, 16052893, 15618177, 15619580, 16057077, 16057076, 15619349, 16065832, 16065831, 15620886, 15619152, 15619895, 16056731, 16056730, 15618354, 16035855, 15621033, 15621878, 16035880, 16035879, 15622640, 15623018, 15623017, 15621249, 16056970, 16056969, 15622250, 15619847, 15615861, 15615862, 15619481, 15623569, 15623568, 15622073, 16071009, 16071008, 15622796, 16082110, 16082109, 15618475, 15619499, 16034985, 16034984, 15619783, 15620334, 15624491, 15624490, 15618690, 16042906, 16042905, 15618066, 16052841, 16052840, 15618470, 16066218, 16066217, 15619837, 15621693, 15621421, 15623565, 15623564, 15621287, 16065960, 16065863, 15622037, 16065413, 16065412, 15618919, 15617480, 15623629, 15623628, 15621523, 15623472, 15623471, 15617578, 16065760, 16065759, 15620472, 16035405, 16035404, 15617401, 15622074, 15616110, 15616111, 15622655, 15623211, 15623210, 15617112, 15623653, 15623652, 15620025, 15621440, 16066040, 16066039, 15618378, 15623141, 15623140, 15617952, 16042917, 16042916, 15618058, 15618542, 16035707, 16035706, 15621221, 15618101, 16035299, 16035298, 15617198, 16065161, 16065160, 15619478, 16057222, 16057221, 15620281, 15619525, 16050860, 16050859, 15617647, 16065828, 16065827, 15620894, 15619843, 16057083, 16057082, 15619310, 16070513, 16070512, 15621030, 15621122, 15623551, 15623550, 15621437, 15623446, 15623445, 15617405, 16035103, 16035102, 15620013, 16065408, 16065407, 15618033, 16050642, 16050641, 15621148, 15622093, 16057358, 16057357, 15622565, 16057278, 16057277, 15621259, 16083914, 16083913, 15619061, 15679448, 15679447, 15618039, 16081991, 16081990, 15620906, 16052475, 16052474, 15622676, 16052746, 16052745, 15622958, 16065746, 16065745, 15621554, 16065676, 16065675, 15617489, 15623418, 15623417, 15618159, 16049774, 16049773, 15617855, 16035129, 16035128, 15620432, 16065714, 16065713, 15617918, 16066104, 16066103, 15617014, 15617129, 16035089, 16035088, 15618930, 15619448, 16065866, 16065865, 15622376, 16052835, 16052834, 15618469, 16057137, 16057136, 15619350, 16050818, 16050817, 15622956, 16083908, 16083907, 15620357, 16056984, 16056983, 15622929, 16065465, 16065464, 15618936, 16052800, 16052799, 15618146, 16042902, 16042901, 15618064, 16042875, 16042874, 15620389, 15615248, 15615249, 15620386, 15623589, 15623588, 15618946, 16050840, 16050839, 15617083, 16035548, 16035547, 15618839, 16041093, 16041092, 15617080, 15679811, 15679810, 15618226, 15620989, 16071031, 16071030, 15622845, 15621425, 16035491, 16035490, 15618160, 15617627, 16065552, 16065551, 15622316, 15618772, 15620070, 15620318, 15621511, 15617875, 16050919, 16050918, 15620206, 15617483, 15617451, 16035764, 16035763, 15618623, 15617778, 15617632, 15620809, 15621828, 15621428, 15617701, 15618182, 15622242, 15618897, 16070509, 16070508, 15621034, 16084066, 16084065, 15618410, 16034903, 16034902, 15618350, 16065151, 16065150, 15619102, 16072887, 16072886, 15617301, 16052698, 16052697, 15622129, 16065951, 16065950, 15621880, 15617945, 16070483, 16070482, 15619015, 15616999, 16042964, 16042963, 15617614, 15620300, 15619809, 15679480, 15679479, 15618167, 16071653, 16071652, 15622428, 15622207, 16043088, 16043087, 15618869, 15617663, 15622711, 16084074, 16084073, 15618507, 15619078, 16066367, 16066366, 15620074, 15622778, 16072877, 16072876, 15622631, 16065119, 16065118, 15618979, 15617646, 15619027, 15679791, 15679790, 15616942, 16057422, 16057421, 15617363, 15621565, 15615996, 15615997, 15620855, 15621648, 15623599, 15623598, 15619779, 16057226, 16057225, 15620286, 16035623, 16035622, 15620321, 16057037, 16057036, 15619425, 15622850, 15620490, 15620399, 15619445, 16035888, 16035887, 15622340, 15621339, 16042839, 16042838, 15619063, 15679460, 15679459, 15622814, 15619297, 16082036, 16082035, 15622395, 16070417, 16070416, 15618452, 16035631, 16035630, 15620378, 15617400, 15623529, 15623528, 15617389, 16065684, 16065683, 15617549, 15622807, 15618928, 16049786, 16049785, 15620079, 16057194, 16057193, 15619924, 15619084, 16066149, 16066148, 15617929, 16057478, 16057477, 15617975, 16050612, 16050611, 15621169, 15617318, 16035367, 16035366, 15622661, 15621758, 16065447, 16065446, 15619582, 16057362, 16057361, 15622578, 16065798, 16065797, 15620007, 16034925, 16034924, 15621910, 15618443, 15620792, 15623322, 15623321, 15622835, 15621685, 16049764, 16049763, 15617399, 16034805, 16034804, 15622651, 16035175, 16035174, 15618107, 16081863, 16081862, 15619815, 16035155, 16035154, 15617669, 16066094, 16066093, 15622669, 15622704, 16081924, 16081923, 16066062, 16066061, 15618395, 15619488, 16052877, 16052876, 15621238, 16050596, 16050595, 15621182, 16050688, 16050687, 15621615, 16065674, 16065673, 15617477, 15619449, 15623216, 15623215, 15617115, 16071065, 16071064, 15617196, 15619002, 15623264, 15623263, 15617293, 16057208, 16057207, 15619915, 15621234, 16071057, 16071056, 15617121, 16052851, 16052850, 15618522, 16042851, 16042850, 15619262, 16057085, 16057084, 15619352, 15617707, 16066044, 16066043, 15618380, 16050901, 16050900, 15619627, 16070419, 16070418, 15618447, 15616073, 15616074, 15621024, 16050540, 16050539, 15620453, 16065382, 16065381, 15622903, 15619861, 15622452, 16050542, 16050541, 15620454, 16071596, 16071595, 15620295, 16042953, 16042952, 15617013, 16034967, 16034966, 15619491, 15619047, 15617239, 16066125, 16066124, 15622905, 16083792, 16083791, 15620198, 15621444, 16057254, 16057253, 15621243, 15623436, 15623435, 15617624, 16043050, 16043049, 15621506, 16035099, 16035098, 15619938, 16035475, 16035474, 15617234, 15621242, 15617316, 15620322, 15617653, 16057139, 16057138, 15619378, 16034715, 16034714, 15622491, 16034783, 16034782, 15618535, 16065419, 16065418, 15618740, 16052865, 16052864, 15620459, 16034885, 16034884, 15617920, 16049678, 16049677, 15622866, 15617672, 16052951, 16052950, 15618424, 15620194, 15623137, 15623136, 15617960, 15622097, 15621904, 16056964, 16056963, 15621994, 16066185, 16066184, 15618826, 16083708, 16083707, 15620367, 15620445, 16070395, 16070394, 15620530, 15623544, 15623543, 15621656, 16057176, 16057175, 15619300, 16035594, 16035593, 15619699, 15622124, 15623499, 15623498, 15617606, 15618459, 16083988, 16083987, 15618793, 15623561, 15623560, 15621837, 16070349, 16070348, 15622752, 15620385, 15622837, 16083910, 16083909, 15618939, 16052913, 16052912, 15619509, 15617662, 16043074, 16043073, 15621486, 16057432, 16057431, 15617394, 16070429, 16070428, 15618499, 16056960, 16056959, 15621801, 15620403, 16065489, 16065488, 15618744, 16035536, 16035535, 15618692, 15617218, 15619193, 15679458, 15679457, 15622822, 16052547, 16052546, 15621857, 16057336, 16057335, 15621484, 16050480, 16050479, 15617758, 16035383, 16035382, 15622645, 16050556, 16050555, 15620518, 16057115, 16057114, 15619387, 15616144, 15616145, 15618155, 16035417, 16035416, 15617904, 15623480, 15623479, 15618253, 16082094, 16082093, 15618404, 16070357, 16070356, 15617032, 16065978, 16065977, 15621821, 15621617, 16052839, 16052838, 15618467, 16065574, 16065573, 15622330, 15623400, 15623399, 15617889, 16065722, 16065721, 15617991, 16070485, 16070484, 15619618, 16052591, 16052590, 15620320, 16083480, 16083479, 15622671, 16066191, 16066190, 15619021, 16070377, 16070376, 15621353, 15623430, 15623429, 15618458, 16052710, 16052709, 15622280, 15623531, 15623530, 15618240, 16071606, 16071605, 15620587, 15623106, 15623105, 15617079, 15623284, 15623283, 15618217, 16035854, 16035853, 15622628, 15619966, 15623595, 15623594, 15619139, 16070473, 16070472, 15618752, 15623509, 15623508, 15618232, 15618980, 15622790, 15622002, 15615744, 15615745, 15621604, 15622166, 15618775, 16034785, 16034784, 15622465, 16082058, 16082057, 15622923, 16035741, 16035740, 15620907, 15621995, 15618111, 15620448, 15618728, 16035947, 16035946, 15618018, 15622686, 15617459, 16052790, 16052789, 15617857, 15622782, 16065943, 16065942, 15621725, 15618654, 15615782, 15615783, 15620717, 15616063, 15616064, 15620849, 15617090, 16084059, 16084058, 15621401, 16050850, 16050849, 15617545, 16032495, 16032494, 15618974, 15619360, 16070487, 16070486, 15619987, 16035778, 16035777, 15618920, 15617075, 15619907, 16050608, 16050607, 15621167, 15615952, 15615953, 15619007, 16081829, 16081828, 15622673, 16057131, 16057130, 15619342, 15621158, 16034833, 16034832, 15622415, 16035918, 16035917, 15617101, 15621726, 16065386, 16065385, 15617162, 15619796, 16070423, 16070422, 15618457, 16052655, 16052654, 15621524, 15622899, 16052706, 16052705, 15622244, 16049680, 16049679, 15616965, 15618796, 15618708, 16050563, 16050561, 15620547, 15623090, 15623089, 15622606, 16082096, 16082095, 15618403, 16065696, 16065695, 15617666, 15619413, 15622210, 16034737, 16034736, 15617600, 16083928, 16083927, 15620419, 16071111, 16071110, 15617728, 16043129, 16043128, 15618761, 15615831, 15615832, 15616967, 15618165, 15615835, 15615836, 15617158, 15623100, 15623099, 15616933, 15623272, 15623271, 15618209, 15622496, 15619228, 16052542, 16052819, 15618326, 16049780, 16049779, 15619605, 15621640, 15623008, 15623007, 15621129, 15619850, 15621672, 16035507, 16035506, 15619948, 16052609, 16052608, 15620595, 16050482, 16050481, 15618025, 16071061, 16071060, 15617123, 16066243, 16066242, 15618801, 16071027, 16071026, 15622843, 15618734, 16070324, 16070323, 15621045, 16034983, 16034982, 15619763, 16081914, 16081913, 15622960, 16035359, 16035358, 15622709, 15615788, 15615789, 15620824, 15622867, 15618900, 16052543, 16052558, 15617908, 15622099, 15621833, 15619161, 15620065, 15617277, 16050580, 16050579, 15617999, 15621224, 16065894, 16065893, 15617297, 16083854, 16083853, 15617754, 16070373, 16070372, 15619813, 15623392, 15623391, 15617796, 15623060, 15623059, 15621893, 16049660, 16049659, 15621675, 15618154, 16035802, 16035801, 15619791, 16050714, 16050713, 15621737, 15618821, 16070443, 16070442, 15620761, 16071681, 16071680, 15617493, 15618784, 16083706, 16083742, 15617153, 15619098, 15624533, 15624532, 15621820, 15622404, 16081879, 16081878, 15621081, 16057051, 16057050, 15619373, 15621838, 16065936, 16065935, 15621558, 16042974, 16042973, 15617807, 16035745, 16035744, 15622238, 15619606, 15679454, 15679453, 15622378, 16065169, 16065168, 15619680, 15621564, 16035415, 16035414, 15617905, 15623010, 15623009, 15621231, 16071121, 16071120, 15617732, 15621603, 16050917, 16050916, 15620099, 15621706, 16071685, 16071684, 15617797, 15620997, 16065173, 16065172, 15619929, 15621928, 15622894, 16034766, 16034765, 15621954, 16057384, 16057383, 15617169, 15617404, 16071109, 16071108, 15617665, 16066013, 16066012, 15619155, 15620274, 15619208, 15616956, 16066389, 16066388, 15620497, 15623497, 15623496, 15617618, 15616045, 15616046, 15620296, 16070399, 16070398, 15619828, 16057342, 16057341, 15622558, 16065362, 16065361, 15622026, 15620063, 16071659, 16071658, 15622590, 15621204, 16035433, 16035432, 15620781, 15618699, 15619265, 16052875, 16052874, 15621071, 16042929, 16042928, 15618074, 16066181, 16066180, 15618737, 16071007, 16071006, 15622795, 16081887, 16081886, 15622687, 15620557, 16050440, 16050439, 15621533, 16083770, 16083769, 15618121, 16052564, 16052563, 15618455, 16035584, 16035583, 15619598, 16057000, 16056999, 15618095, 15623270, 15623269, 15618213, 16065704, 16065703, 15617776, 16070979, 16070978, 15618464, 15619554, 15621093, 16065718, 16065717, 15617836, 16034995, 16034994, 15620328, 16035972, 16035975, 15622344, 15623102, 15623101, 15616934, 16043170, 16043169, 15620514, 16081906, 16081905, 15617801, 16071187, 16071186, 15618198, 15624563, 15624562, 15621824, 15619906, 15619668, 15623643, 15623642, 15621842, 16065810, 16065809, 15620933, 16065527, 16065526, 15619771, 16065372, 16065371, 15622532, 16050618, 16050617, 15621166, 15619944, 16081985, 16081984, 15620770, 15619610, 16071047, 16071046, 15616976, 15617053, 16052764, 16052763, 15617314, 15620978, 16070413, 16070412, 15617958, 15623170, 15623169, 15622762, 15618621, 15622104, 16052939, 16052938, 15622371, 16035520, 16035519, 15618595, 16066139, 16066138, 15617093, 16052722, 16052721, 15622461, 15620208, 15619711, 16057416, 16057415, 15617379, 16066247, 16066246, 15619671, 16070451, 16070450, 15621000, 16066110, 16066108, 15622574, 15623623, 15623622, 15620532, 16052782, 16052781, 15617742, 16065525, 16065524, 15619243, 16070407, 16070406, 15619831, 15620626, 15621297, 16065449, 16065448, 15619239, 16066026, 16066117, 15622586, 15620760, 16052732, 16052731, 15622644, 15618133, 15620826, 16042910, 16042909, 15618068, 15618207, 15623178, 15623177, 15617800, 15619186, 15621585, 15618026, 15620890, 15619180, 15618776, 15620773, 15621417, 16050732, 16050731, 15622392, 15621964, 16052569, 16052568, 15620085, 16050438, 16050437, 15621534, 16066116, 16066115, 15622489, 15622584, 16057472, 16057471, 15617823, 15619042, 16052484, 16052491, 15616998, 15619494, 16035451, 16035450, 15621532, 16034999, 16034998, 15620602, 16066263, 16066262, 15621430, 15616142, 15616143, 15618132, 15621914, 16066034, 16066033, 15618375, 16084116, 16084115, 15617494, 15615765, 15615766, 15619649, 16032465, 16032464, 15619941, 16057392, 16057391, 15617172, 16057021, 16057020, 15619376, 15619160, 15621819, 15621355, 15620800, 15623294, 15623293, 15618228, 15615929, 15615930, 15619675, 16065762, 16065761, 15620918, 16070515, 16070514, 15621736, 16034977, 16034976, 15619746, 15622785, 16052810, 16052809, 15618311, 16035667, 16035666, 15620739, 15623603, 15623602, 15619138, 16071669, 16071668, 15617068, 16082052, 16082051, 15622925, 16070345, 16070344, 15622756, 15618948, 16066257, 16066256, 15620450, 16035655, 16035654, 15620572, 16071587, 16071586, 15618482, 16083972, 16083971, 15621452, 16052786, 16052785, 15617738, 16050646, 16050645, 15621336, 15617228, 16065790, 16065789, 15620109, 16083940, 16083939, 15618663, 15619998, 16065153, 16065152, 15619192, 15623491, 15623490, 15617382, 16071025, 16071024, 15622842, 16083994, 16083993, 15619070, 16035043, 16035042, 15618719, 16052643, 16052642, 15621390, 15621508, 15618720, 16051516, 16051515, 15617553, 16043010, 16043009, 15619220, 15618945, 16072885, 16072884, 15617206, 16050428, 16050427, 15621155, 16065550, 16065549, 15622318, 16071073, 16071072, 15617304, 15620577, 15622348, 16071103, 16071102, 15617587, 16050504, 16050503, 15620181, 16052774, 16052773, 15617481, 16081857, 16081856, 15619644, 16065892, 16065891, 15617200, 16057486, 16057485, 15620838, 15622407, 15620032, 16035085, 16035084, 15619590, 15617915, 16035303, 16035302, 15617925, 15620035, 16070355, 16070354, 15622936, 15679444, 15679443, 15622397, 16052754, 16052753, 15617061, 15619737, 15622257, 15617458, 15617004, 15679807, 15679806, 15617951, 15621059, 15621371, 15619256, 15622836, 16081853, 16081852, 15619224, 15618245, 15616906, 15618497, 15620267, 15617244, 16052639, 16052638, 15621413, 16057310, 16057309, 15621379, 16043096, 16043095, 15620968, 16065431, 16065430, 15618741, 16065616, 16065615, 15622834, 15620101, 15620680, 15617099, 15619988, 16035021, 16035020, 15621293, 16043078, 16043077, 15622311, 15621120, 16050700, 16050699, 15621596, 15624527, 15624526, 15620922, 16066165, 16066164, 15618480, 15620864, 16049756, 16049755, 15622542, 16034721, 16034720, 15622917, 15623408, 15623407, 15618115, 15617680, 15615920, 15615921, 15617722, 15618707, 16071661, 16071660, 15622767, 16043070, 16043069, 15621519, 15621905, 15621719, 16043068, 16043067, 15621861, 15623195, 15623194, 15617105, 16034709, 16034708, 15622369, 16052493, 16052492, 15617000, 15623164, 15623163, 15622270, 16050828, 16050827, 15617097, 15679442, 15679441, 15620329, 16042885, 16042884, 15622068, 16081934, 16081933, 15617803, 15623428, 15623427, 15618428, 15619566, 16042998, 16042997, 15619596, 15617692, 15617225, 15617224, 15617223, 15619631, 15619048, 16070435, 16070434, 15620571, 16057242, 16057241, 15621062, 15619409, 15615891, 15615892, 15621897, 16035473, 16035472, 15617130, 16056775, 16056774, 15617702, 15620765, 15616051, 15616052, 15620496, 15621284, 16066058, 16066057, 15618393, 16066010, 16066009, 15619154, 16065175, 16065174, 15620084, 16066375, 16066374, 15617560, 16066096, 16066095, 15622660, 16065133, 16065132, 15618996, 15624513, 15624512, 15617251, 15615931, 15615979, 15620189, 15620293, 15623657, 15623656, 15618688, 16035558, 16035557, 15619100, 16050720, 16050719, 15622237, 16035389, 16035388, 15622757, 16035015, 16035014, 15620970, 16071081, 16071080, 15617445, 16035173, 16035172, 15618010, 15622363, 16083752, 16083751, 15617357, 15623635, 15623634, 15621466, 15617760, 15622162, 16065554, 16065553, 15622313, 16043032, 16043031, 15620697, 15620983, 16035337, 16035336, 15622720, 16065143, 16065142, 15618836, 16056761, 16056760, 15621442, 15617339, 16065127, 16065126, 15618997, 16035717, 16035716, 15621383, 15619261, 16065374, 16065373, 15622579, 15623571, 15623570, 15622076, 16070481, 16070480, 15619543, 16066213, 16066212, 15619550, 15620250, 16065149, 16065148, 15618953, 15615775, 15615776, 15619969, 16051588, 16051587, 15618299, 15623298, 15623297, 15618259, 15622091, 15620289, 15621895, 16057029, 16057028, 15619379, 16035497, 16035496, 15619980, 16065702, 16065701, 15617762, 15618545, 16065654, 16065653, 15616912, 15623448, 15623447, 15617385, 15615961, 15615962, 15619393, 16083778, 16083777, 15618811, 15618304, 15621976, 15618456, 16081883, 16081882, 15621871, 16057272, 16057271, 15621253, 15622534, 15618619, 16052804, 16052803, 15618310, 15621099, 15621369, 16083768, 16083767, 15618112, 15619115, 16081809, 16081808, 15621498, 16065165, 16065164, 15619498, 15617953, 15623135, 15623134, 15617887, 16071139, 16071138, 15617848, 15615980, 15615981, 15619704, 16052903, 16052888, 15618418, 16071023, 16071022, 15622841, 15623549, 15623548, 15621660, 16049752, 16049751, 15622275, 16035942, 16035941, 15617858, 15617077, 15620340, 15618781, 16035091, 16035090, 15618975, 16035493, 16035492, 15619971, 16052748, 16052747, 15622919, 16050702, 16050701, 15622079, 15618816, 16050610, 16050609, 15621168, 16042853, 16042852, 15619385, 15621930, 15619435, 15622495, 16032497, 16032496, 15618977, 16066381, 16066380, 15618703, 16057344, 16057343, 15622557, 15615779, 15615762, 15620535, 16043016, 16043015, 15619799, 16034811, 16034810, 15620818, 15620058, 16035936, 16035935, 15617591, 15615916, 15615917, 15617465, 15618087, 16070469, 16070468, 15618691, 16034855, 16034854, 15617194, 15618931, 15617719, 15618751, 16052937, 16052936, 15622120, 16065614, 16065613, 15622775, 16042841, 16042840, 15619074, 15615932, 15615933, 15619684, 16052577, 16052576, 15620132, 15619539, 16065962, 16065961, 15622088, 15622106, 16057290, 16057289, 15621358, 15621459, 15617438, 15619902, 15617456, 16065744, 16065743, 15619614, 16083800, 16083799, 15620628, 15618553, 15616083, 15616084, 15621310, 16083783, 16083782, 15619216, 15620014, 15619547, 15622878, 16071618, 16071617, 15621113, 15615784, 15615785, 15620719, 16066153, 16066152, 15617935, 16065680, 16065679, 15617533, 15620631, 16042986, 16042985, 15618161, 16035401, 16035400, 15617237, 16050462, 16050461, 15617081, 15616035, 15616036, 15619800, 15617295, 15622891, 16065364, 16065363, 15622156, 16035065, 16035064, 15619510, 16066189, 16066188, 15618962, 15619280, 16065792, 16065791, 15619612, 15622494, 15622898, 16065706, 16065705, 15617813, 15618030, 16071075, 16071074, 15617305, 15620714, 16035709, 16035708, 15621239, 16081922, 16081921, 15622708, 15618600, 15623581, 15623580, 15622070, 15620036, 15621545, 15621681, 16052863, 16052862, 15620259, 15618560, 16070997, 16070996, 15618520, 16071001, 16071000, 15622792, 15621948, 15621410, 16066255, 16066254, 15620475, 16035916, 16035915, 15617078, 15620060, 15622783, 15620393, 16083960, 16083959, 15621119, 15620173, 16065473, 16065472, 15618870, 16035421, 16035420, 15618758, 16056962, 16056961, 15621793, 16050812, 16050811, 15622735, 16056773, 16056772, 15617608, 16071117, 16071116, 15617731, 15620642, 15622646, 16050662, 16050661, 15621215, 16083860, 16083859, 15617806, 15623084, 15623083, 15622258, 15619410, 16050634, 16050633, 15621146, 15617633, 15621266, 15615954, 15615955, 15619157, 15620649, 16049634, 16049633, 15619051, 16052780, 16052779, 15617595, 16035423, 16035422, 15618779, 16065910, 16065909, 15618004, 15618895, 15620766, 15615971, 15615972, 15619978, 16065678, 16065677, 15617521, 16050598, 16050597, 15621165, 15617983, 15620601, 15616009, 15616010, 15618822, 16071049, 16071048, 15617055, 15620850, 15622224, 16035125, 16035124, 15620370, 15621132, 16042927, 16042926, 15618072, 15621906, 16084174, 16084173, 15621332, 16071041, 16071040, 15616973, 16042921, 16042920, 15618060, 16052686, 16052685, 15622042, 15616041, 15616042, 15619937, 16035772, 16035771, 15618696, 16066229, 16066228, 15620527, 16034867, 16034866, 15617411, 16057412, 16057411, 15617337, 15619594, 15620752, 15620438, 15620799, 15618762, 16035804, 16035803, 15619865, 15619402, 15621014, 15622151, 15619952, 15623127, 15623126, 15617621, 16052503, 16052502, 15617868, 15623557, 15623556, 15621424, 15623254, 15623253, 15617286, 15622964, 16083920, 16083919, 15618966, 16082264, 16082263, 15622381, 16043084, 16043083, 15618674, 16050476, 16050475, 15617571, 15621291, 15619394, 16052752, 16052751, 15616970, 16066141, 16066140, 15617147, 15623456, 15623455, 15617407, 16035586, 16035585, 15619636, 16043110, 16043109, 15621766, 16066383, 16066382, 15621277, 15620549, 15619723, 15618818, 15622448, 16035107, 16035106, 15619692, 15617416, 15616059, 15616060, 15620701, 15623364, 15623363, 15617284, 15617829, 15616043, 15616044, 15619940, 16070505, 16070504, 15620718, 15619910, 16065435, 16065434, 15618868, 15679759, 15679758, 15621008, 16057382, 16057381, 15617168, 16065964, 16065963, 15622110, 16051578, 16051577, 15618027, 15618113, 16057230, 16057229, 15620565, 16057095, 16057094, 15619359, 15622053, 16034861, 16034860, 15617278, 15679793, 15679792, 15617135, 16066301, 16066300, 15617539, 16056992, 16056991, 15617718, 16071141, 16071140, 15617849, 15617883, 16065521, 16065520, 15619443, 16083812, 16083811, 15621746, 16084148, 16084147, 15620751, 15618541, 15623511, 15623510, 15618231, 15619968, 15619487, 16065511, 16065510, 15619053, 15621756, 15620686, 16067396, 16067395, 15622871, 15622338, 15620037, 16081803, 16081802, 15620520, 15622092, 16049702, 16049701, 15618110, 15623068, 15623067, 15621960, 16065406, 16065405, 15618019, 16035059, 16035058, 15619201, 16052768, 16052767, 15617425, 16057338, 16057337, 15621535, 16034881, 16034880, 15617817, 16034719, 16034718, 15622569, 16049754, 16049753, 15622357, 16065483, 16065482, 15618937, 16071608, 16071607, 15620758, 15622759, 16035025, 16035024, 15618571, 16066259, 16066258, 15621073, 16081895, 16081894, 15617204, 16035840, 16035839, 15620709, 15618490, 15679753, 15679752, 15620775, 15623114, 15623113, 15617328, 16035904, 16035903, 15622728, 16034853, 16034852, 15617007, 16050792, 16050791, 15622612, 15619565, 15619537, 15623356, 15623355, 15617207, 16049696, 16049695, 15617994, 15617066, 16050899, 16050898, 15619575, 15623238, 15623237, 15617176, 15621248, 16070405, 16070404, 15617567, 16083824, 16083823, 15622433, 16066060, 16066059, 15618394, 15622859, 15618803, 15619093, 16052509, 16052508, 15618608, 16082022, 16082021, 15621667, 16035341, 16035340, 15622721, 16049690, 16049689, 15617605, 15617530, 16066137, 16066136, 15617008, 15618785, 15618711, 15620276, 16050538, 16050537, 15620338, 16050594, 16050593, 15621181, 16084118, 16084117, 15617495, 15617047, 15623527, 15623526, 15617577, 15679773, 15679772, 15621769, 15620071, 16071021, 16071020, 15622802, 15620266, 15619726, 16050522, 16050521, 15620215, 15621067, 16065816, 16065815, 15620892, 16035615, 16035614, 15620119, 15619558, 16035411, 16035410, 15617651, 15618722, 16042879, 16042878, 15620834, 16052855, 16052854, 15618540, 16050929, 16050928, 15618746, 15618717, 16065710, 16065709, 15617879, 15621331, 16050422, 16050421, 15621085, 16057378, 16057377, 15617163, 15619464, 15619995, 15620302, 15619808, 15616069, 15616070, 15622931, 16050472, 16050471, 15617453, 16083840, 16083839, 15617323, 16081795, 16081794, 15619963, 15620988, 16043094, 16043093, 15620880, 16057184, 16057183, 15619406, 15618088, 15618577, 16049682, 16049681, 15617098, 15621039, 16065682, 16065681, 15617532, 16050552, 16050551, 15620506, 15621818, 16043135, 16043134, 15619080, 16065610, 16065609, 15622744, 15617705, 15619439, 15621909, 16035782, 16035781, 15618992, 16049648, 16049647, 15621065, 16065854, 16065853, 15620984, 16052760, 16052759, 15617159, 16065953, 16065952, 15621898, 16034717, 16034716, 15622567, 15623625, 15623624, 15620726, 15620586, 16043147, 16043146, 15619855, 15621943, 16084061, 16084060, 15618077, 16052845, 16052844, 15618523, 15620335, 16056783, 16056782, 15617840, 15615288, 15615289, 15619050, 16057121, 16057120, 15619344, 16050698, 16050697, 15621938, 15623098, 15623097, 15616935, 15619690, 16082102, 16082101, 15618406, 16050722, 16050721, 15621826, 15621327, 16035749, 16035748, 15622253, 15618642, 15623388, 15623387, 15617794, 15615867, 15615868, 15619486, 16065708, 16065707, 15617818, 15619823, 15618294, 15618856, 16081918, 16081917, 15622962, 16065388, 16065387, 15617374, 16066249, 16066248, 15619670, 16082005, 16081939, 15621343, 15619990, 15620560, 16049642, 16049641, 15620538, 16052890, 16052889, 15617187, 16035123, 16035122, 15620152, 15619103, 16052907, 16052906, 15618604, 16050756, 16050755, 15622223, 15617610, 15623072, 15623071, 15616938, 16035445, 16035444, 15621443, 16043145, 16043144, 15619764, 16050432, 16050431, 15621283, 15622672, 15622528, 16035695, 16035694, 15621114, 16065125, 16065124, 15622402, 15620820, 16035149, 16035148, 15622216, 16065356, 16065355, 15621638, 16050770, 16050769, 15622287, 16049729, 16049728, 15622523, 15622764, 15620913, 15622055, 16065380, 16065379, 15622896, 15621225, 15623262, 15623261, 15617292, 16065916, 16065915, 15618093, 16050937, 16050936, 15618804, 16035788, 16035787, 15619299, 15617183, 16071675, 16071674, 15617157, 16034893, 16034892, 15617992, 16066299, 16066298, 15617027, 16083734, 16083733, 15622439, 16035627, 16035626, 15620341, 16034789, 16034788, 15620915, 16082062, 16082061, 15622926, 16071019, 16071018, 15622801, 15617023, 16034851, 16034850, 15616954, 15620081, 16052513, 16052512, 15618961, 15624515, 15624514, 15617435, 16083862, 16083861, 15617896, 15621668, 15617179, 15615756, 15615757, 15617570, 16035892, 16035891, 15622492, 16070491, 16070490, 15619638, 16083482, 16083481, 15622023, 16082076, 16082075, 15617770, 15621108, 15622689, 15617418, 16035538, 16035537, 15618698, 15615908, 15615909, 15622946, 15622361, 15623024, 15623023, 15621395, 15620242, 15615960, 15619259, 16035828, 16035827, 15620570, 15620877, 15620664, 16056755, 16056754, 15620517, 15621671, 15619049, 16072875, 16072874, 15622150, 16034839, 16034838, 15622607, 15621670, 16050889, 16050888, 15622184, 15618080, 16070389, 16070388, 15619826, 16043028, 16043027, 15620319, 16071687, 16071686, 15617805, 16052533, 16052532, 15620844, 15623328, 15623327, 15622902, 16071137, 16071136, 15617847, 15619557, 15622441, 16057316, 16057315, 15621315, 16050568, 16050567, 15618051, 16050640, 16050639, 15621144, 15622396, 15617508, 15620326, 15623234, 15623233, 15617127, 15620363, 16035872, 16035871, 15621527, 15621068, 16052909, 16052908, 15618697, 15618053, 15679462, 15679461, 15622821, 16083776, 16083775, 15618668, 15623535, 15623534, 15618254, 15622677, 16081945, 16081944, 15619231, 15620861, 15618871, 15620783, 15620024, 16071053, 16071052, 15617057, 16052575, 16052574, 15620098, 15619761, 15618942, 16035075, 16035074, 15619470, 16035503, 16035502, 15619955, 15623368, 15623367, 15617312, 16050674, 16050673, 15621606, 15620616, 16057366, 16057365, 15622571, 16035560, 16035559, 15619104, 16083714, 16083713, 15620646, 16050868, 16050867, 15617426, 15617413, 16035181, 16035180, 15618178, 15679470, 15679469, 15617423, 16065141, 16065140, 15618998, 16035449, 16035448, 15621531, 16050808, 16050807, 15622868, 16035894, 16035893, 15622585, 16034817, 16034816, 15622870, 15618347, 15616087, 15616002, 15621342, 15623398, 15623397, 15617890, 15617344, 16052861, 16052860, 15620258, 16034927, 16034926, 15622212, 15615790, 15615791, 15620822, 16070439, 16070438, 15620623, 16035097, 16035096, 15619571, 16065509, 16065508, 15619169, 16081861, 16081860, 15619676, 15623434, 15623433, 15618511, 16056767, 16056766, 15622688, 16051568, 16051567, 15618180, 15623404, 15623403, 15618034, 15617393, 15621396, 15621247, 16035886, 16035885, 15622630, 15619553, 16065920, 16065919, 15618194, 15623190, 15623547, 15621658, 16065846, 16065845, 15620893, 15623585, 15623584, 15621563, 15621303, 16052730, 16052729, 15622624, 16035955, 16035954, 15618339, 15617148, 16035049, 16035048, 15618620, 16065742, 16065741, 15619615, 16070511, 16070510, 15620974, 16043060, 16043059, 15621566, 15617712, 16057414, 16057413, 15617334, 16071089, 16071088, 15617510, 16052501, 16052500, 15617764, 15618248, 15679779, 15679778, 15622046, 16070995, 16070994, 15618519, 16056721, 16056720, 15618255, 15619083, 15621411, 15618987, 15620715, 16057057, 16057056, 15619389, 16035355, 16035354, 15622703, 15618830, 15624495, 15624494, 15618924, 16084063, 16084062, 15618491, 16034961, 16034960, 15619296, 15618168, 16035117, 16035116, 15619758, 15623118, 15623117, 15617326, 16057172, 16057171, 15619316, 16057069, 16057068, 15619330, 16052583, 16052582, 15620199, 15623515, 15623514, 15617386, 15619442, 15624535, 15624534, 15621831, 16071604, 16071603, 15620488, 15615958, 15615959, 15619264, 15617444, 15620829, 15623352, 15623351, 15617167, 16050474, 16050473, 15617498, 15624489, 15624488, 15618921, 15618957, 15622115, 16056747, 16056746, 15620125, 15620446, 16066187, 16066186, 15618849, 15622103, 16050858, 16050857, 15616996, 15616122, 15616123, 15617535, 16070379, 16070378, 15620833, 16035071, 16035070, 15619480, 16052961, 16052960, 15617156, 15620590, 16035095, 16035094, 15619503, 15618893, 16082072, 16082071, 15617299, 15619591, 16081865, 16081864, 15620003, 16034723, 16034722, 15622937, 16034957, 16034956, 15618978, 16065904, 16065903, 15617744, 16057065, 16057064, 15619367, 16082074, 16082073, 15617519, 15619991, 15618340, 15620161, 16065441, 16065440, 15618747, 15619016, 15618988, 16035794, 16035793, 15619434, 15617332, 15622296, 16065499, 16065498, 15619213, 15616932, 15619593, 16065622, 16065621, 15622893, 15621493, 15619989, 15621318, 15619735, 15618653, 15623621, 15623620, 15619918, 15621700, 16057330, 16057329, 15621485, 16035133, 16035132, 15620528, 15619821, 16035912, 16035911, 15616937, 15623438, 15623437, 15618237, 16065590, 16065589, 15622398, 15620798, 16082007, 16082006, 15621359, 15623042, 15623041, 15621607, 16051584, 16051583, 15618298, 16049750, 16049749, 15622208, 15622360, 15620252, 15618926, 15620026, 15623151, 15623150, 15618451, 15623000, 15622999, 15620741, 15620301, 16083802, 16083801, 15620811, 15620658, 15620652, 15621084, 16052794, 16052793, 15617950, 15623484, 15623483, 15617485, 15622112, 15618807, 15618909, 16066084, 16066083, 15622522, 15618842, 16071085, 16071084, 15617447, 16084102, 16084101, 15618782, 16035031, 16035030, 15619068, 15624521, 15624520, 15618139, 16052647, 16052646, 15621474, 15620678, 15623188, 15623187, 15620002, 16035842, 16035841, 15620755, 15620787, 16071101, 16071100, 15617586, 16082044, 16082043, 15622865, 16035597, 16035514, 15619868, 16035371, 16035370, 15622662, 15620784, 15619194, 15615918, 15615919, 15617464, 16066098, 16066097, 15622696, 16082054, 16082053, 15622924, 16035834, 16035833, 15620698, 15624569, 15624568, 15621749, 15616057, 15616058, 15620669, 16057442, 16057441, 15617469, 16050586, 16050585, 15617978, 15621950, 15621236, 15620548, 16051552, 16051551, 15617671, 16066133, 16066132, 15616995, 16084146, 16084145, 15620730, 16082078, 16082077, 15617827, 16070477, 16070476, 15619283, 15620637, 15619853, 15619584, 16066175, 16066174, 15618559, 15617095, 15622200, 16071063, 16071062, 15617171, 16032481, 16032480, 15619126, 16050814, 16050813, 15622883, 16035940, 16035939, 15617687, 16065844, 16065843, 15620923, 16057015, 16057014, 15619348, 15618483, 16050856, 16050855, 15616982, 15623282, 15623281, 15618216, 15623501, 15623500, 15617543, 16056757, 16056756, 15621007, 15615819, 15615820, 15622588, 15619099, 16043163, 16043162, 15620470, 16049640, 16049639, 15620078, 16065750, 16065748, 15619609, 15623619, 15623618, 15619927, 15619544, 15619854, 16034865, 16034864, 15617412, 16035814, 16035813, 15620430, 15621128, 16082100, 16082099, 15618407, 16057234, 16057233, 15620638, 15623386, 15623385, 15617793, 15617516, 16043006, 16043005, 15619500, 15619459, 15622884, 15618587, 15622527, 16066275, 16066274, 15622268, 16065824, 16065823, 15620901, 16081979, 16081978, 15620409, 15618433, 15620172, 16050494, 16050483, 15620229, 15621272, 15619817, 16071077, 16071076, 15617330, 16071610, 16071609, 15620793, 15622860, 15622829, 16065862, 16065861, 15618291, 16034713, 16034712, 15622478, 16065812, 16065811, 15620476, 15622733, 15620460, 16042994, 16042993, 15618496, 16083728, 16083727, 15621472, 15621123, 16035874, 16035873, 15621722, 15617714, 15617050, 15623426, 15623425, 15618413, 16050718, 16050717, 15621836, 15619858, 16083754, 16083753, 15617630, 15618454, 15617317, 15622358, 16057312, 16057311, 15621380, 16065537, 16065536, 15619820, 15619129, 16049636, 16049635, 15619707, 15619241, 15621957, 15618187, 16084164, 16084163, 15621192, 15618660, 16065425, 16065424, 15618935, 15618419, 16050879, 16050878, 15619781, 16035037, 16035036, 15618715, 15623224, 15623223, 15617119, 15618806, 15619836, 15620609, 16065650, 16065649, 15617282, 15620456, 16066197, 16066196, 15619111, 16035471, 16035470, 15617082, 15615823, 15615824, 15622694, 15617903, 16035653, 16035652, 15620534, 16035373, 16035372, 15622666, 15619295, 16043038, 16043037, 15620841, 16083718, 16083717, 15620917, 16035079, 16035078, 15619466, 15620052, 16057346, 16057345, 15622556, 15617424, 15620725, 15620159, 16057111, 16057110, 15619323, 15618851, 15679452, 15679451, 15621814, 16035922, 16035921, 15617321, 16066273, 16066272, 15621955, 15617360, 15623370, 15623369, 15617343, 15622132, 16043133, 16043132, 15618916, 16052601, 16052600, 15620493, 15616120, 15616121, 15617333, 15621639, 15619824, 16052892, 16052891, 15618086, 16071649, 16071648, 15622346, 15620611, 15623334, 15623333, 15616981, 15622511, 16065171, 16065170, 15619785, 16050448, 16050447, 15622416, 16071029, 16071028, 15622844, 15621388, 16043137, 16043136, 15619524, 16052627, 16052626, 15621023, 16043098, 16043097, 15620969, 15615728, 15615729, 15620090, 16035570, 16035569, 15619428, 16035649, 16035648, 15620499, 15619641, 16052541, 16052540, 15618020, 15618852, 16052814, 16052813, 15618314, 16034863, 16034862, 15617474, 15619681, 15617241, 15622552, 15623274, 15623273, 15618210, 16043076, 16043075, 15621385, 15618875, 15623076, 15623075, 15622215, 15622463, 16035179, 16035178, 15618282, 15621208, 15618845, 15621956, 15622373, 15619787, 16081839, 16081838, 15617899, 16035413, 16035412, 15617787, 16042855, 16042854, 15619538, 16071632, 16071631, 15621792, 16043014, 16043013, 15620287, 16042923, 16042922, 15618061, 15623278, 15623277, 15618214, 16084180, 16084179, 15621415, 15620244, 16035441, 16035440, 15621100, 15619005, 15617581, 15618606, 16057450, 16057449, 15617455, 15620278, 15679478, 15679477, 15618001, 16065945, 16065944, 15621816, 15621934, 15623649, 15623648, 15619657, 16052724, 16052723, 15622459, 15620774, 16066155, 16066154, 15618478, 15620655, 16052744, 16052743, 15616923, 15620876, 15620384, 15616908, 15621222, 15623332, 15623331, 15616924, 16071590, 16071589, 15620027, 16071145, 16071144, 15617851, 15619770, 16057264, 16057263, 15622451, 16049794, 16049793, 15622535, 16052611, 16052610, 15620630, 15617906, 15623416, 15623415, 15618151, 16050460, 16050459, 15616930, 15622488, 15621477, 15620651, 15620053, 16065804, 16065803, 15620108, 16034897, 16034896, 15618118, 16052525, 16052524, 15620414, 16050778, 16050777, 15622543, 16034825, 16034824, 15622374, 15618345, 15622897, 15619092, 16065437, 16065436, 15618911, 15619252, 16056974, 16056973, 15622401, 16050832, 16050831, 15617378, 15618657, 16032475, 16032474, 15620327, 15622422, 16057025, 16057024, 15619382, 16081831, 16081830, 15617195, 15616944, 15622541, 15620667, 15623340, 15623339, 15616979, 15620298, 16056717, 16056716, 15618366, 16065888, 16065887, 15617074, 16050566, 16050565, 15618036, 15621920, 16049704, 16049703, 15621392, 15621663, 16035330, 16035329, 15622678, 15617310, 16065517, 16065516, 15619319, 15623250, 15623249, 15617258, 16050526, 16050525, 15620218, 15618134, 16083946, 16083945, 15620017, 16071693, 16071692, 15617898, 16052621, 16052620, 15620846, 16070361, 16070360, 15617214, 16065986, 16065985, 15621720, 16049630, 16049629, 15618879, 16035601, 16035600, 15619880, 16052843, 16052842, 15618466, 15618179, 15618126, 16034787, 16034786, 15619766, 16065984, 16065983, 15620720, 15623460, 15623459, 15618239, 15620681, 15617799, 15623258, 15623257, 15617288, 15624493, 15624492, 15618644, 15618817, 16049662, 16049661, 15621777, 15617837, 15618787, 16056753, 16056752, 15620487, 16066163, 16066162, 15618152, 15622609, 15615773, 15615774, 15619946, 15621044, 16082046, 16082045, 15622831, 15623256, 15623255, 15617287, 15615906, 15615907, 15622943, 16034793, 16034792, 15618356, 15617781, 15617867, 16057354, 16057353, 15622564, 16065600, 16065599, 15622595, 16065749, 16065747, 15619585, 15618012, 15620018, 16052467, 16052466, 15618864, 15623070, 15623069, 15621972, 16057097, 16057096, 15619346, 15619997, 16083826, 16083825, 15622581, 16057101, 16057100, 15619388, 15623617, 15623616, 15620112, 16065988, 16065987, 15617024, 15622169, 15615988, 15615989, 15620366, 15616013, 15616014, 15618814, 15713592, 15713591, 15619293, 16042859, 16042858, 15619710, 16034949, 16034948, 15618687, 15617063, 15617005, 16065974, 16065973, 15622307, 15618617, 16083950, 16083949, 15620619, 16035703, 16035702, 15621162, 15618127, 15618618, 16043153, 16043152, 15620020, 15617342, 16035379, 16035378, 15622667, 15622141, 16043062, 16043061, 15621516, 15623078, 15623077, 15622261, 16052595, 16052594, 15620458, 15619097, 16052849, 16052848, 15618488, 15623147, 15623146, 15618426, 16049792, 16049791, 15622189, 15617496, 15621847, 15622575, 15618607, 16035812, 16035811, 15620141, 16070401, 16070400, 15619825, 15620273, 16043090, 16043089, 15620656, 15619664, 16071051, 16071050, 15617056, 16065996, 16065995, 15619167, 16057280, 16057279, 15621257, 15617604, 15619567, 15622949, 16034963, 16034962, 15619458, 15623014, 15623013, 15621387, 15622483, 16083962, 16083961, 15621184, 16081811, 16081810, 15621765, 16035081, 16035080, 15619282, 16065886, 16065885, 15616960, 15621967, 16065540, 16065539, 15622306, 15623220, 15623219, 15617117, 15617821, 16057214, 16057213, 15620237, 15622147, 15620343, 15617777, 15622858, 16035534, 16035533, 15618646, 15623563, 15623562, 15621912, 16050436, 16050435, 15621441, 16051534, 16051533, 15617700, 15617878, 15620093, 15619624, 16066231, 16066230, 15620569, 15618530, 16071634, 16071633, 15621911, 16052519, 16052518, 15620415, 16035163, 16035162, 15617398, 15620047, 15620769, 16050804, 16050803, 15622434, 15618898, 15618029, 15622913, 16035902, 16035901, 15622754, 15618684, 15617322, 15619396, 16035047, 16035046, 15618764, 16043048, 16043047, 15620867, 16035111, 16035110, 15619802, 16052833, 16052832, 15618342, 16084120, 16084119, 15617518, 16070425, 16070424, 15618474, 16070359, 16070358, 15617100, 16034869, 16034868, 15617472, 15623553, 15623552, 15621764, 16057464, 16057463, 15617780, 16035914, 16035913, 15616955, 15622100, 16066245, 16066244, 15618635, 16035878, 16035877, 15621876, 15620075, 15616029, 15616030, 15619542, 15618847, 15619267, 15620364, 16050806, 16050805, 15615241, 16050913, 16050912, 15620095, 15618780, 15615879, 15615880, 15621020, 16056749, 16056748, 15620371, 16052551, 16052550, 15622197, 15615796, 15615797, 15621817, 15622105, 16050802, 16050801, 15622399, 15619693, 16070971, 16070970, 15618460, 16082056, 16082055, 15622948, 15620871, 16050870, 16050869, 15617522, 15622438, 16052917, 16052916, 15619795, 16084152, 16084151, 15620881, 16065394, 16065393, 15617546, 15617550, 15621455, 15622876, 16034746, 16034745, 15621542, 15619237, 15617215, 16056771, 16056770, 15617247, 16065390, 16065389, 15617377, 15621840, 16052481, 16052480, 15617548, 16043100, 16043099, 15620991, 16065924, 16065923, 15621502, 16070351, 16070350, 15622888, 15616096, 15616097, 15621787, 16035588, 16035587, 15619637, 15619292, 16065930, 16065929, 15621504, 15622322, 15622768, 16083490, 16083489, 15622833, 15615887, 15615888, 15621352, 15622219, 16050516, 16050515, 15620213, 16056727, 16056726, 15618338, 16065507, 16065506, 15619040, 15621290, 15623066, 15623065, 15621941, 16035183, 16035182, 15618175, 16050712, 16050711, 15621609, 15621052, 16065513, 16065512, 15619513, 15618670, 15615942, 15615943, 15618586, 16050491, 16050490, 15620228, 15620000, 16050514, 16050513, 15620362, 15615877, 15615878, 15620801, 16035659, 16035658, 15620608, 15621435, 15617634, 16035663, 16035662, 15620663, 15619751, 15623186, 15623185, 15618444, 15620640, 16050738, 16050737, 15622081, 15620313, 16034831, 16034830, 15622412, 16051582, 16051581, 15618293, 15616140, 15616141, 15618096, 15617820, 16081902, 16081901, 15618323, 16035884, 16035883, 15622082, 16051532, 16051531, 15617682, 16052633, 16052632, 15621151, 16070383, 16070382, 15621872, 15615821, 15615822, 15622596, 15618889, 15622426, 16071181, 16071180, 15618131, 16082030, 16082029, 15621752, 15621774, 16066179, 16066178, 15619640, 16052762, 16052761, 15617243, 16057418, 16057417, 15617366, 15616910, 16050616, 16050615, 15621171, 16057218, 16057217, 15620279, 15620662, 16035953, 16035952, 15618263, 15615716, 15615717, 15618658, 15617010, 15618557, 15618450, 15619876, 15618960, 15618885, 16081881, 16081880, 15621136, 16042837, 16042836, 15619055, 15620176, 16035121, 16035120, 15620149, 16066183, 16066182, 15618773, 16035731, 16035730, 15619725, 15620238, 15620873, 15623631, 15623630, 15620951, 15617220, 15617201, 16081941, 16081996, 15621002, 15620030, 15616090, 15616091, 15621464, 15619694, 15617974, 16035896, 16035895, 15622592, 16043082, 16043081, 15618892, 15617188, 16057284, 16057283, 15621261, 15623143, 15623142, 15617959, 15620312, 15619521, 15620073, 16057224, 16057223, 15620282, 15617102, 16065535, 16065534, 15619709, 16035957, 16035956, 15618500, 15622589, 16065820, 16065819, 15620931, 15619728, 16070975, 16070974, 15618462, 16057103, 16057102, 15619426, 16065972, 16065971, 15622297, 15623424, 15623423, 15618412, 15622499, 15615754, 15615755, 15617390, 16034756, 16034755, 15621743, 15617256, 15615803, 15615804, 15622771, 16057023, 16057022, 15619351, 16052645, 16052644, 15621491, 16050446, 16050445, 15621859, 16052823, 16052822, 15618334, 15618955, 15619177, 15679785, 15679784, 15622746, 15623300, 15623299, 15618269, 16035544, 16035543, 15618802, 16084150, 16084149, 15620269, 16071045, 16071044, 15616975, 15621659, 15620764, 16065774, 16065773, 15619403, 15621338, 16070315, 16070314, 15620394, 15679484, 15679483, 15620843, 15623517, 15623516, 15618243, 15619738, 15618808, 16084000, 16083999, 15619648, 16034987, 16034986, 15619872, 16081805, 16081804, 15620591, 16035800, 16035799, 15619856, 15620874, 15623380, 15623379, 15617538, 16056968, 16056967, 15622155, 15621562, 15622857, 15622737, 15618652, 16066021, 16066020, 15620732, 16035949, 16035948, 15618120, 16057238, 16057237, 15621027, 16083712, 16083711, 15620644, 16050518, 16050517, 15620210, 15620059, 16071183, 16071182, 15618196, 15618640, 15623639, 15623638, 15621540, 15623396, 15623395, 15617891, 15618827, 16052796, 16052795, 15618092, 15619724, 16071673, 16071672, 15617085, 15619613, 15616079, 15616080, 15621095, 16083736, 16083735, 15622734, 15622379, 15619279, 16052553, 16052552, 15622447, 15617613, 16035735, 16035734, 15620491, 15622187, 16083739, 16083705, 15617139, 15620240, 16057324, 16057323, 15621077, 15620137, 15616990, 16082020, 16082019, 15621665, 16034993, 16034992, 15620162, 15621715, 15616132, 15616133, 15617914, 16057490, 16057489, 15620875, 15621279, 15623266, 15623265, 15617294, 16057370, 16057369, 15622615, 15618857, 15615780, 15615781, 15620599, 16066167, 16066166, 15618481, 16065776, 16065775, 15619106, 15620406, 15619416, 16051586, 16051585, 15618189, 16052635, 16052634, 15621190, 16070320, 16070319, 15620802, 15621966, 16032457, 16032456, 15619273, 15621341, 15615944, 15615945, 15618625, 15619034, 15622869, 15619630, 15615845, 15615846, 15617788, 15621514, 16035747, 16035746, 15622177, 15623390, 15623389, 15617795, 16035790, 16035789, 15619298, 15619075, 16052925, 16052924, 15620123, 16034965, 16034964, 15619457, 15618005, 16071201, 16071200, 15618318, 16035830, 16035829, 15620689, 16057048, 16057047, 15619345, 16070983, 16070982, 15618513, 15620443, 16083932, 16083931, 15619839, 16052490, 16052489, 15622761, 16084057, 16084056, 15621402, 16052740, 16052739, 15622854, 16035699, 16035698, 15621149, 15622273, 16065959, 16065958, 15622013, 15622051, 16052853, 16052852, 15618539, 16052931, 16052930, 15620374, 16057314, 16057313, 15621447, 15619289, 16081873, 16081872, 15620700, 15622126, 15622914, 15615256, 15615257, 15619045, 16083794, 16083793, 15620339, 15619415, 15620612, 15617536, 15621691, 15623006, 15623005, 15616929, 15618637, 16065758, 16065757, 15619110, 15618321, 15618985, 15622849, 16070437, 16070436, 15620546, 16065782, 16065781, 15619768, 15619131, 15615817, 15615818, 15622508, 15621806, 15620368, 15620347, 16032483, 16032482, 15619117, 15622825, 15617468, 15623180, 15623179, 15617870, 15619236, 15622274, 15615963, 15615964, 15619535, 15620738, 16050764, 16050763, 15622377, 15622094, 15615276, 15615277, 15619082, 16035501, 16035500, 15619931, 16057408, 16057407, 15617335, 15617970, 15679464, 15679463, 15622908, 15623162, 15623161, 15622225, 15618647, 16035556, 16035555, 15619087, 16043126, 16043125, 15622233, 16065856, 16065855, 15620992, 15679482, 15679481, 15618166, 16057156, 16057155, 15619308, 15622418, 15616126, 15616127, 15617572, 16081825, 16081824, 15622291, 16065147, 16065146, 15618861, 15621475, 16035619, 16035618, 15620223, 16052665, 16052664, 15621701, 15623172, 15623171, 15622911, 15679783, 15679782, 15622279, 15622006, 15618666, 15618312, 16052887, 16052886, 15622524, 15622614, 15620926, 16057348, 16057347, 15622559, 16049736, 16049735, 15617369, 16034758, 16034757, 15618360, 16050644, 16050643, 15621135, 16035153, 16035152, 15622755, 16065586, 16065585, 15622353, 16065896, 16065895, 15617315, 16035725, 16035724, 15620573, 16057067, 16057066, 15619384, 15623651, 15623650, 15619932, 15620938, 15620246, 15620661, 16065576, 16065575, 15622331, 16057178, 16057177, 15619307, 16081940, 16082012, 15621852, 15622214, 15623260, 15623259, 15617291, 15618701, 16081869, 16081868, 15620373, 15619792, 16050676, 16050675, 15621611, 16070471, 16070470, 15618763, 15617741, 15615938, 15615939, 15619688, 15618188, 15617723, 15622423, 15618565, 15624537, 15624536, 15618383, 16070521, 16070520, 15618191, 16081843, 16081842, 15618330, 15622727, 15615853, 15615854, 15620870, 16052589, 16052588, 15620270, 15621589, 15619438, 16052704, 16052703, 15622248, 16071095, 16071094, 15617512, 16057204, 16057203, 15620066, 16052766, 16052765, 15617346, 16084132, 16084131, 15620077, 16034768, 16034767, 15616985, 16071191, 16071190, 15618200, 16057158, 16057157, 15619327, 15615871, 15615872, 15619894, 16070493, 16070492, 15619647, 16035297, 16035296, 15617514, 16035959, 16035958, 15618439, 16043064, 16043063, 15622122, 15618951, 16083816, 16083815, 15621856, 15618641, 16052716, 16052715, 15622443, 16056763, 16056762, 15622131, 16049788, 16049787, 15621628, 16035962, 16035963, 15618713, 16034801, 16034800, 15617629, 15615875, 15615876, 15620750, 15619421, 16034895, 16034894, 15617997, 15620331, 15622375, 16070977, 16070976, 15618463, 15622449, 16050442, 16050441, 15621652, 16052778, 16052777, 15617565, 15619841, 16050887, 16050886, 15622066, 16052873, 16052872, 15620856, 15615885, 15615886, 15621210, 15618568, 16034857, 16034856, 15617160, 16057011, 16057010, 15619317, 16035691, 16035690, 15621078, 15619576};
	for(int i = 0; i < deli_pos_index; ++i)
		recoverRecord(delimiter, position_file, index_file, C, writeSize, alignment, deli_pos_list[i], bwt_encoded_file, lastIndex, getOCC_aid, bwtBuffer);








	// for(int poIndex = 2 * alignment * startRec; poIndex <= 2 * alignment * endRec; poIndex += 2 * alignment){
	// 	fromFile.seekg(poIndex + alignment, ios::beg);
	// 	// delimiterNumBuffer = new unsigned char[alignment];
	// 	// fromFile.read((char*)(&delimiterNumBuffer[0]), alignment);
	// 	// int curdeliNum = 0;
	// 	// for(int i = 0; i < alignment; ++i){
	// 	// 	curdeliNum = curdeliNum | delimiterNumBuffer[i];
	// 	// 	if(i != alignment - 1)
	// 	// 		curdeliNum = curdeliNum << 8;
	// 	// }
	// 	// delete[] delimiterNumBuffer;


	// 	delimiterNumBuffer = new unsigned char[alignment];
	// 	fromFile.read((char*)(&delimiterNumBuffer[0]), alignment);
	// 	int curdePos = 0;
	// 	for(int i = 0; i < alignment; ++i){
	// 		curdePos = curdePos << 8;
	// 		curdePos = curdePos | delimiterNumBuffer[i];
	// 		// if(i != alignment - 1)
	// 		// 	curdePos = curdePos << 8;
	// 	}
	// 	delete[] delimiterNumBuffer;

	// 	cout << "curdePos :" << curdePos << "\n";


	// 	// cout << ">>>>> curdeliNum:" << curdeliNum << "  curdePos: " << curdePos << "\n";
	// 	// string res = recoverRecord(delimiter, position_file, index_file, C, writeSize, alignment, curdePos, bwt_encoded_file, lastIndex, getOCC_aid);
	// 	//string res = "test";
	// 	// cout << "start recoverRecord:\n";
	// 	recoverRecord(delimiter, position_file, index_file, C, writeSize, alignment, curdePos, bwt_encoded_file, lastIndex, getOCC_aid, bwtBuffer);
	// 	// cout << res << "\n";

	// }


}

void searchWithWrite(char delimiter, char* bwt_encoded_file, char* index_folder, char* mode, char* queryString, int* C, int writeSize, int alignment, int lastIndex, int* charList, int charNb, int* getOCC_aid){

	cout << "writeSize: " << writeSize << "\n";
	cout << "alignment: " << alignment << "\n";
	int inIndex = 0;

	int count[128] = {0};
	int fileSize;
	ifstream fromFile(bwt_encoded_file, ios::binary);
	fromFile.seekg(0, ios::end);

	fileSize = fromFile.tellg();

	fromFile.seekg(0, ios::beg);

	int readBlockSize = 1000000;

	char* readBuffer = new char[readBlockSize];


	string index_file = string(index_folder) + "/" + string(bwt_encoded_file) + "searchIndexFile.aux";
	string position_file = string(bwt_encoded_file) + "positionFile.aux";


	while(fromFile.read(readBuffer, readBlockSize)){
		for(int i = 0; i < readBlockSize; ++i){

			if(inIndex == lastIndex){
				count[0] += 1;
				if((inIndex + 1) % writeSize == 0){
					//write here
					writeIndexFile(count, index_file, alignment, writeSize, inIndex, charList, charNb);
				}
				++inIndex;
				i -= 1;
				continue;
			}

			count[readBuffer[i] - char(0)] += 1;
			if((inIndex + 1) % writeSize == 0){
				//write here
				writeIndexFile(count, index_file, alignment, writeSize, inIndex, charList, charNb);
			}
			++inIndex;
		}
	}

	for(int i = 0; i < fromFile.gcount(); ++i){
		if(inIndex == lastIndex){
			count[0] += 1;
			if((inIndex + 1) % writeSize == 0){
				//write here
				writeIndexFile(count, index_file, alignment, writeSize, inIndex, charList, charNb);
			}
			++inIndex;
			i -= 1;
			continue;
		}

		count[readBuffer[i] - char(0)] += 1;
		if((inIndex + 1) % writeSize == 0){
			//write here
			writeIndexFile(count, index_file, alignment, writeSize, inIndex, charList, charNb);
		}
		++inIndex;
	}


	if(strcmp(mode, "-m") == 0){

		searchMode_M(index_file, alignment, writeSize, queryString, C, bwt_encoded_file, lastIndex, getOCC_aid);
	}

	else if(strcmp(mode , "-n") == 0 || strcmp(mode, "-a") == 0){

		searchMode_N(position_file, index_file, alignment, writeSize, queryString, C, bwt_encoded_file, mode, lastIndex, getOCC_aid);
	}

	else if(strcmp(mode, "-i") == 0){
		int startRec, endRec;

		sscanf(queryString, "%d %d", &startRec, &endRec);
		
		searchMode_I(delimiter, bwt_encoded_file, position_file, index_file, C, writeSize, alignment, startRec, endRec, fileSize, lastIndex, getOCC_aid);
	}
	return;
}


void searchWithoutWriteWithRead(char delimiter, char* bwt_encoded_file, char* index_folder, char* mode, char* queryString, int* C, int writeSize, int alignment, int lastIndex, int* getOCC_aid){

	string index_file = string(index_folder) + "/" + string(bwt_encoded_file) + "searchIndexFile.aux";
	string position_file = string(bwt_encoded_file) + "positionFile.aux";

	if(strcmp(mode, "-m") == 0){

		searchMode_M(index_file, alignment, writeSize, queryString, C, bwt_encoded_file, lastIndex, getOCC_aid);
	}

	else if(strcmp(mode , "-n") == 0 || strcmp(mode, "-a") == 0){

		searchMode_N(position_file, index_file, alignment, writeSize, queryString, C, bwt_encoded_file, mode, lastIndex, getOCC_aid);
	}

	else if(strcmp(mode, "-i") == 0){
		int startRec, endRec;

		// ifstream bwtFile(bwt_encoded_file, ios::binary);
		// bwtFile.seekg(0, ios::end);
		// int fileSize = bwtFile.tellg();
		// bwtFile.close();
		int fileSize = 0;
		sscanf(queryString, "%d %d", &startRec, &endRec);

		// cout << "then here\n";

		searchMode_I(delimiter, bwt_encoded_file, position_file, index_file, C, writeSize, alignment, startRec, endRec, fileSize, lastIndex, getOCC_aid);
	}
	return;
}



int main(int argc, char *argv[]){

	char* arg_delimiter = argv[1];
	char* bwt_encoded_file = argv[2];
	char* index_folder = argv[3];
	char* mode = argv[4];
	char* queryString = argv[5];


	char delimiter;
	int count[128] = {0};
	int C[128] = {0};



	string index_file = string(index_folder) + "/" + string(bwt_encoded_file) + "searchIndexFile.aux";
	string position_file = string(bwt_encoded_file) + "positionFile.aux";


	if(strlen(arg_delimiter) == 2)
		delimiter = char(10);

	else
		delimiter = char(arg_delimiter[0] - char(0));


	int fileSize;

	ifstream fromFile(bwt_encoded_file, ios::binary);
	fromFile.seekg(0, ios::end);

	fileSize = fromFile.tellg();

	fromFile.seekg(0, ios::beg);


	int alignment, charNb = 0;

	if(fileSize < LIMIT_1)
		alignment = 1;
	else if(fileSize >= LIMIT_1 && fileSize < LIMIT_2)
		alignment = 2;
	
	else if(fileSize >= LIMIT_2 && fileSize <= LIMIT_3)
		alignment = 3;
	else
		alignment = 4;



	int lastIndex = 0;
	unsigned char* lastIndexBuffer = new unsigned char[alignment];
	ifstream tempFile(position_file, ios::binary);
	tempFile.read((char*)(&lastIndexBuffer[0]), alignment);

	for(int i = 0; i < alignment; ++i){
		lastIndex = lastIndex | lastIndexBuffer[i];
		if(i != alignment - 1)
			lastIndex = lastIndex << 8;
	}

	tempFile.close();
	delete[] lastIndexBuffer;



	int readBlockSize = 1000000;

	char* readBuffer = new char[readBlockSize];

	while(fromFile.read(readBuffer, readBlockSize)){
		for(int i = 0; i < readBlockSize; ++i){

			count[readBuffer[i] - char(0)] += 1;
		}
	}

	for(int i = 0; i < fromFile.gcount(); ++i){
		count[readBuffer[i] - char(0)] += 1;
	}

	count[0] = 1;

	delete[] readBuffer;
	fromFile.close();

	for(int i = 1; i < 128; ++i){

		int cnt = 0;
		for(int j = 0; j < i; ++j){
			if(count[j] != 0)
				cnt += count[j];
		}

		C[i] = cnt;
	}


	for(int i = 0; i < 128; ++i){
		if(count[i] != 0)
			++charNb;

	}




	int* charList = new int[charNb];

	int charListIndex = 0;

	for(int i = 0; i < 128; ++i){
		if(count[i] != 0){
			charList[charListIndex] = i;
			++charListIndex;
		}
	}

	int* getOCC_aid = new int[128];

	for(int i = 0; i < 128; ++i){
		getOCC_aid[i] = -1;
	}

	for(int i = 0; i < charNb; ++i)
		getOCC_aid[charList[i]] = i;

	// for(int i = 0; i < 128; ++i)
	// 	cout << getOCC_aid[i]<< ", ";
	// cout << "\n";



	int oneRowSize = alignment + charNb * alignment;

	// cout << "charNb:  " << charNb << "\n";


	if(oneRowSize <= fileSize){

		if(!file_exist(index_file))
			searchWithWrite(delimiter, bwt_encoded_file, index_folder, mode, queryString, C, oneRowSize, alignment, lastIndex, charList, charNb, getOCC_aid);
		else{
			// cout << "here===\n";
			searchWithoutWriteWithRead(delimiter, bwt_encoded_file, index_folder, mode, queryString, C, oneRowSize, alignment, lastIndex, getOCC_aid);
		}
	}
	else{
		searchWithoutWriteWithRead(delimiter, bwt_encoded_file, index_folder, mode, queryString, C, oneRowSize, alignment, lastIndex, getOCC_aid);
	}

}