#ifndef IRRYGATION_DISPLAY_H
#define IRRYGATION_DISPLAY_H

/*-----( Declare Constants )-----*/
// LCD geometry
const int LCD_COLS = 20;
const int LCD_ROWS = 4;
//

class Display {
private:
    int lcdStatus = 1; // 1 -on, 0-off

public:
    explicit Display();
    void init();
    void print(int col, int row, int digit);
    void print(int col, int row, char *text);
};

#endif //IRRYGATION_DISPLAY_H