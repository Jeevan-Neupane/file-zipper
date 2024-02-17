

#include <vector>
#include <wx/string.h>

class FileQueue {
public:
    FileQueue() = default;

    void enqueue(const wxString& filename);
    wxString dequeue();
    bool isEmpty() const;
    int size() const;

private:
    std::vector<wxString> queue;
};

