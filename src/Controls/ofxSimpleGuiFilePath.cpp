#include "ofxSimpleGuiFilePath.h"

ofxSimpleGuiFilePath::ofxSimpleGuiFilePath(string name, string &value, bool bFolderSelection) : ofxSimpleGuiControl(name) {
    this->value = &value;
    this->bFolderSelection = bFolderSelection;
    controlType = "FileSystemPath";
    setup();
}

void ofxSimpleGuiFilePath::setup() {
    setSize(config->gridSize.x - config->padding.x, config->buttonHeight);
}

#ifndef OFXMSAGUI_DONT_USE_XML
void ofxSimpleGuiFilePath::loadFromXML(ofxXmlSettings &XML) {
    setValue(XML.getValue(controlType + "_" + key + ":value", getValue()));
}

void ofxSimpleGuiFilePath::saveToXML(ofxXmlSettings &XML) {
    XML.addTag(controlType + "_" + key);
    XML.pushTag(controlType + "_" + key);
    XML.addValue("name", name);
    XML.addValue("value", getValue());
    XML.popTag();
}
#endif

string ofxSimpleGuiFilePath::getValue() {
    return (*value);
}

void ofxSimpleGuiFilePath::setValue(string b) {
    (*value) = b;
}

void ofxSimpleGuiFilePath::onPress(int x, int y, int button) {
    focused = true;
}

void ofxSimpleGuiFilePath::onRelease(int x, int y, int button) {
    ofFileDialogResult result = ofSystemLoadDialog("Select " + name, bFolderSelection);
    if(result.bSuccess) {
        (*value) = result.getPath();
    }
    focused = false;
}

void ofxSimpleGuiFilePath::draw(float x, float y) {
    setPosition(x, y);
    
    ofPushMatrix();
    ofTranslate(x, y, 0);
    
    ofEnableAlphaBlending();
    ofFill();
    setTextBGColor();
    ofRect(0, 0, width, height);
    
    setTextColor();
    ofDrawBitmapString(name, 3, config->sliderHeight);
    
    ofDrawBitmapString(std::filesystem::path(*value).filename(), 3, config->sliderHeight + 14);
    
    ofDisableAlphaBlending();
    
    ofPopMatrix();
}
