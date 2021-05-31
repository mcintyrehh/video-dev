#include <stdio.h>
#include <GLFW/glfw3.h>/

int main(int argc, const char **argv)
{
    GLFWwindow *window;

    if (!glfwInit())
    {
        printf("Couldn't init GLFW\n");
        return 1;
    }

    window = glfwCreateWindow(640, 480, "Hello Onray!", NULL, NULL);
    if (!window)
    {
        printf("Couldn't open window\n");
        return 1;
    }

    // create buffer of 100 pixels by 100 pixels, each pixel has 3 bytes of data: red, blue, green
    unsigned char *data = new unsigned char[100 * 100 * 3];
    for (int y = 0; y < 100; ++y)
    {
        for (int x = 0; x < 100; ++x)
        {
            data[y * 100 * 3 + x * 3] = 0xff;
            data[y * 100 * 3 + x * 3 + 1] = 0x00;
            data[y * 100 * 3 + x * 3 + 2] = 0x00;
        }
    }
    //the window that we just opened, thats the window I want to be working on in this thread
    glfwMakeContextCurrent(window);
    while (!glfwWindowShouldClose(window))
    {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawPixels(100, 100, GL_RGB, GL_UNSIGNED_BYTE, data);
        //glfw has a front buffer and a back buffer, we are rendering/drawing these pixels on the back buffer
        //and need to switch them to the front buffer so the user can see them
        glfwSwapBuffers(window);
        //allows you to get a buffer of pixel data and put it on the screen in the most crude way
        glfwWaitEvents();
    }
    return 0;
}