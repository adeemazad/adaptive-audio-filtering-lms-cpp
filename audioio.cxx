#include "audioio.h"

#include <string>
#include <stdexcept>
#include <sstream>

AudioReader::AudioReader(std::string fname) {
        std::string cmd("sox '" + fname + "' -t dat -c 1 -");
        pipe = popen(cmd.c_str(), "r");
        if (pipe == nullptr)
                throw std::runtime_error("Couldn't open audiofile");
        // The first two lines of the text supplied by sox state the number of
        // channels and the sample rate. Discard them.
        fgets(rdbuf, bufmax, pipe);
        fgets(rdbuf, bufmax, pipe);        
}
        
AudioReader::~AudioReader() {
        if (pipe != nullptr)
                pclose(pipe);
}
        
double AudioReader::get(void) {
        eof_ = (fgets(rdbuf, bufmax, pipe) == nullptr);
        if (!eof_) {
                std::stringstream ss(rdbuf);
                double t, s;
                ss >> t >> s;
                return s;
        } else
                return 0;
}
