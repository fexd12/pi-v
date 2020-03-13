#include <Arduino.h>
#include "parson.h"
#include "AzureIotHub.h"
#include "Esp32MQTTClient.h"
#include <WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <string.h>

#define RST_PIN 22 // pin rfid
#define SS_PIN 21 // pin rfid

const char *ssid = "Fe";
const char *password = "123456789";
const char *host = "localhost";

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.
								  /*
 * Initialize.
 */

byte *data;
String a;
String b;
String c;
String d;

WiFiClient client;
void dump_byte_array(byte *buffer, byte bufferSize);
void setup(){
	Serial.begin(115200); // Initialize serial communications with the PC

	SPI.begin();		// Init SPI bus
	mfrc522.PCD_Init(); // Init MFRC522 card
	//for wifi
	Serial.print("Connecting to ");
	Serial.println(ssid);

	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED){
		delay(500);
		Serial.print(".");
	}
	Serial.println();
	Serial.println("Wifi connected");
	Serial.println("IP adress: ");
	Serial.println(WiFi.localIP());
	Serial.println("Getway: ");
	Serial.println(WiFi.gatewayIP());

	Serial.println(F("RFID Read: "));
}

void loop(){
	//Connection to server

	// Look for new cards
	if (!mfrc522.PICC_IsNewCardPresent()){
		delay(50);
		return;
	}
	// Select one of the cards
	if (!mfrc522.PICC_ReadCardSerial()){
		delay(50);
		return;
	}
	// Show some details of the PICC (that is: the tag/card)
	Serial.print(F("Card UID:"));
	dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
	Serial.println();

	data = mfrc522.uid.uidByte, mfrc522.uid.size;
	for (int i = 0; i < 4; i++){
		Serial.print(data[i], HEX);
	}
	a = String(data[0], HEX);
	b = String(data[1], HEX);
	c = String(data[2], HEX);
	d = String(data[3], HEX);

	a.toUpperCase();
	b.toUpperCase();
	c.toUpperCase();
	d.toUpperCase();

	//Data sending
	String url = "/tag/" + String(a) + String(b) + String(c) + String(d);
	Serial.println("Requesting URL  ");
	//Serial.println(url);

	client.print(String("GET ") + url + " HTTP/1.0\r\n" +
				 "Host: " + host + "\r\n" +
				 "Connection: close\r\n\r\n");

	delay(3000);
	while (client.available()){
		String line = client.readStringUntil('\r');
		Serial.print(line);
	}
	Serial.println();
	Serial.println("Closing connection");
	delay(3000);
}

// Helper routine to dump a byte array as hex values to Serial
void dump_byte_array(byte *buffer, byte bufferSize){
	for (byte i = 0; i < bufferSize; i++){
		Serial.print(buffer[i] < 0x10 ? " 0" : " ");
		Serial.print(buffer[i], HEX);
	}
}
