#pragma once

#include "ofxSimpleGuiControl.h"

class ofxSimpleGuiFilePath : public ofxSimpleGuiControl {
public:
    string*      value;
    bool         bFolderSelection;
    
    ofxSimpleGuiFilePath(string name, string &value, bool bFolderSelection = false);
    void setup();
    void loadFromXML(ofxXmlSettings &XML);
    void saveToXML(ofxXmlSettings &XML);
    string getValue();
    void setValue(string b);
    
    void onPress(int x, int y, int button);
    void onRelease(int x, int y, int button);
    void draw(float x, float y);
};
