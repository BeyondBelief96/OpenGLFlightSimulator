#ifndef OGLDEV_STB_IMAGE_H
#define OGLDEV_STB_IMAGE_H

#ifndef _WIN64
#include <unistd.h>
#endif

#include <glm/glm.hpp>

class STBImage {
 public:
    STBImage() {}

    ~STBImage() {
        if (m_imageData) {
            Unload();
        }
    }

    void Load(const char* pFilename);

    void Unload();

    glm::fvec3 GetColor(int x, int y) const;

    int m_width = 0;
    int m_height = 0;
    int m_bpp = 0;
    unsigned char* m_imageData = NULL;
};


#endif
