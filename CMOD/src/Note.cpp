/*
CMOD (composition module)
Copyright (C) 2005  Sever Tipei (s-tipei@uiuc.edu)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

//----------------------------------------------------------------------------//
//
//   Note.cpp
//
//----------------------------------------------------------------------------//

#include "Note.h"
#include "Event.h"
#include "Output.h"
#include "Rational.h"
#include "tables.h"
#include "tables.cpp"
#include <string>

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

using namespace std;

string timesignature;
int beatEDUs;
static int sBeat;
static int eBeat;
static int sDiv;
static int eDiv;
static int sBar;
static int eBar;
static int barEDUs;
ofstream * outputFile;
//extern ofstream * outputFile;
ofstream * outscore;
string outstring;
vector<Note*> all_notes;
vector<Note*> all_notes_orig;

//----------------------------------------------------------------------------//

Note::Note(TimeSpan ts, Tempo tempo) : ts(ts), tempo(tempo),
  pitchNum(0), octaveNum(0), octavePitch(0), loudnessNum(0) {

//if ( &outputFile == " ") {
//  outputFile = new std::ofstream;
//  outputFile->open( "score.ly", ios::app);



//}
}

//----------------------------------------------------------------------------//

Note::Note() {
}

//----------------------------------------------------------------------------//

Note:: ~Note() {
  delete outputFile;
  delete outscore;
}

//----------------------------------------------------------------------------//

Note::Note(const Note& other) {
  ts = other.ts;
  tempo = other.tempo;
  pitchNum = other.pitchNum;
  octaveNum = other.octaveNum;
  octavePitch = other.octavePitch;
  pitchName = other.pitchName;
  loudnessNum = other.loudnessNum;
  loudnessMark = other.loudnessMark;
  modifiers = other.modifiers;
}

//----------------------------------------------------------------------------//

bool Note::operator < (const Note& rhs) {
  //Sort notes by their *global* start time.
  return (ts.start < rhs.ts.start);
}

//----------------------------------------------------------------------------//

void Note::setPitchWellTempered(int pitchNum) {

//octaveNum = pitchNum / pitchNames.size();
//octavePitch = pitchNum % pitchNames.size();
//pitchName = pitchNames[octavePitch];
  octaveNum = pitchNum / 12;
//*notaFile << "Octave Number " << octaveNum << endl;
  octavePitch = pitchNum % 12;
  pitchName = pitchNames[octavePitch];
  pitch_out = OutNames[octavePitch];
  string signs[8] = {",,,",",,",",","","'","''","'''","''''"};
  string sign = signs[octaveNum];
  pitch_out = pitch_out + sign;
  
/*  -----------old code ---------------------
  if (octavePitch == 0) {
    pitchName = "C";
  } else if (octavePitch == 1) {
    pitchName = "C#";
  } else if (octavePitch == 2) {
    pitchName = "D";
  } else if (octavePitch == 3) {
    pitchName = "Eb";
  } else if (octavePitch == 4) {
    pitchName = "E";
  } else if (octavePitch == 5) {
    pitchName = "F";
  } else if (octavePitch == 6) {
    pitchName = "F#";
  } else if (octavePitch == 7) {
    pitchName = "G";
  } else if (octavePitch == 8) {
    pitchName = "G#";
  } else if (octavePitch == 9) {
    pitchName = "A";
  } else if (octavePitch == 10) {
    pitchName = "Bb";
  } else if (octavePitch == 11) {
    pitchName = "B";
  } else {
    cerr << "Invalid pitchNum or pitchName !" << endl;
  }
*/

  Output::addProperty("Pitch Number", pitchNum, "semitones");
  Output::addProperty("Pitch Name", pitchName);
  Output::addProperty("Octave Number", octaveNum);
  Output::addProperty("Pitch In Octave", octavePitch);

  //*outputFile<< setw(5) <<"Pitch " << pitchNum << setw(5) << octaveNum << setw(3) 
    //   << pitchName;
  

}

//----------------------------------------------------------------------------//

int Note::HertzToPitch(float freqHz) {

  int pitchNum;

  if ( freqHz >= CEILING || freqHz <= MINFREQ) {
    cerr << "WARNING: frequency out of range" << endl; 
  }

  pitchNum = rint(12 * log2(freqHz / C0));

  return pitchNum;
}

//----------------------------------------------------------------------------//

void Note::setLoudnessMark(int dynamicNum, vector<string> dynamicNames) {
  loudnessNum = dynamicNum;
  loudnessMark = dynamicNames[loudnessNum];
  Output::addProperty("Dynamic", loudnessMark);
  Output::addProperty("Dynamic Level", loudnessNum);
}

//----------------------------------------------------------------------------//

void Note::setLoudnessSones(float sones) {
  loudnessNum = -1;
  if(sones < 0 || sones > 256) {
    cerr << "Note received invalid value for sones!" << endl;
    exit(1);
  } else if(sones <= 4) {
    loudnessMark = "ppp";
  } else if(sones <= 8) {
    loudnessMark = "pp";
  } else if(sones <= 16) {
    loudnessMark = "p";
  } else if(sones <= 32) {
    loudnessMark = "mp";
  } else if(sones <= 45) {
    loudnessMark = "mf";
  } else if(sones <= 64) {
    loudnessMark = "f";
  } else if(sones <= 128) {
    loudnessMark = "ff";
  } else if (sones <= 256) {
    loudnessMark = "fff";
  }

  Output::addProperty("Loudness", loudnessMark);
  loudness_out = char(92) + loudnessMark;

  //*outputFile << setw(6) << loudnessMark;
}


//----------------------------------------------------------------------------//

void Note::setModifiers(vector<string> modNames) 
{
  for(int i = 0; i < modNames.size(); i++) {
    modifiers.push_back(modNames[i]);
    Output::addProperty("Playing Techniques:", modNames[i]);

    if (modNames[i] == "tenuto")
	modifiers_out.push_back(modNames[i]);
   // *outputFile << setw(8) << modNames[i];
  }
  
 // *outputFile << endl; *outputFile << endl;
}


//-------------------------------------------------------------
/* bool is_valid(Rational<int> temp)
  Description: check if input rational number is a power of 2
  input: temp - a rational number
  output: true or false
 */
bool is_valid(Rational<int> temp){

  for (int i=0; i<10; i++){
	Rational<int> x(pow_of_2[i]);
	if (x == temp)
		return true;
  }
  return false;
}

//--------------------------------------------------------------
/* void translate(string & s, string pitch_out)
  Description: Some duration need two notes to notate, such notation is marked starting with an '!'. This function read this mark and split it into two notes.
  input: s - string that holds the duration info. s will be modified
	 pitch_out - output pitch
  output: NONE
 */
void translate(string & s, string pitch_out){
  int i = 1;
  string ret = "";
  //get first half
  while (s[i]!=' '){
	ret = ret + s[i];
	i++;
  }
  i++;
  //get second half
  string t = s.substr(i);
  ret = ret + "~ " + pitch_out + t;
  s = ret;
}


//---------------------------------------------------------------
/* string Note::convert_dur_to_type
  Description: convert duration (a number in EDU) to a string represents the type of the note(s)
  input: dur - duration of the note
  output: string
 */
string Note::convert_dur_to_type(int dur){
  /* convert duration to type of notes */
  int temp;
  string buffer;
  Rational<int> ratio(dur, beatEDUs);
  tuplet = 0;
  // regular notes
  if (is_valid(ratio)){
  	Rational<int> four(4);
  	Rational<int> type = four / ratio;
	return type.toPrettyString();
  }
  //dotted notes
  if (ratio == 6)
	return "1.";
  if ((ratio.Num() == 3) && (ratio.Den()%2 == 0)) {
	temp = ratio.Den() * 2;
	Rational<int> x(temp);
	buffer = x.toPrettyString() + ".";
	return buffer;
  }
  //other irregular notes
  for (int i=0; i<8; i++){
	Rational<int> x(ratios[i]);
	if (x == ratio){
		tuplet = 1;
		tuplet_name = tuplet_names[i];
		string type1 = types[i];
		if (type1[0] == '!') 
			translate(type1, pitch_out);
		return type1;
	}
  }
  return "4";

}

// this function is currently not used
string convert(int dur){
  Rational<int> ratio(dur, beatEDUs);
  if (is_valid(ratio)){
  	Rational<int> four(4);
  	Rational<int> type = four / ratio;
	return "r" + type.toPrettyString();
  }
  for (int i=0; i<8; i++){
	Rational<int> x(rest_ratios[i]);
	if (x == ratio){
		return rest_signs[i];
	}
  }
  return "";
}

// this function is currently not used
string restsign_dur_to_type(int start_time, int end_time, int dur, int tuplet, int tuplet_s_t){
  int div = 0;
  int temp;
  int t_time = start_time;
  string buffer = "";
  cout << "rest duration1: " << dur << endl;

  int sBeat = start_time / beatEDUs;
  int eBeat = end_time / beatEDUs;
  if (eBeat > sBeat){
	int dur1 = beatEDUs - (start_time % beatEDUs);
	buffer = buffer + convert(dur1) + " ";
	dur = dur - dur1;
  }
  while (dur > beatEDUs){
	buffer = buffer + "r4 ";
	dur -= beatEDUs;
  }
  cout << "rest duration2: " << dur << endl;

  buffer = buffer + convert(dur) + " ";
  return buffer;
}


//-----------------------------------------------------------
/* void sort_notes(Note * n)
  Description: sort the note into a vector and keep the vector in time increasing order
  input: Note * n - pointer to a note
  output: NONE
 */
void sort_notes(Note * n){
  // check if the vector is empty
  if (all_notes.empty()){
	all_notes.push_back(n);
	return;
  }
  Note* cur;
  int i;
  // find the position inside the vector
  vector<Note*>::iterator it;
  for (it = all_notes.begin(); it!=all_notes.end(); it++){
      cur = *it;
      if (cur->start_t > n->start_t){
		all_notes.insert(it, n);
		return;
	}
  }
  // insert the note
  all_notes.insert(it, n);
}


//-----------------------------------------------------------
/* void sort_notes_orig(Note * n)
  Description: sort the note into a vector and keep the vector in time increasing order (for original notes). This function is similar to prev one but sorts notes into another vector.
  input: Note * n - pointer to a note
  output: NONE
 */
void sort_notes_orig(Note * n){
  if (all_notes_orig.empty()){
	all_notes_orig.push_back(n);
	return;
  }
  Note* cur;
  int i;
  vector<Note*>::iterator it;
  for (it = all_notes_orig.begin(); it!=all_notes_orig.end(); it++){
      cur = *it;
      if (cur->start_t > n->start_t){
		all_notes_orig.insert(it, n);
		return;
	}
  }
  all_notes_orig.insert(it, n);
}

// this function is currently not used
bool need_split(int dur, int beatEDUs){
  Rational<int> x(dur, beatEDUs);
  if (is_valid(x)) return false;
  return true;
}


// this function is currently not used
string Note::split_internal(int dur){
  string ret;
  ret = pitch_out + convert_dur_to_type(dur);
  return ret;
  
}

//-----------------------------------------------------------
/* string Note::split_bars(int bar_diff)
  Description: When a note's duration is longer than barEDU, this function split the middle part of the note (exclude the starting bar and ending bar)
  input: int bar_diff - difference between starting bar and ending bar
  output: string contains notation of middle part of the note
 */
string Note::split_bars(int bar_diff){
  // calculate time signature
  Rational<int> r(barEDUs, beatEDUs);
  string temp, ret;
  ret = "";
  if (r == 4) 
	temp = pitch_out + "1";
  if (r == 3)
	temp = pitch_out + "2~ " + pitch_out + "4";
  if (r == 5)
	temp = pitch_out + "1~ " + pitch_out + "4"; 
  while (bar_diff > 2){
	ret = ret + temp + "~ ";
	bar_diff --;
  }
  ret = ret + temp;
  return ret;
}

// this function is currently not used
string Note::split_head(int x){ //parameter is to indicate if "~" is needed
  string s, t;
  s = "";
  Rational<int> r(beatEDUs-sDiv, beatEDUs);
  t = convert_dur_to_type(beatEDUs-sDiv);
  s = pitch_out + t;
  if (x == 1) s = s + "~";
  s = s + " ";
  if (tuplet == 1)
	s = s + " } " ;
  return s;
}

// this function is currently not used
string Note::split_tail(){
  string s, t;
  s = "";
  Rational<int> r(eDiv, beatEDUs);
  if (eDiv == 0)
	return "DNE";
  else {
	t = convert_dur_to_type(eDiv);
	if (tuplet == 1)
	   s = "\\tuplet " + tuplet_name + " { " + pitch_out + t;
	else s = pitch_out + t;
  }
 // cout << s << endl;
  return s;
}

// this function is currently not used
void Note::split_within_bar(){
  string temp;
  int tie = 0;
  int x = 0;
  if (end_t > sBeat + beatEDUs) x = 1;
  int dur = eBeat - (sBeat + beatEDUs);
  if (dur >= beatEDUs) x = 1;
  if (eDiv > 0) x = 1;
  type_out = split_head(x);
  if (dur >= beatEDUs){
     temp = split_internal(dur);
     tie = 1;
  }
  else temp = " ";
  type_out = type_out + temp;
  if (split_tail()!="DNE"){
     if (tie == 1) type_out = type_out + "~";	
     type_out = type_out + split_tail() + " ";
  }
  cout <<"split within bar " << type_out << " " << endl;

}

// this function is currently not used
void Note::split_across_bar(int bar_diff){
  string temp;
  int x = 0;
  int tie = 0;
  if (end_t > sBeat + beatEDUs) x = 1;
  type_out = split_head(x);
  int dur = (sBar + barEDUs) - (sBeat + beatEDUs);

  if (dur >= beatEDUs){
     temp = split_internal(dur);
     tie = 1;
  }
  else temp = " ";

  type_out = type_out + temp;
  if (bar_diff > 1){
     if (tie == 1) type_out = type_out + "~ ";
     type_out = type_out + split_bars(bar_diff);
     tie = 1;
  }
  dur = eBeat - eBar;

  if (dur >= beatEDUs){
     if (tie == 1) temp = "~ ";
     temp = temp + split_internal(dur);
     tie = 1;
  }
  else temp = " "; 

  type_out = type_out + temp;  
  if (split_tail()!="DNE"){
     if (tie == 1) type_out = type_out + "~ ";
     type_out = type_out + split_tail() + " ";
  }
  cout << "split across bar " << type_out << endl;
}


//-----------------------------------------------------------
/* void verify_valid(int &time)
  Description: Check if time is valid, if it is not, change it to closest valid value
  input: int &time - (reference) time to be verified
  output: none
 */
void verify_valid(int &time){
  cout << "In function - beatEDUs: " << beatEDUs << " time: " << time << endl;
  int i;
  int div = time / beatEDUs;
  int rem = time % beatEDUs;
  int diff = abs(valid_time[0]-rem);
  int closest_time = valid_time[0];
  for (i = 0; i<13; i++)
	if (abs(valid_time[i]-rem) < diff){
		diff = abs(valid_time[i]-rem);
		closest_time = valid_time[i];
	}
  time = closest_time + div * beatEDUs;
}


//-----------------------------------------------------------------
/* void Note::notate()
  Description: the main function that set up all variables for output score
  input: none
  output: none
 */
void Note::notate(){
  // Barlines, beats, subdivisions
  int numSbar = start_t / barEDUs;		//start barline num
  sBar = numSbar * barEDUs ;		//start barline EDUs
  int numSbeat = (start_t - sBar) / beatEDUs;	//start beat num
  sBeat = start_t / beatEDUs * beatEDUs;	//start beat EDUs	
  sDiv = start_t - sBeat;			//start subdivisions EDUs *(40)

                                               //beginning subdiv of dur
  int eBegDiv = ((numSbeat + 1) * beatEDUs + sBar - start_t) % beatEDUs; 
  int numEbar = end_t / barEDUs;		//end barline num
  eBar = numEbar * barEDUs;			//end barline EDUs
  if ( eBar == end_t) eBar += 1;
  int numEbeat = (end_t - eBar) / beatEDUs;	//end beat num
  eBeat = end_t / beatEDUs * beatEDUs;	//end beat EDUs
  eDiv = end_t - eBeat;			//end subdivisions 

  int dur = end_t - start_t;
  int durBeats = ((end_t - eDiv) - (start_t + sDiv)) / beatEDUs;
  int internal_s_time = sBeat + beatEDUs; //beat after start time
  int internal_dur = eBeat - sBeat; 
  int bar_diff = numEbar - numSbar;

//  cout << "note: " << dur << " " << start_t << " " << end_t << endl;


  //spilt long durations
  Note * n = new Note(*this);
  n->split = 0;
  n->tuplet = 0;
  n->type_out = "";
  n->start_t = sBeat+beatEDUs;
  n->end_t = eBeat;
  n->pitch_out = pitch_out;
  n->loudness_out = loudness_out;
  int diff = (eBeat-sBeat) / beatEDUs;
  //cout << "diff is " << diff << endl;
  int d = diff;
  int t = sBeat + beatEDUs;

  while (d > 2){
	if (d > 5 && (barEDUs - t % barEDUs >= beatEDUs * 4)){
 		n->type_out += "1~ ";
		n->type_out +=pitch_out;
		d = d-4;
		t = t + beatEDUs*4;
		continue;
	}
	if (d > 3 && (barEDUs - t % barEDUs >= beatEDUs * 2)){
 		n->type_out += "2~ ";
		n->type_out +=pitch_out;
		d = d-2;
		t = t + beatEDUs*2;
		continue;
	}
	n->type_out += "4~ ";
	n->type_out +=pitch_out;
	t = t + beatEDUs;
	d--;
  }
  if (d==2 && eDiv!=0) n->type_out += "4~ ";
  if (d==2 && eDiv == 0) n->type_out += "4 ";
  if (d==1 && eDiv!=0) n->type_out += "~ ";
  if (d==1 && eDiv == 0) n->type_out += " ";
  if (diff > 1) {
	cout << "note(long): " << (diff-1) * beatEDUs << " " << n->start_t << " " << n->end_t << endl;
	sort_notes(n);
  }


  //split head and tail
  Note * shalf;
  
  if ((diff == 0) || (diff == 1 && eDiv==0)){
     type_out = convert_dur_to_type(dur);
     split = 0;
  }
  else {
	shalf = new Note(*this);
	shalf->pitch_out = pitch_out;
	shalf->loudness_out;
	shalf->start_t = eBeat;
	shalf->end_t = end_t;
	shalf->split = 0;
	end_t = sBeat + beatEDUs;
	split = 1;
        //cout<< "firstpart";
	type_out = convert_dur_to_type(beatEDUs-sDiv);
	//cout<< "lastpart";
	shalf->type_out = shalf->convert_dur_to_type(eDiv);
  	if (eDiv != 0) sort_notes(shalf);
  }
  sort_notes(this);
}

//----------------------------------------------------------------------------//

void Note::notateDurations( string aName, string startEDU, string durationEDU) 
{
  int stime, dur, endTime, bar, beat;
  std::vector<int> spellingBee;

  // Get needed parameters
  string attributeName = aName;
  std::stringstream ss(startEDU);
  ss >> stime; 


  std::stringstream ss1(durationEDU);
  ss1 >> dur; 
  endTime = stime + dur;

  string barEDU_str = tempo.getEDUPerBar().toPrettyString();
  string beatEDU_str = tempo.getEDUPerTimeSignatureBeat().toPrettyString();

  timesignature = tempo.getTimeSignature();
  //cout << timesignature << "================" << endl;
  //barEDUs = atoi(tempo.getEDUPerBar());
  //beatEDUs = atoi(tempo.getEDUPerTimeSignatureBeat());
  int temp = 0;
  for (int i=0; i< barEDU_str.length(); i++){
	int x = int(barEDU_str[i] - 48);
	cout << "digit " << i << ": " << x << endl;
	temp = temp*10 + x;
  }
  barEDUs = temp;

  temp = 0;
  for (int i=0; i< beatEDU_str.length(); i++){
	int x = int(beatEDU_str[i] - 48);
	cout << "digit " << i << ": " << x << endl;
	temp = temp*10 + x;
  }
  beatEDUs = temp;

  //cout << "beadEDUs: " << beatEDUs << endl;
  //cout << "starting time: " << startEDU << "duration: " << durationEDU << endl;
  //check if start time and end time is valid
  verify_valid(stime);
  verify_valid(endTime);
  start_t = stime; 
  end_t = endTime; 
  cout << "cp1: " << start_t << " " << end_t << endl;
  //sort before processing
  sort_notes_orig(this);
  return;

//==============================================================
//==============================================================
//REST PART IS NOT EXECUTED

  // Barlines, beats, subdivisions
  int numSbar = stime / barEDUs;		//start barline num
  sBar = numSbar * barEDUs ;		//start barline EDUs
  int numSbeat = (stime - sBar) / beatEDUs;	//start beat num
  sBeat = stime / beatEDUs * beatEDUs;	//start beat EDUs	
  sDiv = stime - sBeat;			//start subdivisions EDUs *(40)

                                               //beginning subdiv of dur
  int eBegDiv = ((numSbeat + 1) * beatEDUs + sBar - stime) % beatEDUs; 
  int numEbar = endTime / barEDUs;		//end barline num
  eBar = numEbar * barEDUs;			//end barline EDUs
  if ( eBar == endTime) eBar += 1;
  int numEbeat = (endTime - eBar) / beatEDUs;	//end beat num
  eBeat = endTime / beatEDUs * beatEDUs;	//end beat EDUs
  eDiv = endTime - eBeat;			//end subdivisions 

  int durBeats = ((endTime - eDiv) - (stime + sDiv)) / beatEDUs;
  int internal_s_time = sBeat + beatEDUs; //beat after start time
  int internal_dur = eBeat - sBeat; 
  int bar_diff = numEbar - numSbar;

/*
  int split_flag = 0; //flag for split, 1 for yes, 0 for no
  if (bar_diff>0) split_flag = 1;
  if (need_split(dur, beatEDUs)) split_flag = 1;

  if (split_flag == 0) {
     type_out = convert_dur_to_type(dur);
     split = 0;
     sort_notes(this);
  cout << "not split " << type_out << endl;
     return;
  }

  split = 1;
  if (bar_diff == 0){
     split_within_bar();
     sort_notes(this);
     return;
  }

  split_across_bar(bar_diff);
  sort_notes(this);
  return;

*/


  cout << "note: " << dur << " " << start_t << " " << end_t << endl;



  Note * n = new Note(*this);
  n->split = 0;
  n->tuplet = 0;
  n->type_out = "";
  n->start_t = sBeat+beatEDUs;
  n->end_t = eBeat;
  n->pitch_out = pitch_out;
  n->loudness_out = loudness_out;
  int diff = (eBeat-sBeat) / beatEDUs;
  cout << "diff is " << diff << endl;
  int d = diff;
  int t = sBeat + beatEDUs;

  while (d > 2){
	if (d > 5 && (barEDUs - t % barEDUs >= beatEDUs * 4)){
 		n->type_out += "1~ ";
		n->type_out +=pitch_out;
		d = d-4;
		t = t + beatEDUs*4;
		continue;
	}
	if (d > 3 && (barEDUs - t % barEDUs >= beatEDUs * 2)){
 		n->type_out += "2~ ";
		n->type_out +=pitch_out;
		d = d-2;
		t = t + beatEDUs*2;
		continue;
	}
	n->type_out += "4~ ";
	n->type_out +=pitch_out;
	t = t + beatEDUs;
	d--;
  }
  if (d==2 && eDiv!=0) n->type_out += "4~ ";
  if (d==2 && eDiv == 0) n->type_out += "4 ";
  if (d==1 && eDiv!=0) n->type_out += "~ ";
  if (d==1 && eDiv == 0) n->type_out += " ";
  if (diff > 1) {
	cout << "note(long): " << (diff-1) * beatEDUs << " " << n->start_t << " " << n->end_t << endl;
	sort_notes(n);
  }



  Note * shalf;
  
  if ((diff == 0) || (diff == 1 && eDiv==0)){
     type_out = convert_dur_to_type(dur);
     split = 0;
  }
  else {
	shalf = new Note(*this);
	shalf->pitch_out = pitch_out;
	shalf->loudness_out;
	shalf->start_t = eBeat;
	shalf->end_t = end_t;
	shalf->split = 0;
	end_t = sBeat + beatEDUs;
	split = 1;
        cout<< "firstpart";
	type_out = convert_dur_to_type(beatEDUs-sDiv);
	cout<< "lastpart";
	shalf->type_out = shalf->convert_dur_to_type(eDiv);
  	if (eDiv != 0) sort_notes(shalf);
  }
  sort_notes(this);

  /* add modifiers  ------not complete yet
  if (outstring!="")
	while (!modifiers.empty()){
		outstring = outstring + char(92) + modifiers.back();
		modifiers.pop_back();
	}
*/


  // Duration
  spellingBee.push_back(numSbar);
  spellingBee.push_back(numSbeat);
  spellingBee.push_back(sDiv);

  spellingBee.push_back(eBegDiv);		//3
  spellingBee.push_back(durBeats);
  spellingBee.push_back(eDiv);
  spellingBee.push_back(numEbar);

// Print
/*
  *outputFile << "=========================== " << attributeName;
  *outputFile << " ==========================" << endl;
  *outputFile << "  stime=" << stime << " dur=" << dur << " endTime=" << endTime 
       << endl;
  *outputFile << "   barEDUs=" << barEDUs << " beatEDUs=" << beatEDUs <<  endl;

  *outputFile << "sBar=" << sBar << " numSbar=" << numSbar << endl;
  *outputFile << " sBeat=" << sBeat << " numSbeat=" << numSbeat << " sDiv="
       << sDiv << endl;
  *outputFile << "eBar=" << eBar << " numEbar=" << numEbar << endl;
  *outputFile << " eBeat=" << eBeat << " numEbeat=" << numEbeat << " eDiv="
       << eDiv << endl;

  *outputFile << " " << endl;
  *outputFile << "Bar " << spellingBee.at(0) << " [+" 
              << spellingBee.at(1) << "+" << spellingBee.at(2) << "/" 
              << beatEDUs << "]" << setw(5) << spellingBee.at(3) << "/" 
              << beatEDUs << setw(5) << spellingBee.at(4) << " Beats" 
              << setw(5)<< spellingBee.at(5) << "/" << beatEDUs << setw(13) 
              << "Bar " << spellingBee.at(6) << endl;
*/

}  
