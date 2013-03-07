#include "Binary.h"
#include "Loaders.h"

CG3DBinaryManager* CG3DBinaryLoader::s_Mgr = NULL;

BinaryFP::~BinaryFP()
{
	fseek(m_FP, m_LastOffset, SEEK_SET);
	if (m_Root)
		fclose(m_FP);
}

void BinaryFP::NextFile()
{
	ReadHeader(m_Start + m_Size);
}

void BinaryFP::Set(unsigned int offset)
{
	fseek(m_FP, offset, SEEK_SET);
}

void BinaryFP::ReadHeader(unsigned int offset)
{
	m_LastOffset = ftell(m_FP);
	if(offset == 0xffffffff)
		offset = m_LastOffset;
	fseek(m_FP, offset, SEEK_SET);

	//TODO endian
	m_Start = offset;
	fread(&m_Ident, 1, 4, m_FP);
	fread(&m_Size, sizeof(unsigned int), 1, m_FP);
}

int BinaryFP::Read(unsigned char* data, size_t count)
{
	size_t num = Remaining();
	if (num < count)
		return fread(data, sizeof(unsigned char), num, m_FP);
	else
		return fread(data, sizeof(unsigned char), count, m_FP);
}
unsigned char BinaryFP::ReadUInt8()
{
	if (Remaining() == 0) return 0;
	return (unsigned char)fgetc(m_FP);
}
char BinaryFP::ReadInt8() { return (char)ReadUInt8(); }
bool BinaryFP::ReadBool() { return ReadUInt8() != 0; }
unsigned short BinaryFP::ReadUInt16()
{
	if (Remaining()/sizeof(unsigned short) == 0) return 0;
	unsigned short s;
	//TODO endian
	s  = ((unsigned char)fgetc(m_FP));
	s |= ((unsigned char)fgetc(m_FP)) << 8;
	return s;
}
short BinaryFP::ReadInt16() { return (short)ReadUInt16(); }
unsigned int BinaryFP::ReadUInt32()
{
	if (Remaining()/sizeof(unsigned int) == 0) return 0;
	unsigned int i;
	//TODO endian
	i  = ((unsigned char)fgetc(m_FP));
	i |= ((unsigned char)fgetc(m_FP)) << 8;
	i |= ((unsigned char)fgetc(m_FP)) << 16;
	i |= ((unsigned char)fgetc(m_FP)) << 24;
	return i;
}
int BinaryFP::ReadInt32() { return (int)ReadUInt32(); }
float BinaryFP::ReadFloat()
{
	if (Remaining()/sizeof(unsigned int) == 0) return 0;
	unsigned int i;
	//TODO endian
	i  = ((unsigned char)fgetc(m_FP));
	i |= ((unsigned char)fgetc(m_FP)) << 8;
	i |= ((unsigned char)fgetc(m_FP)) << 16;
	i |= ((unsigned char)fgetc(m_FP)) << 24;
	return *((float*)(&i));
}

std::string BinaryFP::ReadString()
{
	std::string str;

	if (Remaining() <= 0)
		return str;

	char c=fgetc(m_FP);
	while (c!=0 && Remaining()>0) {
		str+=c;
		c=fgetc(m_FP);
	}
	return str;
}

void BinaryFile::Release()
{
	m_RefCount--;
	if (m_RefCount <= 0)
		delete this;
}

CG3DBinaryLoader::CG3DBinaryLoader(CG3DGraphics* graphics) : m_CurrFP(NULL), m_Graphics(graphics)
{
	if (s_Mgr == NULL)
	{
		s_Mgr = new CG3DBinaryManager();
		s_Mgr->SetWeakPtr((CG3DRefCount**)&s_Mgr);
	}
	s_Mgr->AddRef();

	AddHandler(IDENT('T','E','X','T'),new TextureHandler());
	AddHandler(IDENT('E','F','C','T'),new EffectHandler());
	AddHandler(IDENT('M','A','T',' '),new MaterialHandler());
	AddHandler(IDENT('M','O','D','L'),new ModelHandler());
	AddHandler(IDENT('S','C','E','N'),new SceneHandler());
	AddHandler(IDENT('G','O','B',' '),new GOBHandler());
	
	AddModuleHandler("scene.simple",new SimpleSceneHandler());
	AddModuleHandler("dummy",new DummyModuleHandler());
	AddModuleHandler("model",new ModelModuleHandler());
	AddModuleHandler("camera",new CameraModuleHandler());
	AddModuleHandler("frameM",new FrameMModuleHandler());
	AddModuleHandler("framePRS",new FramePRSModuleHandler());
	AddModuleHandler("framePQS",new FramePQSModuleHandler());
	AddModuleHandler("frameIdent",new FrameIdentModuleHandler());
}
CG3DBinaryLoader::~CG3DBinaryLoader()
{
	s_Mgr->Release();
}

BinaryFP* CG3DBinaryLoader::OpenFile(BinaryFile* file, unsigned int offset)
{
	BinaryFP* old = NULL;

	if (m_CurrFP !=NULL && m_CurrFP->m_File == file)
		old = m_CurrFP;
	else
	{
		for (int i=0; i<(int)m_FPStack.size(); i++)
			if (m_FPStack[i] != NULL && m_FPStack[i]->m_File == file) {
				old = m_FPStack[i];
				break;
			}
	}

	m_FPStack.push_back(m_CurrFP);

	if (old != NULL)
		m_CurrFP = new BinaryFP(old, offset);
	else
		m_CurrFP = new BinaryFP(file, offset);
	
	return m_CurrFP;
}

bool CG3DBinaryLoader::NextFile()
{
	if (m_CurrFP == NULL || m_FPStack.empty())
		return false;
	if (m_CurrFP->Remaining() > m_FPStack.back()->Remaining())
		return false;

	m_CurrFP->NextFile();
	return true;
}

BinaryFP* CG3DBinaryLoader::CloseFile()
{
	unsigned int offset = 0xffffffff;
	if (m_CurrFP != NULL)
	{
		offset = m_CurrFP->End();
		delete m_CurrFP;
	}

	if (m_FPStack.empty())
		m_CurrFP = NULL;
	else
	{
		m_CurrFP = m_FPStack.back();
		m_FPStack.pop_back();

		if (offset != 0xffffffff)
			m_CurrFP->Set(offset);
	}
	return m_CurrFP;
}

void CG3DBinaryLoader::CloseAllFiles()
{
	if (m_CurrFP != NULL)
		delete m_CurrFP;

	for (int i=0; i<(int)m_FPStack.size(); i++)
		delete m_FPStack[i];

	m_FPStack.clear();
	m_CurrFP = NULL;
}

bool CG3DBinaryLoader::Error(const char* error, ...)
{
#ifdef G3D_DEBUG
	va_list args;
	static char buf[256];

	va_start(args, error);
	vsprintf(buf, error, args);
	va_end(args);

	CG3DSystem::GetSystem()->Error(__FILE__, __LINE__, buf);
#endif

	CloseAllFiles();
	m_CurrFile->Release();

	return false;
}

bool CG3DBinaryLoader::InitFile(const char* filename)
{
	m_CurrFile = new BinaryFile(filename);

	OpenFile(m_CurrFile, 0);
	if (!m_CurrFP->IsLoaded())
		return Error("Cannot find file");
	
	if (m_CurrFP->m_Ident == IDENT('W','D','3','G'))
		return Error("This platform is big endian, contact the developers!");
	else if (m_CurrFP->m_Ident != IDENT('G','3','D','W'))
		return Error("File does not have the correct header");

	m_CurrFile->m_Header.m_Version = m_CurrFP->ReadUInt16();
	m_CurrFile->m_Header.m_Num = m_CurrFP->ReadUInt16();
	m_CurrFile->m_Header.m_Table = m_CurrFP->ReadUInt32();
	m_CurrFile->m_Header.m_Import = m_CurrFP->ReadUInt32();
	m_CurrFile->m_Header.m_Reloc = m_CurrFP->ReadUInt32();

	if (m_CurrFile->m_Header.m_Import != 0)
		if (!ReadImport())
			return false;
	
	OpenFile(m_CurrFile, m_CurrFile->m_Header.m_Table);
	for (int i=0; i<m_CurrFile->m_Header.m_Num; i++)
	{
		if (!ReadTable())
			return false;
		if (!NextFile())
			return Error("failed to find all tables");
	}

	CloseFile();
	m_CurrFile->Release();

	return true;
}

void CG3DBinaryLoader::AddHandler(unsigned int ident, CG3DBinaryHandler* handler)
{
	m_Handlers[ident] = handler;
	handler->Setup(this);
}
void CG3DBinaryLoader::AddModuleHandler(const char* name, CG3DBinaryHandler* handler)
{
	m_ModuleHandlers[name] = handler;
	handler->Setup(this);
}

bool CG3DBinaryLoader::ReadImport()
{
	OpenFile(m_CurrFile, m_CurrFile->m_Header.m_Import);
	
	if (m_CurrFP->m_Ident != IDENT('I','M','P','T'))
		return Error("File does not have correct import section");

	int num = m_CurrFP->ReadInt32();

	ReadStringTable(m_CurrFile->m_Import, num);

	CloseFile();
	return true;
}

void CG3DBinaryLoader::ReadStringTable(std::vector<std::string>& strings, int num)
{
	char* buffer = new char[m_CurrFP->Remaining()];
	m_CurrFP->Read((unsigned char*)buffer, m_CurrFP->Remaining());

	char* ptr = buffer;
	for (int i=0; i<num; i++)
	{
		std::string str = ptr;

		strings.push_back(str);
		ptr += str.size() + 1;
	}

	delete[] buffer;
}

bool CG3DBinaryLoader::ReadTable()
{	
	if (m_CurrFP->m_Ident != IDENT('T','A','B','L'))
		return Error("File does not have correct table section");

	m_CurrTable = new BinaryTable();
	m_CurrFile->AddRef();
	m_CurrTable->m_Files.push_back(m_CurrFile);

	m_CurrTable->m_Header.m_Ident = m_CurrFP->ReadUInt16();
	m_CurrTable->m_Header.m_Num = m_CurrFP->ReadUInt16();
	m_CurrTable->m_Header.m_Flags = (BinaryTableFlags)m_CurrFP->ReadUInt32();
	m_CurrTable->m_Entries = new BinaryEntry[m_CurrTable->m_Header.m_Num];

	for (int i=0; i<m_CurrTable->m_Header.m_Num; i++)
	{
		m_CurrTable->m_Entries[i].m_FileOffset = m_CurrFP->ReadUInt32();
		m_CurrTable->m_Entries[i].m_File = m_CurrFile;
		m_CurrTable->m_Entries[i].m_Ptr = NULL;
	}
	
	std::vector<std::string> strings;
	if ((m_CurrTable->m_Header.m_Flags & BINARY_HAS_EXPORT) == BINARY_HAS_EXPORT)
	{
		ReadStringTable(strings, m_CurrTable->m_Header.m_Num);
	}

	s_Mgr->UpdateTable(m_CurrTable, strings);

	return true;
}

CG3DRefCount* CG3DBinaryLoader::GetResource(BinaryEntry* entry)
{
	if (entry->m_Ptr == NULL)
	{
		entry->m_Ptr = Load(entry->m_File, entry->m_FileOffset);
		if (entry->m_Ptr != NULL)
			entry->m_Ptr->SetWeakPtr(&entry->m_Ptr);
	}

	return entry->m_Ptr;
}

CG3DRefCount* CG3DBinaryLoader::Load(BinaryFile* file, unsigned int offset)
{
	CG3DRefCount* item = NULL;

	OpenFile(file, offset);
	if (m_Handlers.find(m_CurrFP->m_Ident) != m_Handlers.end())
		item = m_Handlers[m_CurrFP->m_Ident]->Load(m_CurrFP);
	CloseFile();

	return item;
}

CG3DRefCount* CG3DBinaryLoader::GetModule()
{
	std::string type = m_CurrFP->ReadString();

	CG3DRefCount* item = NULL;
	if (m_ModuleHandlers.find(type) != m_ModuleHandlers.end())
		item = m_ModuleHandlers[type]->Load(m_CurrFP);
	return item;
}

CG3DRefCount* CG3DBinaryLoader::GetReference()
{
	BinaryIdent ident;
	if (m_CurrFile->m_Import.empty())
	{
		ident.m_Table = m_CurrFP->ReadUInt16();
		ident.m_Entry = m_CurrFP->ReadUInt16();
	}
	else
	{
		unsigned int offset = m_CurrFP->ReadUInt32();
		ident = s_Mgr->GetIdent(m_CurrFile->m_Import[offset].c_str());
	}

	unsigned int offset = m_CurrFP->End() - m_CurrFP->Remaining();
	CG3DRefCount* result = GetData<CG3DRefCount>(ident);
	m_CurrFP->Set(offset);
	return result;
}

CG3DBinaryManager::CG3DBinaryManager() : m_ID(1)
{
#ifdef G3D_THREADED_LOADER
	m_Mutex = CG3DSystem::GetSystem()->MakeMutex();;
#endif
}

void CG3DBinaryManager::UpdateTable(BinaryTable* table, const std::vector<std::string>& strings)
{
	BinaryTable* oldTable = NULL;
	
#ifdef G3D_THREADED_LOADER
	m_Mutex->Lock();
#endif

	if ((table->m_Header.m_Flags & BINARY_TABLE_GENERATE_IDENT) == BINARY_TABLE_GENERATE_IDENT)
	{
		table->m_Header.m_Ident = m_ID++;
		while (m_Tables.find(table->m_Header.m_Ident) != m_Tables.end())
			table->m_Header.m_Ident = m_ID++;
	}
	else if (m_Tables.find(table->m_Header.m_Ident) != m_Tables.end())
	{
		oldTable = m_Tables[table->m_Header.m_Ident];
		for (int i=0; i<(int)oldTable->m_Files.size(); i++)
			table->m_Files.push_back(oldTable->m_Files[i]);

		for (int i=0; i<std::min(table->m_Header.m_Num, oldTable->m_Header.m_Num); i++)
			if (table->m_Entries[i].m_FileOffset == 0)
			{
				table->m_Entries[i].m_FileOffset = oldTable->m_Entries[i].m_FileOffset;
				table->m_Entries[i].m_File = oldTable->m_Entries[i].m_File;
				table->m_Entries[i].m_Ptr = oldTable->m_Entries[i].m_Ptr;

				if (table->m_Entries[i].m_Ptr != NULL)
					table->m_Entries[i].m_Ptr->SetWeakPtr(&table->m_Entries[i].m_Ptr);
			}
	}
	
	for (int i=0; i<(int)strings.size(); i++)
		m_Strings[strings[i].c_str()] = BinaryIdent(table->m_Header.m_Ident, i);

	m_Tables[table->m_Header.m_Ident] = table;
	
#ifdef G3D_THREADED_LOADER
	m_Mutex->UnLock();
#endif
}

BinaryEntry* CG3DBinaryManager::GetEntry(BinaryIdent ident)
{
	BinaryEntry* entry = NULL;
#ifdef G3D_THREADED_LOADER
	m_Mutex->Lock();
#endif

	if (m_Tables.find(ident.m_Table) != m_Tables.end())
		if (ident.m_Entry < m_Tables[ident.m_Table]->m_Header.m_Num)
			entry = &m_Tables[ident.m_Table]->m_Entries[ident.m_Entry];
	
#ifdef G3D_THREADED_LOADER
	m_Mutex->UnLock();
#endif
	return entry;
}

BinaryIdent CG3DBinaryManager::GetIdent(const char* name)
{
	BinaryIdent ident;
#ifdef G3D_THREADED_LOADER
	m_Mutex->Lock();
#endif

	if (m_Strings.find(name) != m_Strings.end())
		return m_Strings[name];
	
#ifdef G3D_THREADED_LOADER
	m_Mutex->UnLock();
#endif
	return ident;
}