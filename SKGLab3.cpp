//Gill, Simran     CS546 Section 13495 May 1, 2019
//Third Laboratory Assignment - Cache Simulation 

#include<iostream>
#include<fstream>
using namespace std;
void readAthruD(ifstream & in, unsigned& lineSize, unsigned& cacheSize, unsigned& assoc, unsigned& memSize);
void initializeMEM(int ** MEMORY, int size, unsigned LS);
void processLine(ifstream & inFile, struct Cache & cache, int **memory, unsigned memSize);
void parseAddress(unsigned address, unsigned &offset, unsigned & index, unsigned & tagVal);
struct Cache {
	int ***cache;
	unsigned LS, CS, AS, Nsets;
	void initialAR();
	bool **True, **False;
	int **tag;
	int **least_used;
	void write(unsigned address, int value, int **MEMORY);
	void read(unsigned address, int **MEMORY);
};
int main() {
	unsigned LineSize, CacheSize, Assoc, MemSize;
	int **MEMORY;
	ifstream in("input.txt");
	if (!in){
		cout << "Could not open file, terminating program";
		exit(0);
	}

	readAthruD(in, LineSize, CacheSize, Assoc, MemSize);

	int size = MemSize / LineSize;
	MEMORY = new int*[size];
    initializeMEM(MEMORY, size, LineSize);// initialize memory to -1;

	Cache obj;
	obj.LS = LineSize;
	obj.CS = CacheSize;
	obj.AS = Assoc;
	obj.initialAR();

	while (!in.eof())
		processLine(in, obj, MEMORY, MemSize);

	in.close();
	system("pause");
	return 0;
}
void readAthruD(ifstream & in, unsigned& lineSize, unsigned& cacheSize, unsigned& assoc, unsigned& memSize) {
	// Reading in the four primary variables (a-d)
	char test;
	in >> test;
	//READING A
	if (test != 'A') {
		cout << "Parameter A not found." << endl;
		exit(0);
	}
	in >> lineSize;
	if (lineSize <= 0 || lineSize % 2 != 0) {             //must be even number
		cout << "Invalid value "<<lineSize<<" for line size"<<endl;
		exit(0);
	}
	in >> test;
	//READING B
	if (test != 'B') {
		cout << "Parameter B not found" << endl;
		exit(0);
	}
	in >> cacheSize;
	if (cacheSize <= 0 || cacheSize % 2 != 0) {
		cout << "Invalid value " << cacheSize << " for cache size" << endl;
		exit(0);
	}
	if (lineSize > cacheSize / 10) {
		cout << "Line size " << lineSize << "too large"  << endl;
		exit(0);
	}
	in>> test;
	//READING C
	if (test != 'C') {
		cout << "Parameter C not found" << endl;
		exit(0);
	}
	in >> assoc;
	if (assoc <= 0 || assoc % 2 != 0) {
		cout << "Invalid value " << assoc << " for associativity" << endl;
		exit(0);
	}
	in >> test;
	//READING D
	if (test != 'D') {
		cout << "Parameter D not found" << endl;
		exit(0);
	}
	in>> memSize;
	if (memSize <= 0 || memSize % 2 != 0) {
		cout << "Invalid value " << memSize << " for memory size." << endl;
		exit(0);
	}
	if (cacheSize > memSize / 10) {
		cout << "Cache size: " << cacheSize << " too large" << endl;
		exit(0);
	}
}
void initializeMEM(int **MEMORY,int size,unsigned LS) {
	for (int i = 0; i < size; i++) {
		MEMORY[i] = new int[LS / sizeof(int)]();
		for (unsigned j = 0; j < LS / sizeof(int); j++) {
			MEMORY[i][j] = -1;
		}
	}
}
void Cache:: initialAR() {
	
	Nsets = CS / LS / AS;

	True = new bool*[Nsets];
	for (unsigned i = 0; i < Nsets; ++i)
	{
		True[i] = new bool[AS]();
	}

	False = new bool*[Nsets];
	for (unsigned i = 0; i < Nsets; ++i)
		False[i] = new bool[AS]();

	tag = new int*[Nsets];
	for (unsigned i = 0; i < Nsets; ++i) {
		tag[i] = new int[AS];
		for (unsigned j = 0; j < AS; ++j)
			tag[i][j] = -1;
	}

	least_used = new int*[Nsets];
	for (unsigned i = 0; i < Nsets; ++i)
		least_used[i] = new int[AS]();

	cache = new int**[Nsets];
	for (unsigned i = 0; i < Nsets; ++i) {
		cache[i] = new int *[AS];
		for (unsigned j = 0; j < AS; ++j)
			cache[i][j] = new int[LS / sizeof(int)];
	}
}
void processLine(ifstream & in, struct Cache  & obj, int **MEMORY, unsigned MemSize) {
	char test;
	unsigned address, value;
	in >> test;
	if (test == 'E'){
		in >> address;
		if (address > MemSize - 1) {
			cout << "Input address " << address << " invalid";

			in.ignore(255, '\n');//////////////
		}
		else {
			in.ignore(3);
			in >> value;
			obj.write(address, value, MEMORY);
		}//else
	}//if E
	else if (test == 'F') {
		in >> address;
		if (address > MemSize - 1) {
			cout << "Input address " << address << " invalid";

			in.ignore(255, '\n');//////////////
		}
		else obj.read(address, MEMORY);
	}//if F
	else
	{
		cout << "Invalid value for instrution";
		exit(0);
	}


}
void Cache::write(unsigned address, int value, int **MEMORY) {

	unsigned offset, index, tagVal;
	bool inCache = false;
	bool indexFull = true;

	parseAddress(address, offset, index, tagVal);

	for (unsigned i = 0; i < AS; i++) {
		if (tag[index][i] == tagVal && True[index][i] == true) {
			inCache = true;
			cache[index][i][offset] = value;
			least_used[index][i]++;
			False[index][i] = true;
			break;
		}
		else if (tag[index][i] == -1) {
			indexFull = false;
		}
	}

	if (!inCache && !indexFull) {
		for (unsigned i = 0; i < AS; i++) {
			if (tag[index][i] == -1) {
				tag[index][i] = tagVal;
				for (unsigned j = 0; j < LS / sizeof(int); j++) {
					cache[index][i][j] = MEMORY[address / LS][j];
				}
				cache[index][i][offset] = value;
				least_used[index][i]++;
				True[index][i] = true;
				False[index][i] = true;
				break;
			}
		}
	}
	else if (!inCache) {
		int leastUsed = least_used[index][0];
		int rep = 0;
		for (unsigned i = 1; i < AS; i++) {
			if (least_used[index][i] < leastUsed) {
				leastUsed = least_used[index][i];
				rep = i;
			}
		}

		if (False[index][rep] == true && True[index][rep] == true) {
			for (unsigned i = 0; i < LS / sizeof(int); i++) {
				MEMORY[((tag[index][rep] << 10) | (index << 4)) / LS][i] = cache[index][rep][i];
			}
			//cout << "Write: Writing back to memory during address: " << address << endl;
		}

		tag[index][rep] = tagVal;
		for (unsigned j = 0; j < LS / sizeof(int); j++) {
			cache[index][rep][j] = MEMORY[address / LS][j];
		}
		cache[index][rep][offset] = value;
	    least_used[index][rep]++;
		True[index][rep] = true;
		False[index][rep] = true;
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	/*	unsigned offset, index, tagV;
	bool inside = false;
	bool full = true;
	parseAddress(address, offset, index, tagV);
	for (unsigned i = 0; i < AS; ++i) {
		if (tag[index][i] == tagV && True[index][i] == true) {
			inside = true;
			cache[index][i][offset] = value;
			least_used[index][i]++;
			False[index][i] = true;
			break;
		}
		else if (tag[index][i] = -1)
			full = false;
	}//for1

	if (!inside && !full) {
		for (unsigned i = 0; i < AS; ++i) {
			if (tag[index][i] = -1) {
				tag[index][i] = tagV;
				for (unsigned j = 0; j < LS / sizeof(int); ++j)
					cache[index][i][j] = MEMORY[address / LS][j];
				cache[index][i][offset] = value;
				least_used[index][i]++;
				True[index][i] = true;
				False[index][i] = true;
				break;
			}//if2
		}//for1

	}//if1

	else if (!inside) {
		int LU = least_used[index][0];
		int counter = 0;
		for (unsigned i = 1; i < AS; ++i) {
			if (least_used[index][i] < LU) {
				LU = least_used[index][i];
				counter = 1;
			}

		}//for1
		if (False[index][counter] == true && True[index][counter] == true) {
			for (unsigned i = 0; i < LS / sizeof(int); ++i) {
				int v = ((tag[index][counter] << 10) | (index << 4)) / LS;
				MEMORY[v][i] = cache[index][counter][i];
			}//for
		}//if1

		tag[index][counter] = tagV;
		for (unsigned i = 0; i < LS / sizeof(int); ++i)
			cache[index][counter][i] = MEMORY[address / LS][i];
		cache[index][counter][offset] = value;
		least_used[index][counter]++;
		True[index][counter] = true;
		False[index][counter] = true;




	}//else1          */

}
void Cache::read(unsigned address, int **MEMORY) {

	unsigned offset, index, tagV;
	bool inside = false, full = true;
	parseAddress(address, offset, index, tagV);
	for (unsigned i = 0; i < AS; ++i) {
		if (tag[index][i] == tagV && True[index][i] == true) {
			inside = true;
			cout << "Address: " << address << "\tMemory: " << MEMORY[address / LS][offset] << "\tCache: " << cache[index][i][offset] << endl;
			least_used[index][i]++;
		}//if1
		else if (tag[index][i] == -1)
			full = false;
	}//for1

	if (!inside && !full) {
		for (unsigned i = 0; i < AS; ++i) {
			if (tag[index][i] == -1) {
				tag[index][i] == tagV;
				for (unsigned j = 0; j < LS / sizeof(int); ++j)
					cache[index][i][j] = MEMORY[address / LS][j];
				cout << "Address: " << address << "\tMemory: " << MEMORY[address / LS][offset] << "\tCache: " << cache[index][i][offset] << endl;
				least_used[index][i]++;
				True[index][i] = true;
				False[index][i] = false;
				break;
			}//if

		}//for1
	}//if

	else if (!inside){
	int LU = least_used[index][0];
	int counter = 0;
	for (unsigned i = 1; i < AS; ++i) {
		if (least_used[index][i] < LU) {
			LU = least_used[index][i];
			counter = 1;
		}//if 1
	}//for1
	if (False[index][counter] == true && False[index][counter] == true) {
		for (unsigned i = 0; i < LS / sizeof(int); ++i) {
			int v = ((tag[index][counter] << 10) | (index << 4)) / LS;
			MEMORY[v][i] = cache[index][counter][i];
		  }//for2
	}//if2

	tag[index][counter] = tagV;
	for (unsigned i = 0; i < LS / sizeof(int); ++i) {
		cache[index][counter][i] = MEMORY[address / LS][i];
	}
	cout << "Address: " << address << "\tMemory: " << MEMORY[address / LS][offset] << "\tCache: " << cache[index][counter][offset] << endl;
	least_used[index][counter]++;
	True[index][counter] = true;
	False[index][counter] = false;
 
	}//else

}
void parseAddress(unsigned address, unsigned &offset, unsigned & index, unsigned & tagVal) {
	const unsigned offsetSize = 4;
	const unsigned cacheIndexSize = 6;

	offset = address << (sizeof(unsigned) * 8 - offsetSize);
	offset >>= (sizeof(unsigned) * 8 - offsetSize);
	offset /= 4;

	index = address << (sizeof(unsigned) * 8 - (offsetSize + cacheIndexSize));
	index >>= (sizeof(unsigned) * 8 - cacheIndexSize);

	tagVal = address >> (offsetSize + cacheIndexSize);
}