#include <dinput.h>

struct a {
	int b;
};

int main (void) {
	long ptr = (long)(a*)0;
//	((LONG)&(((DIMOUSESTATE*)0)->lX));
	const long dimofs_x = DIMOFS_X;
	switch(ptr) {
	case dimofs_x:
 	 break;
	}
}
