#include <grab.h>
#include <fstream>

Color toGreyScale(Color color)
{
    unsigned char avg = (30*color.r+40*color.g+30*color.b)/100;
    return (Color){avg,avg,avg,255};
}

void makeScreenshot(const RGBImage* frame_to_save, const char* file_name) 
{
    std::ofstream image;
    image.open(file_name);
    image << "P6\n" << XRES << " " << YRES << " 255\n";
    image.write((char *) frame_to_save->data, frame_to_save->size);
    image.close(); 
}