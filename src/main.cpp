#include "ConfigState.h"
#include "Window.h"

int main() {
    bf::ConfigState configState;
    bf::Window window(configState);
    window.run(configState);
	return 0;
}