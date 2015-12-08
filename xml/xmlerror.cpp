#include "xml/xml.h"

namespace xml
{



const char* XmlBase::errorString[ XmlBase::XML_ERROR_STRING_COUNT ] =
{
    "No error",
    "Error",
    "Failed to open file",
    "Error parsing Element.",
    "Failed to read Element name",
    "Error reading Element value.",
    "Error reading Attributes.",
    "Error: empty tag.",
    "Error reading end tag.",
    "Error parsing Unknown.",
    "Error parsing Comment.",
    "Error parsing Declaration.",
    "Error document empty.",
    "Error null (0) or unexpected EOF found in input stream.",
    "Error parsing CDATA.",
    "Error when XmlDocument added to document, because XmlDocument can only be at the root.",
};



}