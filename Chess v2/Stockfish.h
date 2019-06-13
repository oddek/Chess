#pragma once
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <string>

STARTUPINFO sti = { 0 };
SECURITY_ATTRIBUTES sats = { 0 };
PROCESS_INFORMATION pi = { 0 };
HANDLE pipin_w, pipin_r, pipout_w, pipout_r;
BYTE buffer[8192];
DWORD writ, excode, read, available;


void ConnectToEngine(char* path)
{
	pipin_w = pipin_r = pipout_w = pipout_r = NULL;
	sats.nLength = sizeof(sats);
	sats.bInheritHandle = TRUE;
	sats.lpSecurityDescriptor = NULL;

	CreatePipe(&pipout_r, &pipout_w, &sats, 0);
	CreatePipe(&pipin_r, &pipin_w, &sats, 0);

	sti.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	sti.wShowWindow = SW_HIDE;
	sti.hStdInput = pipin_r;
	sti.hStdOutput = pipout_w;
	sti.hStdError = pipout_w;

	CreateProcess(NULL, path, NULL, NULL, TRUE, 0, NULL, NULL, &sti, &pi);
}


std::string getNextMove(std::string position)
{
	std::string move;
	std::string command = "position fen " + position + "\ngo\n";
	
	WriteFile(pipin_w, command.c_str(), command.length(), &writ, NULL);
	Sleep(500);
	PeekNamedPipe(pipout_r, buffer, sizeof(buffer), &read, &available, NULL);
	do
	{
		ZeroMemory(buffer, sizeof(buffer));
		if (!ReadFile(pipout_r, buffer, sizeof(buffer), &read, NULL) || !read) break;
		buffer[read] = 0;
		move += (char*)buffer;
	} while (read >= sizeof(buffer));

	int n = move.find("bestmove");
	if (n != -1) return move.substr(n + 9, 4);
	//Merkelig tekst fra stockfish som blir lest når det er mulig med matt osv. 
	//I stockfish.exe er det ikke slik, så skjønner ingenting. 
	n = move.find(" pv ");
	if (n != -1) return move.substr(n + 4, 4);

	return "error";
}

double get_eval(std::string position)
{
	std::string output;
	std::string command = "position fen " + position + "\neval\n";
	WriteFile(pipin_w, command.c_str(), command.length(), &writ, NULL);
	Sleep(100);

	PeekNamedPipe(pipout_r, buffer, sizeof(buffer), &read, &available, NULL);
	do
	{
		ZeroMemory(buffer, sizeof(buffer));
		if (!ReadFile(pipout_r, buffer, sizeof(buffer), &read, NULL) || !read) break;
		buffer[read] = 0;
		output += (char*)buffer;
	} while (read >= sizeof(buffer));

	int n = output.find("Total evaluation: ");
	if (n != -1) output = output.substr(n + 18, 5);
	std::string::size_type sz;
	double eval = std::stod(output, &sz);
	return eval;

}


void CloseConnection()
{
	WriteFile(pipin_w, "quit\n", 5, &writ, NULL);
	if (pipin_w != NULL) CloseHandle(pipin_w);
	if (pipin_r != NULL) CloseHandle(pipin_r);
	if (pipout_w != NULL) CloseHandle(pipout_w);
	if (pipout_r != NULL) CloseHandle(pipout_r);
	if (pi.hProcess != NULL) CloseHandle(pi.hProcess);
	if (pi.hThread != NULL) CloseHandle(pi.hThread);
}