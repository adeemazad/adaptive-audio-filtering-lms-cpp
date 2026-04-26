#ifndef __AUDIOIO_H__
#define __AUDIOIO_H__

// Use low-level C I/O so we can open pipes
#include <stdio.h>

#include <string>

class AudioReader {
public:
        AudioReader(std::string fname);
        ~AudioReader();
        double get(void);
        bool eof(void) { return eof_; };

private:
        FILE* pipe;
        static int constexpr bufmax {64};
        char rdbuf[bufmax];
        bool eof_ { false };
};

#endif
