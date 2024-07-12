#include <WiFi.h>         //Including wifi.h library it will take care of all wifi related task
#include <HTTPClient.h>   //Including HTTPClient.h library to use all api
#include <WebServer.h>

const char* ssid = "my_ssid";             
const char* password =  "my_password";    
const char* timeURL = "http://worldtimeapi.org/api/ip";

String apiKey = "my_signal_key";          
String phone_number = "my_phonenumber";   

String url;                            //url String will be used to store the final generated URL

String newestTime = "No time yet, Havent see anyone"; // message used to display if anyone has been seen
int count = 0;          // counter to hold the number of times movement has been detected and update the past counter
int pastCount = 0;      // counter to hold the nuber of triggering events
int limit = 5;       // limit is so that the it will turn off after 5 messages to consurve the 40 messages per 4 hours    1000

// PID Senser
int led = 21;                // the pin that the LED is atteched to
int sensor = 13;              // the pin that the sensor is atteched to
int state = LOW;             // by default, no motion detected
int val = 0;             

unsigned long previousMillis = 0;
const unsigned long interval = 600000;  // 10 mins cool down for sending text, to not exceed api limit

// webserver stuff
WebServer server(80);

void handleRoot() {
  String content = server.arg("content"); // Get the value of the "content" parameter from the request
  String html = "<html><head><meta http-equiv=\"refresh\" content=\"1\"></head>";
  html += "<body> Motion detected " + String(count) + " Times. Last time was at " + newestTime + "</body></html>";
  
  server.sendHeader("Content-Length", String(html.length()));
  server.send(200, "text/html", html);
}

// update the web server page with new content, this is done when movement is detected and movement counter is increased
void updatePageContent(const String& content) { 
  String path = "/";
  path += "?content=" + content;
  server.sendHeader("Location", path); // Set the Location header to redirect the browser to the updated page
  server.send(302);                    // Send a redirection response
}

// set up device
void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);              // Try to connect with the given SSID and password
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {  // Wait until WiFi is connected
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to the WiFi network"); // Print wifi connect messages
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");

  // PID
  pinMode(led, OUTPUT);      // initalize LED as an output
  pinMode(sensor, INPUT);    // initialize sensor as an input

}

// Loop where continuouse checking occurs
void loop(){ 
  server.handleClient();
  if(count < limit){             // stop to stay within the limit ### chnage to count < limit
    val = digitalRead(sensor);   // read sensor value
    if (val == HIGH) {           // check if the sensor is HIGH
      digitalWrite(led, HIGH);   // turn LED ON
      delay(100);                // delay 100 milliseconds 
      
      if (state == LOW) {        // if motion has been detected
        String time = getTime();
        newestTime = time;       // set curret time of motion to newestTime to show on web serevr when last motion was detected
        count++;
        Serial.println("Motion detected!"); // serial print to show motion if connected to Serial output (computer)
        
        
        unsigned long currentMillis = millis();
        // check if new movement by checking if enough time has passed (interval) to consider this a new movement 
        // if it is a new movement send a message to my sigal app using the api to notify of movement
        if((pastCount < count) && (currentMillis - previousMillis >= interval)){  
          previousMillis = currentMillis;
          pastCount = count;
          String message2send = "Motion detected " + String(count) + " Times. Last time was at " + String(newestTime);
          message_to_signal(message2send);       // send text of movement to phone through api
        }
        Serial.println("Updating WebServer");        
        updatePageContent("Hello and Welcome thanks for coming");
        
        
        delay(2000);        // delay for 2 seconds
        state = HIGH;       // update variable state to HIGH
      }
    } 
    else {
        digitalWrite(led, LOW); // turn LED OFF
        delay(200);             // delay 200 milliseconds 
        
        if (state == HIGH){
          Serial.println("Motion stopped!");
          state = LOW;       // update variable state to LOW
      }
    }
  } else{
    Serial.println("Stopped bc there is a limit of 40 messages per 240mins (4 hours)" + String(count)); // message to explain error
  }
}

// function to return current time 
String getTime() {
  String currentTime = "No time somethings wrong";  // initialise with error message so if is not replaced with current time will return the error message

  // if connected to internet get current time from 
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(timeURL);
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();

      // Extract the time (hh:mm:ss) from the JSON response
      int startIdx = payload.indexOf("utc_datetime");
      int endIdx = payload.indexOf(".", startIdx);
      currentTime = "";
      currentTime = payload.substring(startIdx+1, endIdx);
    } else {
      Serial.println("Error on HTTP request");
      currentTime = "Error on HTTP request";
    }
    http.end();
  }

  return currentTime;
}



// ### code needed for signal

void  message_to_signal(String message)       // user define function to send meassage to Signal app
{
  //adding all number, your api key, your message into one complete url
  url = "https://api.callmebot.com/signal/send.php?phone=" + phone_number + "&apikey=" + apiKey + "&text=" + urlencode(message);

  postData(); // calling postData to run the above-generated url once so that you will receive a message.
}

void postData()     //userDefine function used to call api(POST data)
{
  int httpCode;     // variable used to get the responce http code after calling api
  HTTPClient http;  // Declare object of class HTTPClient
  http.begin(url);  // begin the HTTPClient object with generated url
  httpCode = http.POST(url); // Finaly Post the URL with this function and it will store the http code
  if (httpCode == 200)      // Check if the responce http code is 200
  {
    Serial.println(String(url));
    Serial.println("Sent ok. " + String(count)); // print message sent ok message
  }
  else                      // if response HTTP code is not 200 it means there is some error.
  {
    Serial.println("Error."); // print error message.
  }
  http.end();          // After calling API end the HTTP client object.
}

String urlencode(String str)  // Function used for encoding the url
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str.length(); i++){
      c=str.charAt(i);
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
        //encodedString+=code2;
      }
      yield();
    }
    return encodedString;
}
