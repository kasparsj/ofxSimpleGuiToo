/***********************************************************************
 
 Copyright (c) 2008, 2009, 2010, Memo Akten, www.memo.tv
 *** The Mega Super Awesome Visuals Company ***
 * All rights reserved.
 
 based on Todd Vanderlin's ofxSimpleGui API
 http://toddvanderlin.com/
 
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of MSA Visuals nor the names of its contributors 
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE. 
 *
 * ***********************************************************************/ 

#pragma once

#include "ofMain.h"
#include <functional>
#include <string>

#ifndef OFXMSAGUI_DONT_USE_XML
#include "ofxXmlSettings.h"
#endif

#include "ofxSimpleGuiIncludes.h"

// Event data structure for control change events
struct ofxSimpleGuiControlChangeEvent {
    std::string controlName;  // The key/name of the control
    std::string controlType;  // Type of control (e.g., "SliderFloat", "ComboBox", etc.)
    std::string pageName;     // The name of the page this control belongs to
    void* oldValue;           // Pointer to old value (type-specific)
    void* newValue;           // Pointer to new value (type-specific)
};

// Event data structure for setTargetValue events
struct ofxSimpleGuiSetTargetValueEvent {
    std::string controlName;  // The key/name of the control
    std::string controlType;  // Type of control (e.g., "SliderFloat", "ComboBox", etc.)
    std::string pageName;     // The name of the page this control belongs to
    void* targetValue;        // Pointer to target value (type-specific)
};

// Event data structure for page load events
struct ofxSimpleGuiPageLoadEvent {
    std::string pageName;     // The name of the page that was loaded
    std::string xmlFilepath;  // The file path that was loaded
};

class ofxSimpleGuiToo {
	
public:
	int							guiFocus;
	
	ofxSimpleGuiConfig			*config;	
	
	ofxSimpleGuiToo();
	void						setup();
	
	
	void						loadFromXML(string path = "");
	void						saveToXML(string path = "");
	void						setAutoSave(bool b);
    void						setAlignRight(bool b=true);
    void						setDefaultKeys(bool b=true);
	
	//	int		getValueI(string nameID);
	//	float	getValueF(string nameID);
	//	bool	getValueB(string nameID);
	
	void						drawFocus(float x, float y);
	
	
	void						setDraw(bool b);
	void						toggleDraw();
	void						show();		// simply calls setDraw(true);
	void						hide();		// simply calls setDraw(false);
	bool						isOn();
	
	void						nextPage();
	void						prevPage();
	void						setPage(int i);				// 1 based index of page
	void						setPage(string name);
	void						setForceHeight(int h);
	void						autoHeight();
	
	void						nextPageWithBlank();		// cycles through pages, and closes after last page
	
	ofxSimpleGuiPage&			page(int i);				// 1 based index of page
	ofxSimpleGuiPage&			page(string name);			// returns page by name
	ofxSimpleGuiPage&			currentPage();				// returns current page
	vector <ofxSimpleGuiPage*>&	getPages();
	
	ofxSimpleGuiControl			&control(string name);		// returns control by name
	
	// Event system for control changes
	ofEvent<ofxSimpleGuiControlChangeEvent> controlChangedEvent;
	ofEvent<ofxSimpleGuiSetTargetValueEvent> setTargetValueEvent;
    ofEvent<ofxSimpleGuiPageLoadEvent> pageLoadEvent;
	
	// Notify that a control has changed (called by controls internally)
	void notifyControlChanged(const std::string& controlName, const std::string& controlType, const std::string& pageName = "", void* oldValue = nullptr, void* newValue = nullptr);
	
	// Notify that a control's target value has been set (called by controls internally)
	void notifySetTargetValue(const std::string& controlName, const std::string& controlType, const std::string& pageName = "", void* targetValue = nullptr);

    void notifyPageLoaded(const std::string& pageName = "", const std::string& xmlFilepath = "");
	
	// Register a callback for all control changes
	// Callback signature: void callback(const std::string& controlName, const std::string& controlType, const std::string& pageName)
	void registerControlChangeCallback(std::function<void(const std::string&, const std::string&, const std::string&)> callback);
	
	ofxSimpleGuiPage			&addPage(string name = "");
	ofxSimpleGuiControl			&addControl(ofxSimpleGuiControl& control);
	ofxSimpleGuiContent			&addContent(string name, ofBaseDraws &content, float fixwidth = -1);
	ofxSimpleGuiButton			&addButton(string name, bool &value);
	ofxSimpleGuiFPSCounter		&addFPSCounter();
	//	ofxSimpleGuiMovieSlider		&addMovieSlider(string name, ofVideoPlayer& input);
	ofxSimpleGuiQuadWarp		&addQuadWarper(string name, ofBaseDraws &baseDraw, ofPoint *pts);
	ofxSimpleGuiSliderInt		&addSlider(string name, int &value, int min, int max);
	ofxSimpleGuiSliderFloat		&addSlider(string name, float &value, float min = 0.f, float max = 1.f);
	ofxSimpleGuiSlider2d		&addSlider2d(string name, ofPoint& value, float xmin, float xmax, float ymin, float ymax);
	ofxSimpleGuiTitle			&addTitle(string name="", float height = 0);
	ofxSimpleGuiToggle			&addToggle(string name, bool &value);
	ofxSimpleGuiColorPicker		&addColorPicker(string name, ofFloatColor& color);
    ofxSimpleGuiComboBox        &addComboBox(string name, int &value, int numChoices, string* choiceTitles=NULL, bool includeBlank=false);
    ofxSimpleGuiComboBox        &addComboBox(string name, int &value, vector<string>& choiceTitles, bool includeBlank=false);
    ofxSimpleGuiTextInput       &addTextInput(string name, string &value);
    ofxSimpleGuiFilePath        &addFilePath(string name, string &value, bool bFolderSelection=false);
    ofxSimpleGuiFileComboBox    &addFileComboBox(string name, string &value, ofDirectory &dir);
	
	
	void						draw();
	
protected:
	bool							doAutoSave = true;
	bool							alignRight = false;
	bool							doDefaultKeys = false;
	bool							doSave = false;//, doSaveBackup;
    bool                            savePreset = false;
    bool                            loadPreset = false;
	bool							doNextPage = false;
	bool							doPrevPage = false;
	int								forceHeight = false;
    int                             currentPageIndex = 0;            // 1 based index of page (0 is for global controls)
	
	bool							doDraw = true;
	float							border;
    bool                            doUndo = true;
	
	ofxSimpleGuiPage				*headerPage;
	ofxSimpleGuiButton				*titleButton;
	vector <ofxSimpleGuiPage*>		pages;				// 0 is for headerPage
	
	struct UndoState {
		string pageName;
		string controlName;
		string controlType;
		string previousValue;
		float timestamp;
	};
	
	vector<UndoState> undoStates;
	int maxUndoStates = 50;
	bool isUndoing = false;
	float lastUndoSaveTime = 0;
	float undoDebounceDelay = 1000; // 1 second in milliseconds
	
	void scheduleUndoSave(const std::string& pageName, const std::string& controlName, const std::string& controlType, const std::string& previousValue);
	void saveUndoState(const std::string& pageName, const std::string& controlName, const std::string& controlType, const std::string& previousValue);
	void performUndo();
	
	void addListeners();
	void removeListeners();
	
	//	void setup(ofEventArgs &e);
	void update(ofEventArgs &e);
	//  void draw(ofEventArgs &e);
	//	void exit(ofEventArgs &e);
	
	void mouseMoved(ofMouseEventArgs &e);
	void mousePressed(ofMouseEventArgs &e);	
	void mouseDragged(ofMouseEventArgs &e);	
	void mouseReleased(ofMouseEventArgs &e);
	
	void keyPressed(ofKeyEventArgs &e);
	void keyReleased(ofKeyEventArgs &e);
};


extern ofxSimpleGuiToo gui;



