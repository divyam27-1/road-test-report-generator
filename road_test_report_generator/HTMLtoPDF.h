#ifndef HTMLTOPDF_H
#define HTMLTOPDF_H

#include "HTMLtoPDF.h"

#include <QString>

#include <vector>
#include <map>

#include "wkhtmltox/pdf.h"

bool pdf_convert(std::vector<QString> in_paths,
                 std::string outpaths,
                 QString lookups
                 );

#endif // HTMLTOPDF_H
