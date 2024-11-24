#ifndef CSV_WRITER_H
#define CSV_WRITER_H

#include "text_data.h"

#include <string>

class CSVWriter
{
public:
    int writeTextData(const TextData &td, std::string outFileName);
};

#endif
