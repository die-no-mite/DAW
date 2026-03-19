#include "document.h"

wxIMPLEMENT_DYNAMIC_CLASS(Document, wxDocument);

std::ostream& Document::SaveObject(std::ostream& stream)
{
    auto wrapper = OStreamWrapper(stream);
    return stream;
}

std::istream& Document::LoadObject(std::istream& stream)
{
    auto wrapper = IStreamWrapper(stream);

    stream.clear();

    return stream;
}