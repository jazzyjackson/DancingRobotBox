#ifndef debug_h
#define debug_h

#include "PoseStruct.h"

/**
 * If I wanted to be fancier I could make the first arg a prefix that checks whether an array of debug messages I'm interested in matches, only print if I'm interested
 */

char buf[80];


void debug(const char* debugstring){
  Serial.println(debugstring);
}

void debug(const char* formatstring, int firstarg){
  sprintf(buf, formatstring, firstarg);
  Serial.println(buf);
}

void debug(const char* formatstring, unsigned long firstarg){
  sprintf(buf, formatstring, firstarg);
  Serial.println(buf);
}

void debug(const char* formatstring, int firstarg, int secondarg){
  sprintf(buf, formatstring, firstarg, secondarg);
  Serial.println(buf);
}


void debug(const char* formatstring, unsigned long firstarg, unsigned long secondarg){
  sprintf(buf, formatstring, firstarg, secondarg);
  Serial.println(buf);
}

void debug(const char* prefixstring, Pose* arrayptr, int arraylength){
  // oh eff I'm probably just printing the first value of each struct hear...
  Serial.print(prefixstring);
  for(int i = 0; i < arraylength; i++){
    sprintf(buf, "%4d", arrayptr[i]);
    // sprintf(buf, "%4d %4d", arrayptr[i].stage, arrayptr[i].backdrop)
    Serial.print(buf);
  }
  Serial.println("");
}

void debug(const char* prefixstring, unsigned long* arrayptr, int arraylength){
  Serial.print(prefixstring);
  for(int i = 0; i < arraylength; i++){
    sprintf(buf, "%12lu  ", arrayptr[i]);
    Serial.print(buf);
  }
  Serial.println("");
}

#endif
