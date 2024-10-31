#define RXD1 16  // Pin for RX1 (Receiving)
#define TXD1 17  // Pin for TX1 (Sending)
#define LASERON "$0003260130&"
#define LASERSTART "$00022123&"

String readResponse() {
  String response = "";
  while (Serial1.available()) {
    char ch = (char)Serial1.read();
    response += ch;
    if (ch == '&') break;  // Stop reading when '&' is detected
  }
  return response;
}

int getDistance() {
  while (Serial1.available()) {
    String response = readResponse();
    if (!response.startsWith("$000621")) continue;       // Check if response is valid
    float distance = response.substring(7, 17).toInt();  // Extract distance measurement
    if (distance < 1600) return -1;                      // Return -1 if the distance is below threshold
    return distance;
  }
}

void setup() {
  Serial.begin(115200);
  // Initialize UART communication with specific RX and TX pins
  Serial1.begin(115200, SERIAL_8N1, RXD1, TXD1);
  delay(100);
}

void loop() {
  static int previousDistance = -1;
  static unsigned long previousTime = 0;

  // Start a new distance measurement
  Serial1.print(LASERSTART);

  // Get the current distance
  int currentDistance = getDistance();
  unsigned long currentTime = millis();

  // Check if a valid distance measurement is available
  if (currentDistance > 0) {
    Serial.print("Distance: ");
    Serial.print((currentDistance / 100000.0) + 0.116, 6);
    Serial.print(" m");

    // Calculate speed if a previous measurement exists
    if (previousDistance > 0) {
      float timeDifference = (currentTime - previousTime) / 1000.0;                // Time difference in seconds
      float distanceDifference = (previousDistance - currentDistance) / 100000.0;  // Distance difference in meters

      // Calculate speed
      float speed = distanceDifference / timeDifference;  // Speed in meters per second

      Serial.print("\tSpeed: ");
      Serial.print(speed, 6);
      Serial.println(" m/s");
    } else {
      Serial.println();
    }

    // Update previous values for the next speed calculation
    previousDistance = currentDistance;
    previousTime = currentTime;
  } else {
    Serial.println("No valid measurement.");
  }

  delay(100);  // Wait before the next measurement
}
