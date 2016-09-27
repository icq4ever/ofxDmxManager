#include "ofxDmxManager.h"

string ofxDmxManager::EVENTS_ID_TAG = "DMXEvents";
string ofxDmxManager::EVENT_ID_TAG = "DMXEvent";
string ofxDmxManager::SETTINGS_ID_TAG = "GlobalSettings";
string ofxDmxManager::CHANNELS_ID_TAG = "Channel";

//--------------------------------------------------------------
void ofxDmxManager::setupDmxManager(string _xmlFilename)
{
	initializeValues(_xmlFilename);

	loadSettingsXML(xmlFileName);

	if (dmx.connect(deviceID, numChannels))
	{
		cout << "DMX device is connected" << endl;
	}
	else
	{
		cout << "DMX device is not connected" << endl;
	}
	//	dmx.activateMk2();
	dmx.update(true);

	// The fixtures list is populated. It is not used at the moment but it could be used for DMX events
	// that happen across all the connected devices
	for (int i = 0; i < allEventList.size(); i++)
	{
		fixtures.insert(allEventList[i].fixture);
	}
}

//--------------------------------------------------------------
void ofxDmxManager::initializeValues(string _xmlFilename)
{
	eventIndex = 0;
	numEvents = 0;
	xmlFileName = _xmlFilename;

	//this values are for initialization the actual values come from the XML
	deviceID = 0;
	numChannels = 48;

	allEventList.clear();
	activeEventList.clear();
}

//--------------------------------------------------------------
void ofxDmxManager::loadSettingsXML(string filename)
{
	ofxXmlSettings XML;
	string fullXmlFile = ofToDataPath(filename);
	ofLog(OF_LOG_NOTICE, "Loading: " + fullXmlFile);
	if (!XML.loadFile(fullXmlFile)) ofLog(OF_LOG_ERROR, "Settings xml file not loaded - check file path");

	if (!XML.tagExists(SETTINGS_ID_TAG)) {
		string err = "Incorrrect xml formating. No '" + SETTINGS_ID_TAG + "' tag found";
		ofLog(OF_LOG_ERROR, err);
		return;
	}

	ofLog(OF_LOG_NOTICE, "XML found and contains settings tag: " + SETTINGS_ID_TAG);

	XML.pushTag(SETTINGS_ID_TAG, 0);

	if (XML.tagExists("DeviceID")) {
		deviceID = XML.getValue("DeviceID", 0);
		ofLog(OF_LOG_NOTICE, "DeviceID tag found: " + deviceID);
	}

	if (XML.tagExists("NumberChannels")) {
		numChannels = XML.getValue("NumberChannels", 48);
		ofLog(OF_LOG_NOTICE, "NumberChannels tag found: " + numChannels);
	}

	if (XML.tagExists("MaximumTimeBetweenUpdates")) {
		maximumTimeBetweenUpdates = XML.getValue("MaximumTimeBetweenUpdates", 0.5f);
		ofLog(OF_LOG_NOTICE, "MaximumTimeBetweenUpdates tag found: " + ofToString(maximumTimeBetweenUpdates));
	}

	XML.popTag();

	if (!XML.tagExists(EVENTS_ID_TAG)) {
		string err = "Incorrrect xml formating. No '" + EVENTS_ID_TAG + "' tag found";
		ofLog(OF_LOG_ERROR, err);
		return;
	}

	XML.pushTag(EVENTS_ID_TAG);

	numEvents = XML.getNumTags(EVENT_ID_TAG);
	ofLog(OF_LOG_NOTICE, ofToString(numEvents) + " found: ");

	for (int which = 0; which < numEvents; which++) {
		DMXEvent tempDMXEvent;
		tempDMXEvent.name = XML.getAttribute(EVENT_ID_TAG, "name", "noNameDeclared", which);
		tempDMXEvent.start = ofToFloat(XML.getAttribute(EVENT_ID_TAG, "start", "-1", which));
		tempDMXEvent.duration = ofToFloat(XML.getAttribute(EVENT_ID_TAG, "duration", "0", which));
		tempDMXEvent.type = XML.getAttribute(EVENT_ID_TAG, "type", "linear", which);
		tempDMXEvent.universe = ofToFloat(XML.getAttribute(EVENT_ID_TAG, "universe", "1", which));
		tempDMXEvent.fixture = ofToFloat(XML.getAttribute(EVENT_ID_TAG, "fixture", "0", which));

		XML.pushTag(EVENT_ID_TAG, which);

		tempDMXEvent.channelsForDevice = XML.getNumTags(CHANNELS_ID_TAG);

		for (int i = 0; i < tempDMXEvent.channelsForDevice; i++)
		{
			tempDMXEvent.channels[i] = ofToFloat(XML.getValue(CHANNELS_ID_TAG, "0", i));
		}

		XML.popTag();

		allEventList.push_back(tempDMXEvent);
	}

	sort(allEventList.begin(), allEventList.end(), sortByStart);

	XML.popTag();
}
//--------------------------------------------------------------
void ofxDmxManager::saveSettingsXML()
{
	ofLogNotice("Generating a new ClipMetaData Settings XML");
	ofxXmlSettings xml;

	xml.clear();
	xml.addTag("GlobalSettings");
	xml.pushTag("GlobalSettings");

	xml.setValue("DeviceID", deviceID);
	xml.setValue("NumberChannels", numChannels);
	xml.setValue("MaximumTimeBetweenUpdates", maximumTimeBetweenUpdates);

	xml.popTag();

	xml.addTag(EVENTS_ID_TAG);
	xml.pushTag(EVENTS_ID_TAG);

	int numEvents = allEventList.size();

	for (int which = 0; which < numEvents; which++) {
		xml.addTag(EVENT_ID_TAG);

		xml.setAttribute(EVENT_ID_TAG, "name", allEventList[which].name, which);
		xml.setAttribute(EVENT_ID_TAG, "start", allEventList[which].start, which);
		xml.setAttribute(EVENT_ID_TAG, "duration", allEventList[which].duration, which);
		xml.setAttribute(EVENT_ID_TAG, "type", allEventList[which].type, which);
		xml.setAttribute(EVENT_ID_TAG, "universe", allEventList[which].universe, which);
		xml.setAttribute(EVENT_ID_TAG, "fixture", allEventList[which].fixture, which);

		xml.pushTag(EVENT_ID_TAG, which);

		int numChannels = allEventList[which].channelsForDevice;

		for (int i = 0; i < numChannels; i++)
		{
			xml.addTag(CHANNELS_ID_TAG);
			xml.setValue(CHANNELS_ID_TAG, allEventList[which].channels[i], i);
		}

		xml.popTag();
	}

	xml.popTag();

	ofLogVerbose("Created a new XML containing with the Movie Settings");

	//string xmlFilename = xmlFileName.substr(0, movieFileName.size() - 3);

	//xmlFilename = "ClipMetadata\\" + xmlFilename + "xml";

	//xml.save("ClipMetadata\\" + xmlFilename + "xml");

	xml.save( xmlFileName );
}
//--------------------------------------------------------------
void ofxDmxManager::update(float movieTime)
{
	// This was used to test "jump" events
	// movieTime = movieTime + delay;

	// If the movieTime difference between two updates is bigger than a set amount that is defined in the XML file it is considered a "jump"
	// and the eventIndex is re-evaluated
	if (movieTime - lastTimestamp > maximumTimeBetweenUpdates)
	{
		evaluateEventIndex(movieTime);
	}
	// If the movieTime is lower that the timeStamp of the previous update it is also considered a "jump"
	else if (movieTime < lastTimestamp)
	{
		evaluateEventIndex(movieTime);
	}
	//  If none of the above applies then the normal update happens
	else
	{
		updateInactiveEvents(movieTime);
		updateActiveEvents(movieTime);
	}

	lastTimestamp = movieTime;
}
//--------------------------------------------------------------
void ofxDmxManager::evaluateEventIndex(float movieTime)
{
	// This goes through all the events
	for (int i = 0; i < allEventList.size(); i++)
	{
		// When it finds an event that is after the current time it set the index at this event
		if (allEventList[i].start >= movieTime)
		{
			eventIndex = i;
			activeEventList.clear();
			if (i > 0)
			{
				// It set the DMX value as the one that was supposed to be if the movie was playing normally
				setDMXValues(allEventList[i - 1]);
			}		
			break;
		}
		// This is to evaluate if the current time is after the end of the last event on the list
		else if (movieTime > allEventList.back().start)
		{
			// It set the DMX value as the one that was supposed to be if the movie was playing normally
			setDMXValues(allEventList.back());
		}
	}
}
//--------------------------------------------------------------
void ofxDmxManager::updateInactiveEvents(float movieTime)
{
	if (allEventList.size() > 0 && eventIndex > -1) {
		DMXEvent currentEvent = allEventList.at(eventIndex);
		if (movieTime >= currentEvent.start) {
			triggerDMXEvent(currentEvent, movieTime);
			//	cout << "MovieTime: " << movieTime << " Event is going into the active list: " << currentEvent.name << endl;
		}
	}
	//else cout << "No events" << endl;
}
//--------------------------------------------------------------
void ofxDmxManager::updateActiveEvents(float movieTime)
{
	if (activeEventList.size() > 0)
	{
		float eventStart;
		for (int i = 0; i < activeEventList.size(); i++)
		{
			eventStart = activeEventList[i].start;

			if (movieTime >= eventStart + activeEventList[i].duration)
			{
				activeEventList.erase(activeEventList.begin() + i);
			}
			else
			{
				setDMXValuesLerp(activeEventList[i], eventStart, movieTime);
			//	setDMXValues(activeEventList[i]);
			}
		}
	}
}

//--------------------------------------------------------------
void ofxDmxManager::triggerDMXEvent(DMXEvent currentEvent, float movieTime)
{
	activeEventList.push_back(currentEvent);
	if (eventIndex < numEvents - 1)
	{
		eventIndex++;
	}
	else
	{
		eventIndex = -1;
	}

	cout << "Event:" << currentEvent.name << " + " << movieTime << endl;
}

//--------------------------------------------------------------
void ofxDmxManager::setDMXValuesLerp(DMXEvent currentEvent, float eventStart, float movieTime)
{
	float DMXvalue = 0;
	int fixture = currentEvent.fixture;

	for (int i = 0; i < currentEvent.channelsForDevice; i++)
	{
		float temp = dmx.getLevel(fixture + i);
		DMXvalue = ofxeasing::map(movieTime, eventStart, eventStart + currentEvent.duration, temp / 255.0f, currentEvent.channels[i], ofxeasing::linear::easeIn);
		dmx.setLevel(fixture + i, (int)(DMXvalue*255.0f));
		cout << "Event:" << currentEvent.name << " + " << movieTime << " + " << fixture + i << endl;
	}
	dmx.update(true);
}

//--------------------------------------------------------------
void ofxDmxManager::setDMXValues(DMXEvent currentEvent)
{
	float DMXvalue = 0;
	int fixture = currentEvent.fixture;

	for (int i = 0; i < currentEvent.channelsForDevice; i++)
	{
		DMXvalue = currentEvent.channels[i];
		dmx.setLevel(fixture + i, (int)(DMXvalue*255.0f));
		cout << "Event:" << currentEvent.name << " + " << fixture + i << endl;
	}
	dmx.update(true);
}

//--------------------------------------------------------------
void ofxDmxManager::setDelay(float _delay)
{
	delay += _delay;
}