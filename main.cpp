#define OLC_PGE_APPLICATION
#include <windows.h>
#include "olcPixelGameEngine.h"
#include <fstream>
#include <array>

/* commenting out all this windows app stuff for now
/*  Declare Windows procedure  */
//LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM); 

/*  Make the class name into a global variable  *
char szClassName[ ] = "WindowsApp";

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nFunsterStil)

{
    
    HWND hwnd;               /* This is the handle for our window *
    MSG messages;            /* Here messages to the application are saved *
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure *
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows *
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks *
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer *
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu *
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class *
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default color as the background of the window *
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program *
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation *
           szClassName,         /* Classname *
           "Windows App",       /* Title Text *
           WS_OVERLAPPEDWINDOW, /* default window *
           CW_USEDEFAULT,       /* Windows decides the position *
           CW_USEDEFAULT,       /* where the window ends up on the screen *
           544,                 /* The programs width *
           375,                 /* and height in pixels *
           HWND_DESKTOP,        /* The window is a child-window to desktop *
           NULL,                /* No menu *
           hThisInstance,       /* Program Instance handler *
           NULL                 /* No Window Creation data *
           );

    /* Make the window visible on the screen *
    ShowWindow (hwnd, nFunsterStil);

    /* Run the message loop. It will run until GetMessage() returns 0 *
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages *
        TranslateMessage(&messages);
        /* Send message to WindowProcedure *
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave *
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  *

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages *
    {
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue *
            break;
        default:                      /* for messages that we don't deal with *
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
*/
struct MidiEvent
{
       enum class Type
       {
            NoteOff,
            NoteOn,
            Other
       } event;
       
       uint8_t nKey = 0;
       uint8_t = nVelocity = 0;
       uint32_t = nWallTick = 0;
       uint32_t = nDeltaTick = 0;
};

struct MidiNote
{
       uint8_t nKey = 0;
       uint8_t nVelocity = 0;
       uint32_t nStartTime = 0;
       uint32_t nDuration = 0;
};

struct MidiTrack
{
       std::string sName;
       std::string sInstrument;
       std::vector<MidiEvent> vecEvents;
       std::vector<MidiNote> vecNotes;
       uint8_t nMaxNote = 64;
       uint8_t nMinNote = 64;
};

class MidiFile
{
      enum EventName : uint_t
      {
           VoiceNoteOff = 0x80,
           VoiceNoteOn = 0x90,
           VoiceAftertouch = 0xA0,
           VoiceControlChange = 0xB0,
           VoiceProgramChange = 0xC0,
           VoiceChannelPressure = 0xD0,
           VoicePitchBend = 0xE0,
           SystemExclusive = 0xF0,
      };
      
      enum MetaEventName : uint8_t
	  {
		   MetaSequence = 0x00,
		   MetaText = 0x01,
		   MetaCopyright = 0x02,
		   MetaTrackName = 0x03,
		   MetaInstrumentName = 0x04,
		   MetaLyrics = 0x05,
		   MetaMarker = 0x06,
		   MetaCuePoint = 0x07,
		   MetaChannelPrefix = 0x20,
		   MetaEndOfTrack = 0x2F,
 	 	   MetaSetTempo = 0x51,
		   MetaSMPTEOffset = 0x54,
		   MetaTimeSignature = 0x58,
           MetaKeySignature = 0x59,
           MetaSequencerSpecific = 0x7F,
	   };
      
      public:
             MidiFile()
             {
                       
             }
             
             MidiFile(const std::string& sFileName)
             {
                  ParseFile(sFileName);
             }
             
             bool ParseFile(const std::string& sFileName)
             {
                  // opens the midi file as a stream, returns false if theres an error
                  std::ifstream ifs;
                  ifs.open(sFileName, std::fstream::in | std::ios::binary);
                  if (!ifs.isOpen())
                     return false;
                     
                  auto Swap32 = [](uint32_t n) // takes and unsigned integer and swaps it to the correct order
                  {
                       return (((n >> 24) & 0xff) | ((n << 8) & 0xff0000) | ((n >> 8) & 0xff00 | ((n << 24) & 0xff000000));
                  };
                  
                  auto Swap16 = [](uint16_t n) // swaps 16bit numbers to the correct order
                  {
                       return ((n >> 8) | (n << 8));
                  };
                  
                  // reads strings from the file by building them byte by byte from the file stream into a single string 
                  auto ReadString = [&ifs](uint32_t nLength)
                  {
                       std::string s;
                       for (uint32_t i=0; i<nLength; i++) s += ifs.get();
                       return s;
                  };
                  
                  // reads and stores if the first bit of any midi event is signalling that more than one byte will need to be read
                  auto ReadValue = [&ifs]()
                  {
                       uint32_t nValue = 0;
                       uint8_t nByte = 0;
                       
                       // reads byte
                       nValue = ifs.get();
                       
                       // if the bit is set, it keeps reading
                       if(nValue & 0x80)
                       {
                             // saves the bottom 7 bits
                             nValue &= 0x7f;
                             
                             do
                             {      // reads bytes and saves the bottom 7 bits by adding them to the end of nValue
                                    nByte = ifs.get();
                                    nValue = (nValue << 7) | (nByte & 0x7f);
                             } while (nByte & 0x80); // loops until the first bit is unset 
                       }
                       
                       return nValue;
                  };
                  
                  uint32 n32 = 0;
                  uint16 n16 = 0;
                  
                  // reads header
                  ifs.read((char*)&n32, sizeof(uint32_t));
                  uint32_t nFileID = Swap32(n32);
                  ifs.read((char*)&n32, sizeof(uint32_t));
                  uint32_t nHeaderLength = Swap32(n32);
                  ifs.read((char*)&n16, sizeof(uint16_t));
                  uint16_t nFormat = Swap16(n16);
                  ifs.read((char*)&n16, sizeof(uint16_t));
                  uint16_t nTrackChunks = Swap16(n16); // number of MIDI tracks in the file
                  ifs.read((char*)&n16, sizeof(uint16_t));
                  uint16_t nDivision = Swap16(n16);
                  
                  for (uint16_t nChunk = 0; nChunk < nTrackChunks; nChunk++)
                  {
                      bool bEndOfTrack = false:
                      uint8_t nPreviousStatus = 0;
                      
                      std:cout << "New Track" << std::endl; // line probably not needed but keep it for now
                      
                      // reads track header
                      ifs.read((char*)&n32, sizeof(uint32_t));
                      uint32_t nTrackID = Swap32(n32);
                      ifs.read((char*)&n32, sizeof(uint32_t));
                      uint32_t nTrackLength = Swap32(n32);
                      
                      vecTracks.push_back(MidiTrack()); // puts a blank MIDI track into the vector
                      
                      while (!ifs.eof() && !bEndOfTrack) // loops until the end of the file has been reached or the end of track flag has been set
                      {
                            uint32_t nStatusTime = 0;
                            uint8_t nStatus = 0;
                            
                            nStatusTimeDelta = ReadValue(); // gets the previous MIDI event's delta time value
                            nStatus = ifs.get(); // gets status from the file stream
                            
                            if(nStatus < 0x80) // if the event has no ID, the ID is set to that of the previous read event
                            {
                                   nStatus = nPreviousStatus;
                                   ifs.seekg(-1, std::ios_base::cur);  // steps backwards into the stream to ensure we stay on track
                            }
                            
                            // handles each type of MIDI event
                            if ((nStatus & 0xF0) == EventName::VoiceNoteOff)
                            {
                                   // sets up the previous status var
                                   nPreviousStatus = nStatus;
                                   
                                   // reads the channel, note ID (pitch), and velocity
                                   uint8_t nChannel = nStatus; & 0x0F;
                                   uint8_t nNoteID = ifs.get();
                                   uint8_t nNoteVelocity = ifs.get();
                                   
                                   //pushes event into vector for the appropriate track
                                   vecTracks[nChunk].vecEvents.push_back({ MidiEvent::Type::NoteOff, nNoteID, nVelocity, nStatusTimeDelta });
                            
                            } else if ((nStatus & 0xF0) == EventName::VoiceNoteOn)
                            {      
                                   // sets up the previous status var
                                   nPreviousStatus = nStatus;
                                   
                                   // reads the channel, note ID (pitch), and velocity
                                   uint8_t nChannel = nStatus; & 0x0F;
                                   uint8_t nNoteID = ifs.get();
                                   uint8_t nNoteVelocity = ifs.get();
                                   
                                   //pushes event into vector for the appropriate track
                                   if (nNoteVelocity == 0)
                                       vecTracks[nChunk].vecEvents.push_back({ MidiEvent::Type::NoteOff, nNoteID, nVelocity, nStatusTimeDelta });
                                   else
                                       vecTracks[nChunk].vecEvents.push_back({ MidiEvent::Type::NoteOn, nNoteID, nVelocity, nStatusTimeDelta });
                                   
                            } else if ((nStatus & 0xF0) == EventName::VoiceAfterTouch)
                            {
                                   // sets up the previous status var
                                   nPreviousStatus = nStatus;
                                   
                                   // reads the channel, note ID (pitch), and velocity
                                   uint8_t nChannel = nStatus; & 0x0F;
                                   uint8_t nNoteID = ifs.get();
                                   uint8_t nNoteVelocity = ifs.get();
                                   
                                   // pushes "other" event into vector, we weill be changing this later
                                   vecTracks[nChunk].vecEvents.push_back({ MidiEvent::Type::Other });
                                   
                            } else if ((nStatus & 0xF0) == EventName::VoiceControlChange)
                            {
                                   // sets up the previous status var
                                   nPreviousStatus = nStatus;
                                   
                                   // reads the channel, note ID (pitch), and velocity
                                   uint8_t nChannel = nStatus; & 0x0F;
                                   uint8_t nNoteID = ifs.get();
                                   uint8_t nNoteVelocity = ifs.get();
                                   
                                   // pushes "other" event into vector, we weill be changing this later
                                   vecTracks[nChunk].vecEvents.push_back({ MidiEvent::Type::Other });
                            } else if ((nStatus & 0xF0) == EventName::VoiceProgramChange)
                            {
                                   // sets up the previous status var
                                   nPreviousStatus = nStatus;
                                   
                                   // reads the channel and program ID
                                   uint8_t nChannel = nStatus; & 0x0F;
                                   uint8_t nProgramID = ifs.get();
                                   
                                   // pushes "other" event into vector, we weill be changing this later
                                   vecTracks[nChunk].vecEvents.push_back({ MidiEvent::Type::Other });
                            } else if ((nStatus & 0xF0) == EventName::VoiceChannelPressure)
                            {
                                   // sets up the previous status var
                                   nPreviousStatus = nStatus;
                                   
                                   // reads the channel and pressure
                                   uint8_t nChannel = nStatus; & 0x0F;
                                   uint8_t nChannelPressure = ifs.get();
                                   
                                   // pushes "other" event into vector, we weill be changing this later
                                   vecTracks[nChunk].vecEvents.push_back({ MidiEvent::Type::Other });
                            } else if ((nStatus & 0xF0) == EventName::VoicePitchBend)
                            {
                                   // sets up the previous status var
                                   nPreviousStatus = nStatus;
                                   
                                   // reads the channel and idk what those mean lmao
                                   uint8_t nChannel = nStatus; & 0x0F;
                                   uint8_t nLS7B = ifs.get();
                                   uint8_t nMS7B = ifs.get();
                                   
                                   // pushes "other" event into vector, we weill be changing this later
                                   vecTracks[nChunk].vecEvents.push_back({ MidiEvent::Type::Other });
                            } else if ((nStatus & 0xF0) == EventName::SystemExclusive)
                            {
                                   nPreviousStatus = 0;

					               if (nStatus == 0xFF)
					               {
                                       // Meta Message
						               uint8_t nType = ifs.get();
						               uint8_t nLength = ReadValue();

						               switch (nType)
						               {
                                       case MetaSequence:
				                            std::cout << "Sequence Number: " << ifs.get() << ifs.get() << std::endl;
				                            break;
                                       case MetaText:
							                std::cout << "Text: " << ReadString(nLength) << std::endl;
							                break;
						               case MetaCopyright:
							                std::cout << "Copyright: " << ReadString(nLength) << std::endl;
							                break;
						               case MetaTrackName:
							                vecTracks[nChunk].sName = ReadString(nLength);
							                std::cout << "Track Name: " << vecTracks[nChunk].sName << std::endl;							
							                break;
						               case MetaInstrumentName:
                                            vecTracks[nChunk].sInstrument = ReadString(nLength);
							                std::cout << "Instrument Name: " << vecTracks[nChunk].sInstrument << std::endl;
							                break;
						               case MetaLyrics:
							                std::cout << "Lyrics: " << ReadString(nLength) << std::endl;
							                break;
						               case MetaMarker:
							                std::cout << "Marker: " << ReadString(nLength) << std::endl;
							                break;
						               case MetaCuePoint:
							                std::cout << "Cue: " << ReadString(nLength) << std::endl;
							                break;
						               case MetaChannelPrefix:
							                std::cout << "Prefix: " << ifs.get() << std::endl;
							                break;
						               case MetaEndOfTrack:
							                bEndOfTrack = true;
							                break;
                      	               case MetaSetTempo:
							                // Tempo is in microseconds per quarter note	
							                if (m_nTempo == 0)
							                {
								                 (m_nTempo |= (ifs.get() << 16));
								                 (m_nTempo |= (ifs.get() << 8));
								                 (m_nTempo |= (ifs.get() << 0));
								                 m_nBPM = (60000000 / m_nTempo);
								                 std::cout << "Tempo: " << m_nTempo << " (" << m_nBPM << "bpm)" << std::endl;
							                }
							                break;
						                case MetaSMPTEOffset:
							                std::cout << "SMPTE: H:" << ifs.get() << " M:" << ifs.get() << " S:" << ifs.get() << " FR:" << ifs.get() << " FF:" << ifs.get() << std::endl;
							                break;
						                case MetaTimeSignature:
							                std::cout << "Time Signature: " << ifs.get() << "/" << (2 << ifs.get()) << std::endl;
							                std::cout << "ClocksPerTick: " << ifs.get() << std::endl;

							                // A MIDI "Beat" is 24 ticks, so specify how many 32nd notes constitute a beat
							                std::cout << "32per24Clocks: " << ifs.get() << std::endl;
							                break;
						                case MetaKeySignature:
							                std::cout << "Key Signature: " << ifs.get() << std::endl;
							                std::cout << "Minor Key: " << ifs.get() << std::endl;
							                break;
						                case MetaSequencerSpecific:
							                std::cout << "Sequencer Specific: " << ReadString(nLength) << std::endl;
							                break;
						                default:
							                std::cout << "Unrecognised MetaEvent: " << nType << std::endl;
					                    }
					               }

					               if (nStatus == 0xF0)
					               {
                                       // System Exclusive Message Begin
						               std::cout << "System Exclusive Begin: " << ReadString(ReadValue())  << std::endl;
					               }

					               if (nStatus == 0xF7)
					               {
						               // System Exclusive Message End
						               std::cout << "System Exclusive End: " << ReadString(ReadValue()) << std::endl;
					               }
                            }
                            else // error message for invalid status byte
                            {
                                std::cout << "Invalid Status Byte: " << nStatus << std::endl;
                            }
                            
                      }
             }
             
             for (auto& track : vecTracks)
             {
                 uint32_t nWallTime = 0;
                 
                 std::list<MidiNote> listNotesBeingProcessed;
                 
                 for (auto& event : track.vecEvents)
                 {
                     nWallTime += event.nDeltaTick;
                     
                     if (event.event == MidiEvent::Type::NoteOn)
                     {
                          // adds new note
                          listNotesBeingProcessed.push_back({ event.nKey, event.nVelocity, nWallTime, 0 });
                     }
                     
                     if (event.event == MidiEvent::Type::NoteOff)
                     {
                          auto note = std::find_if(listNotesBeingProcessed.begin(), listNotesBeingProcessed.end(), [&](const MidiNote& n) { return n.nKey == event.nKey; }); // searches for the note that is being turned off
                          if (note != listNotesBeingProcessed.end())
                          {
                               note->nDuration = nWallTime - note->nStartTime; // figures out what the note duration is   
                               track.vecNotes.push_back(*note); // placed in the vector of notes
                               
                               // determines the min and max note which will be used for visualization
                               track.nMinNote = std::min(track.nMinNote, note->nKey);
                               track.nMaxNote = std::max(track.nMaxNote, note->nKey);
                               
                               listNotesBeingProcessed.erase(note); // removed from processing list
                          }
                     }
                                     
                 }
             }
             
      public:
             std::vector<MidiTrack> vecTracks;
};

class olcMIDIViewer : public olc::PixelGameEngine
{
public:
	olcMIDIViewer()
	{
		sAppName = "MIDI File Viewer";
	}


	MidiFile midi;

	//HMIDIOUT hInstrument;
	size_t nCurrentNote[16]{ 0 };

	double dSongTime = 0.0;
	double dRunTime = 0.0;
	uint32_t nMidiClock = 0;
	

public:
	bool OnUserCreate() override
	{

		midi.ParseFile("battle-theme");

		/*
		int nMidiDevices = midiOutGetNumDevs();
		if (nMidiDevices > 0)
		{
			if (midiOutOpen(&hInstrument, 2, NULL, 0, NULL) == MMSYSERR_NOERROR)
			{
				std::cout << "Opened midi" << std::endl;
			}
		}
		*/


		return true;
	}

	float nTrackOffset = 1000;

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLACK);
		uint32_t nTimePerColumn = 50;
		uint32_t nNoteHeight = 2;
		uint32_t nOffsetY = 0;
		
		if (GetKey(olc::Key::LEFT).bHeld) nTrackOffset -= 10000.0f * fElapsedTime;
		if (GetKey(olc::Key::RIGHT).bHeld) nTrackOffset += 10000.0f * fElapsedTime;


		for (auto& track : midi.vecTracks)
		{
			if (!track.vecNotes.empty())
			{
				uint32_t nNoteRange = track.nMaxNote - track.nMinNote;

				FillRect(0, nOffsetY, ScreenWidth(), (nNoteRange + 1) * nNoteHeight, olc::DARK_GREY);
				DrawString(1, nOffsetY + 1, track.sName);

				for (auto& note : track.vecNotes)
				{
					FillRect((note.nStartTime - nTrackOffset) / nTimePerColumn, (nNoteRange - (note.nKey - track.nMinNote)) * nNoteHeight + nOffsetY, note.nDuration / nTimePerColumn, nNoteHeight, olc::WHITE);
				}
				 
				nOffsetY += (nNoteRange + 1) * nNoteHeight + 4;
			}
		}

		// BELOW - ABSOLUTELY HORRIBLE BODGE TO PLAY SOUND
		// DO NOT USE THIS CODE...
		
		/*
		dRunTime += fElapsedTime;
		uint32_t nTempo = 4;
		int nTrack = 1;
		while (dRunTime >= 1.0 / double(midi.m_nBPM * 8))
		{
			dRunTime -= 1.0 / double(midi.m_nBPM * 8);

			// Single MIDI Clock
			nMidiClock++;

			int i = 0;
			int nTrack = 1;
			//for (nTrack = 1; nTrack < 3; nTrack++)
			{
				if (nCurrentNote[nTrack] < midi.vecTracks[nTrack].vecEvents.size())
				{
					if (midi.vecTracks[nTrack].vecEvents[nCurrentNote[nTrack]].nDeltaTick == 0)
					{
						uint32_t nStatus = 0;
						uint32_t nNote = midi.vecTracks[nTrack].vecEvents[nCurrentNote[nTrack]].nKey;
						uint32_t nVelocity = midi.vecTracks[nTrack].vecEvents[nCurrentNote[nTrack]].nVelocity;

						if (midi.vecTracks[nTrack].vecEvents[nCurrentNote[nTrack]].event == MidiEvent::Type::NoteOn)
							nStatus = 0x90;
						else
							nStatus = 0x80;

						midiOutShortMsg(hInstrument, (nVelocity << 16) | (nNote << 8) | nStatus);
						nCurrentNote[nTrack]++;
					}
					else
						midi.vecTracks[nTrack].vecEvents[nCurrentNote[nTrack]].nDeltaTick--;
				}
			}
		}

		if (GetKey(olc::Key::SPACE).bPressed)
		{
			midiOutShortMsg(hInstrument, 0x00403C90);
		}

		if (GetKey(olc::Key::SPACE).bReleased)
		{
			midiOutShortMsg(hInstrument, 0x00003C80);
		}
		*/


		return true;
	}

	
};

int main()
{
	olcMIDIViewer demo;
	if (demo.Construct(1280, 960, 1, 1))
		demo.Start();
	return 0;
}
