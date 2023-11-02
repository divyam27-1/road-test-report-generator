#ifndef HTMLTOPDF_H
#define HTMLTOPDF_H

#include<string>
#include<vector>
#include<map>

#include<QString>
#include<QObject>

class HTMLtoPDF
{
private:
    int max_pages;
    std::vector<QString> in_paths;
    std::string out_path;
    std::map<int, QString> lookup;

public:
    HTMLtoPDF();
};

#endif // HTMLTOPDF_H
