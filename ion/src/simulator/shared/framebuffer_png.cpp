#if EPSILON_SIMULATOR_HAS_LIBPNG

#include "framebuffer.h"
#include <ion/display.h>
#include <stdlib.h>
#include <png.h>

typedef struct {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} pixel_t;

void Ion::Simulator::Framebuffer::writeToFile(const char * filename) {
  FILE * file = fopen(filename, "wb"); // Write in binary mode
  //ENSURE(file != NULL, "Opening file %s", filename);

  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  //ENSURE(png != NULL, "Allocating PNG write structure");

  png_infop info = png_create_info_struct(png);
  //ENSURE(info != NULL, "Allocating info structure");

  png_init_io(png, file);

  png_set_IHDR(png, info,
      Ion::Display::Width, Ion::Display::Height,
      8, // Number of bits per channel
      PNG_COLOR_TYPE_RGB,
      PNG_INTERLACE_NONE,
      PNG_COMPRESSION_TYPE_DEFAULT,
      PNG_FILTER_TYPE_DEFAULT);

  png_write_info(png, info);

  static_assert(sizeof(pixel_t) == 3, "pixel_t shall be 3 bytes long (RGB888 format)");
  pixel_t * row = (pixel_t *)malloc(3*Ion::Display::Width);
  const KDColor * pixels = address();
  for (int j=0;j<Ion::Display::Height;j++) {
    for (int i=0; i<Ion::Display::Width; i++) {
      KDColor c = pixels[i+Ion::Display::Width*j];
      row[i].red = c.red();
      row[i].green = c.green();
      row[i].blue = c.blue();
    }
    png_write_row(png, (png_bytep)row);
  }
  free(row);

  png_write_end(png, NULL);

  png_free_data(png, info, PNG_FREE_ALL, -1); // -1 = all items
  png_destroy_write_struct(&png, (png_infopp)NULL);
  fclose(file);
}

#endif
