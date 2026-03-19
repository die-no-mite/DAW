#pragma once

#include <wx/docview.h>
#include <wx/stdstream.h>

#include <iostream>

class Document : public wxDocument
{
public:
    std::ostream& SaveObject(std::ostream& stream) override;
    std::istream& LoadObject(std::istream& stream) override;

    wxDECLARE_DYNAMIC_CLASS(Document);
};