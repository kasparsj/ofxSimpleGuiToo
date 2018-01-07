#pragma once

#include "../ofxSimpleGuiControl.h"

class ofxSimpleGuiPage;

class ofxSimpleGuiFileComboBox : public ofxSimpleGuiControl {
  
public:
    ofxSimpleGuiFileComboBox(string name, string &value, ofDirectory &dir, ofxSimpleGuiPage *owner ) ;
    virtual ~ofxSimpleGuiFileComboBox();
    
    void setup();
#ifndef OFXMSAGUI_DONT_USE_XML
    void loadFromXML(ofxXmlSettings &XML);
    void saveToXML(ofxXmlSettings &XML);
#endif
    void keyPressed( int key );
    void onPress(int x, int y, int button);
    void onRelease(int x, int y, int button);
    void onReleaseOutside(int x, int y, int button);
    void draw(float x, float y);
    
    //returns the selected index number of the current choice
    string  getValue();
    
    // set the current selected choice to number
    void setValue(int index);
    
    // set the current selected to text
    void setValue(string value);
    
    
    // get index for title
    int getIndexForValue(string value);
    
    //Get the current choice title an invalid index (default is -1),
    //Otherwise get the title of the index asked for.
    string getValueForIndex(int index = -1);
    
    virtual void onPressOutside(int x, int y, int button);
    virtual void onMouseMove(int x, int y);
    virtual void onDragOver(int x, int y, int button);
    virtual void onDragOutside(int x, int y, int button);
    
    virtual bool hitTest(int tx, int ty);
    
protected:
    //Add a new choice with a specified title.
    //If an invalid index (default = -1) is used then append to the end.
    //If an invalid title is supplied, then the title is set to the index number of the new choice.
    void addChoice(string title = NULL, int index = -1);
    
    void setCBTextColor();
    void setCBTextBGColor();
    void releaseEventStealingFocus();
    
    int            m_mouseChoice;
    string*        value;
    bool           m_hasFocus;
    bool           m_mouseMovedSinceClick;
    string          m_title;
    vector<string>  m_choices;
    ofDirectory     &dir;
    ofxSimpleGuiPage* m_page;
    
    
};
