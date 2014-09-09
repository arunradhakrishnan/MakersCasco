#include <GPS.h>

GPS::GPS(){
	itsAGPRMCMessage = 0;
	dataStatus = 'V';  // A = valid data, V = invalid data
}

char GPS::getData()
{
  float lat, lon = 0.0;
  unsigned long fixAge = 0;
  
  for (unsigned long initialTime = millis(); millis() - initialTime < PARSE_TIME;)
    while (Serial1.available()) {
      char charRead = Serial1.read();
      GPSDevice.encode(charRead);
	  // Serial.print(charRead);    // Uncomment this line to see the GPS data flowing
      if ((charRead == ',') && (GPSDevice._term_number == 1)) {
        itsAGPRMCMessage = !GPSDevice.gpsstrcmp(GPSDevice._term, _GPRMC_TERM);
      }

      if ((charRead == 'A' || charRead == 'V') && itsAGPRMCMessage) {
        dataStatus = charRead;
        itsAGPRMCMessage = 0;
      }
 }

    GPSDevice.f_get_position(&lat, &lon, &fixAge);
  
	sprintf(latitude, "%.4f", lat);
	sprintf(longitude, "%.4f", lon);
	sprintf(altitude, "%.1f", GPSDevice.f_altitude());

  return dataStatus;
}

