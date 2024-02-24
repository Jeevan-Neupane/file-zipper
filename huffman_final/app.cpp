#include "app.h"
#include "huffman.h"
#include <wx/filename.h>

bool MyApp::OnInit() {
    wxFrame* frame = new wxFrame(nullptr, wxID_ANY, "Huffman Compression Tool", wxDefaultPosition, wxSize(1200, 800));
    frame->SetBackgroundColour(*wxWHITE);

    // Create a sizer for organizing controls
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // Add a static text label for operation selection
    sizer->Add(new wxStaticText(frame, wxID_ANY, "Choose operation:"), 0, wxALL, 5);

    // Add a radio box for operation selection
    wxString choices[2] = { "Compress", "Decompress" };
    wxRadioBox* operationRadioBox = new wxRadioBox(frame, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 2, choices, 1, wxRA_SPECIFY_COLS);
    sizer->Add(operationRadioBox, 0, wxALL, 5);

    // Add a text control for input file path
    wxTextCtrl* inputFileTextCtrl = new wxTextCtrl(frame, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    sizer->Add(new wxStaticText(frame, wxID_ANY, "Input File:"), 0, wxALL, 5);
    sizer->Add(inputFileTextCtrl, 0, wxEXPAND | wxALL, 5);

    // Add a button to browse for input file
    wxButton* browseInputFileButton = new wxButton(frame, wxID_ANY, "Browse");
    sizer->Add(browseInputFileButton, 0, wxALL, 5);

    // Add a text control for output file path
    wxTextCtrl* outputFileTextCtrl = new wxTextCtrl(frame, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    sizer->Add(new wxStaticText(frame, wxID_ANY, "Output File:"), 0, wxALL, 5);
    sizer->Add(outputFileTextCtrl, 0, wxEXPAND | wxALL, 5);

    // Add a button to browse for output file
    wxButton* browseOutputFileButton = new wxButton(frame, wxID_ANY, "Browse");
    sizer->Add(browseOutputFileButton, 0, wxALL, 5);

    // Add a button to start the operation
    wxButton* startButton = new wxButton(frame, wxID_ANY, "Start");
    sizer->Add(startButton, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

    // Set the sizer for the frame
    frame->SetSizer(sizer);
    frame->Layout();

    // Bind events
    browseInputFileButton->Bind(wxEVT_BUTTON, [this, inputFileTextCtrl, operationRadioBox](wxCommandEvent& event) {
        wxString wildcard;
        if (operationRadioBox->GetSelection() == 0) {
            // Compress: Allow only .txt files
            wildcard = "Text files (*.txt)|*.txt";
        }
        else {
            // Decompress: Allow only .lh files
            wildcard = "Compressed files (*.lh)|*.lh";
        }

        wxFileDialog openFileDialog(nullptr, _("Open File"), "", "", wildcard, wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        if (openFileDialog.ShowModal() == wxID_CANCEL)
            return;

        wxString filePath = openFileDialog.GetPath();

        // Set the selected file path to the input file text control
        inputFileTextCtrl->SetValue(filePath);

        if (operationRadioBox->GetSelection() == 1) {
            // Decompress: You can handle the decompression here if needed
            wxString inputFile = filePath;
            wxString outputFile = inputFile + ".decompressed"; // Modify output file name as desired

            // Call decompressFile with wxString file paths
            // decompressFile(inputFile, outputFile);
        }
        });

    browseOutputFileButton->Bind(wxEVT_BUTTON, [outputFileTextCtrl](wxCommandEvent& event) {
        wxFileDialog saveFileDialog(nullptr, _("Save File"), "", "", "All files (*.*)|*.*", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        if (saveFileDialog.ShowModal() == wxID_CANCEL)
            return;
        outputFileTextCtrl->SetValue(saveFileDialog.GetPath());
        });

    wxListBox* compressedFilesListBox = new wxListBox(frame, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxLB_SINGLE | wxLB_ALWAYS_SB);
    sizer->Add(compressedFilesListBox, 1, wxEXPAND | wxALL, 5);

    // Inside the startButton event handler
    startButton->Bind(wxEVT_BUTTON, [this, operationRadioBox, compressedFilesListBox, inputFileTextCtrl](wxCommandEvent& event) {
        wxString inputFile = inputFileTextCtrl->GetValue();
        wxString outputFile;

        if (inputFile.IsEmpty()) {
            wxMessageBox("Please select a file.", "Error", wxOK | wxICON_ERROR);
            return;
        }

        if (operationRadioBox->GetSelection() == 0) { // Compress
            outputFile = inputFile + ".lh"; // Modify output file name as desired
            compressFile(inputFile, outputFile);
        }
        else { // Decompress
            // Modify output file name to have .txt extension
            outputFile = inputFile + ".txt";
            decompressFile(inputFile, outputFile);
        }

        // Extract filename from the full path
        wxFileName outputFileName(outputFile);
        wxString fileName = outputFileName.GetFullName();

        // Clear the list box before compressing/decompressing new files
        compressedFilesListBox->Clear();

        // Append the compressed/decompressed file name to the list box
        compressedFilesListBox->Append(fileName);
        });

    frame->Show(true);
    return true;
}

wxIMPLEMENT_APP(MyApp);
