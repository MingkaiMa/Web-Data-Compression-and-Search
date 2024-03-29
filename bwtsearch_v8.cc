#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <map>
#include <string>

#define LIMIT_1 256
#define LIMIT_2 65536
#define LIMIT_3 16777216

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
		writeBuffer[writeIndex] = char(charList[i]);
		++writeIndex;
		writeBufferFunc(writeBuffer, writeIndex, count[charList[i]], alignment);

	}

	// for(int i = 0; i < 128; ++i){
	// 	if(count[i] == 0)
	// 		continue;


	// 	writeBuffer[writeIndex] = char(i);
	// 	++writeIndex;

	// 	// writeBufferFunc(writeBuffer, writeIndex, i, alignment);

	// 	cout << "i : " << i << "count[i]: " << count[i] << " alignment: " << alignment << "\n";

	// 	writeBufferFunc(writeBuffer, writeIndex, count[i], alignment);
	// }
	ofstream toFile(index_file, ios::out | ios::app | ios::binary);
	toFile.write((char*)(&writeBuffer[0]), writeIndex);
	delete[] writeBuffer;
	toFile.close();
}


int getOCC(char c, int position, string index_file, int alignment, int writeSize, char* bwt_encoded_file, int lastIndex){


	// cout << "comhhhh\n";
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

	// cout << "mayhere??\n";
	// cout << "position: " << position << " writeSize: " << writeSize << "\n";
	int readIndex = (position + 1) / writeSize;

	readIndex -= 1;

	readIndex = readIndex * writeSize;


	// cout << "index_file: " << index_file << "\n";
	// cout << "readIndex : " << readIndex << "\n";
	fromFile.seekg(readIndex, ios::beg);

	unsigned char* getBuffer = new unsigned char[writeSize];

	fromFile.read((char*)(&getBuffer[0]), writeSize);

	int rowNb = 0;

	int i;
	// cout << "alignment: " << alignment << "\n";
	for(i = 0; i < alignment; ++i){
		// cout << "i is: " << i << "\n";
		// cout << "getBuffer[i]: " << getBuffer[i] - char(0) << "\n";
		rowNb = rowNb | getBuffer[i];

		// cout << "rowNb:n  " << rowNb << "\n";

		if(i != alignment - 1)
			rowNb = rowNb << 8;

		// cout << "-rowNb: " << rowNb << "\n";
	}

	int nowCount = 0;

	i = alignment;

	while(i < writeSize){
		if(getBuffer[i] == c){

			for(int j = i + 1; j < i + alignment + 1; ++j){
				nowCount = nowCount | getBuffer[j];
				if(j != i + alignment)
					nowCount = nowCount << 8;
			}
			break;
		}
		i = i + alignment + 1;
	}

	delete[] getBuffer;
	fromFile.close();


	if(position >= lastIndex)
		position -= 1;

	if(rowNb >= lastIndex){
		rowNb -= 1;
	}

	int readSize = position - rowNb + 1;
	char* locateBuffer = new char[readSize];

	
	// if(rowNb >= lastIndex){
	// 	rowNb -= 1;
	// }


	fromFile2.seekg(rowNb, ios::beg);

	fromFile2.read(locateBuffer, readSize);

	for(int j = 1; j < readSize; ++j){

		if(locateBuffer[j] == c)
			++nowCount;
	}

	delete[] locateBuffer;
	fromFile2.close();
	return nowCount;
}


int getNextPosition(string index_file, string position_file, int position, int alignment, int writeSize, int* C, char* bwt_encoded_file, int lastIndex){

	if(position == lastIndex){
		char c = char(0);
		return C[c - char(0)] + getOCC(c, position, index_file, alignment, writeSize, bwt_encoded_file, lastIndex) - 1;
	}

	if(position < lastIndex){


		ifstream fromFile(bwt_encoded_file, ios::binary);

		fromFile.seekg(position, ios::beg);

		char* charBuffer = new char[1];
		fromFile.read(charBuffer, 1);

		char c = charBuffer[0];
		delete[] charBuffer;
		fromFile.close();
		return C[c - char(0)] + getOCC(c, position, index_file, alignment, writeSize, bwt_encoded_file, lastIndex) - 1;
	}

	if(position > lastIndex){


		ifstream fromFile(bwt_encoded_file, ios::binary);

		fromFile.seekg(position - 1, ios::beg);
		char* charBuffer = new char[1];
		fromFile.read(charBuffer, 1);

		char c = charBuffer[0];
		delete[] charBuffer;
		fromFile.close();
		return C[c - char(0)] + getOCC(c, position, index_file, alignment, writeSize, bwt_encoded_file, lastIndex) - 1;
	}

	return -1;

}



int getPosition(string position_file, string index_file, int position, int alignment, int writeSize, int* C, char* bwt_encoded_file, int lastIndex){

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


	// in position file, no suffix array is stored
	if(fromFile.peek() == EOF){

		int posCnt = 0;

		while(true){
			if(position == lastIndex){
				return posCnt;
			}
			++posCnt;
			position = getNextPosition(index_file, position_file, position, alignment, writeSize, C, bwt_encoded_file, lastIndex);
			// cout << "next: position" << position << "\n"; 
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
	// cout << "beginNb number: " << beginNb << "\n";

	fromFile.read((char*)(&delimiterNumBuffer[0]), alignment);
	int beginVal = 0;
	for(int i = 0; i < alignment; ++i){
		beginVal = beginVal | delimiterNumBuffer[i];
		if(i != alignment - 1)
			beginVal = beginVal << 8;
	}

	delete[] delimiterNumBuffer;

	//fromFile.seekg(alignment, ios::cur);


	// in position file, only one suffix array value is stored
	if(fromFile.peek() == EOF){
		int posCnt = 0;

		while(true){
			if(position == lastIndex)
				return posCnt;
			else if(position == beginNb){
				return posCnt + beginVal;
			}
			++posCnt;
			position = getNextPosition(index_file, position_file, position, alignment, writeSize, C, bwt_encoded_file, lastIndex);

		}
	}

	int beginEndIndex = fromFile.tellg();

	// cout << "beginEndIndex: " << beginEndIndex << "\n";


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

	// cout << "lastNb number: " << lastNb << "\n";


	fromFile.read((char*)(&delimiterNumBuffer[0]), alignment);
	int lastVal = 0;
	for(int i = 0; i < alignment; ++i){
		lastVal = lastVal | delimiterNumBuffer[i];
		if(i != alignment - 1)
			lastVal = lastVal << 8;
	}
	delete[] delimiterNumBuffer;

	// fromFile.seekg(alignment, ios::cur);

	int lastEndIndex = fromFile.tellg();

	// cout << "lastEndIndex: " << lastEndIndex << "\n";

	// cout << lastNb << ", " << beginNb << ", " << lastEndIndex << ", " << beginEndIndex << ",  " << alignment << "\n";

	int step = (lastNb - beginNb) / ((lastEndIndex - beginEndIndex) / (2 * alignment));

	// cout << "step is: " << step << "\n";

	// cout << "position is: " << position << "\n";

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
			// cout << "movebit: " << movebit << "\n";
			fromFile.seekg(2 * alignment + deliNum * (alignment * 2) + (movebit) * (2 * alignment) + alignment , ios::beg);
			delimiterNumBuffer = new unsigned char[alignment];
			fromFile.read((char*)(&delimiterNumBuffer[0]), alignment);
			int saValue = 0;
			for(int i = 0; i < alignment; ++i){
				saValue = saValue | delimiterNumBuffer[i];
				if(i != alignment - 1)
					saValue = saValue << 8;
			}
			// cout <<"sa value: " << saValue << "\n";
			delete[] delimiterNumBuffer;
			return saValue + posCnt;
		}
		++posCnt;
		// cout << "come here?????\n";
		position = getNextPosition(index_file, position_file, position, alignment, writeSize, C, bwt_encoded_file, lastIndex);
		// cout << "next position: " << position << "\n";
	}
	return -1;
}

void searchMode_M(string index_file, int alignment, int writeSize, const char* queryString, int* C, char* bwt_encoded_file, int lastIndex){

	// cout << "query is: " << queryString << "\n";
	int queryLength = strlen(queryString);

	int i = queryLength - 1;

	char c = queryString[i];

	int first = C[c - char(0)] + 1 - 1;
	int last = C[c - char(0) + 1] - 1;

	// cout << "first: " << first << "\n";
	// cout << "last: " << last << "\n";
	if(first > last){
		cout << 0 << "\n";
		return;
	}


	while(first <= last && i >= 1){
		c = queryString[i - 1];
		// cout << "c is: " << c << "\n";
		//char c, int position, string index_file, int alignment, int writeSize, char* bwt_encoded_file
		first = C[c - char(0)] + getOCC(c, first - 1, index_file, alignment, writeSize, bwt_encoded_file, lastIndex);
		// cout << "f: " << first << "\n";
		last =  C[c - char(0)] + getOCC(c, last,      index_file, alignment, writeSize, bwt_encoded_file, lastIndex) - 1;
		// cout << "l: " << last << "\n";
		--i;
	}

	// cout << "final first: " << first << "  last: " << last << "\n";
	cout << last - first + 1 << "\n";
}

void searchMode_N(string position_file, string index_file, int alignment, int writeSize, const char* queryString, int* C, char* bwt_encoded_file, char* mode, int lastIndex){

	// cout << "query is: " << queryString << "\n";
	// cout << "mode NNNNNNN\n";
	int queryLength = strlen(queryString);

	int i = queryLength - 1;

	char c = queryString[i];

	int first = C[c - char(0)] + 1 - 1;
	int last = C[c - char(0) + 1] - 1;

	// cout << "first: " << first << "\n";
	// cout << "last: " << last << "\n";
	if(first > last){
		cout << 0 << "\n";
		return;
	}


	while(first <= last && i >= 1){
		c = queryString[i - 1];
		// cout << "c is: " << c << "\n";
		//char c, int position, string index_file, int alignment, int writeSize, char* bwt_encoded_file
		first = C[c - char(0)] + getOCC(c, first - 1, index_file, alignment, writeSize, bwt_encoded_file, lastIndex);
		// cout << "f: " << first << "\n";
		last =  C[c - char(0)] + getOCC(c, last,      index_file, alignment, writeSize, bwt_encoded_file, lastIndex) - 1;
		// cout << "l: " << last << "\n";
		--i;
	}

	// cout << "final first: " << first << "  last: " << last << "\n";
	// cout << last - first + 1 << "\n";


	ifstream bwtFile(bwt_encoded_file, ios::binary);
	bwtFile.seekg(0, ios::end);
	int fileSize = bwtFile.tellg();
	bwtFile.close();


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

	// cout << "delimiter number is: " << deliNum << "\n";

	if(deliNum == 1){
		cout << 1 << "\n";
		return;
	}

	char* flagBits = new char[deliNum];
	memset(flagBits, '0', deliNum);
	int flagBitsIndex = deliNum - 1;

	int readIndex;

	for(int i = first; i <= last; ++i){
		int saValue = getPosition(position_file, index_file, i, alignment, writeSize, C, bwt_encoded_file, lastIndex);
		if(saValue == 0){
			saValue = fileSize;
		}
		// cout << "i: " << i << " suffix array value: " << saValue << "\n";
		flagBitsIndex = deliNum - 1;
		readIndex = 2 * alignment + alignment * (deliNum - 2);
		while(true){
			fromFile.seekg(readIndex, ios::beg);
			delimiterNumBuffer = new unsigned char[alignment];
			fromFile.read((char*)(&delimiterNumBuffer[0]), alignment);
			int curdeliNum = 0;
			for(int i = 0; i < alignment; ++i){
				curdeliNum = curdeliNum | delimiterNumBuffer[i];
				if(i != alignment - 1)
					curdeliNum = curdeliNum << 8;
			}
			delete[] delimiterNumBuffer;

			// cout << "***** curdeliNum: " << curdeliNum << "\n";

			if(saValue > curdeliNum && saValue != fileSize){
				flagBits[flagBitsIndex] = '1';
				break;
			}
			// cout << "****** readIndex: " << readIndex << "\n";
			if(readIndex == 2 * alignment || saValue == fileSize){
				flagBits[0] = '1';
				break;
			}
			--flagBitsIndex;
			readIndex -= alignment;

		}
	}

	// for(int i = 0; i < deliNum; ++i)
	// 	cout << flagBits[i] << ", ";
	// cout << "\n";

	

	if(strcmp(mode, "-n") == 0){
		cout << "n mode:\n";
		int ans = 0;

		for(int i = 0; i < deliNum; ++i){
			if(flagBits[i] == '1'){
				++ans;
			}
		}

		cout << ans << "\n";
		return;
	}


	cout << "a mode:\n";
	for(int i = 0; i < deliNum; ++i){
		if(flagBits[i] == '1')
			cout << i + 1 << "\n";
	}
}

//getNextPosition(string index_file, string position_file, int position, int alignment, int writeSize, int* C, char* bwt_encoded_file)
string recoverRecord(char delimiter, string position_file, string index_file, int* C, int writeSize, int alignment, int position, char* bwt_encoded_file, int lastIndex){

	char c;
	char* charBuffer;
	string res = "";
	ifstream fromFile(bwt_encoded_file, ios::binary);

	int nextPosition = getNextPosition(index_file, position_file, position, alignment, writeSize, C, bwt_encoded_file, lastIndex);

	// cout << "next Position:: " << nextPosition << "\n";


	if(nextPosition > lastIndex){
		int tempnextPosition = nextPosition - 1;



		fromFile.seekg(tempnextPosition, ios::beg);

		charBuffer = new char[1];
		fromFile.read(charBuffer, 1);

		c = charBuffer[0];

		// cout << "next char: " << c << "\n";

		res.push_back(c);
		delete[] charBuffer;
	}
	else if(nextPosition < lastIndex){


		fromFile.seekg(nextPosition, ios::beg);

		charBuffer = new char[1];
		fromFile.read(charBuffer, 1);

		c = charBuffer[0];

		// cout << "next char: " << c << "\n";

		res.push_back(c);
		delete[] charBuffer;
	}
	else
		c = 0;



	int tempnextPosition;
	while(c != delimiter){
		nextPosition = getNextPosition(index_file, position_file, nextPosition, alignment, writeSize, C, bwt_encoded_file, lastIndex);

		// cout << "here nextPosition: " << nextPosition << "\n";
		if(nextPosition > lastIndex){
			tempnextPosition = nextPosition - 1;
		}
		else
			tempnextPosition = nextPosition;

		if(nextPosition == lastIndex)
			continue;



		fromFile.seekg(tempnextPosition, ios::beg);
		charBuffer = new char[1];
		fromFile.read(charBuffer, 1);
		c = charBuffer[0];
		// cout << "here c is: " << c << "\n";
		if(c == delimiter)
			break;
		res.push_back(c);
	}
	
	reverse(res.begin(), res.end());
	// cout <<"[][][][]]]"<< res << "\n";
	return res;
}

void searchMode_I(char delimiter, char* bwt_encoded_file, string position_file, string index_file, int* C, int writeSize, int alignment, int startRec, int endRec, int fileSize, int lastIndex){

	// int bwtEncodedBufferSize = 100000;
	// char* bwtEncodedBuffer = new char[bwtEncodedBufferSize];
	// ifstream toFile(bwt_encoded_file, ios::binary);
	// int fileIndex = 0;


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

	// cout << "delimiter number is: " << deliNum << "\n";

	// map<int, string> Map;


	// get delimiter index from position file
	//for(int poIndex = alignment + alignment * startRec; poIndex <= alignment + alignment * endRec; poIndex += alignment){

	for(int poIndex = 2 * alignment * startRec; poIndex <= 2 * alignment * endRec; poIndex += 2 * alignment){
		fromFile.seekg(poIndex, ios::beg);
		delimiterNumBuffer = new unsigned char[alignment];
		fromFile.read((char*)(&delimiterNumBuffer[0]), alignment);
		int curdeliNum = 0;
		for(int i = 0; i < alignment; ++i){
			curdeliNum = curdeliNum | delimiterNumBuffer[i];
			if(i != alignment - 1)
				curdeliNum = curdeliNum << 8;
		}
		delete[] delimiterNumBuffer;
		// cout << "=====curdeliNum: " << curdeliNum << "\n";

		delimiterNumBuffer = new unsigned char[alignment];
		fromFile.read((char*)(&delimiterNumBuffer[0]), alignment);
		int curdePos = 0;
		for(int i = 0; i < alignment; ++i){
			curdePos = curdePos | delimiterNumBuffer[i];
			if(i != alignment - 1)
				curdePos = curdePos << 8;
		}
		delete[] delimiterNumBuffer;

		// cout << "=====curdePos: " << curdePos << "\n";

		string res = recoverRecord(delimiter, position_file, index_file, C, writeSize, alignment, curdePos, bwt_encoded_file, lastIndex);

		cout << res << "\n";
		// Map[curdeliNum] = "";
	}

	// cout << "Empty Map: \n";
	// for(auto it = Map.begin(); it != Map.end(); ++it){
	// 	cout << it->first << "\n";
	// }

	// cout << "delimiter row nb: \n";
	// cout << "file size: " << fileSize << "\n";

	// bool setFlag = false;
	// while(toFile.read(bwtEncodedBuffer, bwtEncodedBufferSize)){
	// 	for(int i = 0; i < bwtEncodedBufferSize; ++i){

	// 		if(fileIndex >= lastIndex && setFlag == false){
	// 			fileIndex += 1;
	// 			setFlag = true;
	// 		}



	// 		if(bwtEncodedBuffer[i] == delimiter){
	// 			int saValue = getPosition(position_file, index_file, fileIndex, alignment, writeSize, C, bwt_encoded_file, lastIndex);
	// 			// cout << fileIndex << "\n";
				
	// 			if(saValue == 0){
	// 				saValue = fileSize;
	// 			}
	// 			cout << "saValue: " << saValue << "\n";
	// 			if(Map.count(saValue - 1) > 0){
	// 				// cout << "YES\n";
	// 				// cout << "fileIndex: " << fileIndex << "\n";
	// 				string res = recoverRecord(delimiter, position_file, index_file, C, writeSize, alignment, fileIndex, bwt_encoded_file, lastIndex);
	// 				// cout << res << "\n";
	// 				Map[saValue - 1] = res;
	// 			}
	// 		}
	// 		++fileIndex;
	// 	}
	// }

	// for(int i = 0; i < toFile.gcount(); ++i){
	// 	// cout << "*\n";
	// 	if(bwtEncodedBuffer[i] == delimiter){

	// 			if(fileIndex >= lastIndex  && setFlag == false){
	// 				fileIndex += 1;
	// 				setFlag = true;
	// 			}
				
	// 			int saValue = getPosition(position_file, index_file, fileIndex, alignment, writeSize, C, bwt_encoded_file, lastIndex);
	// 			// cout << "fileIndex: "<< fileIndex << "\n";


	// 			if(saValue == 0){
	// 				saValue = fileSize;
	// 			}
	// 			// cout << "saValue: " << saValue  << "\n";
	// 			if(Map.count(saValue - 1) > 0){
	// 				// cout << "YES\n";
	// 				// cout << "fileIndex: " << fileIndex << "\n";
	// 				string res = recoverRecord(delimiter, position_file, index_file, C, writeSize, alignment, fileIndex, bwt_encoded_file, lastIndex);
	// 				// cout << res << "\n";
	// 				// cout << "saValue - 1: " << saValue - 1 << "\n";
	// 				Map[saValue - 1] = res;
	// 			}
	// 	}
	// 	++fileIndex;
	// }
	// // cout << "final res:\n";
	// for(auto it = Map.begin(); it != Map.end(); ++it){
	// 	cout << it->second << "\n";
	// }

}

void searchWithWrite(char delimiter, char* bwt_encoded_file, char* index_folder, char* mode, char* queryString, int* C, int writeSize, int alignment, int lastIndex, int* charList, int charNb){

	// cout << "come in\n";
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
	string position_file = string(index_folder) + "/" + string(bwt_encoded_file) + "positionFile.aux";
	// cout << "index_file: " << index_file << "\n";

	// cout << "writeSize: " << writeSize << "\n";

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
			// cout << "inIndex: " << inIndex << "\n";
			writeIndexFile(count, index_file, alignment, writeSize, inIndex, charList, charNb);
		}
		++inIndex;
	}


	if(strcmp(mode, "-m") == 0){
		cout << "search m mode\n";
		cout << "search: " << queryString << "\n";
		searchMode_M(index_file, alignment, writeSize, queryString, C, bwt_encoded_file, lastIndex);
	}

	else if(strcmp(mode , "-n") == 0 || strcmp(mode, "-a") == 0){
		cout << "search n mode\n";
		//void searchMode_N(string position_file, string index_file, int alignment, int writeSize, const char* queryString, int* C, int* count, char* bwt_encoded_file)
		searchMode_N(position_file, index_file, alignment, writeSize, queryString, C, bwt_encoded_file, mode, lastIndex);
	}

	else if(strcmp(mode, "-i") == 0){
		int startRec, endRec;

		sscanf(queryString, "%d %d", &startRec, &endRec);
		cout << "startRec: " << startRec << "  endRec: " << endRec << "\n";
		searchMode_I(delimiter, bwt_encoded_file, position_file, index_file, C, writeSize, alignment, startRec, endRec, fileSize, lastIndex);
	}
	return;
}


void searchWithoutWriteWithRead(char delimiter, char* bwt_encoded_file, char* index_folder, char* mode, char* queryString, int* C, int writeSize, int alignment, int lastIndex){

	string index_file = string(index_folder) + "/" + string(bwt_encoded_file) + "searchIndexFile.aux";
	string position_file = string(index_folder) + "/" + string(bwt_encoded_file) + "positionFile.aux";

	if(strcmp(mode, "-m") == 0){
		cout << "search m mode\n";
		cout << "search: " << queryString << "\n";
		//void searchMode_M(string index_file, int alignment, int writeSize, const char* queryString, int* C, int* count, char* bwt_encoded_file)
		searchMode_M(index_file, alignment, writeSize, queryString, C, bwt_encoded_file, lastIndex);
	}

	else if(strcmp(mode , "-n") == 0 || strcmp(mode, "-a") == 0){
		cout << "search n mode\n";
		searchMode_N(position_file, index_file, alignment, writeSize, queryString, C, bwt_encoded_file, mode, lastIndex);
	}

	else if(strcmp(mode, "-i") == 0){
		int startRec, endRec;

		ifstream bwtFile(bwt_encoded_file, ios::binary);
		bwtFile.seekg(0, ios::end);
		int fileSize = bwtFile.tellg();
		bwtFile.close();
		sscanf(queryString, "%d %d", &startRec, &endRec);
		cout << "startRec: " << startRec << "  endRec: " << endRec << "\n";
		searchMode_I(delimiter, bwt_encoded_file, position_file, index_file, C, writeSize, alignment, startRec, endRec, fileSize, lastIndex);
	}
	return;
}



int main(int argc, char *argv[]){

	char* arg_delimiter = argv[1];
	char* bwt_encoded_file = argv[2];
	char* index_folder = argv[3];
	char* mode = argv[4];
	char* queryString = argv[5];

	// cout << "begin: queryString: " << queryString << "\n";
	char delimiter;
	int count[128] = {0};
	int C[128] = {0};



	string index_file = string(index_folder) + "/" + string(bwt_encoded_file) + "searchIndexFile.aux";
	string position_file = string(index_folder) + "/" + string(bwt_encoded_file) + "positionFile.aux";


	if(strlen(arg_delimiter) == 2)
		delimiter = char(10);

	else
		delimiter = char(arg_delimiter[0] - char(0));


	int fileSize;

	ifstream fromFile(bwt_encoded_file, ios::binary);
	fromFile.seekg(0, ios::end);

	fileSize = fromFile.tellg();

	fromFile.seekg(0, ios::beg);

	cout << "fileSize: " << fileSize << "\n";

	int alignment, charNb = 0;

	if(fileSize < LIMIT_1)
		alignment = 1;
	else if(fileSize >= LIMIT_1 && fileSize < LIMIT_2)
		alignment = 2;
	
	else if(fileSize >= LIMIT_2 && fileSize <= LIMIT_3)
		alignment = 3;
	else
		alignment = 4;

	cout << "alignment: " << alignment << "\n";


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

	// cout << "lastIndex: " << lastIndex << "\n";




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
		// if(count[i] == 0)
		// 	continue;
		int cnt = 0;
		for(int j = 0; j < i; ++j){
			if(count[j] != 0)
				cnt += count[j];
		}

		C[i] = cnt;
	}


	// cout << "count is:\n";
	for(int i = 0; i < 128; ++i){
		if(count[i] != 0)
			++charNb;
		// cout  << count[i] << ", ";
	}

	// cout << "\n";

	// cout << "charNb: " << charNb << "\n";


	int* charList = new int[charNb];

	int charListIndex = 0;

	for(int i = 0; i < 128; ++i){
		if(count[i] != 0){
			charList[charListIndex] = i;
			++charListIndex;
		}
	}

	// cout << "C is:\n";
	// for(int i = 0; i < 128; ++i){
	// 	cout << C[i] << ", ";
	// }
	// cout << "\n";

	int oneRowSize = alignment + charNb * (1 + alignment);

	cout << "oneRowSize: " << oneRowSize << "\n";


	if(oneRowSize <= fileSize){

		if(!file_exist(index_file))
			searchWithWrite(delimiter, bwt_encoded_file, index_folder, mode, queryString, C, oneRowSize, alignment, lastIndex, charList, charNb);
		else
			searchWithoutWriteWithRead(delimiter, bwt_encoded_file, index_folder, mode, queryString, C, oneRowSize, alignment, lastIndex);
	}
	else{
		cout << "here\n";
		searchWithoutWriteWithRead(delimiter, bwt_encoded_file, index_folder, mode, queryString, C, oneRowSize, alignment, lastIndex);
	}

}