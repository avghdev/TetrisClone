#include <iostream>
#include <thread>
#include <vector>

using namespace std;

//include required for working w/ screen buffer / Windows ONLY unfortunately :(
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
int nScreenWidth = 120;     //my default screen width on my computer holds 120 characters so have to make this 120 or else it wont work
int nScreenHeight = 30;

//Rotation function
int Rotate(int px, int py, int r) {
    //rotation represented by r
    //algorithms derived from rotating a box of width 4
    switch (r % 4) {                        //using modulo 4 so when we reach 5 rotates itll go back to original rotation
        case 0: return py * 4 + px;         //0 degrees
        case 1: return 12 + py - (px * 4);  //90 degrees
        case 2: return 15 - (py * 4) - px;  //180 degrees
        case 3: return 3 - py + (px * 4);   //270 degrees
        default: return py * 4 + px;
    }
}

//collision detection function
bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY) {

    //loop through nTetromino grid (4x4)
    for (int px = 0; px < 4; px++) {
        for (int py = 0; py < 4; py++) {
            //get index into piece
            int pi = Rotate(px, py, nRotation);

            //get index into field
            int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

            //make sure that piece index is within the field
            if (nPosX + px >= 0 && nPosX + px < nFieldWidth) {
                if (nPosY + py >= 0 && nPosY + py < nFieldHeight) {
                    //basically, if the piece we are checking is not an empty space
                    //AND if we hit something, return false (can't place a piece here)
                    if (tetromino[nTetromino][pi] == L'X' && pField[fi] != 0)
                        return false; //fail on first hit
                }
            }
        }
    }

    return true;
}

int main()
{
    //Create assets
    //put L before string to make sure it's compatible with "wstring" type.
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");

    tetromino[1].append(L"..X.");
    tetromino[1].append(L".XX.");
    tetromino[1].append(L".X..");
    tetromino[1].append(L"....");

    tetromino[2].append(L"....");
    tetromino[2].append(L".XX.");
    tetromino[2].append(L".XX.");
    tetromino[2].append(L"....");

    tetromino[3].append(L".X..");
    tetromino[3].append(L".XX.");
    tetromino[3].append(L"..X.");
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

    //Game Logic Stuff
    bool bGameOver = false;

    //variables for storing information about the current piece
    int nCurrentPiece = 1;
    int nCurrentRotation = 0;
    int nCurrentX = nFieldWidth / 2;
    int nCurrentY = 0;

    //get keys
    bool bKey[5];
    //flag makes sure rotate button can't be held down
    bool bRotateHold = false;

    //game tick variables (moves piece down)
    int nSpeed = 20;
    int nSpeedCounter = 0;
    bool bForceDown = false;

    //variable to keep track of how many pieces have been placed
    int nPieceCount = 0;

    //score variable
    int nScore = 0;

    vector<int> vLines;

    while (!bGameOver) {

        //GAME TIMING =======================
        //makes the thread wait for 50ms (a game tick is 50ms long then)
        //ensures game experience is same for everyone
        this_thread::sleep_for(50ms);
        nSpeedCounter++;
        bForceDown = (nSpeedCounter == nSpeed);

        //INPUT =============================
        for (int k = 0; k < 5; k++)                             //R   L   D   U
            bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28\x26Z"[k]))) != 0;

        //GAME LOGIC ========================
        nCurrentX += (bKey[0] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0;
        nCurrentX -= (bKey[1] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX -1 , nCurrentY)) ? 1 : 0;
        nCurrentY += (bKey[2] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1 : 0;
        //up movement for shits and giggles
        //nCurrentY -= (bKey[3] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY - 1)) ? 1 : 0;

        //rotation
        if (bKey[4]) {
            nCurrentRotation += (!bRotateHold && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0;
            bRotateHold = true;
        }
        else {
            //we are not holding down the key anymore so reset flag (allowing us to rotate again)
            bRotateHold = false;
        }

        //forcing piece down
        if (bForceDown) {
            if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
                nCurrentY++;
            else {
                // Lock current piece to field
                for (int px = 0; px < 4; px++)
                    for (int py = 0; py < 4; py++)
                        if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
                            pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;

                //increase piece counter once locked to field
                nPieceCount++;
                //if 10 pieces have been placed, decrease speed var (which will actually make things drop down faster)
                if(nPieceCount % 10 == 0)
                    if (nSpeed >= 10) nSpeed--;


                // Check if we have any lines (only need to check the 4 lines the tetronimo just placed resides on
                for (int py = 0; py < 4; py++)
                    if (nCurrentY + py < nFieldHeight - 1) {
                        bool bLine = true;
                        for (int px = 1; px < nFieldWidth - 1; px++)
                            bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;
                        if (bLine) {
                            //if we have a line, set everything in the line to "="
                            for (int px = 1; px < nFieldWidth - 1; px++)
                                pField[(nCurrentY + py) * nFieldWidth + px] = 8;
                            vLines.push_back(nCurrentY + py);   //need to review what exactly this does
                        }
                    }

                nScore += 25;
                //if there are lines, add to score exponentially based on how many lines (uses left shift operator to do this)
                //basically shifts bits over one, which means it'll be 2^(vLines.size()) * 100
                //so 1 line will give you an additional 200 points, but 4 will give you an additional 1600
                if (!vLines.empty()) nScore += (1 << vLines.size()) * 100;

                // Choose next piece
                nCurrentX = nFieldWidth / 2;
                nCurrentY = 0;
                nCurrentRotation = 0;
                nCurrentPiece = rand() % 7;


                // if piece does not fit
                bGameOver =  !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
            }

            nSpeedCounter = 0;
        }

        //RENDER OUTPUT =====================

        //Draw Field | offset by two so its not in the corner of the screen
        for (int x = 0; x < nFieldWidth; x++)
            for(int y = 0; y < nFieldHeight; y++)
                screen[(y + 2) * nScreenWidth + (x + 2)] = L" ABCDEFG=#"[pField[y * nFieldWidth + x]];  //add to screen based off of what is inside field array
                                                                                                        //e.g. 0 in pField will be " " and 9 will be "#"!

        //Draw Current Piece
        for (int px = 0 ; px < 4; px++) {
            for (int py = 0; py < 4; py++) {
                //check to see if the space currently being checked is not empty
                if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
                    //the plus 2 account for our screen offset | Adding 65 to the current piece turns it
                    //into "A,B,C,D,E,F,G" in ASCII depending on the piece
                    screen[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65;
            }

        }

        //Draw Score (need to review this too)
        swprintf_s(&screen[2 * nScreenWidth + nFieldHeight + 6], 16, L"SCORE: %8d", nScore);

        //CODE TO BE REVIEWED ========================

        //if we have got lines we need to update
        if (!vLines.empty()) {
            //Display Frame
            WriteConsoleOutputCharacterW(hConsole, screen, nScreenWidth * nScreenHeight, {0,0}, &dwBytesWritten);
            this_thread::sleep_for(400ms); //Delay so player has time to see that they got a line!

            //review this code!
            for (auto &v : vLines)
                for(int px = 1; px < nFieldWidth - 1; px++){
                    for (int py = v; py > 0; py--)
                        pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
                    pField[px] = 0;
                }

            vLines.clear();

        }

        //Display Frame | add the WriteConsoleOutputCharacterW bc it doesn't work w/ wchars if you don't (tutorial is from 2017, must've been an update)
        WriteConsoleOutputCharacterW(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
    }

    //Game Over!
    CloseHandle(hConsole);
    cout << "Game Over!! Score: " << nScore << endl;
    system("pause"); //pause so console doesn't close

    return 0;
}
