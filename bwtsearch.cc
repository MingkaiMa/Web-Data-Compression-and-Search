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

	ofstream toFile(index_file, ios::out | ios::app | ios::binary);
	toFile.write((char*)(&writeBuffer[0]), writeIndex);
	delete[] writeBuffer;
	toFile.close();
}


int getOCC(char c, int position, string index_file, int alignment, int writeSize, char* bwt_encoded_file, int lastIndex){

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

	unsigned char* getBuffer = new unsigned char[writeSize];

	fromFile.read((char*)(&getBuffer[0]), writeSize);

	int rowNb = 0;

	int i;

	for(i = 0; i < alignment; ++i){

		rowNb = rowNb | getBuffer[i];

		if(i != alignment - 1)
			rowNb = rowNb << 8;

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


	if(fromFile.peek() == EOF){

		int posCnt = 0;

		while(true){
			if(position == lastIndex){
				return posCnt;
			}
			++posCnt;
			position = getNextPosition(index_file, position_file, position, alignment, writeSize, C, bwt_encoded_file, lastIndex);
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
			position = getNextPosition(index_file, position_file, position, alignment, writeSize, C, bwt_encoded_file, lastIndex);

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

		position = getNextPosition(index_file, position_file, position, alignment, writeSize, C, bwt_encoded_file, lastIndex);

	}
	return -1;
}

void searchMode_M(string index_file, int alignment, int writeSize, const char* queryString, int* C, char* bwt_encoded_file, int lastIndex){


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

		first = C[c - char(0)] + getOCC(c, first - 1, index_file, alignment, writeSize, bwt_encoded_file, lastIndex);

		last =  C[c - char(0)] + getOCC(c, last,      index_file, alignment, writeSize, bwt_encoded_file, lastIndex) - 1;
		
		--i;
	}
	cout << last - first + 1 << "\n";
}

void searchMode_N(string position_file, string index_file, int alignment, int writeSize, const char* queryString, int* C, char* bwt_encoded_file, char* mode, int lastIndex){


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

		first = C[c - char(0)] + getOCC(c, first - 1, index_file, alignment, writeSize, bwt_encoded_file, lastIndex);

		last =  C[c - char(0)] + getOCC(c, last,      index_file, alignment, writeSize, bwt_encoded_file, lastIndex) - 1;
		
		--i;
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




	if(deliNum == 1){
		cout << 1 << "\n";
		return;
	}

	char* flagBits = new char[deliNum];
	memset(flagBits, '0', deliNum);


	for(int i = first; i <= last; ++i){
		

		int saValue = getPosition(position_file, index_file, i, alignment, writeSize, C, bwt_encoded_file, lastIndex);

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
}


string recoverRecord(char delimiter, string position_file, string index_file, int* C, int writeSize, int alignment, int position, char* bwt_encoded_file, int lastIndex){

	char c;
	char* charBuffer;
	string res = "";
	ifstream fromFile(bwt_encoded_file, ios::binary);

	int nextPosition = getNextPosition(index_file, position_file, position, alignment, writeSize, C, bwt_encoded_file, lastIndex);


	if(nextPosition > lastIndex){
		int tempnextPosition = nextPosition - 1;



		fromFile.seekg(tempnextPosition, ios::beg);

		charBuffer = new char[1];
		fromFile.read(charBuffer, 1);

		c = charBuffer[0];

		res.push_back(c);
		delete[] charBuffer;
	}
	else if(nextPosition < lastIndex){


		fromFile.seekg(nextPosition, ios::beg);

		charBuffer = new char[1];
		fromFile.read(charBuffer, 1);

		c = charBuffer[0];

		res.push_back(c);
		delete[] charBuffer;
	}
	else
		c = 0;



	int tempnextPosition;
	while(c != delimiter){
		nextPosition = getNextPosition(index_file, position_file, nextPosition, alignment, writeSize, C, bwt_encoded_file, lastIndex);

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

		if(c == delimiter)
			break;
		res.push_back(c);
	}
	
	reverse(res.begin(), res.end());

	return res;
}

void searchMode_I(char delimiter, char* bwt_encoded_file, string position_file, string index_file, int* C, int writeSize, int alignment, int startRec, int endRec, int fileSize, int lastIndex){


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


		delimiterNumBuffer = new unsigned char[alignment];
		fromFile.read((char*)(&delimiterNumBuffer[0]), alignment);
		int curdePos = 0;
		for(int i = 0; i < alignment; ++i){
			curdePos = curdePos | delimiterNumBuffer[i];
			if(i != alignment - 1)
				curdePos = curdePos << 8;
		}
		delete[] delimiterNumBuffer;


		string res = recoverRecord(delimiter, position_file, index_file, C, writeSize, alignment, curdePos, bwt_encoded_file, lastIndex);

		cout << res << "\n";

	}


}

void searchWithWrite(char delimiter, char* bwt_encoded_file, char* index_folder, char* mode, char* queryString, int* C, int writeSize, int alignment, int lastIndex, int* charList, int charNb){

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

		searchMode_M(index_file, alignment, writeSize, queryString, C, bwt_encoded_file, lastIndex);
	}

	else if(strcmp(mode , "-n") == 0 || strcmp(mode, "-a") == 0){

		searchMode_N(position_file, index_file, alignment, writeSize, queryString, C, bwt_encoded_file, mode, lastIndex);
	}

	else if(strcmp(mode, "-i") == 0){
		int startRec, endRec;

		sscanf(queryString, "%d %d", &startRec, &endRec);
		
		searchMode_I(delimiter, bwt_encoded_file, position_file, index_file, C, writeSize, alignment, startRec, endRec, fileSize, lastIndex);
	}
	return;
}


void searchWithoutWriteWithRead(char delimiter, char* bwt_encoded_file, char* index_folder, char* mode, char* queryString, int* C, int writeSize, int alignment, int lastIndex){

	string index_file = string(index_folder) + "/" + string(bwt_encoded_file) + "searchIndexFile.aux";
	string position_file = string(index_folder) + "/" + string(bwt_encoded_file) + "positionFile.aux";

	if(strcmp(mode, "-m") == 0){

		searchMode_M(index_file, alignment, writeSize, queryString, C, bwt_encoded_file, lastIndex);
	}

	else if(strcmp(mode , "-n") == 0 || strcmp(mode, "-a") == 0){

		searchMode_N(position_file, index_file, alignment, writeSize, queryString, C, bwt_encoded_file, mode, lastIndex);
	}

	else if(strcmp(mode, "-i") == 0){
		int startRec, endRec;

		ifstream bwtFile(bwt_encoded_file, ios::binary);
		bwtFile.seekg(0, ios::end);
		int fileSize = bwtFile.tellg();
		bwtFile.close();
		sscanf(queryString, "%d %d", &startRec, &endRec);

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



	int oneRowSize = alignment + charNb * (1 + alignment);


	if(oneRowSize <= fileSize){

		if(!file_exist(index_file))
			searchWithWrite(delimiter, bwt_encoded_file, index_folder, mode, queryString, C, oneRowSize, alignment, lastIndex, charList, charNb);
		else
			searchWithoutWriteWithRead(delimiter, bwt_encoded_file, index_folder, mode, queryString, C, oneRowSize, alignment, lastIndex);
	}
	else{
		searchWithoutWriteWithRead(delimiter, bwt_encoded_file, index_folder, mode, queryString, C, oneRowSize, alignment, lastIndex);
	}

}