ofxDmxManager
============

This is a basic interface to setup DMX events and trigger them. It could be used in conjuction with a movie player or just trigger events according to a set timeline. 

It works with ofxDmx. 

#XML Settings Tags

GlobalSettings - This is the root element for the general settings

DeviceID - This is where the id of the connected DMX controller device is set

NumberChannels - This is the amount of channels that the DMX controller will use

MaximumTimeBetweenUpdates - If the movie is not playing linearly this is where the maximum amount of time between two timestamps 

Here are the main tags for an event:

name - A name for the event

start - The start time of the event

duration - The duration of the interpolation between the previous value and the final value

type - This is to define the type of easing that is going to be used. It is not working at the moment. Eveything is linear. 

universe - The universe the DMX controller is working

fixture - The id of the fixture 

Then the amount of channels can be set and the individual values for every channel. As many channels per fixture as needed can be set by using as many channel tags as required.
 
#Versions

1.0 Simple interface to setup DMX events and trigger them

#Author

Igloovision

#Dependencies

ofxDmx

ofxXmlSettings

ofxEasing
