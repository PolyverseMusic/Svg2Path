#pragma once

#include <JuceHeader.h>
#include "SvgParser.h"

class MainComponent: public Component, public TextEditor::Listener,
public CodeDocument::Listener, public FileDragAndDropTarget
{
    SvgParser parser;
    
    XmlTokeniser xmlTokenizer;
    CodeDocument svgDoc;
    CodeEditorComponent svgEditor;
    
    CPlusPlusCodeTokeniser cppTokenizer;
    CodeDocument codeDoc;
    CodeEditorComponent codeEditor;
    TextEditor pathDataEditor;
    TextEditor pathNameEditor;
    Path path;
    Label svgLabel;
    Label codeLabel;
    Label pathDataLabel;
    AffineTransform xform;
    
    bool isDragging{false};
public:
    MainComponent();
    ~MainComponent() override;
    
    void paint(juce::Graphics&) override;
    void resized() override;
    
private:
    //! @brief parse the svg file
    void parseSVG();
    
    CodeEditorComponent::ColourScheme getColourScheme();
    
    bool isInterestedInFileDrag (const StringArray& files) override;
    void fileDragEnter (const StringArray& files, int x, int y) override;
    void fileDragExit (const StringArray& files) override;
    void filesDropped (const StringArray& files, int x, int y) override;
    
    void textEditorReturnKeyPressed(TextEditor&) override;
    void codeDocumentTextInserted (const String& newText, int insertIndex) override;
    void codeDocumentTextDeleted (int startIndex, int endIndex) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
