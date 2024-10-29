#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() :
svgEditor(svgDoc, &xmlTokenizer),
codeEditor(codeDoc, &cppTokenizer)
{
    setSize(600, 600);
    addAndMakeVisible(svgEditor);
    svgDoc.addListener(this);
    
    addAndMakeVisible(codeEditor);
    addAndMakeVisible(pathDataEditor);
    pathDataEditor.setMultiLine(true);
    pathDataEditor.setSelectAllWhenFocused(true);
    
    addAndMakeVisible(pathNameEditor);
    pathNameEditor.setSelectAllWhenFocused(true);
    pathNameEditor.setTextToShowWhenEmpty("Path name", Colour(0x55dddddd));
    pathNameEditor.setColour(TextEditor::backgroundColourId, Colour(0xff001122));
    pathNameEditor.addListener(this);
    
    svgEditor.setColourScheme(getColourScheme());
    svgEditor.setColour(CodeEditorComponent::backgroundColourId, Colour(0xff001122));
    codeEditor.setColourScheme(getColourScheme());
    codeEditor.setColour(CodeEditorComponent::backgroundColourId, Colour(0xff001122));
    pathDataEditor.setColour(TextEditor::backgroundColourId, Colour(0xff001122));
    
    addAndMakeVisible(svgLabel);
    addAndMakeVisible(codeLabel);
    addAndMakeVisible(pathDataLabel);
    
    svgLabel.setText("Paste your svg code here:", NotificationType::dontSendNotification);
    codeLabel.setText("Juce Code:", NotificationType::dontSendNotification);
    pathDataLabel.setText("Path Data:", NotificationType::dontSendNotification);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(Colour(isDragging ? 0xff223344 : 0xff112233));
    g.setColour(juce::Colours::white);
    g.fillPath(path);
    g.fillPath(path, xform);
}

void MainComponent::resized()
{
    int hh = getHeight() / 3;
    int ww = getWidth() / 2;
    
    pathNameEditor.setBounds(0, hh, 150, 20);
    svgLabel.setBounds(160, hh, getWidth() - 170, 20);
    svgEditor.setBounds(0, hh + 20, getWidth(), hh - 20);
    
    int y = hh + hh;
    codeLabel.setBounds(10, y, ww - 20, 20);
    pathDataLabel.setBounds(ww + 10, y, ww - 20, 20);
    y += 20;
    hh -= 20;
    codeEditor.setBounds(0, y, ww, hh);
    pathDataEditor.setBounds(ww, y, ww, hh);
    
    xform = AffineTransform::scale(4.f).translated(ww, 0.f);
}

void MainComponent::parseSVG()
{
    pathDataEditor.clear();
    path.clear();
    String name = pathNameEditor.getText();
    String juceCode = parser.parse(svgDoc.getAllContent(), path);
    String binData = parser.getBinary(path, name);
    
    // Output the generated JUCE code
    codeEditor.loadContent(juceCode);
    
    pathDataEditor.setText(binData);
    
    //this takes less space but slower to process
    //pathDataEditor.setText(path.toString());
    
    repaint();
}

CodeEditorComponent::ColourScheme MainComponent::getColourScheme()
{
    struct Type
    {
        const char* name;
        juce::uint32 colour;
    };
    
    const Type types[] = {{"Error", 0xffe60000},       {"Comment", 0xff72d20c},          {"Keyword", 0xffee6f6f},
        {"Operator", 0xffc4eb19},    {"Identifier", 0xffcfcfcf},       {"Integer", 0xff42c8c4},
        {"Float", 0xff885500},       {"String", 0xffbc45dd},           {"Bracket", 0xff058202},
        {"Punctuation", 0xffcfbeff}, {"Preprocessor Text", 0xfff8f631}};
    
    CodeEditorComponent::ColourScheme cs;
    
    for (auto& t: types)
        cs.set(t.name, Colour(t.colour));
    
    return cs;
}

bool MainComponent::isInterestedInFileDrag(const StringArray& files)
{
    File file(files[0]);
    String ext = file.getFileExtension();
    return file.existsAsFile() && (file.getFileExtension() == ".svg");
}

void MainComponent::fileDragEnter(const StringArray& files, int x, int y)
{
    isDragging = true;
    setMouseCursor(MouseCursor::CopyingCursor);
    repaint();
}

void MainComponent::fileDragExit(const StringArray& files)
{
    isDragging = false;
    setMouseCursor(MouseCursor::NormalCursor);
    repaint();
}

void MainComponent::filesDropped(const StringArray& files, int x, int y)
{
    isDragging = false;
    File file(files[0]);
    svgEditor.loadContent(file.loadFileAsString());
    setMouseCursor(MouseCursor::NormalCursor);
    repaint();
}

void MainComponent::codeDocumentTextInserted(const String& newText, int insertIndex)
{
    parseSVG();
}

void MainComponent::textEditorReturnKeyPressed(TextEditor&)
{
    parseSVG();
}

void MainComponent::codeDocumentTextDeleted(int startIndex, int endIndex)
{
    parseSVG();
}
