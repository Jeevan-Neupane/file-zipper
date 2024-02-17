#include <wx/wx.h>
#include "queue.h"

class MyApp : public wxApp {
public:
    virtual bool OnInit();

private:
    FileQueue fileQueue;
    std::vector<wxString> compressedFiles;

};

