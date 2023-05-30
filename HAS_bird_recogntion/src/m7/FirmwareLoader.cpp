#include "FirmwareLoader.h"
#include <Arduino.h>
#include <FATFileSystem.h>
#include <MBRBlockDevice.h>
#include <PluggableUSBMSD.h>
#include <QSPIFBlockDevice.h>
#include <SDRAM.h>

QSPIFBlockDevice root;
mbed::MBRBlockDevice ota_data(&root, 2);
mbed::FATFileSystem ota_data_fs("qspi");

void USBMSD::begin() {}

USBMSD MassStorage(&root);

long getFileSize(FILE *fp) {
  fseek(fp, 0, SEEK_END);
  int size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  return size;
}

void waitForCorrectQspi() {
  int err = ota_data_fs.mount(&ota_data);
  if (err) {
    Serial.println("Error mounting qspi not formatted correctly");
    while (1) {
      delay(10000);
    }
  }
  Serial.println("QSPI correctly formatted");
    // write small file to QSPI too see if it works
    FILE *fp = fopen("/qspi/placeFwHere", "w");
    if (fp == NULL) {
      Serial.println("Error opening file");
      while (1) {
        delay(10000);
      }
    }
    fprintf(fp, "Hello World");
    fclose(fp);

}

void fwToSdram() {
  SDRAM.begin(0);

  // Copy M4 firmware to SDRAM
  FILE *fw = fopen("/qspi/fw.bin", "r");
  if (fw == NULL) {
    Serial.println(
        "Please copy a firmware for M4 core in the PORTENTA mass storage");
    Serial.println(
        "When done, please unmount the mass storage and reset the board");
    MassStorage.begin();
    while (1) {
      delay(10000);
    }
  }
  Serial.println("Copying M4 firmware to SDRAM");
  fread((uint8_t *)CM4_BINARY_START, getFileSize(fw), 1, fw);
  fclose(fw);
  Serial.println("Done copying M4 firmware to SDRAM");


}

/*
 * Boot M4 core
 * wait's for corretly formmated QSPI and copies the firmware to SDRAM
 */
void setupM4Firmware() {
  waitForCorrectQspi();
  fwToSdram();
}