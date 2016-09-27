#pragma once

#include "ofxXmlSettings.h"
#include "ofxDmx.h"
#include "ofxEasing.h"

typedef struct {
	string name;
	float start;
	float duration;
	string type;
	int universe;
	int fixture;
	float channels[6];
	int channelsForDevice;
} DMXEvent;

class ofxDmxManager {

public:

	// This is the initialization method that initializes all the required values. It takes as an argument the name of the DMX settings file 
	void setupDmxManager(string _xmlFilename);

	// This is used if a new xml file is loaded so the values need to be re-initialized
	void initializeValues(string _xmlFilename);

	// This is the general update method that determines if there was an abnormal jump in time and triggers DMX events according to the current timestamp
	void update(float time);

	// These are used to load and save the DMX seetings
	void loadSettingsXML(string _xmlFilename);
	void saveSettingsXML();

	// This stores all the available fixtures (devices connected) 
	// It is not used at the moment but it is quite usefull for commands that involve all the connected devices
	// For example close all lights, open all smoke machines etc.
	std::set<int> fixtures;

	// test function 
	void setDelay(float _delay);

private:
	// The xml ID tags
	static string EVENTS_ID_TAG;
	static string SETTINGS_ID_TAG;
	static string EVENT_ID_TAG;
	static string CHANNELS_ID_TAG;
	
	// The number of channels that the DMX device is using
	static int NUM_CHANNELS; 
	
	// A static function to sort the event by order of time
	static bool sortByStart(const DMXEvent &lhs, const DMXEvent &rhs) { return lhs.start < rhs.start; }

	// This method calculates is an event is to be triggered
	void updateInactiveEvents(float movieTime);

	// It updates the events that are already triggered and evaluates if they are to be stopped
	void updateActiveEvents(float movieTime);

	// It evaluates the new event index after a jump in time etheir backwards or forward 
	void evaluateEventIndex(float movieTime);

	// It pushes the triggered event in the ActiveEventlist and updates the current event index
	void triggerDMXEvent(DMXEvent currentEvent, float movieTime);

	// It updates the values of the DMX device using a linear interpolation 
	void setDMXValuesLerp(DMXEvent currentEvent, float eventStart, float movieTime);

	// it updates the values of the DMX device directly
	void setDMXValues(DMXEvent currentEvent);

	// The ofxDmx object that handles the actual communication with the DMX device
	ofxDmx dmx;

	// This is saved on every update and is used to compare the between the current timestamp and the previous one in order to determine if the movies has jumped 
	float lastTimestamp;

	// This has all the events from the xml file
	vector <DMXEvent> allEventList;

	// This has all the events that are active at any given moment
	vector <DMXEvent> activeEventList;

	// This is the next event that is to be triggered. This an index ton the alleventlist. 
	int eventIndex;

	// The number of DMX events in the list 
	int numEvents;

	// The id of the DMX controller in the list of COM devices connected
	int deviceID;

	// The number of channels that the DMX device is using
	int numChannels;

	// This is set in the xml file and is the maximum time between two updates that after that is considered a "jump"
	float maximumTimeBetweenUpdates;

	// This is the filename of settings xml file 
	string xmlFileName;

	// This is a test variable just to test "jumps"
	float delay;
};
