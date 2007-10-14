#include <X11/Xlib.h>

int main(void)
{
  int screen;
  Display *display;
  int depth;
  Window window;

  display = XOpenDisplay(NULL); 
  screen = DefaultScreen(display);   
  int width=320;
  int height=200;
  window = XCreateSimpleWindow(display,RootWindow(display,0),
			       0,0,width,height,
			       0/*border_width*/,
			       0/*bordercolor*/,
			       3/*backgroundcolor*/);
  XSelectInput(display,window,ExposureMask|ButtonPressMask);

  XMapWindow(display,window);
  XFlush(display);  
}
