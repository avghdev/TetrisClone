#include <iostream>
using std::wstring;

#include <Windows.h>

//array of 7 strings, used to represent 7 tetrominos
wstring tetromino[7];   //wstring apparently uses a different Unicode set?
                        //also apparently good practice to use wstring on Windows!

//variables for playing field, size derived from gameboy tetris cells
int nFieldWidth = 12;
int nFieldHeight = 18;
unsigned char * pField = nullptr;   //initialize char array that will be playing field as null pointer
                                    //using unsigned char so numbers actually represent themselves (i.e. 0 won't be converted to some weird character)

//variables for screen size
int nScreenWidth = 120;     //changing this to 120 makes it work for some reason RESEARCH THIS IM CONFUSED
int nScreenHeight = 30;

//Rotation function
int Rotate(int px, int py, int r) {
    //rotation represented by r
    //algorithms derived from rotating a box of width 4
    switch (r) {
        case 0: return py * 4 + px;         //0 degrees
        case 1: return 12 + py - (px * 4);  //90 degrees
        case 2: return 15 - (py * 4) - px;  //180 degrees
        case 3: return 3 - py + (px * 4);   //270 degrees
        default: return py * 4 + px;        //don't rotate if you get an unknown case
                                            //using this instead of the modulo solution used in video
    }
}

int main()
{
    //Create assets
    //put L before string to make sure it's compatible with "wstring" type.
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");

    tetromino[1].append(L"....");
    tetromino[1].append(L"..XX");
    tetromino[1].append(L".XX.");
    tetromino[1].append(L"....");

    tetromino[2].append(L"....");
    tetromino[2].append(L".XX.");
    tetromino[2].append(L".XX.");
    tetromino[2].append(L"....");

    tetromino[3].append(L"....");
    tetromino[3].append(L"XX..");
    tetromino[3].append(L".XX.");
    tetromino[3].append(L"....");

    tetromino[4].append(L"..X.");
    tetromino[4].append(L"..X.");
    tetromino[4].append(L".XX.");
    tetromino[4].append(L"....");

    tetromino[5].append(L".X..");
    tetromino[5].append(L".X..");
    tetromino[5].append(L".XX.");
    tetromino[5].append(L"....");

    tetromino[6].append(L"....");
    tetromino[6].append(L".XXX");
    tetromino[6].append(L"..X.");
    tetromino[6].append(L"....");

    //initialize playing field
    pField = new unsigned char[nFieldWidth * nFieldHeight]; //again, using single dimension arrays
    for (int x = 0; x < nFieldWidth; x++)
        for (int y = 0; y < nFieldHeight; y++)
            //set to 9 if it is on the border, otherwise set to 0
            pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;

    //code to write to screen
    wchar_t * screen = new wchar_t[nScreenWidth * nScreenHeight];
    for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';    //fill with blank space, according to field size
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    //enter game loop
    bool bGameOver = false;

    while (!bGameOver) {

        //Draw Field | offset by two so its not in the corner of the screen
        for (int x = 0; x < nFieldWidth; x++)
            for(int y = 0; y < nFieldHeight; y++)
                screen[(y + 2) * nScreenWidth + (x + 2)] = L" ABCDEFG=#"[pField[y * nFieldWidth + x]];    //add to screen based off of what is inside field array
                                                                                                        //e.g. 0 in pField will be " " and 9 will be "#"!

        //Display Frame
        WriteConsoleOutputCharacterW(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
    }


    return 0;
}
