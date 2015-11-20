template <typename T, typename K>
class threadsafe_lookup_table{
public:
	threadsafe_lookup_table(UInt32 size){
		fHashTable = new (T*[size]);
		memset(fHashTable, 0, sizeof(T*)*size);
		fSize = size;
		fMask = fSize - 1;
		if((fMask&fSize) != 0)
			fMask = 0;
		fNumEntries =0;
	}
	~threadsafe_lookup_table(){
		delete [] fHashTable;
	}

	void Add(T *entry){
		
	}

private:
	T **fHashTable;
	UInt32 fSize;
	UInt32 fMask;
	UInt64 fNumEntries;

	UInt32 ComputeIndex( UInt32 hashKey ){
		if(fMask)
			return (hashKey&fMask);
		else
			return (hashKey%fSize);
	}
}