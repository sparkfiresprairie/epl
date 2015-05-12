#include <stdio.h>
#include <stdlib.h>

#if !(NO_WINDOW)
#include <FL/fl_draw.H>
#include <FL/Fl_Group.H>
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Window.H>
#include <FL/x.H>
#endif

#if defined(_AIX) && !defined(XLC_IS_STUPID) && !defined(__GNUG__)
#define XLC_IS_STUPID
#include <bool.h>
#endif /* _AIX */

#if DEBUG
# include <iostream>
using namespace std;
#endif /* DEBUG */

#include "Window.h"

/* Allocate colors in an X color map and set them up. */
void Canvas::initcolor()
{
#if !(NO_WINDOW) 

  color_map[BLACK] = FL_BLACK;
  color_map[GREEN] = FL_GREEN;
  color_map[CYAN] = FL_CYAN;
  color_map[MAGENTA] = FL_MAGENTA;
  color_map[RED] = FL_RED;
  color_map[ORANGE] = fl_rgb_color(255, 165, 0);
  color_map[YELLOW] = FL_YELLOW;

#endif /* !(NO_WINDOW) */
}

/*
    Set up the mapping of color numbers into monochrome values.
    Change the raster operations to the logically opposite functions
    if black is pixel #1.
*/
unsigned int Canvas::initmono(void)
{
    color_map[BLACK] = FL_BLACK;
    for (int i = 1; i < 8; i++)
      color_map[i] = FL_WHITE;
    return 0;
}

/*
   Main procedure:  initialize X display, set up monochrome or color
   mapping, create a window, wait for it to be mapped, draw rectangles
   in various forms, and exit.
*/
Canvas::Canvas(int w, int h) : width(w), height(h)
{
#if !(NO_WINDOW)

    window = new Fl_Double_Window(w, h, "LifeForm Simulation");
    initcolor();

#endif /* !(NO_WINDOW) */
}

void Canvas::display(void)
{
#if !(NO_WINDOW)

    window->color(FL_BLACK);
    window->show();
    Fl::wait(0);

#endif
}


void Canvas::set_color(Color x)
{
#if !(NO_WINDOW)

    color = x;

#endif
}

int point_size = 3;
void Canvas::draw_point(int x, int y)
{
    draw_rectangle(x, y, x + point_size, y + point_size, true);
}

void Canvas::flush(void) 
{
#if !(NO_WINDOW)

#if defined (__APPLE__)
    Fl::flush();
#else
    Fl::wait();
#endif

#endif
}


void Canvas::draw_rectangle(int x1, int y1, int x2, int y2, bool fill)
{
#if !(NO_WINDOW)

    Fl_Box *box;
    if (fill) {
        box = new Fl_Box(x1, y1, (x2 - x1), (y2 - y1));
        box->color(color_map[color]);
        box->box(FL_FLAT_BOX);
    }
    else {
        box = new Fl_Box(x1, y1, (x2 - x1), (y2 - y1));
        box->color(color_map[color]);
        box->box(FL_BORDER_FRAME);
    }
    window->add(box);
    box->redraw();

#endif /* !(NO_WINDOW) */
}

class DrawLine : public Fl_Widget {
public:
    DrawLine(int X, int Y, int W, int H, const char*L = 0) : Fl_Widget(X, Y, W, H, L) {
    }
    void draw() {
        // DRAW BLACK 'X'
        fl_color(FL_BLACK);
        int x1 = x(), y1 = y();
        int x2 = x() + w() - 1, y2 = y() + h() - 1;
        fl_line(x1, y1, x2, y2);
        fl_line(x1, y2, x2, y1);
    }
};
void Canvas::draw_line(int x1, int y1, int x2, int y2)
{
#if !(NO_WINDOW)

    DrawLine drawLine(x1, y1, x2, y2);
    window->add(drawLine);
    drawLine.redraw();

#endif /* !(NO_WINDOW) */
}
 

void Canvas::clear(void)
{
#if !(NO_WINDOW)

    set_color(BLACK);
    draw_rectangle(0, 0, width, height, true);

#endif /* !(NO_WINDOW) */
}
