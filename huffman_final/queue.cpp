#include "Queue.h"

void FileQueue::enqueue(const wxString& filename) {
    queue.push_back(filename);
}

wxString FileQueue::dequeue() {
    wxString front = queue.front();
    queue.erase(queue.begin());
    return front;
}

bool FileQueue::isEmpty() const {
    return queue.empty();
}

int FileQueue::size() const {
    return queue.size();
}
