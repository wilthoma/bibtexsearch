#ifndef BIBENTRY_H
#define BIBENTRY_H

#include <string>
#include <iostream>
#include <map>

class bibentry
{
public:
    std::string entrytype;
    std::string entrykey;
    std::map<std::string, std::string> attribus;
    std::string BibFile;
    std::string FullText;
    std::string SearchableText;
    int startpos, endpos, lineNr;
    int type; //0=entry, 1=bibfile, 2=error, 3=texfile

public:
    bibentry();
    std::string getAttribute(const std::string& key) const;
};

#endif // BIBENTRY_H
