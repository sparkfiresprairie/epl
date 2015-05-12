#if !_Window_h
#define _Window_h 1

#include "Color.h"

#if !(NO_WINDOW)
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#endif

class Canvas {
private:
    int width;
    int height;

    unsigned long color_map[8];
    Color color;

#if ! (NO_WINDOW)
    Fl_Double_Window *window;  // defined in Fl_Window.H
#endif 

    int scrn;
    unsigned int plane_mask;

    /* unsigned */ void initcolor(void);
    unsigned initmono(void);

public:
    Canvas(int width = 600, int height = 450);
    ~Canvas() {} // should close the window, yes?

    void set_color(Color);
    void display(void);
    void draw_line(int, int, int, int);
    void draw_point(int x, int y);
    void draw_rectangle(int x1, int y1, int, int, bool = true);
    void flush(void);
    void clear(void);

};

#endif /* ! _Window_h */
