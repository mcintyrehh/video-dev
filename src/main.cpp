#include <stdio.h>
#include <GLFW/glfw3.h>

/**
 * Returns false if it can't load a frame
 * Give it a file name and it returns width, height and data for image data
 * */
bool load_frame(const char* filename, int* width, int* height, unsigned char** data);

int main(int argc, const char **argv)
{
    GLFWwindow *window;

    if (!glfwInit())
    {
        printf("Couldn't init GLFW\n");
        return 1;
    }

    window = glfwCreateWindow(640, 480, "Hello Onray!", NULL, NULL);
    //the window that we just opened, thats the window I want to be working on in this thread
    glfwMakeContextCurrent(window);

    if (!window)
    {
        printf("Couldn't open window\n");
        return 1;
    }

    int frame_width, frame_height;
    unsigned char* frame_data;

    if (!load_frame("../build/Wayfinder_MediaHub_Test", &frame_width, &frame_height, &frame_data)) {
        printf("Couldn't load video frame\n");
        return 1;
    }

    //Create texture
    GLuint tex_handle;
    //generate 1 texture, give me back the name of the texture in the handle
    glGenTextures(1, &tex_handle);
    //the next operations that we're going to do on GL_TEXTURE_2D are on this texture we just generated
    glBindTexture(GL_TEXTURE_2D, tex_handle);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    //load in texture data into gl texture image 2d https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame_width, frame_height, 0, GL_RGB, GL_UNSIGNED_BYTE, frame_data);

    while (!glfwWindowShouldClose(window))
    {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Set up orphographic projection
        //Projection matrix describes how things should be projected
        //The other is Model View Matrix, describes how points should be transformed within the space
        // change camera/how things look? Projection View
        // change things in the world after camera is set? Model View
        int window_width, window_height;
        glfwGetFramebufferSize(window, &window_width, &window_height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, window_width, 0, window_height, -1, 1);
        glMatrixMode(GL_MODELVIEW);
    
        // Render whatever you want
        glEnable(GL_TEXTURE_2D); // want to use texture as paintbrush, use it for drawing
        glBindTexture(GL_TEXTURE_2D, tex_handle);
        glBegin(GL_QUADS);
            glTexCoord2d(0,0); glVertex2i(0,0); // lower left corner
            glTexCoord2d(1,0); glVertex2i(frame_width,0); // lower right corner
            glTexCoord2d(1,1); glVertex2i(frame_width, frame_height); // upper right corner
            glTexCoord2d(0,1); glVertex2i(0, frame_height); // upper left corner
        glEnd();
        glDisable(GL_TEXTURE_2D);

        //glfw has a front buffer and a back buffer, we are rendering/drawing these pixels on the back buffer
        //and need to switch them to the front buffer so the user can see them
        glfwSwapBuffers(window);
        //allows you to get a buffer of pixel data and put it on the screen in the most crude way
        glfwWaitEvents();
    }
    return 0;
}