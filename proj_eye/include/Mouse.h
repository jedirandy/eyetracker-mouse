#ifndef MOUSE_H
#define MOUSE_H

#include <X11/Xlib.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define SINGLE_CLICK_LEFT 1
#define SINGLE_CLICK_RIGHT 2
#define DOUBLE_CLICK 3
#define DRAG 4
#define BUTTON_PRESSED 0
#define BUTTON_RELEASED 1



class Mouse
{
    public:
        virtual ~Mouse();
        void move(int x,int y);
        void action(int actionType);
        static Mouse* getInstance();
    protected:
    private:
        Mouse();
        Display* display;
        Window root;
        XEvent event;
        static Mouse* mouseInstance;
        static int ButtonState;
        void singleClick(int button);
        void doubleClick(int button);
        void initEvent();
        void drag();
};

#endif // MOUSE_H
