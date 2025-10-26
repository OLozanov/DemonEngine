#pragma once

#include <string>

class Image;
class Sound;

Image* LoadBmp(const char* fname);
Image* LoadTga(const char* fname);
Image* LoadDds(const char* fname);
Sound* LoadWav(const std::string& fname);