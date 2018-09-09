#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <cassert>

#define MAX_NB_OF_CHAR 127

using namespace std;

class BoolArray
{
public:

	unsigned char* boolArray;
	int arrayLength;
	int nbBytes;

	BoolArray();
	BoolArray(int arrLength);
	~BoolArray();

	void reset(int arrLength);


	bool getVal_returnBool(const int index) const;
	inline int getValN(const int index) const;

	void setVal_bool(const int index, const bool val);
	inline void setValN(const int index, const int val);

	void setAll(const bool val);

	void print(const char* delimiter) const;

	bool isAllTrue() const;
	bool isAllFalse() const;
	
};


BoolArray::BoolArray(){
	arrayLength = -1;
	nbBytes = -1;
}

BoolArray::BoolArray(int arrLength){
	arrayLength = arrLength;
	nbBytes = arrLength / 8;
	if(arrLength % 8 != 0)
		++nbBytes;
	boolArray = new unsigned char[nbBytes];
}

BoolArray::~BoolArray(){
	if(arrayLength != -1)
		delete[] boolArray;
}

void BoolArray::reset(int arrLength){
	if(arrayLength != -1)
		delete[] boolArray;
	arrayLength = arrLength;
	nbBytes = arrLength / 8;
	if(arrLength % 8 != 0)
		++nbBytes;

	boolArray = new unsigned char[nbBytes];
}

bool BoolArray::getVal_returnBool(const int index) const
{
	int byteNum;
	int bitNum;
	unsigned char currChar;

	byteNum = index >> 3;
	bitNum = index & 7;
	currChar = boolArray[byteNum];

	currChar = currChar >> (7 - bitNum);

	currChar = currChar & 1;

	if((int)currChar == 0)
		return false;
	else if((int)currChar == 1)
		return true;
	else
	{
		cout << "wrong:\n";
		return false;
	}
}

inline int BoolArray::getValN(const int index) const
{
	int byteNum;
	int bitNum;
	unsigned char currChar;
	int res;

	byteNum = index >> 3;
	bitNum = index & 7;
	currChar = boolArray[byteNum];

	currChar = currChar >> (7 - bitNum);

	currChar = currChar & 1;

	res = (int) currChar;

	return res;
}



void BoolArray::setVal_bool(const int index, const bool val){

	int byteNum;
	int bitNum;
	unsigned char original, currChar;
	int putValue;

	if(val == true)
		putValue = 1;
	else
		putValue = 0;

	byteNum = index >> 3;
	bitNum = index & 7;
	original = boolArray[byteNum];
	currChar = boolArray[byteNum];

	currChar = currChar >> (7 - bitNum);

	currChar = currChar & 1;

	currChar = (int) currChar;

	if(currChar != putValue){
		original = original ^ (1 << (7 - bitNum));
	}
	boolArray[byteNum] = original;
}

inline void BoolArray::setValN(const int index, const int val){

	int byteNum;
	int bitNum;
	unsigned char original, currChar;

	byteNum = index >> 3;
	bitNum = index & 7;
	original = boolArray[byteNum];
	currChar = boolArray[byteNum];

	currChar = currChar >> (7 - bitNum);

	currChar = currChar & 1;

	currChar = (int) currChar;

	if(currChar != val){
		original = original ^ (1 << (7 - bitNum));
	}
	boolArray[byteNum] = original;
}



void BoolArray::setAll(const bool val){
	if(val == true){
		memset(boolArray, 255, nbBytes);
	}
	else
		memset(boolArray, 0, nbBytes);
}

void BoolArray::print(const char* delimiter) const
{
	int i;
	for(i = 0; i < arrayLength; ++i)
		cout << getVal_returnBool(i) << delimiter;
	cout << endl;
}

bool BoolArray::isAllTrue() const{

	int i;
	for(i = 0; i < (arrayLength >> 3); ++i){
		if(boolArray[i] != (unsigned char)(255))
			return false;
	}

	for(i = 0; i < (arrayLength % 8); ++i){
		if(getVal_returnBool(arrayLength - i - 1) == false)
			return false;
	}

	return true;
}

bool BoolArray::isAllFalse() const{

	int i;
	for(i = 0; i < (arrayLength >> 3); ++i){
		if(boolArray[i] != (unsigned char)(0))
			return false;
	}

	for(i = 0; i < (arrayLength % 8); ++i){
		if(getVal_returnBool(arrayLength - i - 1) == true)
			return false;
	}

	return true;
}


void getType(const unsigned char* inputData, const int inputLength, int& nbStype, int& nbLtype, BoolArray& typeTable){

	int i, j, cnt = 0;
	nbStype = 0;
	nbLtype = 0;

	typeTable.setAll(true);

	for(i = 0; i < inputLength - 2; ++i){

		if(inputData[i] < inputData[i + 1]){
			for(j = i - cnt; j <= i; ++j){
				++nbStype;
				typeTable.setValN(j, 1);
			}
			cnt = 0;
		}
		else if(inputData[i] > inputData[i + 1]){
			for(j = i - cnt; j <= i; ++j){
				++nbLtype;
				typeTable.setValN(j, 0);
			}
			cnt = 0;
		}
		else
			++cnt;
	}

	for(j = i - cnt; j <= i; ++j){
		++nbLtype;
		typeTable.setValN(j, 0);
	}


	if(nbStype < nbLtype){
		typeTable.setValN(inputLength - 1, 1);
		++nbStype;
	}
	else{
		typeTable.setValN(inputLength - 1, 0);
		++nbLtype;
	}
}

void getType(const int* inputData, const int inputLength, int& nbStype, int& nbLtype, BoolArray& typeTable){
	
	cout << "getType start: \n";

	int i, j, cnt = 0;
	nbStype = 0;
	nbLtype = 0;

	typeTable.setAll(true);

	for(i = 0; i < inputLength - 1; ++i){

		if(inputData[i] < inputData[i + 1]){
			for(j = i - cnt; j <= i; ++j){
				++nbStype;
				typeTable.setValN(j, 1);
			}
			cnt = 0;
		}
		else if(inputData[i] > inputData[i + 1]){
			for(j = i - cnt; j <= i; ++j){
				++nbLtype;
				typeTable.setValN(j, 0);
			}
			cnt = 0;
		}
		else
			++cnt;

	}


	if(nbStype < nbLtype){
		typeTable.setValN(inputLength - 1, 1);
		++nbStype;
	}
	else{
		typeTable.setValN(inputLength - 1, 0);
		++nbLtype;
	}

	cout << "gettype end here\n";
}

inline void getMaxMin(const int* inputData, const int inputLength, int& maxVal, int& minVal){

	maxVal = inputData[0];
	minVal = inputData[0];

	for(int i = 1; i < inputLength; ++i){
		if(maxVal < inputData[i])
			maxVal = inputData[i];

		if(minVal > inputData[i])
			minVal = inputData[i];
	}
}


void SA_build_Stype(int* array2, const int arr2Length, const unsigned char* inputData, const int inputLength,
					BoolArray& typeTable, int* saRes)
{
	int* C;
	int i, saIndex, array2Index, origCount, tempCount, charBefore, bucketIndex;

	C = new int[MAX_NB_OF_CHAR];
	memset(C, 0, MAX_NB_OF_CHAR * sizeof(int));

	for(i = 0; i < inputLength - 1; ++i)
		++C[inputData[i]];


	origCount = C[0];
	C[0] = 1;

	for(i = 1; i < MAX_NB_OF_CHAR; ++i){
		tempCount = C[i];
		C[i] = C[i - 1] + origCount;
		origCount = tempCount;
	}

	memset(saRes, -1, inputLength * sizeof(int));


	array2Index = 0;

	for(saIndex = 0; saIndex < inputLength; ++saIndex){


		if(saRes[saIndex] == -1){
			assert(array2Index < arr2Length);

			saRes[saIndex] = array2[array2Index];
			++array2Index;
		}

		charBefore = saRes[saIndex] - 1;
		if(charBefore >= 0){
			if(typeTable.getValN(charBefore) == 0){
				bucketIndex = inputData[charBefore];

				if(C[bucketIndex] > saIndex){
					saRes[C[bucketIndex]] = charBefore;
					++C[bucketIndex];
				}
			}
		}
	}
	delete[] C;
}


void SA_build_Stype(int* array2, const int arr2Length, const int* inputData, const int inputLength,
					BoolArray& typeTable, int* saRes)
{
	int maxVal, minVal, range;
	int* C;
	int i, saIndex, array2Index, origCount, tempCount, charBefore, bucketIndex;

	getMaxMin(inputData, inputLength, maxVal, minVal);
	range = maxVal - minVal + 1;

	C = new int[range];
	memset(C, 0, range * sizeof(int));

	for(i = 0; i < inputLength; ++i)
		++C[inputData[i] - minVal];



	origCount = C[0];
	C[0] = 0;

	for(i = 1; i < range; ++i){
		tempCount = C[i];
		C[i] = C[i - 1] + origCount;
		origCount = tempCount;
	}

	memset(saRes, -1, inputLength * sizeof(int));


	array2Index = 0;

	for(saIndex = 0; saIndex < inputLength; ++saIndex){


		if(saRes[saIndex] == -1){
			assert(array2Index < arr2Length);

			saRes[saIndex] = array2[array2Index];
			++array2Index;
		}

		charBefore = saRes[saIndex] - 1;
		if(charBefore >= 0){
			if(typeTable.getValN(charBefore) == 0){
				bucketIndex = inputData[charBefore];

				if(C[bucketIndex] > saIndex){
					saRes[C[bucketIndex]] = charBefore;
					++C[bucketIndex];
				}
			}
		}
	}
	delete[] C;
}


void SA_build_Ltype(int* array2, const int arr2Length, const unsigned char* inputData, const int inputLength, 
						BoolArray& typeTable, int* saRes)
{

	int* C;

	int i;

	C = new int[MAX_NB_OF_CHAR];
	memset(C, 0, sizeof(int) * MAX_NB_OF_CHAR);

	for(i = 0; i < inputLength - 1; ++i)
		++C[inputData[i]];

	C[0] = C[0];
	for(i = 1; i < MAX_NB_OF_CHAR; ++i)
		C[i] += C[i - 1];

	memset(saRes, -1, sizeof(int) * inputLength);

	int array2Index = arr2Length - 1;

	int saIndex, charBefore, bucketIndex;

	for(saIndex = inputLength - 1; saIndex >= 0; --saIndex){
		if(saRes[saIndex] == -1){
			saRes[saIndex] = array2[array2Index];
			--array2Index;
		}
		charBefore = saRes[saIndex] - 1;

		if(charBefore >= 0){
			if(typeTable.getValN(charBefore) == 1){
				bucketIndex = inputData[charBefore];

				if(C[bucketIndex] < saIndex){
					saRes[C[bucketIndex]] = charBefore;
					--C[bucketIndex];
				}

			}
		}

	}
	delete[] C;
}


void SA_build_Ltype(int* array2, const int arr2Length, const int* inputData, const int inputLength, 
						BoolArray& typeTable, int* saRes)
{

	int maxVal, minVal, range;
	int* C;
	int i, saIndex, array2Index, charBefore, bucketIndex;

	getMaxMin(inputData, inputLength, maxVal, minVal);
	range = maxVal - minVal + 1;

	C = new int[range];

	memset(C, 0, sizeof(int) * range);

	for(i = 0; i < inputLength; ++i)
		++C[inputData[i] - minVal];

	C[0] -= 1;

	for(i = 1; i < range; ++i)
		C[i] += C[i - 1];

	memset(saRes, -1, inputLength * sizeof(int));

	array2Index = arr2Length - 1;

	for(saIndex = inputLength - 1; saIndex >= 0; --saIndex){
		if(saRes[saIndex] == -1){
			saRes[saIndex] = array2[array2Index];
			--array2Index;
		}
		charBefore = saRes[saIndex] - 1;

		if(charBefore >= 0){
			if(typeTable.getValN(charBefore) == 1){
				bucketIndex = inputData[charBefore];
				if(C[bucketIndex] < saIndex){
					saRes[C[bucketIndex]] = charBefore;
					--C[bucketIndex];
				}
			}
		}
	}
	delete[] C;
}




void reverse_s_distance(BoolArray& typeTable, int inputLength, int& maxDist){

	int i, temp, prevDist;

	maxDist = 0;

	prevDist = 1;

	for(i = inputLength - 2; i >= 0; --i){
		temp = maxDist - prevDist;
		temp = temp >> 31;
		maxDist = maxDist + ((prevDist - maxDist) & temp);

		temp = typeTable.getValN(i) - 1;
		prevDist = (prevDist & temp) + 1;

	}
}


void reverse_l_distance(BoolArray& typeTable, int inputLength, int& maxDist){

	int i, temp, prevDist;

	maxDist = 0;

	prevDist = 1;

	for(i = inputLength - 2; i >= 0; --i){
		temp = maxDist - prevDist;
		temp = temp >> 31;
		maxDist = maxDist + ((prevDist - maxDist) & temp);

		temp = 0 - typeTable.getValN(i);
		prevDist = (prevDist & temp) + 1;
	}
}





void sort_s_substrings(const unsigned char* inputData, const int inputLength, int* array2, 
						BoolArray& bucket2, const int arr2Length, BoolArray& typeTable, const int maxDist)
{

	int* buffer;
	int *aidArray; 
	int *tBucket;
	int i;

	int bufferLength = MAX_NB_OF_CHAR * 2;
	bucket2.setAll(false);

	buffer = new int[bufferLength];
	aidArray = new int[arr2Length];

	memset(aidArray, 0, sizeof(int) * arr2Length);
	tBucket = new int[arr2Length];

	aidArray[0] = arr2Length;

	int offset, startPos, endPos, prePos, curIndex, tempVar;

	for(offset = 0; offset < maxDist + 1; ++offset){
		startPos = 0;
		prePos = 0;

		while(startPos < arr2Length){

			prePos = startPos;
			while((startPos < arr2Length) && (aidArray[startPos] < 0))
				startPos = -1 * (aidArray[startPos]);

			endPos = aidArray[startPos] - 1;
			aidArray[prePos] = -1 * startPos;
			memset(buffer, 0, sizeof(int) * bufferLength);

			if(startPos < arr2Length){

				int check_flag = 0;

				for(i = startPos; i < endPos + 1; ++i){
					tBucket[i] = array2[i];

					curIndex = array2[i] + offset;
					if(curIndex < (inputLength - 1)){

						tempVar = inputData[curIndex] << 1;
						tempVar += typeTable.getValN(curIndex);
						++buffer[tempVar];
					}
					else
						check_flag = 1;
				}
				


				int preVar = buffer[0];
				buffer[0] = startPos + check_flag;

				for(i = 1; i < bufferLength; ++i){
					tempVar = buffer[i];
					buffer[i] = buffer[i - 1] + preVar;
					preVar = tempVar;
				}

				for(i = startPos; i < endPos + 1; ++i){
					curIndex = tBucket[i] + offset;

					if(curIndex < (inputLength - 1)){
						tempVar = inputData[curIndex] << 1;
						tempVar += typeTable.getValN(curIndex);
						array2[buffer[tempVar]] = tBucket[i];
						++buffer[tempVar];
					}
					else
						array2[startPos] = tBucket[i];
				}

				if(check_flag != 0){
					bucket2.setValN(startPos, 1);
					aidArray[startPos] = -startPos - 1;
				}

				if(buffer[0] > (startPos + check_flag)){
					bucket2.setValN(buffer[0] - 1, 1);
					aidArray[startPos + check_flag] = -1 * buffer[0];
				}

				i = 1;
				if(offset > 0){
					for(i = 1; i < bufferLength; ++i){
						if(buffer[i] == buffer[i - 1] + 1){
							bucket2.setValN(buffer[i] - 1, 1);
							aidArray[buffer[i - 1]] = -1 * buffer[i];
						}
						else if(buffer[i] > buffer[i - 1] + 1){
							bucket2.setValN(buffer[i] - 1, 1);
							tempVar = -1 * (i & 1);
							tempVar = (buffer[i] ^ tempVar) - tempVar;
							aidArray[buffer[i - 1]] = tempVar;
						}

					}
				}

				else{
					for(i = 1; i < bufferLength; ++i){
						if(buffer[i] == buffer[i - 1] + 1){
							bucket2.setValN(buffer[i] - 1, 1);
							aidArray[buffer[i - 1]] = -1 * buffer[i];
						}
						else if(buffer[i] > buffer[i - 1] + 1){
							bucket2.setValN(buffer[i] - 1, 1);
							aidArray[buffer[i - 1]] = buffer[i];
						}
					}
				}

				if(startPos == endPos){
					aidArray[startPos] = -endPos - 1;
					bucket2.setValN(startPos, 1);
				}

				startPos = endPos + 1;
			}
		}
	}

	delete[] aidArray;
	delete[] buffer;
	delete[] tBucket;
}

void sort_l_substrings(const unsigned char* inputData, const int inputLength, int* array2, 
						BoolArray& bucket2, const int arr2Length, BoolArray& typeTable, const int maxDist)
{
	int* buffer;
	int* aidArray;
	int* tBucket;
	int i;
	int bufferLength = MAX_NB_OF_CHAR * 2;

	bucket2.setAll(false);

	buffer = new int[bufferLength];
	aidArray = new int[arr2Length];

	memset(aidArray, 0, sizeof(int) * arr2Length);
	tBucket = new int[arr2Length];

	aidArray[0] = arr2Length;

	int offset, startPos, endPos, prePos, curIndex, tempVar;

	for(offset = 0; offset < maxDist + 1; ++offset){

		startPos = 0;
		prePos = 0;

		while(startPos < arr2Length){

			prePos = startPos;
			while((startPos < arr2Length) && (aidArray[startPos] < 0))
				startPos = -1 * aidArray[startPos];

			endPos = aidArray[startPos] - 1;
			aidArray[prePos] = -1 * startPos;

			memset(buffer, 0, sizeof(int) * bufferLength);

			if(startPos < arr2Length){

				int check_flag = 0;

				for(i = startPos; i < endPos + 1; ++i){
					tBucket[i] = array2[i];

					curIndex = array2[i] + offset;

					if(curIndex < (inputLength - 1)){
						tempVar = inputData[curIndex] << 1;
						tempVar += typeTable.getValN(curIndex);
						++buffer[tempVar];
					}
					else{
						check_flag = 1;
					}
				}

				int preVar = buffer[0];
				buffer[0] = startPos + check_flag;

				for(i = 1; i < bufferLength; ++i){
					tempVar = buffer[i];
					buffer[i] = buffer[i - 1] + preVar;
					preVar = tempVar;
				}

				for(i = startPos; i < endPos + 1; ++i){
					curIndex = tBucket[i] + offset;

					if(curIndex < (inputLength - 1)){
						tempVar = inputData[curIndex] << 1;
						tempVar += typeTable.getValN(curIndex);
						array2[buffer[tempVar]] = tBucket[i];
						++buffer[tempVar];
					}
					else
						array2[startPos] = tBucket[i];
				}


				if(check_flag != 0){
					bucket2.setValN(startPos, 1);
					aidArray[startPos] = -startPos - 1;
				}

				if(buffer[0] > (startPos + check_flag)){
					bucket2.setValN(buffer[0] - 1, 1);
					aidArray[startPos + check_flag] = -1 * buffer[0]; 
				}

				if(offset > 0){
					for(i = 1; i < bufferLength; ++i){
						if(buffer[i] == buffer[i - 1] + 1){
							bucket2.setValN(buffer[i] - 1, 1);
							aidArray[buffer[i - 1]] = -1 * buffer[i];
						}
						else if(buffer[i] > buffer[i - 1] + 1){
							bucket2.setValN(buffer[i] - 1, 1);
							tempVar = -1 * ((i & 1) ^ 1);
							tempVar = (buffer[i] ^ tempVar) - tempVar;
							aidArray[buffer[i - 1]] = tempVar; 
						}
					}
				}

				else{

					for(i = 1; i < bufferLength; ++i){
						if(buffer[i] == buffer[i - 1] + 1){
							bucket2.setValN(buffer[i] - 1, 1);
							aidArray[buffer[i - 1]] = -1 * buffer[i];
						}
						else if(buffer[i] > buffer[i - 1] + 1){
							bucket2.setValN(buffer[i] - 1, 1);
							aidArray[buffer[i - 1]] = buffer[i];
						}
					}
				}

				startPos = endPos + 1;
			}
		}
	}

	delete[] buffer;
	delete[] tBucket;
	delete[] aidArray;
}



void TApos_build_Stype(int* array2, const int arr2Length, const BoolArray& bucket2, int* tApos, const int inputLength, const BoolArray& typeTable){



	int* bucket = new int[inputLength];
	// memset(bucket, -1, sizeof(int) * inputLength);

	// bucket2.print(" ");
	int curBucket = 0;

	int i, tempVar, tempIndex;

	for(i = 0; i < arr2Length; ++i){
		bucket[array2[i]] = curBucket;
		curBucket += bucket2.getValN(i);
	}

	int j = 0;

	for(i = 0; i < inputLength; ++i){
		tempVar = -1 * (typeTable.getValN(i));
		tempIndex = ~tempVar;
		tApos[j] = (tApos[j] & tempIndex) | (bucket[i] & tempVar);
		j += (1 & tempVar);
	}
	delete[] bucket;
}


void TApos_build_Ltype(int* array2, const int arr2Length, const BoolArray& bucket2, int* tApos, const int inputLength, const BoolArray& typeTable){

	int* bucket = new int[inputLength];


	int curBucket = 0;

	int i, tempVar, tempIndex;

	for(i = 0; i < arr2Length; ++i){
		bucket[array2[i]] = curBucket;
		curBucket += bucket2.getValN(i);
	}

	int j = 0;
	for(i = 0; i < inputLength; ++i){
		tempVar = (typeTable.getValN(i) - 1);
		tempIndex = ~tempVar;
		tApos[j] = (tApos[j] & tempIndex) | (bucket[i] & tempVar);
		j += (1 & tempVar);
	}
	delete[] bucket;
}



void rebuild_array2_Stype(int* array2, int arr2Length, const BoolArray& typeTable, const int inputLength){

	int* aidArray = new int[arr2Length];

	int i, j = 0;

	int temp1, temp2;

	for(i = 0; i < inputLength; ++i){

		temp1 = typeTable.getValN(i);
		temp1 = temp1 << 31;
		temp1 = temp1 >> 31;
		temp2 = ~temp1;

		aidArray[j] = (i & temp1) | (aidArray[j] & temp2);
		j += (1 & temp1);
	}

	for(i = 0; i < arr2Length; ++i){
		temp1 = array2[i];
		array2[i] = aidArray[temp1];
	}
	delete[] aidArray;
}

void rebuild_array2_Ltype(int* array2, int arr2Length, const BoolArray& typeTable, const int inputLength){

	int* aidArray = new int[arr2Length];

	int i, j = 0;

	int temp1, temp2;

	for(i = 0; i < inputLength; ++i){
		temp1 = typeTable.getValN(i);
		temp1 = temp1 << 31;
		temp1 = temp1 >> 31;
		temp2 = ~temp1;

		aidArray[j] = (i & temp2) | (aidArray[j] & temp1);
		j += (1 & temp2);
	}

	for(i = 0; i < arr2Length; ++i){
		temp1 = array2[i];
		array2[i] = aidArray[temp1];
	}
	delete[] aidArray;
}


void counting_sort(const int* inputData, const int inputLength, int* array1, BoolArray& bucket1, int* buffer){

	int maxVal, minVal, range, oriCount;
	int i, temp;

	getMaxMin(inputData, inputLength, maxVal, minVal);

  cout << "max is: " << maxVal << "\n";
  cout << "min is: " << minVal << "\n";
	range = maxVal - minVal + 1;

	memset(buffer, 0, sizeof(int) * range);

	for(i = 0; i < inputLength; ++i){
		++buffer[inputData[i] - minVal];
	}

	oriCount = buffer[0];
	buffer[0] = 0;

	for(i = 1; i < range; ++i){
		temp = buffer[i];
		buffer[i] = buffer[i - 1] + oriCount;
		oriCount = temp;
	}

	for(i = 0; i < inputLength; ++i){
		temp = inputData[i] - minVal;

		array1[buffer[temp]] = i;
		++buffer[temp];
	}

	bucket1.setAll(false);

	for(i = 0; i < range; ++i)
		bucket1.setValN(buffer[i] - 1, 1);
}


// void build_array2_Stype(const int* array1, const int inputLength, const BoolArray& bucket1, 
// 							int* array2, BoolArray& bucket2, const BoolArray& typeTable)
// {

// 	cout << "build_array2_Stype start\n";
// 	int i, j = 0, temp;

// 	bucket2.setAll(false);

// 	cout << "futher1?\n";

// 	cout << "print: \n";
// 	cout << array1[11304875] << "\n";
// 	cout << array1[11304876] << "\n";
// 	cout << "inputLength: " <<inputLength << "\n";
// 	for(i = 0; i < inputLength; ++i){
// 		//cout << "i is: " << i << ", ";
// 		temp = array1[i];
// 		if(typeTable.getValN(temp) == 1){
// 			array2[j] = array1[i];
// 			++j;
// 		}
// 		if(bucket2.getValN(i) == 1 && j - 1 >= 0)
// 			bucket2.setValN(j - 1, 1);
// 	}
// 	cout << "build_array2_Stype end\n";
// }
void build_array2_Stype(const int* ArrayA, const int inputLength, 
			    const BoolArray& BuckA, 
			    int* ArrayB, BoolArray& BuckB, 
			    const BoolArray& suffixType)
{
  cout << "new \n";
  int i, j;
  int temp;

  BuckB.setAll(false);

  j = 0;
 
  for (i = 0; i < inputLength; i++)
  {
    temp = ArrayA[i];
    if (suffixType.getValN(temp) == 1)
    {
      ArrayB[j] = ArrayA[i];
      j++;
    }
    if (BuckA.getValN(i) == 1 && j - 1 >= 0)
    {
      BuckB.setValN(j - 1, 1);
    }
  }
}

// void build_array2_Ltype(const int* array1, const int inputLength, const BoolArray& bucket1, 
// 							int* array2, BoolArray& bucket2, const BoolArray& typeTable)
// {

// 	int i, j = 0, temp;

// 	bucket2.setAll(false);

// 	for(i = 0; i < inputLength; ++i){
// 		temp = array1[i];
// 		if(typeTable.getValN(temp) == 0){
// 			array2[j] = array1[i];
// 			++j;
// 		}
// 		if(bucket2.getValN(i) == 1 && j - 1 >= 0)
// 			bucket2.setValN(j - 1, 1);
// 	}
// }

void build_array2_Ltype(const int* ArrayA, const int inputLength, 
			    const BoolArray& BuckA, 
			    int* ArrayB, BoolArray& BuckB, 
			    const BoolArray& suffixType)
{
  int i, j;
  int temp;

  BuckB.setAll(false);

  j = 0;
 
  for (i = 0; i < inputLength; i++)
  {
    temp = ArrayA[i];
    if (suffixType.getValN(temp) == 0)
    {
      ArrayB[j] = ArrayA[i];
      j++;
    }
    if (BuckA.getValN(i) == 1 && j - 1 >= 0)
    {
      BuckB.setValN(j - 1, 1);
    }
  }
}


void s_distance(const BoolArray& typeTable, const int inputLength, int* Dist, int& maxDist){

	int i, j, temp, prevDist;

	Dist[0] = 0;
	maxDist = 0;

	i = 0;
	while(typeTable.getValN(i) == 0){
		Dist[i] = 0;
		++i;
	}

	Dist[i] = 0;

	if(i < inputLength - 1)
		maxDist = 1;

	j = i + 1;
	prevDist = 1;

	for(i = j; i < inputLength; ++i){
		Dist[i] = prevDist;

		prevDist = prevDist - prevDist * typeTable.getValN(i) + 1;

		temp = maxDist - Dist[i];
		temp = temp >> 31;
		maxDist = maxDist + ((Dist[i] - maxDist) & temp);
	}
}

void l_distance(const BoolArray& typeTable, const int inputLength, int* Dist, int& maxDist){

	int i, j, temp, prevDist;

	Dist[0] = 0;
	maxDist = 0;

	i = 0;
	while(typeTable.getValN(i) == 1){
		Dist[i] = 0;
		++i;
	}

	Dist[i] = 0;

	if(i < inputLength - 1)
		maxDist = 1;

	j = i + 1;
	prevDist = 1;

	for(i = j; i < inputLength; ++i){
		Dist[i] = prevDist;

		temp = typeTable.getValN(i) - 1;

		prevDist = prevDist - (prevDist & temp) + 1;

		temp = maxDist - Dist[i];
		temp = temp >> 31;
		maxDist = maxDist + ((Dist[i] - maxDist) & temp);
	}
}

void build_distcount(const int inputLength, int* Dist, int*& DistCount, const int maxDist){

	int i, j, prevCount, temp;

	DistCount = new int[maxDist + 1];

	memset(DistCount, 0, sizeof(int) * (maxDist + 1));

	j = 0;

	while(Dist[j] == 0)
		++j;

	for(i = j; i < inputLength; ++i)
		DistCount[Dist[i] - 1]++;

	prevCount = DistCount[0];
	DistCount[0] = 0;

	for(i = 1; i < maxDist + 1; ++i){
		temp = DistCount[i];
		DistCount[i] = DistCount[i - 1] + prevCount;
		prevCount = temp;
	}
}

int* build_list_Stype(int*& array1, const int inputLength, int* Dist,
	   						int* DistCount, const int maxDist, const BoolArray& bucket1,
	   						BoolArray& bucketList, const int listLength)
{
	int i, j, listPos, tempVar, start2Pos, end2Pos;

	bucketList.setAll(false);
	i = 0;

	while(i < inputLength){
		start2Pos = i;

		while(bucket1.getValN(i) != 1 && i < inputLength){
			tempVar = Dist[array1[i]];
			if(tempVar > 0){
				listPos = DistCount[tempVar - 1];
				Dist[array1[i]] = listPos;
				bucketList.setValN(listPos, 1);
				++DistCount[tempVar - 1];
			}
			else
				Dist[array1[i]] = -1;
			++i;
		}

		tempVar = Dist[array1[i]];
		if(tempVar != 0){
			listPos = DistCount[tempVar - 1];
			Dist[array1[i]] = listPos;
			bucketList.setValN(listPos, 1);
			++DistCount[tempVar - 1];
		}
		else
			Dist[array1[1]] = -1;

		end2Pos = i;


		for(j = start2Pos; j < end2Pos; ++j){
			listPos = Dist[array1[j]];
			if(listPos >= 0 && listPos != listLength - 1){
				if(bucketList.getValN(listPos + 1) == 1)
					bucketList.setValN(listPos, 0);
			}
		}

		++i;
	}

	for(i = 0; i < inputLength; ++i){
		if(Dist[i] >= 0)
			array1[Dist[i]] = i;
	}

	for(i = 0; i < maxDist; ++i)
		bucketList.setValN(DistCount[i] - 1, 1);

	for(i = 0; i < maxDist; ++i){
		if(i == 0)
			j = 0;
		
		else{
			j = DistCount[i - 1];
		}
		while(j < DistCount[i]){
			array1[j] = array1[j] - i - 1;
			++j;
		}
	}

	return array1;
}


int* build_list_Ltype(int*& array1, const int inputLength, int* Dist,
	   						int* DistCount, const int maxDist, const BoolArray& bucket1,
	   						BoolArray& bucketList, const int listLength)
{

	int i, j, listPos, tempVar, start2Pos, end2Pos;
	bool flag;

	bucketList.setAll(false);

	i = inputLength - 1;

	while(i >= 0){
		end2Pos = i;

		if(i > 0){
			if(bucket1.getValN(i - 1) != 1)
				flag = false;
			else
				flag = true;
		}

		else{
			flag = true;
		}

		while(!flag){
			tempVar = Dist[array1[i]];
			if(tempVar > 0){
				listPos = DistCount[tempVar - 1];
				Dist[array1[i]] = listPos;
				bucketList.setValN(listPos, 1);
				++DistCount[tempVar - 1];
			}
			else
				Dist[array1[i]] = -1;

			--i;
			if(i > 0){
				if(bucket1.getValN(i - 1) != 1)
					flag = false;
				else
					flag = true;
			}

			else
				flag = true;
		}

		tempVar = Dist[array1[i]];
		if(tempVar != 0){
			listPos = DistCount[tempVar - 1];
			Dist[array1[i]] = listPos;
			bucketList.setValN(listPos, 1);
			++DistCount[tempVar - 1];
		}
		else
			Dist[array1[1]] = -1;

		start2Pos = i;


		for(j = end2Pos; j >= start2Pos; --j){
			listPos = Dist[array1[j]];
			if(listPos >= 0 && listPos != listLength - 1){
				if(bucketList.getValN(listPos + 1) == 1)
					bucketList.setValN(listPos, 0);
			}
		}
		--i;
	}


	for(i = 0; i < inputLength; ++i){
		if(Dist[i] >= 0)
			array1[Dist[i]] = i;
	}

	for(i = 0; i < maxDist; ++i)
		bucketList.setValN(DistCount[i] - 1, 1);

	for(i = 0; i < maxDist; ++i){
		if(i == 0)
			j = 0;

		else
			j = DistCount[i - 1];

		while(j < DistCount[i]){
			array1[j] = array1[j] - i - 1;
			++j;
		}
	}
	return array1;
}


void sort_list_Stype(int* array2, BoolArray& bucket2, int* List, const BoolArray& bucketList,
						const int inputLength, const int listLength, const int arr2Length)
{

	int* Rev, *left;
	int i, j, newBucketNb, bucketNb;
	int bucketRight;


	Rev = new int[inputLength];
	left = new int[arr2Length];


	memset(Rev, -1, sizeof(int) * inputLength);
	memset(left, -1, sizeof(int) * arr2Length);


	bucketRight = arr2Length - 1;

	for(i = arr2Length - 1; i > 0; --i){
		Rev[array2[i]] = bucketRight;
		if(bucket2.getValN(i - 1) == 1){
			left[bucketRight] = i;
			bucketRight = i - 1;
		}
	}

	Rev[array2[0]] = bucketRight;
	left[bucketRight] = 0;


	i = 0;

	while(i < listLength){
		j = i;

		while(bucketList.getValN(j) == 0){
			++left[Rev[List[j]]];
			++j;
		}

		++left[Rev[List[j]]];

		j = i;

		while(bucketList.getValN(j) == 0){
			newBucketNb = left[Rev[List[j]]] - 1;
			Rev[List[j]] = newBucketNb;
			++j;
		}

		newBucketNb = left[Rev[List[j]]] - 1;
		Rev[List[j]] = newBucketNb;

		j = i;

		while(bucketList.getValN(j) == 0){
			newBucketNb = Rev[List[j]];
			if(left[newBucketNb] == -1)
				left[newBucketNb] = newBucketNb;
			else
				--left[newBucketNb];

			bucket2.setValN(newBucketNb, 1);
			++j;
		}

		newBucketNb = Rev[List[j]];
		if(left[newBucketNb] == -1)
			left[newBucketNb] = newBucketNb;
		else
			--left[newBucketNb];

		bucket2.setValN(newBucketNb, 1);

		i = j + 1;
	}

	for(i = 0; i < inputLength; ++i){
		bucketNb = Rev[i];
		if(bucketNb > -1){
			array2[left[bucketNb]] = i;
			++left[bucketNb];
		}
	}

	delete[] Rev;
	delete[] left;
}

void sort_list_Ltype(int* array2, BoolArray& bucket2, int* List, const BoolArray& bucketList,
						const int inputLength, const int listLength, const int arr2Length)
{
	int* Rev, *right;
	int i, j, newBucketNb, bucketNb;
	int bucketLeft;

	Rev = new int[inputLength];
	right = new int[arr2Length];

	memset(Rev, -1, sizeof(int) * inputLength);
	memset(right, -1, sizeof(int) * arr2Length);

	bucketLeft = 0;

	for(i = 0; i < arr2Length; ++i){
		Rev[array2[i]] = bucketLeft;
		if(bucket2.getValN(i) == 1){
			right[bucketLeft] = i;
			bucketLeft = i + 1;
		}
	}

	i = 0;
	while(i < listLength){
		j = i;

		while(bucketList.getValN(j) == 0){
			--right[Rev[List[j]]];
			++j;
		}

		--right[Rev[List[j]]];

		j = i;
		while(bucketList.getValN(j) == 0){
			newBucketNb = right[Rev[List[j]]] + 1;
			Rev[List[j]] = newBucketNb;
			++j;
		}

		newBucketNb = right[Rev[List[j]]] + 1;
		Rev[List[j]] = newBucketNb;

		j = i;
		while(bucketList.getValN(j) == 0){
			newBucketNb = Rev[List[j]];
			if(right[newBucketNb] == -1)
				right[newBucketNb] = newBucketNb;
			else
				++right[newBucketNb];

			if(newBucketNb > 0)
				bucket2.setValN(newBucketNb - 1, 1);
			++j;
		}


		newBucketNb = Rev[List[j]];
		if(right[newBucketNb] == -1)
			right[newBucketNb] = newBucketNb;
		else
			++right[newBucketNb];

		if(newBucketNb > 0)
			bucket2.setValN(newBucketNb - 1, 1);

		i = j + 1;
	}

	for(i = 0; i < inputLength; ++i){
		bucketNb = Rev[i];
		if(bucketNb > -1){
			array2[right[bucketNb]] = i;
			--right[bucketNb];
		}
	}
	delete[] right;
	delete[] Rev;
}


int* KA_suffixArray(const int* inputData, const int inputLength){

	cout << "int\n";
	BoolArray typeTable(inputLength);
	BoolArray bucket1(inputLength);
	BoolArray bucket2, bucketList;

	int nbStype, nbLtype, maxDist; 
	int* array1, *array2, *tApos;
	int* Dist, *DistCount;
	int* List;
	int* _buffer;
	int listLength;


	getType(inputData, inputLength, nbStype, nbLtype, typeTable);


	if(typeTable.getValN(inputLength - 1) == 1 && nbStype == 1){
		array2 = new int[1];
		array2[0] = inputLength - 1;
		array1 = new int[inputLength];
		SA_build_Stype(array2, nbStype, inputData, inputLength, typeTable, array1);
		delete[] array2;
		return array1;
	}

	array1 = new int[inputLength];
	_buffer = new int[inputLength];
	counting_sort(inputData, inputLength, array1, bucket1, _buffer);
	delete[] _buffer;

	cout << "here?\n";
	if(typeTable.getValN(inputLength - 1) == 1){


#ifndef NDEBUG
    std::cout << "Sorting type S suffixes" << std::endl;
    std::cout << nbStype << " " << nbLtype << " " << inputLength << std::endl;
    cout << "can come here?\n";
#endif


    	cout << "can come here?\n";
		array2 = new int[nbStype];
		bucket2.reset(nbStype);

		build_array2_Stype(array1, inputLength, bucket1, array2, bucket2, typeTable);


		cout << "here2222\n";
		Dist = new int[inputLength];

		s_distance(typeTable, inputLength, Dist, maxDist);

		build_distcount(inputLength, Dist, DistCount, maxDist);

		bucketList.reset(DistCount[maxDist]);

		listLength = DistCount[maxDist];

		List = build_list_Stype(array1, inputLength, Dist, DistCount, maxDist,
									bucket1, bucketList, listLength);

		delete[] DistCount;
		delete[] Dist;


		sort_list_Stype(array2, bucket2, List, bucketList, inputLength, listLength, nbStype);

		delete[] List;

		if(bucket2.isAllTrue()){
			array1 = new int[inputLength];
			SA_build_Stype(array2, nbStype, inputData, inputLength, typeTable, array1);
			delete[] array2;
			return array1;
		}

		tApos = new int[nbStype];

		TApos_build_Stype(array2, nbStype, bucket2, tApos, inputLength, typeTable);
		delete[] array2;

		array2 = KA_suffixArray(tApos, nbStype);
		delete[] tApos;


		rebuild_array2_Stype(array2, nbStype, typeTable, inputLength);
		array1 = new int[inputLength];
		SA_build_Stype(array2, nbStype, inputData, inputLength, typeTable, array1);
		delete[] array2;
		return array1;
	}

	else
	{

#ifndef NDEBUG
    std::cout << "Sorting type L suffixes" << std::endl;
    std::cout << nbStype << " " << nbLtype << " " << inputLength << std::endl;
#endif

    	cout << "ccchere?\n";

		array2 = new int[nbLtype];
		bucket2.reset(nbLtype);

		build_array2_Ltype(array1, inputLength, bucket1, array2, bucket2, typeTable);

		Dist = new int[inputLength];

		l_distance(typeTable, inputLength, Dist, maxDist);
		build_distcount(inputLength, Dist, DistCount, maxDist);

		bucketList.reset(DistCount[maxDist]);
		listLength = DistCount[maxDist];

		List = build_list_Ltype(array1, inputLength, Dist, DistCount, maxDist, bucket1,
									bucketList, listLength);

		delete[] DistCount;
		delete[] Dist;


		sort_list_Ltype(array1, bucket2, List, bucketList, inputLength, listLength, nbLtype);
		delete[] List;

		if(bucket2.isAllTrue()){
			array1 = new int[inputLength];
			SA_build_Ltype(array2, nbLtype, inputData, inputLength, typeTable, array1);
			delete[] array2;
			return array1;
		}

		tApos = new int[nbLtype];

		TApos_build_Ltype(array2, nbLtype, bucket2, tApos, inputLength, typeTable);
		delete[] array2;

		array2 = KA_suffixArray(tApos, nbLtype);
		delete[] tApos;

		rebuild_array2_Ltype(array2, nbLtype, typeTable, inputLength);
		array1 = new int[inputLength];
		SA_build_Ltype(array2, nbLtype, inputData, inputLength, typeTable, array1);
		delete[] array2;
		return array1;
	}
}


int* KA_suffixArray(const unsigned char* inputData, const int inputLength){


	cout << "char\n";
	int* array1, *array2, *tApos;
	int nbStype, nbLtype, maxDist; //maxDist;
	BoolArray typeTable(inputLength);
	BoolArray bucket2;

	// get type
	getType(inputData, inputLength, nbStype, nbLtype, typeTable);


	// this is the simplest situation
	if(typeTable.getValN(inputLength - 1) == 1 && nbStype == 1){

		// array2 is the sorted s suffix
		array2 = new int[1];
		array2[0] = inputLength - 1;
		array1 = new int[inputLength];
		SA_build_Stype(array2, nbStype, inputData, inputLength, typeTable, array1);
		delete[] array2;
		return array1;
	}


	// sort S type suffixes
	//

	if(typeTable.getValN(inputLength - 1) == 1){


#ifndef NDEBUG
    std::cout << "Sorting type S suffixes" << std::endl;
    std::cout << nbStype << " " << nbLtype << " " << inputLength << std::endl;
#endif


		array2 = new int[nbStype];
		bucket2.reset(nbStype);



		reverse_s_distance(typeTable, inputLength, maxDist);


		int i, j = 0;
		for(i = 0; i < inputLength; ++i){
			if(typeTable.getValN(i) == 1){
				array2[j] = i;
				++j;
			}
		}



		sort_s_substrings(inputData, inputLength, array2, bucket2, nbStype, typeTable, maxDist);



		if(bucket2.isAllTrue()){
			array1 = new int[inputLength];
			SA_build_Stype(array2, nbStype, inputData, inputLength, typeTable, array1);
			delete[] array2;
			return array1;
		}

		tApos = new int[nbStype];
		TApos_build_Stype(array2, nbStype, bucket2, tApos, inputLength, typeTable);



		delete[] array2;

		// recursively
		//
		array2 = KA_suffixArray(tApos, nbStype);


		delete[] tApos;

		rebuild_array2_Stype(array2, nbStype, typeTable, inputLength);

		array1 = new int[inputLength];

		SA_build_Stype(array2, nbStype, inputData, inputLength, typeTable, array1);
		delete[] array2;
		return array1;
	}


	else{


#ifndef NDEBUG
    std::cout << "Sorting type L suffixes" << std::endl;
    std::cout << nbStype << " " << nbLtype << " " << inputLength << std::endl;
#endif


		array2 = new int[nbLtype];
		bucket2.reset(nbLtype);

		reverse_l_distance(typeTable, inputLength, maxDist);

		int i, j = 0;

		for(i = 0; i < inputLength; ++i){
			if(typeTable.getValN(i) == 0){
				array2[j] = i;
				++j;
			}
		}

		sort_l_substrings(inputData, inputLength, array2, bucket2, nbLtype, typeTable, maxDist);


		if(bucket2.isAllTrue()){
			array1 = new int[inputLength];
			SA_build_Ltype(array2, nbLtype, inputData, inputLength, typeTable, array1);
			delete[] array2;
			return array1;
		}

		tApos = new int[nbLtype];

		TApos_build_Ltype(array2, nbLtype, bucket2, tApos, inputLength, typeTable);

		delete[] array2;

		array2 = KA_suffixArray(tApos, nbLtype);

		delete[] tApos;

		rebuild_array2_Ltype(array2, nbLtype, typeTable, inputLength);
		array1 = new int[inputLength];

		SA_build_Ltype(array2, nbLtype, inputData, inputLength, typeTable, array1);
		delete[] array2;
		return array1;
	}

}







int main(int argc, char* argv[]){



	ifstream sourceFile(argv[1], ios::in | ios::binary);

	sourceFile.seekg(0, ios::end);
	int size = sourceFile.tellg();
	cout << "size is: "<< size << endl;

	sourceFile.seekg(0, ios::beg);

	unsigned char* inputData = new unsigned char[size + 1];

	sourceFile.read((char*)inputData, size);
	inputData[size] = char(0);



	sourceFile.close();

	int inputLength = size + 1;

	int* A = KA_suffixArray(inputData, inputLength);




	char* toWrite = new char[inputLength - 1];


	for(int i = 1; i < inputLength; ++i){

		if(A[i] - 1 >= 0){

			toWrite[i - 1] = inputData[A[i] - 1];

		}
		else{

			toWrite[i - 1] = inputData[inputLength - 2];
		}
	}

	ofstream writeFile("bwt.txt", ios::binary);

	writeFile.write(toWrite, inputLength - 1);






	delete[] A;


	// BoolArray typeTable(inputLength);
	// typeTable.print(" ");

	// int nbStype, nbLtype;

	// getType(inputData, inputLength, nbStype, nbLtype, typeTable);

	// std::cout << "numS: " << nbStype << std::endl;
	// std::cout << "numL: " << nbLtype << std::endl;
	// typeTable.print(" ");

	// int maxdis;
	// reverse_s_distance(typeTable, inputLength, maxdis);

	// cout << "Max dist:  " << maxdis << "\n";

}



