#ifndef _cSoundPlayerHG_
#define _cSoundPlayerHG_
#include <vector>
#include "MediaObject.h"


enum eChangingAttribute {
	VOLUME = 0,
	BALANCE,
	PITCH,
	PLAYBACKSPEED,
	FREQUENCY
};

class cSoundPlayer
{
public:
	cSoundPlayer();
	~cSoundPlayer();
	//methods

	void UpdateSystem();
	void Startup();
	void Stop();

	//Master and groups
	FMOD::ChannelGroup *group_a, *group_b, *group_c, *group_d, *group_master;
	FMOD::ChannelGroup *selectedGroupPointer;
	//data members
	eChangingAttribute changingAttribute = eChangingAttribute::VOLUME;
	float masterPan;
	float aPan;
	float bPan;
	float cPan;
	float selectedGroupPan;
	int selectedGroup = 0;
	int selectedSoundIndex;
	int changingSoundMode;
	std::vector<MediaObject*> mediaObjects;
	float masterPitch;
	void PlaySound(int index);
	void SwitchToGameOverMusic();
	void StartGameMusic();

};


#endif // !_cSoundPlayerHG_
