#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	dmxManager.setupDmxManager("Settings.xml");
}

//--------------------------------------------------------------
void ofApp::update(){
	float currentTime = ofGetElapsedTimef();

	dmxManager.update(currentTime);
}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 'q')
	{
		dmxManager.saveSettingsXML();
	}
	if (key == '+')
	{
		dmxManager.setDelay(10.0f);
	}
	if (key == '-')
	{
		dmxManager.setDelay(-10.0f);
	}

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
