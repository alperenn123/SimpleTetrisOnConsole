#include <iostream>
#include <Windows.h>
#include <thread>
#include <vector>
using namespace std;

static const int nFieldWidth = 12;
static const int nFieldHeight = 18;

static const int nScreenWidth  = 80;
static const int nScreenHeight = 30;
unsigned char* pField = nullptr;
wstring tetromino[7];
int Rotate(int px,int py,int r)
{
	switch(r%4)
	{
    case 0:{return py*4 + px;} // 0 degrees
    case 1:{return 12 + py - (px * 4);} //90 degrees
    case 2:{return 15 - (py * 4) - px;} // 180 degrees
    case 3:{return 3 - py + (px * 4);} //270 degrees
	}
    return 0;
}
bool DoesPieceFit(int nTetromino,int nRotation,int nPosx,int nPosy)
{
    for(int px = 0; px<4; px++)
    {
        for(int py = 0; py<4;py++)
        {
			// Get index into piece
			int pi = Rotate (px,py,nRotation);
			// Get index into field
			int fi = (nPosy + py) * nFieldWidth + (nPosx + px);

			if(nPosx + px >= 0 && nPosx + px < nFieldWidth)
			{
				if(nPosy + py >= 0 && nPosy +  py < nFieldHeight)
				{
					if(tetromino[nTetromino][pi] == L'x' && pField[fi] != 0)
						return false;
				}
			}
		}
	}
	return true;
}
int main()
{
    pField = new unsigned char[nFieldHeight * nFieldWidth];

    for(int x = 0; x<nFieldWidth; x++)
    {
        for(int y = 0; y<nFieldHeight;y++)
        {
            pField[y * nFieldWidth + x] = (x == 0
										   ||x == nFieldWidth - 1
				                           ||y == nFieldHeight - 1 ) ? 9 : 0; 
        }
    }
	// logic for converting to 2d array y*w + x
	tetromino[0].append(L"..x.");
	tetromino[0].append(L"..x.");
	tetromino[0].append(L"..x.");
	tetromino[0].append(L"..x.");

	tetromino[1].append(L"..x.");
	tetromino[1].append(L".xx.");
	tetromino[1].append(L".x..");
	tetromino[1].append(L"....");

	tetromino[2].append(L".x..");
	tetromino[2].append(L".xx.");
	tetromino[2].append(L"..x.");
	tetromino[2].append(L"....");

	tetromino[3].append(L"....");
	tetromino[3].append(L".xx.");
	tetromino[3].append(L".xx.");
	tetromino[3].append(L"....");

	tetromino[4].append(L"..x.");
	tetromino[4].append(L".xx.");
	tetromino[4].append(L"..x.");
	tetromino[4].append(L"....");

	tetromino[5].append(L"....");
	tetromino[5].append(L".xx.");
	tetromino[5].append(L"..x.");
	tetromino[5].append(L"..x.");

	tetromino[6].append(L"....");
	tetromino[6].append(L".xx.");
	tetromino[6].append(L".x..");
	tetromino[6].append(L".x..");

	wchar_t *screen = new wchar_t[nScreenWidth * nScreenHeight];
	for(int i = 0; i < nScreenWidth * nScreenHeight; i++)
		screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,0,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;
	bool bGameOver = false;
	bool is_key_holding = false;
	int nCurrentPiece = 1;
	int nCurrentRotation = 0;
	int nCurrentX =  nFieldWidth / 2;
	int nCurrentY = 0;
	bool bKey[4];
	int nSpeed = 20;
	int nSpeedCounter = 0;
	bool bForceDown = false;
	vector <int> vLines;
	int nPiecesCount = 0;
	int nScore = 0;
	while(!bGameOver)
	{

		this_thread::sleep_for(50ms);
		nSpeedCounter++;
		bForceDown = (nSpeed == nSpeedCounter);
		for (int k = 0; k < 4; k++)								// R   L   D Z
		 bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;

		nCurrentX += (bKey[0] && DoesPieceFit(nCurrentPiece,nCurrentRotation,nCurrentX + 1,nCurrentY)) ? 1 : 0;
		nCurrentX -= (bKey[1] && DoesPieceFit(nCurrentPiece,nCurrentRotation,nCurrentX - 1,nCurrentY)) ? 1 : 0;
		nCurrentY += (bKey[2] && DoesPieceFit(nCurrentPiece,nCurrentRotation,nCurrentX,nCurrentY + 1)) ? 1 : 0;
		// Rotate
		if(bKey[3])
		{
			nCurrentRotation += (!is_key_holding && bKey[3] && DoesPieceFit(nCurrentPiece,nCurrentRotation + 1,nCurrentX,nCurrentY)) ? 1 : 0;
			is_key_holding = true;
		}
		else
		{
			is_key_holding = false;
		}

		if(bForceDown)
		{
			nSpeedCounter = 0;
			if(DoesPieceFit(nCurrentPiece,nCurrentRotation,nCurrentX,nCurrentY +1))
				nCurrentY++;
	
		   else
		   {
			   
			   // lock the tetromino in current location
			   for(int px = 0; px<4; px++)
			   {
				   for(int py = 0; py<4;py++)
				   {
					   // If the tetromino doesn't fit in vertical pos and vertical pos is fill lock the piece in current place
					   if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'x')
							pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;
				   }
			   }

			  for (int py = 0; py < 4; py++)
			  {
				  if(nCurrentY + py < nFieldHeight - 1)
				  {
					  bool is_line = true;
					  for (int px = 1; px < nFieldWidth - 1; px++)
						 is_line &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;

					  if (is_line)
					  {
							
						 for (int px = 1; px < nFieldWidth - 1; px++)
							 pField[(nCurrentY + py) * nFieldWidth + px] = 8;
						 vLines.push_back(nCurrentY + py);
					  }						
				  }
			  }
			  nScore += 25;
			  if(!vLines.empty())nScore += (1 << (vLines.size() - 1)) * 100;			 
			   // Choose the next tetromino
			   nCurrentPiece = rand()% 7;
			   nCurrentRotation = 0;
			   nCurrentX =  nFieldWidth / 2;
			   nCurrentY = 0;
			   // Make the game harder:)
               nPiecesCount++;
			   if(nPiecesCount % 10 == 0)
				   nSpeed--;
			   //if piece does not fit
			   bGameOver = !DoesPieceFit(nCurrentPiece,nCurrentRotation,nCurrentX,nCurrentY);
			   
			   
		   }

	   }
		
		
		for(int x = 0; x<nFieldWidth; x++)
		{
			for(int y = 0; y<nFieldHeight;y++)
			{
				screen[(y+2)*nScreenWidth + (x+2)] = L" ABCDEFG=#"[pField[y*nFieldWidth + x]];
			}
		}
	    for(int px = 0; px < 4; px++)
		{
			for(int py = 0; py < 4;py++)
			{
				if(tetromino[nCurrentPiece][Rotate(px,py,nCurrentRotation)] == L'x')
					screen[(nCurrentY + py + 2)*nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65;// convert to ASCII
			}
		}
		swprintf_s(&screen[2 * nScreenWidth + nFieldWidth + 6], 16, L"SCORE: %8d", nScore);
		if (!vLines.empty())
		{
			// Display Frame
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
			this_thread::sleep_for(400ms); // Delay a bit

			for (auto &v : vLines)
				for (int px = 1; px < nFieldWidth - 1; px++)
				{
					for (int py = v; py > 0; py--)
						pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
					pField[px] = 0;
				}

			vLines.clear();
		}
		WriteConsoleOutputCharacter(hConsole,screen,nScreenWidth*nScreenHeight,{0,0},&dwBytesWritten);
	}
	
	return 0;
}
