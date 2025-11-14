#include "ofxSimpleGuiPage.h"
#include "ofxSimpleGuiToo.h"
#include "Controls/ofxSimpleGuiSliderFloat.h"
#include "Controls/ofxSimpleGuiSliderInt.h"
#include "Controls/ofxSimpleGuiToggle.h"
#include "Controls/ofxSimpleGuiComboBox.h"
#include <regex>

ofxSimpleGuiPage::ofxSimpleGuiPage(string name) : ofxSimpleGuiControl(name) {
	disableAllEvents();
	width = 0;
	height = ofGetHeight();
	eventStealingControl = NULL;
	isUpdatingSimilarPages = false;
	setXMLName(name + "_settings.xml");
	
	// Check if page name matches pattern: baseName + number
	std::regex pagePattern(R"((.+?)(\d+)$)");
	pageNameMatchesPattern = std::regex_match(name, pagePattern);
	
	// If page name matches pattern, listen to control change events
	if(pageNameMatchesPattern) {
		ofAddListener(gui.controlChangedEvent, this, &ofxSimpleGuiPage::onControlChanged);
	}
}

ofxSimpleGuiPage::~ofxSimpleGuiPage() {
	// delete all controls
	if(pageNameMatchesPattern) {
		ofRemoveListener(gui.controlChangedEvent, this, &ofxSimpleGuiPage::onControlChanged);
	}
}


ofxSimpleGuiPage &ofxSimpleGuiPage::setXMLName(string s) {
	xmlFilename = s;
	return *this;
}


void ofxSimpleGuiPage::loadFromXML(string xmlFilepath) {
    string fullPath = xmlFilepath + xmlFilename;
	ofLog(OF_LOG_VERBOSE, "ofxSimpleGuiPage::loadFromXML: " + fullPath);
#ifndef OFXMSAGUI_DONT_USE_XML
	
	if(xmlFilename.compare("") == 0) return;

	if(XML.load(fullPath) == false) {
		ofLog(OF_LOG_ERROR, "Error loading xmlFilename: " + fullPath);
		return;
	}
	
	XML.pushTag("controls");
	for(int i=0; i < controls.size(); i++) {
		controls[i]->loadFromXML(XML);
	}
	XML.popTag();
#endif    
}

void ofxSimpleGuiPage::saveToXML(string xmlFilepath) {
	if(controls.size() <= 0 || xmlFilename.compare("") == 0) return;	// if it has no controls (title counts as one control)
	
#ifndef OFXMSAGUI_DONT_USE_XML
	XML.clear();	// clear cause we are building a new xml file
	
	XML.addTag("controls");
	XML.pushTag("controls");
	for(int i=0; i < controls.size(); i++) {
		controls[i]->saveToXML(XML);
	}
	XML.popTag();
	
    string fullPath = xmlFilepath + xmlFilename;
	XML.save(fullPath);
	//	if(doSaveBackup) 
	ofLog(OF_LOG_VERBOSE, "ofxSimpleGuiPage::saveToXML: " + fullPath + " " + ofToString(controls.size(), 0) + " items");
#endif    
}


float ofxSimpleGuiPage::getNextY(float y) {
	return y;
	int iy = (int)ceil(y/config->gridSize.y);
	return (iy) * config->gridSize.y;
}


void ofxSimpleGuiPage::draw(float x, float y, bool alignRight) {
	setPosition(x += config->offset.x, y += config->offset.y);
	if(alignRight) x = ofGetWidth() - x -  config->gridSize.x;
	
	float posX		= 0;
	float posY		= 0;
	float stealingX = 0;
	float stealingY = 0;
	float pageY 	= 0;
	
	ofSetRectMode(OF_RECTMODE_CORNER);
	
	for(int i=0; i<controls.size(); i++) {
		ofxSimpleGuiControl &control = *controls[i];
		
		if(control.newColumn) {
			if(alignRight) posX -= config->gridSize.x;
			else posX += config->gridSize.x;
			posY = pageY;
		}
		
		float controlX = posX + x;
		float controlY = posY + y;
		
		// Check for horizontal overflow and wrap to new row
		// Reset to first column (posX = 0) and move to next row (posY)
		if(controlX + control.width > ofGetWidth()) {
			posX = 0;
			posY = getNextY(posY + control.height + config->padding.y);
			pageY = posY;
			controlX = posX + x;
			controlY = posY + y;
			// Update page height to accommodate the new row
			float newBottom = controlY + control.height + config->padding.y;
			if(newBottom > height) {
				height = newBottom;
			}
		}
		
		//we don't draw the event stealing controls until the end because they can expand and overlap with other controls (e.g. combo box)
		if(eventStealingControl == &control) {
			stealingX = controlX;
			stealingY = controlY;
		} else {
//			printf("drawing control: %s %s\n", control.controlType.c_str(), control.name.c_str());
			control.draw(controlX, controlY);
		}
		
		if(control.hasTitle) {
			ofNoFill();
			ofSetHexColor(config->borderColor);
			glLineWidth(0.5f);
			ofDrawRectangle(controlX, controlY, control.width, control.height);
		}
		
		// Update posY for next control
		posY = getNextY(posY + control.height + config->padding.y);
		
		// Update page height to track the maximum bottom position
		float currentBottom = controlY + control.height + config->padding.y;
		if(currentBottom > height) {
			height = currentBottom;
		}
		
		if(posY + y >= height - control.height - config->padding.y) {
			if(alignRight) posX -= config->gridSize.x;
			else posX += config->gridSize.x;
			posY = pageY;
		}
		
		//		if(guiFocus == controls[i]->guiID) controls[i]->focused = true;		// MEMO
		//		else							   controls[i]->focused = false;
	}
	//event stealing controls get drawn on top
	if(eventStealingControl) {
		eventStealingControl->draw(stealingX, stealingY);
		if(eventStealingControl->hasTitle) {
			ofNoFill();
			ofSetHexColor(config->borderColor);
			glLineWidth(0.5f);
			ofDrawRectangle(stealingX, stealingY, eventStealingControl->width, eventStealingControl->height);
		}
	}
}


ofxSimpleGuiControl &ofxSimpleGuiPage::addControl(ofxSimpleGuiControl& control) {
	controls.push_back(&control);
	control.setPage(this);  // Set the page for the control
	width += control.width + config->padding.x;
	return control;
}

ofxSimpleGuiButton &ofxSimpleGuiPage::addButton(string name, bool &value) {
	return (ofxSimpleGuiButton &)addControl(* new ofxSimpleGuiButton(name, value));
}

ofxSimpleGuiContent &ofxSimpleGuiPage::addContent(string name, ofBaseDraws &content, float fixwidth) {
	if(fixwidth == -1) fixwidth = config->gridSize.x - config->padding.x;
	return (ofxSimpleGuiContent &)addControl(* new ofxSimpleGuiContent(name, content, fixwidth));
}

ofxSimpleGuiFPSCounter &ofxSimpleGuiPage::addFPSCounter() {
	return (ofxSimpleGuiFPSCounter &)addControl(* new ofxSimpleGuiFPSCounter());
}

ofxSimpleGuiQuadWarp &ofxSimpleGuiPage::addQuadWarper(string name, ofBaseDraws &baseDraw, ofPoint *pts) {
	return (ofxSimpleGuiQuadWarp &)addControl(* new ofxSimpleGuiQuadWarp(name, baseDraw, pts));
}
//
//ofxSimpleGuiMovieSlider &ofxSimpleGuiPage::addMovieSlider(string name, ofVideoPlayer& input) {
//	return (ofxSimpleGuiMovieSlider &)addControl(* new ofxSimpleGuiMovieSlider(name, input));
//}

ofxSimpleGuiSliderInt &ofxSimpleGuiPage::addSlider(string name, int &value, int min, int max) {
	return (ofxSimpleGuiSliderInt &)addControl(* new ofxSimpleGuiSliderInt(name, value, min, max));
}

ofxSimpleGuiSliderFloat &ofxSimpleGuiPage::addSlider(string name, float &value, float min, float max) {
	return (ofxSimpleGuiSliderFloat &)addControl(* new ofxSimpleGuiSliderFloat(name, value, min, max));
}

ofxSimpleGuiSlider2d &ofxSimpleGuiPage::addSlider2d(string name, ofPoint& value, float xmin, float xmax, float ymin, float ymax) {
	return (ofxSimpleGuiSlider2d &)addControl(* new ofxSimpleGuiSlider2d(name, value, xmin, xmax, ymin, ymax));
}

ofxSimpleGuiTitle &ofxSimpleGuiPage::addTitle(string name, float height) {
	return (ofxSimpleGuiTitle &)addControl(* new ofxSimpleGuiTitle(name, height));
}

ofxSimpleGuiToggle &ofxSimpleGuiPage::addToggle(string name, bool &value) {
	return (ofxSimpleGuiToggle &)addControl(* new ofxSimpleGuiToggle(name, value));
}

ofxSimpleGuiColorPicker &ofxSimpleGuiPage::addColorPicker(string name, ofFloatColor& color) {
	return (ofxSimpleGuiColorPicker &)addControl(* new ofxSimpleGuiColorPicker(name, color));
}


ofxSimpleGuiComboBox &ofxSimpleGuiPage::addComboBox(string name, int &choice_out, int numChoices, string* choiceTitles, bool includeBlank) {
	return (ofxSimpleGuiComboBox &)addControl(* new ofxSimpleGuiComboBox(name, choice_out, numChoices, this, choiceTitles, includeBlank));
}

ofxSimpleGuiTextInput &ofxSimpleGuiPage::addTextInput(string name, string &value) {
    return (ofxSimpleGuiTextInput &)addControl(* new ofxSimpleGuiTextInput(name, value));
}

ofxSimpleGuiFilePath &ofxSimpleGuiPage::addFilePath(string name, string &value, bool bFolderSelection) {
    return (ofxSimpleGuiFilePath &)addControl(* new ofxSimpleGuiFilePath(name, value, bFolderSelection));
}

ofxSimpleGuiFileComboBox &ofxSimpleGuiPage::addFileComboBox(string name, string &value, ofDirectory &dir) {
    return (ofxSimpleGuiFileComboBox &)addControl(* new ofxSimpleGuiFileComboBox(name, value, dir, this));
}


void ofxSimpleGuiPage::update(ofEventArgs &e) {
	for(int i=0; i<controls.size(); i++) controls[i]->update();
}

void ofxSimpleGuiPage::SetEventStealingControl(ofxSimpleGuiControl &control) {
	eventStealingControl = &control;
}
void ofxSimpleGuiPage::ReleaseEventStealingControl() {
	eventStealingControl = NULL;
}

void ofxSimpleGuiPage::mouseMoved(ofMouseEventArgs &e) {
	if(eventStealingControl)
		eventStealingControl->_mouseMoved(e);
	else
		for(int i=0; i<controls.size(); i++) controls[i]->_mouseMoved(e);
}

void ofxSimpleGuiPage::onControlChanged(ofxSimpleGuiControlChangeEvent& event) {
    // Only process if shift is down and we're not already updating (prevent recursion)
    if(!ofGetKeyPressed(OF_KEY_SHIFT) || isUpdatingSimilarPages) {
        return;
    }
    
    // Only process events from this page
    if(event.pageName != name) {
        return;
    }
    
    // Extract base name from page name
    std::regex pagePattern(R"((.+?)(\d+)$)");
    std::smatch match;
    if(!std::regex_match(name, match, pagePattern)) {
        return;
    }
    
    string baseName = match[1].str();
    vector<ofxSimpleGuiPage*>& allPages = gui.getPages();
    
    // Find the source control that triggered the event
    ofxSimpleGuiControl* sourceControl = nullptr;
    for(size_t i = 0; i < controls.size(); i++) {
        if(controls[i]->key == event.controlName) {
            sourceControl = controls[i];
            break;
        }
    }
    
    if(!sourceControl) {
        return;
    }
    
    // Set flag to prevent recursive updates
    isUpdatingSimilarPages = true;
    
    // Find all pages with the same base name but different numbers
    for(size_t i = 0; i < allPages.size(); i++) {
        ofxSimpleGuiPage* otherPage = allPages[i];
        if(otherPage == this) continue;
        
        std::smatch otherMatch;
        if(std::regex_match(otherPage->name, otherMatch, pagePattern) && 
           otherMatch[1].str() == baseName) {
            // Found a matching page, find control with same key
            vector<ofxSimpleGuiControl*>& otherControls = otherPage->getControls();
            for(size_t j = 0; j < otherControls.size(); j++) {
                if(otherControls[j]->key == event.controlName && 
                   otherControls[j]->controlType == event.controlType) {
                    // Copy value based on control type
                    if(event.controlType == "SliderFloat") {
                        ofxSimpleGuiSliderFloat* src = dynamic_cast<ofxSimpleGuiSliderFloat*>(sourceControl);
                        ofxSimpleGuiSliderFloat* dst = dynamic_cast<ofxSimpleGuiSliderFloat*>(otherControls[j]);
                        if(src && dst) {
                            dst->setValue(src->getValue());
                        }
                    } else if(event.controlType == "SliderInt") {
                        ofxSimpleGuiSliderInt* src = dynamic_cast<ofxSimpleGuiSliderInt*>(sourceControl);
                        ofxSimpleGuiSliderInt* dst = dynamic_cast<ofxSimpleGuiSliderInt*>(otherControls[j]);
                        if(src && dst) {
                            dst->setValue(src->getValue());
                        }
                    } else if(event.controlType == "Toggle") {
                        ofxSimpleGuiToggle* src = dynamic_cast<ofxSimpleGuiToggle*>(sourceControl);
                        ofxSimpleGuiToggle* dst = dynamic_cast<ofxSimpleGuiToggle*>(otherControls[j]);
                        if(src && dst) {
                            dst->setValue(src->getValue());
                        }
                    } else if(event.controlType == "ComboBox") {
                        ofxSimpleGuiComboBox* src = dynamic_cast<ofxSimpleGuiComboBox*>(sourceControl);
                        ofxSimpleGuiComboBox* dst = dynamic_cast<ofxSimpleGuiComboBox*>(otherControls[j]);
                        if(src && dst) {
                            dst->setValue(src->getValue());
                        }
                    }
                    break;
                }
            }
        }
    }
    
    // Reset flag
    isUpdatingSimilarPages = false;
}

void ofxSimpleGuiPage::mousePressed(ofMouseEventArgs &e) {
	if(eventStealingControl)
		eventStealingControl->_mousePressed(e);
	else
        for(int i=0; i<controls.size(); i++) controls[i]->_mousePressed(e);
}

void ofxSimpleGuiPage::mouseDragged(ofMouseEventArgs &e) {
	if(eventStealingControl)
		eventStealingControl->_mouseDragged(e);
	else
		for(int i=0; i<controls.size(); i++) controls[i]->_mouseDragged(e);
}

void ofxSimpleGuiPage::mouseReleased(ofMouseEventArgs &e) {
	if(eventStealingControl)
		eventStealingControl->_mouseReleased(e);
	else
        for(int i=0; i<controls.size(); i++) controls[i]->_mouseReleased(e);
}

void ofxSimpleGuiPage::keyPressed(ofKeyEventArgs &e) {
	bool keyUp		= e.key == OF_KEY_UP;
	bool keyDown	= e.key == OF_KEY_DOWN;
	bool keyLeft	= e.key == OF_KEY_LEFT;
	bool keyRight	= e.key == OF_KEY_RIGHT;
	bool keyEnter	= e.key == OF_KEY_RETURN;
    bool keyBackspace = e.key == OF_KEY_BACKSPACE;
	
	for(int i=0; i<controls.size(); i++) {
		ofxSimpleGuiControl *c = controls[i];
		if(c->isMouseOver()) {
			if(keyUp)		c->onKeyUp();
			if(keyDown)		c->onKeyDown();
			if(keyLeft)		c->onKeyLeft();
			if(keyRight)	c->onKeyRight();
			if(keyEnter)	c->onKeyEnter();
            if(keyBackspace) c->onKeyBackspace();
			c->_keyPressed(e);
		}
	}
}

void ofxSimpleGuiPage::keyReleased(ofKeyEventArgs &e) {
	for(int i=0; i<controls.size(); i++) if(controls[i]->isMouseOver()) controls[i]->_keyReleased(e);
}


vector <ofxSimpleGuiControl*>&	ofxSimpleGuiPage::getControls() {
	return controls;
}

