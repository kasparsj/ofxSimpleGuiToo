#include "ofxSimpleGuiFilePath.h"
#include <algorithm>
#include <cctype>
#include <filesystem>

namespace {

std::string normalizePath(std::string path) {
    std::replace(path.begin(), path.end(), '\\', '/');
    return path;
}

bool hasPrefix(const std::string& value, const std::string& prefix) {
    return value.size() >= prefix.size() && value.compare(0, prefix.size(), prefix) == 0;
}

std::string ensureTrailingSlash(std::string path) {
    if (!path.empty() && path.back() != '/') {
        path.push_back('/');
    }
    return path;
}

std::string trimLeadingSlash(std::string path) {
    while (!path.empty() && (path.front() == '/' || path.front() == '\\')) {
        path.erase(path.begin());
    }
    return path;
}

bool isAbsolutePath(const std::string& path) {
    if (path.empty()) {
        return false;
    }
    if (path.front() == '/' || path.front() == '\\') {
        return true;
    }
    return path.size() > 1 &&
           std::isalpha(static_cast<unsigned char>(path[0])) &&
           path[1] == ':';
}

std::string resolveDataRelativeCandidate(const std::string& sourcePath) {
    const std::string normalizedPath = normalizePath(sourcePath);
    const std::string dataRoot = ensureTrailingSlash(normalizePath(ofToDataPath("", true)));

    if (hasPrefix(normalizedPath, dataRoot)) {
        return trimLeadingSlash(normalizedPath.substr(dataRoot.size()));
    }

    const std::string markers[] = {
        "/bin/data/",
        "/Contents/Resources/data/"
    };

    for (const auto& marker : markers) {
        const size_t markerPos = normalizedPath.rfind(marker);
        if (markerPos == std::string::npos) {
            continue;
        }
        const std::string candidate = trimLeadingSlash(normalizedPath.substr(markerPos + marker.size()));
        if (candidate.empty()) {
            continue;
        }
        const std::string localCandidate = ofToDataPath(candidate, true);
        if (std::filesystem::exists(localCandidate)) {
            return candidate;
        }
    }

    return "";
}

std::string toRuntimePath(const std::string& xmlValue) {
    if (xmlValue.empty()) {
        return xmlValue;
    }

    const std::string relativeCandidate = resolveDataRelativeCandidate(xmlValue);
    if (!relativeCandidate.empty()) {
        return ofToDataPath(relativeCandidate, true);
    }

    if (!isAbsolutePath(xmlValue)) {
        return ofToDataPath(xmlValue, true);
    }

    return xmlValue;
}

std::string toXmlPathValue(const std::string& runtimeValue) {
    if (runtimeValue.empty()) {
        return runtimeValue;
    }

    const std::string relativeCandidate = resolveDataRelativeCandidate(runtimeValue);
    if (!relativeCandidate.empty()) {
        return relativeCandidate;
    }

    return normalizePath(runtimeValue);
}

} // namespace

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
    string loadedPath = XML.getValue(controlType + "_" + key + ":value", getValue());
    setValue(toRuntimePath(loadedPath));
}

void ofxSimpleGuiFilePath::saveToXML(ofxXmlSettings &XML) {
    XML.addTag(controlType + "_" + key);
    XML.pushTag(controlType + "_" + key);
    XML.addValue("name", name);

    const string pathToSave = toXmlPathValue(getValue());
    XML.addValue("value", pathToSave);
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

void ofxSimpleGuiFilePath::onKeyBackspace() {
    (*value) = "";
}

void ofxSimpleGuiFilePath::draw(float x, float y) {
    setPosition(x, y);
    
    ofPushMatrix();
    ofTranslate(x, y, 0);
    
    ofEnableAlphaBlending();
    ofFill();
    setTextBGColor();
    ofDrawRectangle(0, 0, width, height);
    
    setTextColor();
    ofDrawBitmapString(name, 3, config->sliderHeight);
    
    ofDrawBitmapString(std::filesystem::path(*value).filename(), 3, config->sliderHeight + 14);
    
    ofDisableAlphaBlending();
    
    ofPopMatrix();
}
