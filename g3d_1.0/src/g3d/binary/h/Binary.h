#ifndef G3D_HELPER_BINARY_H
#define G3D_HELPER_BINARY_H

#include <string>
#include <vector>
#include <map>
#include <stdio.h>

#include "G3DCore.h"

// This loads data from a set of binary files that contain a collection of blocks of data
// The main interface to this is CG3DBinaryLoader

//FOURCC style identifier
#define IDENT(a, b, c, d) (((unsigned int)a) | (((unsigned int)b)<< 8) | (((unsigned int)c)<<16) | (((unsigned int)d)<<24))

class CG3DBinaryLoader;

//This is a reference from one piece of binary data to another in any binary file
struct BinaryIdent {
	unsigned short m_Table;	//GUID of the table the data is in
	unsigned short m_Entry;	//entry in that table

	BinaryIdent(unsigned short table, unsigned short entry) : m_Table(table), m_Entry(entry) {}
	BinaryIdent() : m_Table(0), m_Entry(0) {}
};

//header of the whole binary file
struct BinaryHeader {
	unsigned short m_Version;
	unsigned short m_Num;		//number of tables
	unsigned int   m_Table;		//absolute offset to first table
	unsigned int   m_Import;	//absolute offset to import list
	unsigned int   m_Reloc;		//absolute offset to relocation list
};

enum BinaryTableFlags {
	BINARY_TABLE_GENERATE_IDENT = 1,	//table GUID has not been assigned, generate one
	BINARY_TABLE_PATCH = 2,				//this table patches one with the same GUID
	BINARY_HAS_EXPORT = 4,				//has list of absolute names of the table items
};

//header of an individual table
struct BinaryTableHeader {
	unsigned short   m_Ident;			//table GUID (or 0)
	unsigned short   m_Num;				//number of entries
	BinaryTableFlags m_Flags;
};

//information about an individual file
class BinaryFile {
public:
	std::string m_FileName;
	std::vector<std::string> m_Import;	//if we have this then references are a uint index into this table, else a BinaryIdent
	int m_RefCount;
	BinaryHeader m_Header;
	bool m_BigEndian;	//currently unused
public:
	BinaryFile(const char* filename) : m_RefCount(1), m_FileName(filename), m_BigEndian(false) {}
	void AddRef() { m_RefCount++; }
	void Release();
};

//this is an actively open file, used during loading
class BinaryFP {
public:
	BinaryFile* m_File;
	unsigned int m_Ident;		//FOURCC ident of the current file section
private:
	FILE* m_FP;
	bool m_Root;
	unsigned int m_LastOffset;
	unsigned int m_Start;
	unsigned int m_Size;
public:
	BinaryFP(BinaryFile* file, unsigned int offset) : m_File(file), m_FP(fopen(file->m_FileName.c_str(), "rb")), m_Root(true) { ReadHeader(offset); }
	BinaryFP(BinaryFP* fp, unsigned int offset) : m_File(fp->m_File), m_FP(fp->m_FP), m_Root(false) { ReadHeader(offset); }
	~BinaryFP();

	bool IsLoaded() { return m_FP != NULL; }

	//read values from the file
	unsigned char ReadUInt8();
	char ReadInt8();
	bool ReadBool();
	unsigned short ReadUInt16();
	short ReadInt16();
	unsigned int ReadUInt32();
	int ReadInt32();
	float ReadFloat();
	std::string ReadString();
	int Read(unsigned char* data, size_t count);

	int Remaining() { return m_Size - (ftell(m_FP) - m_Start); }	//amount left to read in this section
	unsigned int End() { return m_Start + m_Size; }					//absolute position of the end
	void NextFile();												//move on to the next section
	void Set(unsigned int offset);									//set absolute position
private:
	void ReadHeader(unsigned int offset);
};

//entry in the table
class BinaryEntry {
public:
	unsigned int m_FileOffset;	//absolute offset in the file the data starts
	BinaryFile* m_File;			//file the data is in
	CG3DRefCount* m_Ptr;		//pointer to the data if it is loaded
};

//table of entries
class BinaryTable {
public:
	std::vector<BinaryFile*> m_Files;	//list of all files referenced (main and patches)
	BinaryTableHeader m_Header;			//file header
	BinaryEntry* m_Entries;				//array of entries
};

//slave class, do not instantiate
class CG3DBinaryManager : public CG3DRefCount {
	friend class CG3DBinaryLoader;
public:
	std::map<unsigned short, BinaryTable*> m_Tables;	//dictionary of all tables, referenced by GUID
	std::map<std::string, BinaryIdent> m_Strings;		//map of global names to idents
private:
	unsigned short m_ID;
#ifdef G3D_THREADED_LOADER
	CG3DPtr<CG3DMutex> m_Mutex;
#endif
public:
	CG3DBinaryManager();
private:
	void UpdateTable(BinaryTable* table, const std::vector<std::string>& strings);
	BinaryEntry* GetEntry(BinaryIdent ident);
	BinaryIdent GetIdent(const char* name);
};

//base class for all file section handlers
class CG3DBinaryHandler {
protected:
	CG3DBinaryLoader* m_Loader;
public:
	void Setup(CG3DBinaryLoader* loader) { m_Loader = loader; }
	virtual CG3DRefCount* Load(BinaryFP* fp) = 0;	//override to load data
};

// main interface to the binary classes, one per thread
class CG3DBinaryLoader : public CG3DRefCount {
private:
	struct Header {
		char type[4];
		unsigned int size;
	};
private:
	static CG3DBinaryManager* s_Mgr;

	BinaryFP* m_CurrFP;
	std::vector<BinaryFP*> m_FPStack;

	BinaryFile* m_CurrFile;
	BinaryTable* m_CurrTable;

	std::map<unsigned int, CG3DBinaryHandler*> m_Handlers;
	std::map<std::string, CG3DBinaryHandler*> m_ModuleHandlers;
public:
	CG3DPtr<CG3DGraphics> m_Graphics;
public:
	CG3DBinaryLoader(CG3DGraphics* graphics);
	~CG3DBinaryLoader();
	void AddHandler(unsigned int ident, CG3DBinaryHandler* handler);	//add a file section handler for a particular FOURCC identifier
	void AddModuleHandler(const char* name, CG3DBinaryHandler* handler);	//add a handler for a sub part, for instance modules

	bool InitFile(const char* filename);	//add a binary file to the system

	//load data from disk (or retreive from memory if previously loader)
	template<class T> T* GetData(BinaryIdent ident) { return (T*)GetResource(s_Mgr->GetEntry(ident)); }
	template<class T> T* GetData(unsigned int ident) { return GetData<T>(BinaryIdent(ident>>16, ident&0xffff)); }
	template<class T> T* GetData(const char* name) { return GetData<T>(s_Mgr->GetIdent(name)); }

	//read a reference ident then load the data referenced
	CG3DRefCount* GetReference();
	//read a module string then load the module referenced
	CG3DRefCount* GetModule();
	
	//read a sub file section's header, this is shack based
	BinaryFP* OpenFile() { return OpenFile(m_CurrFile,0xffffffff); }
	bool NextFile();	//move on to the next sub file section (finishing with the previous sub file section)
	BinaryFP* CloseFile();	//finish with a sub file section
private:
	bool ReadImport();
	bool ReadTable();
	
	CG3DRefCount* GetResource(BinaryEntry* entry);
	CG3DRefCount* Load(BinaryFile* file, unsigned int offset);
	
	BinaryFP* OpenFile(BinaryFile* file, unsigned int offset);
	void CloseAllFiles();
	
	void ReadStringTable(std::vector<std::string>& strings, int num);
	bool Error(const char* error, ...);
};

#endif