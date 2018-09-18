#include <cstdio>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <fstream>

#define LIMIT_1 256
#define LIMIT_2 65536
#define LIMIT_3 16777216


using namespace std;


class bwtEncoder{
public:
	int max_alp;
	int max_num;
	char* s;
	int* sa;
	int* c;
	int n;
	int m;

	bwtEncoder(int fileSize, char* readBuffer){
		cout << "fileSize: " << fileSize << "\n";

		max_alp = 128;

		if(fileSize < 128){
			max_num = 129;
		}
		else
			max_num = fileSize;


		// max_num = 12000050;

		m = 128;
		n = fileSize;
		s = readBuffer;
		sa = new int[n];
		//c = new int[max_num];
	}
	~bwtEncoder(){
		delete[] s;
		delete[] sa;
		//delete[] c;
	}

	void construct_SA_Core(){
		cout << "m is: " << m << " n is: " << n <<" max_num: " << max_num << "\n";
		int* x = new int[max_num];
		int* y = new int[max_num];
		int* c = new int[max_num];


	    for(int i=0; i<m; i++) c[i] = 0;
	    for(int i=0; i<n; i++) c[x[i] = s[i]]++;
	    for(int i=1; i<m; i++) c[i] += c[i-1];
	    for(int i=n-1; i>=0; i--){
	    	// cout << c[x[i]] << "\n";
	    	sa[--c[x[i]]] = i;
	    }
	    // cout<<"x[  ";
	    //     for(int i=0;i<n;i++){
	    //         cout<<x[i]<<"  ";
	    //     }
	    //     cout<<"  ]"<<endl;
	    // cout<<"sa[  ";
	    //     for(int i=0;i<n;i++){
	    //         cout<<sa[i]<<"  ";
	    //     }
	    //     cout<<"  ]"<<endl;
	    for(int k=1; k<=n; k = k<<1)
	    {
	        int p = 0;
	 
	        for(int i=n-k; i<n; i++) y[p++] = i;
	 
	        for(int i=0; i<n; i++) if(sa[i] >= k) y[p++] = sa[i]-k;
	        // cout<<"y[  ";
	        // for(int i=0;i<n;i++){
	        //     cout<<y[i]<<"  ";
	        // }
	        // cout<<"  ]"<<endl;

	        // cout<<"c[  ";
	        // for(int i=0;i<n;i++){
	        //     cout<<c[i]<<"  ";
	        // }
	        // cout<<"  ]"<<endl;


	        for(int i=0; i<m; i++) c[i] = 0;

	        // cout<<"c[  ";
	        // for(int i=0;i<n;i++){
	        //     cout<<c[i]<<"  ";
	        // }
	        // cout<<"  ]"<<endl;

	        for(int i=0; i<n; i++) c[x[y[i]]]++;

	        // cout<<"c[  ";
	        // for(int i=0;i<n;i++){
	        //     cout<<c[i]<<"  ";
	        // }
	        // cout<<"  ]"<<endl;

	        for(int i=1; i<m; i++) c[i] += c[i-1];
	        	
	        // cout<<"c[  ";
	        // for(int i=0;i<n;i++){
	        //     cout<<c[i]<<"  ";
	        // }
	        // cout<<"  ]"<<endl;

	        for(int i=n-1; i>=0; i--){
	        	// cout << ">>";
	        	// cout << c[x[y[i]]] << "  " << y[i] << "\n";
	        	sa[--c[x[y[i]]]] = y[i];
	        }
	 
	        // cout<<"sa[  ";
	        // for(int i=0;i<n;i++){
	        //     cout<<sa[i]<<"  ";
	        // }
	        // cout<<"  ]"<<endl;
	 
	        swap(x,y);
	        p = 1; x[sa[0]] = 0;
	 
	        for(int i=1; i<n; i++)
	            x[sa[i]] = y[sa[i-1]] == y[sa[i]] && y[sa[i-1]+k] == y[sa[i]+k] ? p-1 : p++;
	 
	        // cout<<"x[  ";
	        // for(int i=0;i<n;i++){
	        //     cout<<x[i]<<"  ";
	        // }
	        // cout<<"  ]"<<endl;
	        if(p >= n) break;
	        m = p;
	    }

	    cout << "finished\n";
	}

	void print(){
		for(int i = 0; i < n; ++i)
			cout << sa[i] << ", ";
		cout << "\n";
	}


};

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



void coreFunc(char delimiter,  char* indexFolder, char* sour_file, char* dest_file){


	ifstream fromFile(sour_file, ios::binary);
	fromFile.seekg(0, ios::end);

	int fileSize = fromFile.tellg();

	cout << "file size: " << fileSize << "\n";

	fromFile.seekg(0, ios::beg);

	int alignment;

	if(fileSize < LIMIT_1)
		alignment = 1;
	else if(fileSize >= LIMIT_1 && fileSize < LIMIT_2)
		alignment = 2;
	
	else if(fileSize >= LIMIT_2 && fileSize <= LIMIT_3)
		alignment = 3;
	else
		alignment = 4;





	char* readBuffer = new char[fileSize + 1];

	fromFile.read(readBuffer, fileSize);

	readBuffer[fileSize] = char(0);



	int delimiterCount = 0;

	for(int i = 0; i < fileSize; ++i){
		if(readBuffer[i] == delimiter)
			++delimiterCount;
	}


	int firstPartSize = alignment * 2 + delimiterCount * alignment;
	unsigned char* delimiterBuffer = new unsigned char[firstPartSize];
	int delimiterBufferIndex = 0;

	writeBufferFunc(delimiterBuffer, delimiterBufferIndex, 2, alignment);

	writeBufferFunc(delimiterBuffer, delimiterBufferIndex, delimiterCount, alignment);


	for(int i = 0; i < fileSize; ++i){

		if(readBuffer[i] == delimiter)
			writeBufferFunc(delimiterBuffer, delimiterBufferIndex, i, alignment);

	}

	string position_file = string(indexFolder) + "/" + string(dest_file) + "positionFile.aux";
	
	ofstream toFile(position_file, ios::out | ios::binary);

	cout << "delimiterBufferIndex: " << delimiterBufferIndex << "\n";

	toFile.write((char*)(&delimiterBuffer[0]), delimiterBufferIndex);
	delete[] delimiterBuffer;


	int leftSize = fileSize - delimiterBufferIndex;
	int rowSize = alignment * 2;

	int rowNb = leftSize / rowSize;


	cout << "rowNb: " << rowNb << "\n";
	if(rowNb == 0){

		bwtEncoder encoder(fileSize + 1, readBuffer);
		encoder.construct_SA_Core();

		encoder.print();

		unsigned char* writeBuffer = new unsigned char[fileSize];
		int writeIndex = 0;
		int lastIndex = -1;


		ofstream toFile2(dest_file, ios::out | ios::binary);

		for(int i = 0; i < encoder.n; ++i){
			if(encoder.sa[i] == 0){
				lastIndex = i;
				continue;
				// writeBuffer[writeIndex] = readBuffer[fileSize - 1];
				// ++writeIndex;
			}
			else{
				writeBuffer[writeIndex] = readBuffer[encoder.sa[i] - 1];
				++writeIndex;
			}
		}

		toFile2.write((char*)(&writeBuffer[0]), writeIndex);
		delete[] writeBuffer;

		toFile.seekp(0, ios::beg);

		unsigned char* lastBuffer = new unsigned char[alignment];
		int lastbufferIndex = 0;

		cout << "lastIndex: " << lastIndex << "\n";
		cout << "lastbufferIndex: " << lastbufferIndex << "\n";
		writeBufferFunc(lastBuffer, lastbufferIndex, lastIndex, alignment);

		cout << "lastbufferIndex: " << lastbufferIndex << "\n";
		toFile.write((char*)(&lastBuffer[0]), lastbufferIndex);
		toFile.close();

		return;
	}

	int row2Nb = fileSize / rowNb;

	cout << "leftSize: " << leftSize << "\n";
	cout << "rowSize: " << rowSize << "\n";

	cout << "rowNb: " << rowNb << "\n";
	cout << "row2Nb: " << row2Nb << "\n";

	bwtEncoder encoder(fileSize + 1, readBuffer);
	encoder.construct_SA_Core();

	encoder.print();

	unsigned char* writeBuffer = new unsigned char[rowSize];
	int writeBufferIndex = 0;


	int rowCnt = 0;
	int i;
	for(i = 1; i < encoder.n; ++i){

		if((i + 1) % row2Nb == 0 && rowCnt < rowNb){
			writeBufferIndex = 0;
			writeBufferFunc(writeBuffer, writeBufferIndex, i, alignment);
			writeBufferFunc(writeBuffer, writeBufferIndex, encoder.sa[i], alignment);
			toFile.write((char*)(&writeBuffer[0]), writeBufferIndex);
			++rowCnt;
		}
	}

	delete[] writeBuffer;



	writeBuffer = new unsigned char[fileSize];
	int writeIndex = 0;

	int lastIndex = -1;


	ofstream toFile2(dest_file, ios::out | ios::binary);

	for(int i = 0; i < encoder.n; ++i){
		if(encoder.sa[i] == 0){
			lastIndex = i;
			continue;
			// writeBuffer[writeIndex] = readBuffer[fileSize - 1];
			// ++writeIndex;
		}
		else{
			writeBuffer[writeIndex] = readBuffer[encoder.sa[i] - 1];
			++writeIndex;
		}
	}

	toFile2.write((char*)(&writeBuffer[0]), writeIndex);
	delete[] writeBuffer;

	toFile.seekp(0, ios::beg);

	unsigned char* lastBuffer = new unsigned char[alignment];
	int lastbufferIndex = 0;

	cout << "lastIndex: " << lastIndex << "\n";
	cout << "lastbufferIndex: " << lastbufferIndex << "\n";
	writeBufferFunc(lastBuffer, lastbufferIndex, lastIndex, alignment);

	cout << "lastbufferIndex: " << lastbufferIndex << "\n";
	toFile.write((char*)(&lastBuffer[0]), lastbufferIndex);
	toFile.close();

}




int main(int argc, char *argv[])
{

	char* arg_delimiter = argv[1];
	char* indexFile = argv[2];
	char* sour_file = argv[3];
	char* dest_file = argv[4];

	char delimiter;
	cout << "delimiter is: " << arg_delimiter  << "length is: " << strlen(arg_delimiter) << "\n";

	if(strlen(arg_delimiter) == 2)
		delimiter = char(10);

	else
		delimiter = char(arg_delimiter[0] - char(0));



	// if(delimiter == '\n')
	// 	cout << "haha\n";
	coreFunc(delimiter, indexFile, sour_file, dest_file);
}
