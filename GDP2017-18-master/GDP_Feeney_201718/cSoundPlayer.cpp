#include "cSoundPlayer.h"
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

string g_baseSoundFilePath = "../Sounds/";
FMOD_RESULT g_result;
//global variables
FMOD::System *g_psystem = NULL;

FMOD_VECTOR mlistenerposition = { 0.0f, 0.0f, -10000.0f };
FMOD_VECTOR mforward = { 0.0f, 0.0f, 1.0f };
FMOD_VECTOR mup = { 0.0f, 1.0f, 0.0f };
FMOD_VECTOR mvel = { 0.0f, 0.0f, 0.0f };

//a method to check if an error has occured
void checkForErrors() {
	if (::g_result != FMOD_OK) {
		fprintf(stderr, "FMOD error! (%d) %s\n", ::g_result, FMOD_ErrorString(::g_result));
		exit(-1);
	}
}

//for initliazing fmod
void init_fmod() {

	// Create the main system object.
	::g_result = FMOD::System_Create(&::g_psystem);
	checkForErrors();

	//Initializes the system object, and the msound device. This has to be called at the start of the user's program
	::g_result = ::g_psystem->init(512, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.
	checkForErrors();

}


cSoundPlayer::cSoundPlayer()
{
	masterPitch = 1.0f;
	masterPan = 0.0f;
	aPan = 0.0f;
	bPan = 0.0f;
	cPan = 0.0f;
	selectedGroupPan = 0.0f;
	init_fmod();
	//Here is where you will go through a text file and load all the needed sounds,
	//maybe their locations in the party as well
	string filename = "SoundsList.txt";
	string line;
	ifstream inFile(filename.c_str());
	MediaObject tempMediaObject;
	bool inScene = false;
	bool inSample = false;
	int groupIndex = -1;
	selectedSoundIndex = 0;
	changingSoundMode = 0;

	//get instance of master channel
	::g_result = ::g_psystem->getMasterChannelGroup(&group_master);
	checkForErrors();

	//create channel groups
	::g_result = ::g_psystem->createChannelGroup("Group A", &group_a);
	checkForErrors();
	selectedGroupPointer = group_a;

	::g_result = ::g_psystem->createChannelGroup("Group B", &group_b);
	checkForErrors();

	::g_result = ::g_psystem->createChannelGroup("Group C", &group_c);
	checkForErrors();

	::g_result = ::g_psystem->createChannelGroup("Group D", &group_d);
	checkForErrors();

	//Add channel groups to master
	::g_result = group_master->addGroup(group_a);
	checkForErrors();
	::g_result = group_master->addGroup(group_b);
	checkForErrors();
	::g_result = group_master->addGroup(group_c);
	checkForErrors();
	::g_result = group_master->addGroup(group_d);
	checkForErrors();




	try {
		while (getline(inFile, line))
		{
			if (line == "")
			{
				continue;
			}


			if (line.find("stream:") != string::npos) {
				//create a new media object and load that into the scene
				line.replace(0, 8, "");
				FMOD::Sound* sound1;
				FMOD::Channel* channel1 = 0;
				tempMediaObject.fileName = line;
				line = ::g_baseSoundFilePath + line;
				::g_result = ::g_psystem->createSound(line.c_str(), FMOD_CREATESTREAM, 0, &sound1);
				checkForErrors();

				::g_result = sound1->setMode(FMOD_LOOP_NORMAL);
				checkForErrors();

				::g_result = ::g_psystem->playSound(sound1, 0, false, &channel1);
				checkForErrors();

				channel1->setVolume(0.2f);
				checkForErrors();

				//assign sound and channel to the temp media object
				tempMediaObject.sound = sound1;
				tempMediaObject.channel = channel1;
				//make a new mediaobject
				MediaObject* mediaObject = new MediaObject(tempMediaObject);
				mediaObject->streamObject = true;
				mediaObject->volume = 0.2f;
				mediaObject->groupID = groupIndex;
				//put the media object on the backest scene object
				switch (groupIndex)
				{
				case 0:
					mediaObject->channel->setChannelGroup(group_a);
					break;
				case 1:
					mediaObject->channel->setChannelGroup(group_b);
					break;
				case 2:
					mediaObject->channel->setChannelGroup(group_c);
					break;
				case 3:
					mediaObject->channel->setChannelGroup(group_d);
					break;
				}

				mediaObjects.push_back(mediaObject);


				continue;
			}

			if (line.find("sample:") != string::npos) {
				//create a new media object and load that into the scene
				line.replace(0, 8, "");
				FMOD::Sound* sound1;
				FMOD::Channel* channel1 = 0;
				tempMediaObject.fileName = line;
				line = ::g_baseSoundFilePath + line;
				::g_result = ::g_psystem->createSound(line.c_str(), FMOD_CREATESAMPLE, 0, &sound1);
				checkForErrors();

				::g_result = sound1->setMode(FMOD_LOOP_OFF);
				checkForErrors();

				//i think setting true in the following line will start it paused
				::g_result = ::g_psystem->playSound(sound1, 0, true, &channel1);
				checkForErrors();

				channel1->setVolume(0.2f);
				checkForErrors();

				//assign sound and channel to the temp media object
				tempMediaObject.sound = sound1;
				tempMediaObject.channel = channel1;
				//make a new mediaobject
				MediaObject* mediaObject = new MediaObject(tempMediaObject);
				mediaObject->streamObject = true;
				mediaObject->volume = 0.2f;
				mediaObject->groupID = groupIndex;
				//put the media object on the backest scene object
				switch (groupIndex)
				{
				case 0:
					mediaObject->channel->setChannelGroup(group_a);
					break;
				case 1:
					mediaObject->channel->setChannelGroup(group_b);
					break;
				case 2:
					mediaObject->channel->setChannelGroup(group_c);
					break;
				case 3:
					mediaObject->channel->setChannelGroup(group_d);
					break;
				}

				mediaObjects.push_back(mediaObject);


				continue;
			}
			if (line.find("Group Start") != string::npos) {
				groupIndex++;
				continue;
				//change the timesplayed for the most recent media object
			}
			if (line.find("Group End") != string::npos) {
				continue;
				//change the timesplayed for the most recent media object
			}

			if (line.find("volume: ") != string::npos) {
				line.replace(0, 8, "");
				mediaObjects.back()->volume = atof(line.c_str());
				mediaObjects.back()->channel->setVolume(mediaObjects.back()->volume);
				continue;
				//change the timesplayed for the most recent media object
			}
			if (line.find("position: ") != std::string::npos) {
				line.replace(0, 10, "");

				mediaObjects.back()->ThirdDimension = true;

				std::string number;
				bool xValue = true;
				for (int stringIndex = 0; stringIndex < line.size(); stringIndex++)
				{
					if (line[stringIndex] != ',')
					{
						number += line[stringIndex];
					}
					else if (xValue)
					{
						xValue = false;
						mediaObjects.back()->position.x = stof(number);
						number = "";
					}
					else
					{
						mediaObjects.back()->position.y = stof(number);
						number = "";
					}
				}
				mediaObjects.back()->position.z = stof(number);

				FMOD_VECTOR sound_position = { mediaObjects.back()->position.x,
					0.0f,
					mediaObjects.back()->position.z };
				FMOD_VECTOR sound_velocity = { 0.0f, 0.0f, 0.0f };

				mediaObjects.back()->channel->set3DAttributes(&sound_position, &sound_velocity);
				checkForErrors();

				checkForErrors();
				continue;
			}
		}

	}
	catch (...)
	{
	}

	//set 3d listener attributes
	::g_result = ::g_psystem->set3DListenerAttributes(0, &mlistenerposition, &mvel, &mforward, &mup);
	checkForErrors();

	//group_master->setMute(true);
	group_a->setMute(false);
	group_b->setMute(true);
	group_c->setMute(true);
	group_d->setMute(false);

}

cSoundPlayer::~cSoundPlayer()
{
	int size = mediaObjects.size();
	for (int i = 0; i < mediaObjects.size(); i++)
	{
		mediaObjects[i]->Release();
		delete mediaObjects[i];
	}
	group_a->release();
	group_b->release();
	group_c->release();
	group_d->release();

}


void cSoundPlayer::UpdateSystem()
{
	::g_psystem->update();
}

void cSoundPlayer::Startup()
{
}

void cSoundPlayer::Stop()
{
}

void cSoundPlayer::PlaySound(int index)
{
	if (mediaObjects.size() > index)
	{
		::g_psystem->playSound(mediaObjects[index]->sound,
			0, false, &mediaObjects[index]->channel);
	}
}

void cSoundPlayer::SwitchToGameOverMusic()
{
	//group_a->setMute(true);
	//group_b->setMute(false);
	//group_c->setMute(true);
	//group_d->setMute(true);
}

void cSoundPlayer::StartGameMusic()
{
	group_c->setMute(false);
	group_d->setMute(true);
	this->PlaySound(12);
	this->mediaObjects[12]->channel->setVolume(mediaObjects[12]->volume);
	this->mediaObjects[12]->channel->setChannelGroup(group_c);
}
