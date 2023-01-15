#include "ofxSimpleGuiFileComboBox.h"
#include "../ofxSimpleGuiPage.h"

#define kMaxChoiceStringLen 150
#define kMaxNameStringLen 100

ofxSimpleGuiFileComboBox::ofxSimpleGuiFileComboBox(string name, string &value, ofDirectory &dir, ofxSimpleGuiPage *owner ) :
ofxSimpleGuiControl(name),
dir(dir),
m_page(owner)
{
    m_mouseChoice = 0;
    m_hasFocus=false;
    m_title = name;
    
    dir.listDir();
    for(int i=0; i<dir.size(); i++){
        addChoice(dir.getName(i));
    }
    this->value = &value;
    controlType = "ComboBox";
    setup();
}


ofxSimpleGuiFileComboBox::~ofxSimpleGuiFileComboBox() {
}


string ofxSimpleGuiFileComboBox::getValueForIndex(int index) {
    if(index < 0 || index >= m_choices.size())return m_choices.size() ? getValue() : "No Choices Available";
    return m_choices[index];
}


void ofxSimpleGuiFileComboBox::addChoice(string title, int index) {
    int insertIndex = m_choices.size();
    
    if(index >= 0 && index < m_choices.size()) insertIndex = index;
    
    m_choices.insert(m_choices.begin() + insertIndex, title);
}


int ofxSimpleGuiFileComboBox::getIndexForValue(string value) {
    for(int i=0; i<m_choices.size(); i++) {
        if(value.compare(m_choices[i]) == 0) return i;
    }
    return -1;
}

void ofxSimpleGuiFileComboBox::setup() {
    setSize(config->gridSize.x - config->padding.x, config->comboBoxHeight);
}

#ifndef OFXMSAGUI_DONT_USE_XML
void ofxSimpleGuiFileComboBox::loadFromXML(ofxXmlSettings &XML) {
    setValue(XML.getValue(controlType + "_" + key + ":value", getValue()));
}

void ofxSimpleGuiFileComboBox::saveToXML(ofxXmlSettings &XML) {
    XML.addTag(controlType + "_" + key);
    XML.pushTag(controlType + "_" + key);
    XML.addValue("name", name);
    XML.addValue("value", getValue());
    XML.popTag();
}
#endif

void ofxSimpleGuiFileComboBox::keyPressed( int key ) {
}

string ofxSimpleGuiFileComboBox::getValue() {
    return *value;
}

void ofxSimpleGuiFileComboBox::setValue(int index) {
    setValue(dir.getPath(index));
}

void ofxSimpleGuiFileComboBox::setValue(string val) {
    *value = val;
}


void ofxSimpleGuiFileComboBox::onPress(int x, int y, int button) {
    //    beenPressed = true;
    m_mouseMovedSinceClick=false;
    //a click toggles focus state if we are off
    if(!m_hasFocus) {
        //expand the height for all choices
        //      setSize(config->gridSize.x - config->padding.x, config->comboBoxHeight * m_choices.size());
        m_hasFocus = true;
        //notify that we want to steal all events from the page
        m_page->SetEventStealingControl(*this);
    } else {
        //if we have focus, a click signals that we should lose it
        releaseEventStealingFocus();
    }
}

void ofxSimpleGuiFileComboBox::onPressOutside(int x, int y, int button){
    if(m_hasFocus)
        releaseEventStealingFocus();
}


void ofxSimpleGuiFileComboBox::onDragOver(int x, int y, int button){
    //same behavior as mouse move
    onMouseMove(x,y);
}

void ofxSimpleGuiFileComboBox::onDragOutside(int x, int y, int button){
    //same behavior as mouse move
    onMouseMove(x,y);
}

bool ofxSimpleGuiFileComboBox::hitTest(int tx, int ty) {
    if(!m_hasFocus)
        return ofxMSAInteractiveObject::hitTest(tx,ty);
    
    int fullheight = height + config->comboBoxTextHeight * m_choices.size();
    
    return ((tx > x) && (tx < x + width) && (ty > y) && (ty < y + fullheight));
}

void ofxSimpleGuiFileComboBox::onMouseMove(int x, int y) {
    m_mouseMovedSinceClick=true;
    if(m_hasFocus) {
        //see which index was selected.
        float fChoice = (y - (height - config->comboBoxTextHeight) - (this->y + config->comboBoxTextHeight))/config->comboBoxTextHeight;
        //TODO:replace with OF constrain macro.
        m_mouseChoice = fChoice < 0?-1:(fChoice>= m_choices.size()? -1:fChoice);
    }
}

void ofxSimpleGuiFileComboBox::onReleaseOutside(int x, int y, int button) {
    onRelease(x, y, button);
}

void ofxSimpleGuiFileComboBox::onRelease(int x, int y, int button) {
    if(m_hasFocus && m_mouseMovedSinceClick) {
        releaseEventStealingFocus();
    }
}

void ofxSimpleGuiFileComboBox::releaseEventStealingFocus(){
    //see which index was selected, but only if the user actually moved around.
    if (m_mouseChoice >= 0) {
        setValue(m_mouseChoice);
    }
    
    //a release toggles focus state if we are on - TODO: unless x and y don't change
    m_hasFocus = false;
    //      setSize(config->gridSize.x - config->padding.x, config->comboBoxHeight);
    //also let the page know we don't need to steal all the events and draw over anymore
    m_page->ReleaseEventStealingControl();
}

//special overloads - this is a hack - later think about making ofxSimpleGuiControl's methods virtual.
void ofxSimpleGuiFileComboBox::setCBTextColor() {
    if(m_hasFocus) ofSetHexColor(config->textOverColor);
    else ofSetHexColor(config->textColor);
}

void ofxSimpleGuiFileComboBox::setCBTextBGColor() {
    if(m_hasFocus) ofSetHexColor(config->textBGOverColor);
    else ofSetHexColor(config->textBGColor);
}


#define kSGCBTriangleWidth   10
#define KSGCBTrianglePadding 5
#define kSGCBTextPaddingX    3
#define kSGCBTextPaddingY    15
void ofxSimpleGuiFileComboBox::draw(float x, float y) {
    //we assume a max of 256 characters.
    char choiceBuf[256];
    
    setPosition(x, y);
    
    ofPushMatrix();
    ofTranslate(x, y, 0);
    
    ofEnableAlphaBlending();
    ofFill();
    setTextBGColor();
    ofDrawRectangle(0, 0, width, height);
    
    setTextColor();
    //    sprintf(choiceBuf, "%s: %s", m_title, m_choices.size() ? m_choices[m_selectedChoice] : "(No Choices Available)");
    
    ofDrawBitmapString(m_title + "\n" + (m_choices.size() ? getValue() : "N/A"), kSGCBTextPaddingX, kSGCBTextPaddingY);
    //draw a combobox down triangle icon so the users know to click
    ofDrawTriangle(width - (kSGCBTriangleWidth + KSGCBTrianglePadding), kSGCBTextPaddingY/2,
                   width - (KSGCBTrianglePadding), kSGCBTextPaddingY/2,
                   width - (kSGCBTriangleWidth/2 + KSGCBTrianglePadding), kSGCBTextPaddingY);
    
    if(m_hasFocus) {
        setCBTextBGColor();
        ofDrawRectangle(0, height, width, config->comboBoxTextHeight * m_choices.size());
        setTextColor();
        ofDrawLine(0, config->comboBoxHeight-1, width, config->comboBoxHeight-1);
        
        for(int i=0; i < m_choices.size(); i++) {
            setCBTextColor();
            //invert for selected choice
            float curY = height + i*config->comboBoxTextHeight;
            if(i==m_mouseChoice){
                //draw a text colored rect so we can see the inverse
                ofDrawRectangle(0, curY, width, config->comboBoxTextHeight);
                setCBTextBGColor();
            }
            
            ofDrawBitmapString(m_choices[i], kSGCBTextPaddingX, curY + kSGCBTextPaddingY);
        }
    }
    ofDisableAlphaBlending();
    
    ofPopMatrix();
}
